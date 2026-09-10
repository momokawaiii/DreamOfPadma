const test = require('node:test');
const assert = require('node:assert/strict');
const fs = require('node:fs');
const vm = require('node:vm');
const F = require('./encounter-fx.js');
const E = require('./engine.js');
const D = require('./data.js');

function clock() {
  let now = 0, sequence = 0;
  const jobs = new Map(), history = [];
  return {
    schedule(fn, ms) { const id = ++sequence; const job = { id, at: now + ms, fn }; jobs.set(id, job); history.push(job); return id; },
    unschedule(id) { jobs.delete(id); },
    advance(ms) {
      const end = now + ms;
      for (;;) {
        const job = [...jobs.values()].filter(j => j.at <= end).sort((a,b) => a.at-b.at || a.id-b.id)[0];
        if (!job) break;
        jobs.delete(job.id); now = job.at; job.fn();
      }
      now = end;
    },
    history,
    get pending() { return [...jobs.values()]; }
  };
}
function combat(overrides = {}) {
  const game = new E.Game(overrides);
  game.dispatch('advance');
  const id = game.run.hand.find(id => game.card(id).defId === 'dancer');
  assert.ok(game.dispatch('deploy', {id,node:'home'}).ok);
  for (const node of ['fire','plain','gate']) assert.ok(game.dispatch('move',{ids:[id],node,mode:'encounter'}).ok);
  return game;
}
function attack(game) {
  const r = game.dispatch('attack',{targets:game.battle.units.filter(u => u.side === 'ruler' && u.hp > 0).map(u => u.id)});
  assert.ok(r.ok, r.text); return r.presentation;
}

// Exercise the shipped browser-global adapter, not an injected Playback clock.
// WebIDL accepts the global/undefined receiver; an arbitrary Playback is invalid.
function installBrowserRuntime(context, timer) {
  context.clockSchedule = timer.schedule;
  context.clockClear = timer.unschedule;
  vm.createContext(context);
  vm.runInContext(`
    globalThis.setTimeout = function(fn, ms) {
      'use strict';
      if (this !== undefined && this !== globalThis) throw new TypeError('Illegal invocation: setTimeout receiver');
      return clockSchedule(fn, ms);
    };
    globalThis.clearTimeout = function(id) {
      'use strict';
      if (this !== undefined && this !== globalThis) throw new TypeError('Illegal invocation: clearTimeout receiver');
      return clockClear(id);
    };
  `,context);
  vm.runInContext(fs.readFileSync(require.resolve('./encounter-fx.js'),'utf8'),context,{filename:'encounter-fx.js'});
  return context.PadmaEncounterFX;
}

test('timeline predicts repeated enemy turns from copies without changing the battle', () => {
  const game = combat(), before = E.clone(game.battle), result = F.nextTurns(game.battle);
  assert.equal(result[0].id, game.battle.active);
  assert.deepEqual(result.slice(0,3).map(t => t.id), [game.battle.active,'enemy-1','enemy-0']);
  assert.deepEqual(game.battle, before);
  assert.equal(new Set(result.map(t => t.id)).size,3);
  game.battle.units[1].hp = 0;
  assert.ok(F.nextTurns(game.battle).every(t => t.id !== game.battle.units[1].id));
  game.battle.units.forEach(u => u.hp = 0);
  assert.deepEqual(F.nextTurns(game.battle),[]);
});

test('rain is three serial waves; multiple targets share one impact frame per wave', () => {
  const game = combat(), receipt = game.dispatch('skill',{id:'rain'}).presentation, steps = F.plan(receipt);
  const impacts = steps.filter(s => s.phase === 'impact');
  assert.deepEqual(impacts.map(s => s.wave),[0,1,2]);
  assert.ok(impacts.every(s => s.effects.length === 3));
  assert.ok(steps.every((s,i) => !i || steps[i-1].at < s.at));
  assert.equal(F.plan(attack(combat())).filter(s => s.phase === 'impact').length,1);
});

test('effect text distinguishes actual health, full healing, true damage, absorption, block and recoil', () => {
  const pairs = [
    [{kind:'damage',amount:3,isTrue:true},'真伤 −3'],
    [{kind:'damage',amount:0,absorbed:9},'归墟吸收 9'],
    [{kind:'damage',amount:2,absorbed:4},'归墟吸收 4 / 生命 −2'],
    [{kind:'damage',amount:0,blocked:true},'抗衡抵挡 · 消耗 1 层'],
    [{kind:'heal',amount:0},'生命已满'], [{kind:'heal',amount:2},'生命 +2'],
    [{kind:'recoil',amount:.5},'反伤 −0.5'],
    [{kind:'shield',amount:2},'归墟 +2'], [{kind:'block',amount:1},'抗衡 +1']
  ];
  for(const [effect, text] of pairs) assert.equal(F.effectText(effect),text);
});

test('playback keeps the acting unit and before HP until impact; state and receipts stay detached', () => {
  const game = combat(), receipt = attack(game), frozen = E.clone(receipt), current = E.clone(game.battle);
  const timer = clock(), player = new F.Playback(timer), stages = [];
  player.play(receipt,{onStep:(step,view) => stages.push({phase:step.phase,view:E.clone(view)})});
  assert.equal(player.busy,true);
  assert.equal(stages[0].view.active,receipt.actorId);
  timer.advance(739);
  assert.equal(stages.at(-1).phase,'launch');
  assert.deepEqual(stages.at(-1).view.units,receipt.before.units);
  timer.advance(1);
  assert.equal(stages.at(-1).phase,'impact');
  const targetId = receipt.effects[0].targetId;
  assert.equal(stages.at(-1).view.units.find(u => u.id === targetId).hp,receipt.effects[0].hpAfter);
  assert.deepEqual(game.battle,current); assert.deepEqual(receipt,frozen);
  timer.advance(3000); assert.equal(player.busy,false);
});

test('cancel invalidates even already-queued callbacks and never completes old playback', () => {
  const timer = clock(), player = new F.Playback(timer), receipt = attack(combat());
  let deliveries = 0, completions = 0;
  player.play(receipt,{onStep:() => deliveries++,onDone:() => completions++});
  const obsolete = timer.history.map(j => j.fn);
  player.cancel();
  obsolete.forEach(fn => fn());
  assert.equal(deliveries,1); assert.equal(completions,0); assert.equal(player.busy,false);
  assert.equal(timer.pending.length,0);
  player.play(receipt,{onDone:() => completions++}); timer.advance(10000);
  obsolete.forEach(fn => fn());
  assert.equal(completions,1);
});

test('cancel from the first callback schedules no stale work; completion is idempotent', () => {
  const timer = clock(), player = new F.Playback(timer), receipt = attack(combat());
  player.play(receipt,{onStep:() => player.cancel()});
  assert.equal(timer.pending.length,0);
  let count = 0;
  player.play(receipt,{onDone:() => count++});
  const done = timer.history.at(-1).fn;
  timer.advance(10000); done(); done();
  assert.equal(count,1);
});

test('2x changes delays only; lethal actions hold the battle through impact and settlement', () => {
  const game = combat();
  game.battle.units.filter(u => u.side === 'ruler').forEach(u => u.hp=1);
  const receipt = attack(game); assert.equal(receipt.ended,'victory'); assert.equal(game.battle,null);
  const timer = clock(), player = new F.Playback(timer), seen=[];
  player.play(receipt,{speed:2,onStep:s => seen.push(s.phase),onDone:() => seen.push('done')});
  timer.advance(370); assert.equal(seen.at(-1),'impact'); assert.equal(player.busy,true);
  timer.advance(524); assert.equal(seen.at(-1),'impact');
  timer.advance(1); assert.equal(seen.at(-1),'settle'); assert.equal(player.busy,true);
  timer.advance(525); assert.equal(seen.at(-1),'done'); assert.equal(player.busy,false);
});

test('presentation template names the actor, selected targets, status and typed source-target action', () => {
  const game=combat(), battle=E.clone(game.battle), receipt=attack(game);
  const html=F.render(battle,{targets:['enemy-0','enemy-1'],busy:true,receipt});
  assert.match(html,/正在行动/); assert.match(html,/data-unit-id="enemy-0"/); assert.match(html,/data-unit-id="enemy-1"/);
  assert.match(html,/希律的舞女 · 普攻 → 关隘守卫 1、关隘守卫 2/);
  assert.match(html,/生命 <strong>16/);
  assert.match(html,/role="status" aria-live="polite"/);
  assert.equal((html.match(/data-action="target"/g)||[]).length,3);
  assert.match(html,/id="encounterTrails"/);
  battle.units[0].name='<script>alert(1)</script>';
  assert.doesNotMatch(F.render(battle),/<script>/);
});

// The following harness executes real app event/timer wiring, with lightweight
// document doubles and a paint spy. It is not browser, layout or animation evidence.
function appHarness({ failPaint = null } = {}) {
  const timer=clock(), elements=new Map(), handlers=new Map(), windowHandlers=new Map(), paints=[];
  let game, hitElement=null;
  function element(id='') {
    if (elements.has(id)) return elements.get(id);
    const listeners=new Map(), classes=new Set();
    const el={id,innerHTML:'',textContent:'',hidden:false,disabled:false,open:false,dataset:{},style:{},attrs:{},focus:()=>{},setAttribute(key,value){this.attrs[key]=value;},getContext:()=>new Proxy({},{get:(target,key)=>target[key]||(()=>{})}),
      classList:{add:(...v)=>v.forEach(x=>classes.add(x)),remove:(...v)=>v.forEach(x=>classes.delete(x)),toggle:(key,value)=>value?classes.add(key):classes.delete(key),contains:key=>classes.has(key)},
      addEventListener:(name,fn)=>listeners.set(name,fn),querySelectorAll:selector=>{
        if(id!=='battle')return [];
        if(selector==='[data-unit-id]')return (game?.battle?.units||[]).map(u=>{const e=element('unit:'+u.id);e.dataset.unitId=u.id;return e;});
        if(selector==='[data-skill]')return D.skills.map(s=>{const e=element('skill:'+s.id);e.dataset.skill=s.id;return e;});
        return [];
      },scrollIntoView:()=>{},
      showModal(){this.open=true;},close(){this.open=false;},listeners};
    elements.set(id,el); return el;
  }
  const menus=Array.from({length:6},(_,i)=>element('menu-'+i));
  const document={
    getElementById:element, querySelector:()=>null,elementFromPoint:()=>hitElement,
    querySelectorAll:selector=>selector.includes('.masthead nav button')?menus:[],
    addEventListener:(name,fn)=>handlers.set(name,fn)
  };
  class ObservedGame extends E.Game { constructor(){super();game=this;} }
  const window={PadmaData:D,PadmaEngine:{...E,Game:ObservedGame},PadmaHome:require('./home-ui.js'),
    addEventListener:(name,fn)=>windowHandlers.set(name,fn)};
  const errors=[];
  const context={window,document,console:{error:(...args)=>errors.push(args)},localStorage:{setItem:()=>{},getItem:()=>null},requestAnimationFrame:()=>{}};
  const runtime=installBrowserRuntime(context,timer);
  vm.runInContext(fs.readFileSync(require.resolve('./card-input.js'),'utf8'),context,{filename:'card-input.js'});
  window.PadmaCardInput=context.PadmaCardInput;
  let paintFailed=false;
  window.PadmaEncounterFX={...runtime,paint:(container,step,view,receipt)=>{
    if(!paintFailed&&step.phase===failPaint){paintFailed=true;throw new Error('test paint failure');}
    paints.push({step:E.clone(step),view:E.clone(view),receipt});
  }};
  vm.runInContext(fs.readFileSync(require.resolve('./app.js'),'utf8'),context,{filename:'app.js'});
  const click=(action,extra={},detail=0)=>handlers.get('click')({detail,target:{closest:()=>({dataset:{action,...extra},disabled:false})}});
  const node=id=>element('map').listeners.get('click')({target:{closest:()=>({dataset:{node:id}})}});
  const key=(value,target={})=>{let prevented=false;handlers.get('keydown')({key:value,target,repeat:false,preventDefault:()=>{prevented=true;}});return prevented;};
  click('start'); click('advance');
  const id=game.run.hand.find(id=>game.card(id).defId==='dancer');
  click('select-card',{id});click('deploy');
  handlers.get('change')({target:{dataset:{select:id},checked:true}});
  for(const destination of ['fire','plain']) {node(destination);click('move');handlers.get('change')({target:{dataset:{select:id},checked:true}});}
  node('gate');click('choose-mode',{mode:'encounter'});
  const snapshot=E.clone(game.run);
  click('move');
  assert.equal(game.battle.mode,'encounter');
  const hitNode=hit=>({id:hit?.unitId?'unit:'+hit.unitId:'encounterArena',closest:selector=>{
    if(selector==='#encounterArena')return hit?.arena?element('encounterArena'):null;
    if(selector==='[data-unit-id]')return hit?.unitId?Object.assign(element('unit:'+hit.unitId),{dataset:{unitId:hit.unitId}}):null;
    if(selector==='[data-action]')return hit?.unitId?{dataset:{action:'target',target:hit.unitId}}:null;
    return null;
  }});
  const arenaClick=()=>element('battle').listeners.get('click')({target:hitNode({arena:true})});
  const castSkill=(id,target)=>{
    click('skill',{skill:id});
    const s=D.skills.find(s=>s.id===id);
    if(s.target==='all')arenaClick();
    else click('target',{target:target||(s.target==='ally'?game.battle?.active:'enemy-0')});
    click('confirm-card');
  };
  const pointer=(type,{skillId,pointerId=1,x=100,y=100,hit={arena:false},button=0}={})=>{
    hitElement=hitNode(hit);
    const skill=skillId?{dataset:{skill:skillId},disabled:false,focus:()=>{}}:null;
    const target=skill?{closest:selector=>selector==='[data-skill]'?skill:null}:hitElement;
    handlers.get(type)({target,pointerId,clientX:x,clientY:y,button,preventDefault:()=>{}});
  };
  const reenter=mode=>{handlers.get('change')({target:{dataset:{select:id},checked:true}});node('gate');click('choose-mode',{mode});click('move');};
  const ordinaryAttack=(targets=['enemy-0'])=>{
    if(targets.length>1)click('attack-count',{count:String(targets.length)});
    click('attack');targets.forEach(target=>click('target',{target}));
  };
  return {timer,click,key,game,element,menus,paints,snapshot,windowHandlers,errors,castSkill,arenaClick,pointer,reenter,ordinaryAttack};
}

test('app locks repeated clicks/keys and enemy autoplay until visual playback has ended', () => {
  const h=appHarness();
  h.ordinaryAttack(); const state=JSON.stringify(h.game.run), count=h.game.battle.actionCount;
  assert.match(h.element('battle').innerHTML,/正在行动 · 希律的舞女/);
  h.click('attack');h.click('guard');h.castSkill('cut');h.key('1');h.key('Tab');h.click('start');
  assert.equal(JSON.stringify(h.game.run),state); assert.equal(h.game.battle.actionCount,count);
  h.timer.advance(2008); assert.equal(h.game.battle.actionCount,count);
  h.timer.advance(2); assert.match(h.element('battle').innerHTML,/敌方即将行动 · 关隘守卫 2/);
  h.timer.advance(949); assert.equal(h.game.battle.actionCount,count);
  h.timer.advance(1); assert.equal(h.game.battle.actionCount,count+1);
  assert.equal(h.paints.at(-1).receipt.actorId,'enemy-1');
});

test('app exit during animation rolls back exactly and cancels future impacts and enemy turns', () => {
  const h=appHarness();
  h.ordinaryAttack();h.timer.advance(400);
  const before=h.paints.length;
  h.click('exit');h.timer.advance(20000);
  assert.equal(h.game.battle,null);assert.deepEqual(h.game.run,h.snapshot);
  assert.equal(h.paints.length,before);
  assert.equal(h.element('battle').hidden,true);
});

test('app retains the final hit view even after engine settlement; menus reopen only after playback', () => {
  for(const won of [true,false]) {
    const h=appHarness();
    if(won) {
      h.game.battle.units.filter(u=>u.side==='ruler').forEach(u=>u.hp=1);
      h.ordinaryAttack(['enemy-0','enemy-1']);
    } else {
      const p=h.game.battle.units.find(u=>u.side==='player');p.hp=.5;
      h.game.battle.units.find(u=>u.side==='ruler').def=100;
      h.ordinaryAttack();
    }
    assert.equal(h.game.battle,null);
    assert.equal(h.element('battle').hidden,false);
    assert.equal(h.element('world').hidden,true);
    assert.ok(h.menus.every(el=>el.disabled));
    const state=JSON.stringify(h.game.run);h.click('start');h.click('exit');h.key('1');
    assert.equal(JSON.stringify(h.game.run),state);
    h.timer.advance(740);assert.equal(h.paints.at(-1).step.phase,'impact');
    assert.equal(h.element('battle').hidden,false);
    h.timer.advance(2100);assert.equal(h.element('battle').hidden,true);
    assert.ok(h.menus.every(el=>!el.disabled));
    if(!won)assert.deepEqual(h.game.run,h.snapshot);
  }
});

test('a rejected app command does not start a fake effect or advance the turn', () => {
  const h=appHarness();h.game.run.resources.flow=0;
  const before=E.clone(h.game.battle);
  h.ordinaryAttack();h.timer.advance(5000);
  assert.equal(h.paints.length,0);assert.deepEqual(h.game.battle,before);
});

test('after a nonlethal basic skill, the same player retains the action and cannot use a second skill', () => {
  const h=appHarness(),actor=h.game.battle.active;
  h.castSkill('wind');h.timer.advance(2010);
  assert.equal(h.game.battle.active,actor);
  assert.match(h.element('battle').innerHTML,/轮到你了 · 希律的舞女/);
  assert.match(h.element('battle').innerHTML,/基础技能已用/);
  const count=h.paints.length;h.castSkill('cut');
  assert.equal(h.paints.length,count);
  h.click('guard');assert.equal(h.paints.at(-1).receipt.actionLabel,'防御');
});

test('invalid numeric skill during enemy windup cannot strand the enemy after transactional rollback', () => {
  const h=appHarness();h.click('guard');h.timer.advance(2010);
  const actor=h.game.battle.active,count=h.game.battle.actionCount;
  assert.equal(actor,'enemy-1');
  h.key('1');assert.equal(h.game.battle.actionCount,count);
  h.timer.advance(950);assert.equal(h.game.battle.actionCount,count+1);
  assert.equal(h.paints.at(-1).receipt.actorId,actor);
});

test('back-forward cache restoration rebuilds ongoing or settled battle UI without stale callbacks', () => {
  for(const ended of [false,true]) {
    const h=appHarness();
    if(ended)h.game.battle.units.filter(u=>u.side==='ruler').forEach(u=>u.hp=1);
    h.ordinaryAttack(ended?['enemy-0','enemy-1']:['enemy-0']);h.timer.advance(400);
    const oldEffects=h.paints.length;
    h.windowHandlers.get('pagehide')({});
    h.timer.advance(5000);assert.equal(h.paints.length,oldEffects);
    h.windowHandlers.get('pageshow')({persisted:true});
    if(ended){
      assert.equal(h.element('battle').hidden,true);
      assert.ok(h.menus.every(el=>!el.disabled));
    }else{
      assert.equal(h.element('battle').hidden,false);
      assert.match(h.element('battle').innerHTML,/敌方即将行动/);
      h.timer.advance(950);assert.equal(h.paints.length,oldEffects+1);
    }
  }
});

function paintHarness(receipt) {
  const view=E.clone(receipt.before), nodes=new Map();
  function node(){
    const classes=new Set();
    return {innerHTML:'',textContent:'',style:{setProperty:()=>{}},attrs:{},
      classList:{add:(...v)=>v.forEach(s=>classes.add(s)),remove:(...v)=>v.forEach(s=>classes.delete(s)),contains:s=>classes.has(s)},
      setAttribute(key,value){this.attrs[key]=value;},
      getBoundingClientRect:()=>({left:0,top:0,width:1000,height:400})};
  }
  const units=view.units.map((unit,i)=>{
    const el=node(),parts=new Map();el.dataset={unitId:unit.id};
    for(const key of ['.unit-portrait','.unit-floats','.hp i','.unit-hp','.unit-buffs','.unit-state'])parts.set(key,node());
    parts.get('.unit-portrait').getBoundingClientRect=()=>({left:100+i*300,top:80,width:100,height:130});
    el.querySelector=selector=>parts.get(selector);
    return el;
  });
  for(const key of ['#encounterArena','#encounterTrails','#turnTitle','#actionCaption','#actionReadout'])nodes.set(key,node());
  const container={querySelectorAll:()=>units,querySelector:selector=>nodes.get(selector)};
  return {view,units,nodes,container};
}

test('actual painter keeps an earlier rain death down through later waves and displays directional effects', () => {
  const game=combat();game.battle.units[1].hp=1;
  const receipt=game.dispatch('skill',{id:'rain'}).presentation;
  const h=paintHarness(receipt), dead=h.units.find(u=>u.dataset.unitId==='enemy-0');
  for(const step of F.plan(receipt).filter(s=>!['done','settle'].includes(s.phase))){
    if(step.phase==='impact')F.applyEffects(h.view,step.effects);
    F.paint(h.container,step,h.view,receipt);
    if(step.phase==='launch'){
      assert.match(h.nodes.get('#encounterTrails').innerHTML,/fishbone-fall/);
      if(step.wave>0)assert.equal(dead.classList.contains('fx-fallen'),true);
    }
  }
  assert.equal(dead.classList.contains('fx-fallen'),true);
  assert.equal(dead.querySelector('.unit-state').textContent,'已倒下');
  assert.equal(dead.querySelector('.hp i').style.width,'0%');
  assert.match(h.nodes.get('#actionReadout').textContent,/鱼骸骤雨 →/);
});

test('actual painter uses receipt support/block texts and maintains action status through settlement', () => {
  const game=combat();
  const receipt=game.dispatch('skill',{id:'wind',target:game.battle.active}).presentation,h=paintHarness(receipt);
  for(const step of F.plan(receipt).filter(s=>s.phase!=='done')){
    if(step.phase==='impact')F.applyEffects(h.view,step.effects);
    F.paint(h.container,step,h.view,receipt);
  }
  const actor=h.units.find(u=>u.dataset.unitId===receipt.actionOwnerId);
  assert.match(actor.querySelector('.unit-floats').innerHTML,/归墟 \+2/);
  assert.equal(actor.classList.contains('fx-support'),true);
  assert.match(h.nodes.get('#actionCaption').textContent,/继续普攻或防御/);
});

test('after rain kills its caster, the banner and usable controls pass to the next living ally', () => {
  const h=appHarness(),caster=h.game.battle.units.find(u=>u.side==='player');
  const ally={...E.clone(caster),id:'ally-2',name:'第二位造物',ready:caster.ready+.01};
  h.game.battle.units.push(ally);caster.hp=1;
  h.castSkill('rain');
  assert.equal(h.game.battle.active,ally.id);
  h.timer.advance(4210);
  assert.match(h.element('battle').innerHTML,/轮到你了 · 第二位造物/);
  const count=h.paints.length;h.click('guard');
  assert.equal(h.paints.length,count+1);
  assert.equal(h.paints.at(-1).receipt.actorId,ally.id);
});

test('default browser timers complete and cancel with their host receiver', () => {
  const timer=clock(),runtime=installBrowserRuntime({},timer),player=new runtime.Playback();
  let done=0,errors=0;
  player.play(attack(combat()),{onDone:()=>done++,onError:()=>errors++});
  assert.equal(player.busy,true);assert.equal(timer.pending.length,4);
  timer.advance(2010);
  assert.equal(done,1);assert.equal(errors,0);assert.equal(player.busy,false);
  player.play(attack(combat()),{onDone:()=>done++,onError:()=>errors++});
  player.cancel();assert.equal(timer.pending.length,0);
  timer.advance(10000);assert.equal(done,1);assert.equal(errors,0);
});

test('painting exceptions release playback exactly once and discard later callbacks', () => {
  for(const phase of ['windup','launch','impact','settle']) {
    const timer=clock(),player=new F.Playback(timer),receipt=attack(combat());
    let done=0,errors=0,steps=0;
    player.play(receipt,{onStep:s=>{steps++;if(s.phase===phase)throw new Error(phase);},onDone:()=>done++,onError:(error,r)=>{
      errors++;assert.equal(error.message,phase);assert.deepEqual(r,receipt);
    }});
    timer.advance(10000);
    const delivered=steps;timer.history.forEach(j=>j.fn());
    assert.equal(player.busy,false);assert.equal(timer.pending.length,0);
    assert.equal(errors,1);assert.equal(done,0);assert.equal(steps,delivered);
  }
});

test('partial scheduling failure cancels already scheduled stages and releases the presentation', () => {
  const timer=clock();let calls=0,errors=0,done=0;
  const player=new F.Playback({schedule:(fn,ms)=>{if(++calls===2)throw new Error('scheduler failed');return timer.schedule(fn,ms);},unschedule:timer.unschedule});
  player.play(attack(combat()),{onError:()=>errors++,onDone:()=>done++});
  assert.equal(player.busy,false);assert.equal(timer.pending.length,0);
  timer.advance(10000);timer.history.forEach(j=>j.fn());
  assert.equal(errors,1);assert.equal(done,0);
});

test('completion callback failure takes the recovery path without retaining the busy flag', () => {
  const timer=clock(),player=new F.Playback(timer);let errors=0;
  player.play(attack(combat()),{onDone:()=>{throw new Error('completion failed');},onError:()=>errors++});
  timer.advance(10000);assert.equal(player.busy,false);assert.equal(errors,1);
});

test('app recovers from skill painting failure without replaying damage, payment or card use', () => {
  for(const phase of ['windup','impact']) {
    const h=appHarness({failPaint:phase}),enemy=h.game.battle.units.find(u=>u.id==='enemy-0'),hp=enemy.hp,flow=h.game.run.resources.flow;
    const skill=D.skills.find(s=>s.id==='cut'),actor=h.game.battle.active;
    h.castSkill('cut');h.timer.advance(2010);
    assert.equal(h.errors.length,1);assert.equal(enemy.hp,hp-3);
    assert.equal(h.game.run.resources.flow,flow-skill.flow);
    assert.equal(h.game.run.skills.discard.filter(id=>id==='cut').length,1);
    assert.equal(h.game.battle.active,actor);
    assert.match(h.element('battle').innerHTML,/轮到你了/);
    assert.doesNotMatch(h.element('battle').innerHTML,/动作播放中/);
    assert.match(h.element('toast').textContent,/不会重复扣费/);
    h.click('guard');assert.equal(h.paints.at(-1).receipt.actionLabel,'防御');
  }
});

test('app recovery preserves enemy handoff and final victory or defeat settlement', () => {
  const h=appHarness({failPaint:'windup'});
  h.ordinaryAttack();const count=h.game.battle.actionCount;
  assert.match(h.element('battle').innerHTML,/敌方即将行动/);
  h.timer.advance(950);assert.equal(h.game.battle.actionCount,count+1);
  assert.equal(h.errors.length,1);
  for(const won of [true,false]) {
    const g=appHarness({failPaint:'impact'});
    if(won)g.game.battle.units.filter(u=>u.side==='ruler').forEach(u=>u.hp=1);
    else{g.game.battle.units[0].hp=.5;g.game.battle.units[1].def=100;}
    g.ordinaryAttack(won?['enemy-0','enemy-1']:['enemy-0']);g.timer.advance(740);
    assert.equal(g.game.battle,null);assert.equal(g.element('battle').hidden,true);
    assert.ok(g.menus.every(el=>!el.disabled));assert.equal(g.errors.length,1);
    const run=JSON.stringify(g.game.run);g.timer.advance(10000);
    assert.equal(JSON.stringify(g.game.run),run);
    if(!won)assert.deepEqual(g.game.run,g.snapshot);
  }
});

test('two-character screenshot fixture advances after Vitruvian attack and skill using default timers', () => {
  for(const kind of ['attack','skill']) {
    const h=appHarness(),battle=h.game.battle;
    const vitruvian={...E.clone(battle.units[0]),id:'vitruvian-test',defId:'vitruvian',name:'维特鲁威人',hp:25,maxHP:25,atk:2,def:4,attackCost:1,ready:battle.actionTime};
    battle.units.push(vitruvian);battle.active=vitruvian.id;
    h.click('fx-speed');h.click('fx-speed');
    if(kind==='skill')h.castSkill('wind');else h.ordinaryAttack();
    assert.match(h.element('battle').innerHTML,kind==='skill'?/基础卡展开 · 挟风鸣/:/正在行动 · 维特鲁威人/);
    h.timer.advance(2010);
    assert.equal(h.errors.length,0);
    assert.doesNotMatch(h.element('battle').innerHTML,/动作播放中/);
    assert.match(h.element('battle').innerHTML,kind==='skill'?/轮到你了 · 维特鲁威人/:/轮到你了 · 希律的舞女/);
  }
});

test('ordinary attacks arm first, release on one target by default and allow an explicit Dancer two-target count', () => {
  const h=appHarness(),before=JSON.stringify(h.game.run);
  h.click('attack');assert.equal(h.paints.length,0);assert.equal(JSON.stringify(h.game.run),before);
  assert.match(h.element('battle').innerHTML,/等待选择敌人/);
  h.click('target',{target:'enemy-1'});
  assert.deepEqual(Array.from(h.paints[0].receipt.effects,e=>e.targetId),['enemy-1']);
  const single=appHarness();
  single.game.battle.units[0].maxTargets=1;
  single.click('target',{target:'enemy-0'});assert.equal(single.paints.length,0);
  single.click('attack');single.click('target',{target:'enemy-1'});
  assert.deepEqual(Array.from(single.paints[0].receipt.effects,e=>e.targetId),['enemy-1']);
  const multi=appHarness();
  multi.click('attack-count',{count:'2'});multi.click('attack');
  multi.click('target',{target:'enemy-0'});assert.equal(multi.paints.length,0);
  multi.click('target',{target:'enemy-1'});
  assert.deepEqual(Array.from(multi.paints[0].receipt.effects,e=>e.targetId),['enemy-0','enemy-1']);
});

test('ordinary targeting does not silently retain another actor selection', () => {
  const h=appHarness();
  h.ordinaryAttack();h.timer.advance(7930);
  // Player → enemy 2 → enemy 1 → player, with explicit presentation waits.
  assert.match(h.element('battle').innerHTML,/轮到你了/);
  const count=h.paints.length;h.click('attack');assert.equal(h.paints.length,count);
  assert.match(h.element('battle').innerHTML,/等待选择敌人/);
});

test('long-press opens unit details without attacking and closing keeps the action usable', () => {
  const h=appHarness(),before=JSON.stringify(h.game.run);
  h.click('attack');
  h.pointer('pointerdown',{hit:{arena:true,unitId:'enemy-1'}});
  h.timer.advance(499);assert.equal(h.element('dialog').open,false);
  h.timer.advance(1);assert.equal(h.element('dialog').open,true);
  assert.equal(h.element('dialog').classList.contains('unit-detail-modal'),true);
  h.pointer('pointerup',{hit:{arena:true,unitId:'enemy-1'}});
  h.click('target',{target:'enemy-1'},1);
  assert.equal(h.paints.length,0);assert.equal(JSON.stringify(h.game.run),before);
  h.click('close');assert.equal(h.element('dialog').open,false);
  h.click('cancel-attack');assert.equal(JSON.stringify(h.game.run),before);
  h.ordinaryAttack(['enemy-1']);assert.equal(h.paints.length,1);
});

test('dragging a single-target skill previews the chosen enemy and charges once only on confirmation', () => {
  const h=appHarness(),before=JSON.stringify(h.game.run),p=h.game.battle.units.find(u=>u.id==='enemy-1'),hp=p.hp;
  h.pointer('pointerdown',{skillId:'cut',x:100,y:700});
  h.pointer('pointermove',{x:760,y:250,hit:{arena:true,unitId:p.id}});
  assert.equal(h.element('cardGhost').hidden,false);
  assert.equal(h.element('unit:'+p.id).classList.contains('drop-hover'),true);
  h.pointer('pointerup',{x:760,y:250,hit:{arena:true,unitId:p.id}});
  assert.equal(h.element('cardGhost').hidden,true);
  assert.equal(JSON.stringify(h.game.run),before);assert.equal(hp,p.hp);assert.equal(h.paints.length,0);
  assert.match(h.element('cardAimPanel').innerHTML,/敌方 · 关隘守卫 2/);
  h.click('confirm-card');h.click('confirm-card');
  assert.equal(p.hp,hp-3);assert.equal(h.game.run.skills.discard.filter(id=>id==='cut').length,1);
  assert.equal(h.paints.length,1);
  assert.equal(h.paints[0].receipt.actorId,null);
  assert.equal(h.paints[0].receipt.source.kind,'card');
  assert.equal(h.paints[0].receipt.effects[0].targetId,p.id);
});

test('invalid drop, wrong side, cancel and Escape leave both card and combat unchanged', () => {
  for(const ending of ['outside','wrong-side','cancel','escape']){
    const h=appHarness(),before=JSON.stringify({run:h.game.run,battle:h.game.battle});
    h.pointer('pointerdown',{skillId:'cut',x:100,y:700});
    h.pointer('pointermove',{x:750,y:250,hit:{arena:true,unitId:'enemy-1'}});
    if(ending==='escape'){
      h.key('Escape');h.pointer('pointerup',{hit:{arena:true,unitId:'enemy-1'}});
      h.click('skill',{skill:'cut'},1); // Captured native click after cancellation must not re-arm.
    }else{
      const hit=ending==='outside'?{arena:false}:ending==='wrong-side'?{arena:true,unitId:h.game.battle.active}:{arena:true,unitId:'enemy-1'};
      h.pointer('pointerup',{hit});
      if(ending==='cancel')h.click('cancel-card');
    }
    h.click('confirm-card');
    assert.equal(h.paints.length,0);assert.equal(h.element('cardGhost').hidden,true);
    assert.equal(JSON.stringify({run:h.game.run,battle:h.game.battle}),before);
  }
});

test('all-target card previews both sides and does not borrow ordinary target selection', () => {
  const h=appHarness(),before=JSON.stringify(h.game.run);
  h.pointer('pointerdown',{skillId:'rain',x:100,y:700});
  h.pointer('pointermove',{x:500,y:250,hit:{arena:true}});
  h.pointer('pointerup',{hit:{arena:true}});
  assert.equal(JSON.stringify(h.game.run),before);
  const panel=h.element('cardAimPanel').innerHTML;
  assert.match(panel,/我方 · 希律的舞女/);assert.match(panel,/敌方 · 关隘守卫 1/);assert.match(panel,/敌方 · 关隘守卫 2/);
  h.click('confirm-card');assert.equal(h.paints.length,1);
  assert.equal(new Set(h.paints[0].receipt.effects.map(e=>e.targetId)).size,3);
  assert.ok(h.paints[0].receipt.effects.every(e=>e.sourceId===null));
});

test('click and numeric input arm, target and confirm; Tab/Space remain accessible during targeting', () => {
  const h=appHarness(),before=JSON.stringify(h.game.run);
  assert.equal(h.key('Tab'),true);
  h.key('1');
  assert.equal(h.paints.length,0);assert.equal(h.key('Tab'),false);
  assert.equal(h.key(' ',{closest:selector=>selector==='button'?{}:null}),false);
  h.click('target',{target:'enemy-1'});
  assert.equal(h.key('Tab'),false);
  assert.equal(JSON.stringify(h.game.run),before);
  h.click('confirm-card');assert.equal(h.paints.length,1);
  const all=appHarness();all.key('5');all.key('Enter',{id:'encounterArena'});
  assert.equal(all.paints.length,0);assert.match(all.element('cardAimPanel').innerHTML,/包含我方/);
  all.click('confirm-card');assert.equal(all.paints.length,1);
});

test('a changed target or lost resources at confirmation cannot spend a stale preview', () => {
  for(const failure of ['dead','resource']){
    const h=appHarness();h.click('skill',{skill:'cut'});h.click('target',{target:'enemy-1'});
    if(failure==='dead')h.game.battle.units.find(u=>u.id==='enemy-1').hp=0;
    else h.game.run.resources.flow=0;
    const before=JSON.stringify(h.game.run);h.click('confirm-card');
    assert.equal(h.paints.length,0);assert.equal(JSON.stringify(h.game.run),before);
    assert.ok(h.game.run.skills.available.includes('cut'));
  }
});

test('pointer cancellation and duplicate pointer-up never auto-confirm or duplicate a card', () => {
  const h=appHarness();h.pointer('pointerdown',{skillId:'cut'});
  h.pointer('pointermove',{x:700,y:250,hit:{arena:true,unitId:'enemy-1'}});
  h.pointer('pointercancel',{pointerId:2});
  assert.equal(h.element('cardGhost').hidden,false);
  h.pointer('pointerup',{hit:{arena:true,unitId:'enemy-1'}});
  h.pointer('pointerup',{hit:{arena:true,unitId:'enemy-0'}});
  assert.equal(h.paints.length,0);
  h.click('confirm-card');assert.equal(h.paints[0].receipt.effects[0].targetId,'enemy-1');
});

test('basic presentation has no C casting pose/trail and Rain appears from the sky for every target', () => {
  const game=combat(),receipt=game.dispatch('skill',{id:'rain'}).presentation;
  const html=F.render(receipt.before,{busy:true,receipt});
  assert.match(html,/基础卡展开 · 鱼骸骤雨/);assert.match(html,/当前出牌窗口/);
  assert.match(html,/card-cast-stage/);assert.doesNotMatch(F.actionText(receipt),/希律的舞女 ·/);
  const h=paintHarness(receipt),owner=h.units.find(u=>u.dataset.unitId===receipt.actionOwnerId);
  for(const step of F.plan(receipt).filter(s=>s.phase==='windup'||s.phase==='launch')){
    F.paint(h.container,step,h.view,receipt);
    assert.equal(owner.classList.contains('fx-casting'),false);
    if(step.phase==='launch'){
      const svg=h.nodes.get('#encounterTrails').innerHTML;
      assert.equal((svg.match(/fishbone-fall/g)||[]).length,step.effects.length*5);
      assert.doesNotMatch(svg,/fx-route|animateMotion/);
    }
  }
});

test('cancelled Encounter drag cannot suppress the first native ACT skill click', () => {
  const h=appHarness();
  h.pointer('pointerdown',{skillId:'cut',x:100,y:700});
  h.pointer('pointermove',{x:500,y:250});
  h.pointer('pointerup',{hit:{arena:false}});
  h.click('exit');h.reenter('act');
  assert.equal(h.game.battle.mode,'act');
  assert.equal(h.key('Tab'),true);
  assert.equal(h.game.battle.tab,true);
  const hp=h.game.battle.units.find(u=>u.id==='enemy-0').hp;
  h.click('skill',{skill:'cut'},1);
  assert.equal(h.game.battle.units.find(u=>u.id==='enemy-0').hp,hp-3);
  assert.equal(h.game.run.skills.available.includes('cut'),false);
});
