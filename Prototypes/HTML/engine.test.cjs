const test = require('node:test');
const assert = require('node:assert/strict');
const D = require('./data.js');
const { Game, faithBonus, clone } = require('./engine.js');

function command(g, type, payload) { const r = g.dispatch(type,payload); assert.equal(r.ok,true,r.text); return r; }
function ready(p = {}) { const g = new Game(p); command(g,'advance'); return g; }
function hand(g,def) { return g.run.cards.find(c=>c.zone==='hand'&&c.defId===def).id; }
function pair(g,a='dawn',b='cherry') { const id=hand(g,a);command(g,'deploy',{id,node:'home'});const bid=hand(g,b);command(g,'deploy',{id:bid,node:'home',anchor:id});return [id,bid]; }
function dancer(g) { const id=hand(g,'dancer');command(g,'deploy',{id,node:'home'});return id; }
function approach(g,id,boss=false) { for(const node of ['fire','plain',boss?'forge':'gate']) { if(node==='gate')break;command(g,'move',{ids:[id],node}); } }
function enter(g,mode='encounter',boss=false) { const id=dancer(g);approach(g,id,boss);command(g,'move',{ids:[id],node:boss?'boss':'gate',mode});return id; }
function completeDialogue(g,node) { command(g,'operation',{node});command(g,'dialogue-next');command(g,'dialogue-next',{choiceId:'accept'}); }

test('clear source values and fixed family attributes remain distinct from HTML numbers',()=>{
  assert.equal(D.cards.dancer.atk,10);assert.equal(D.cards.dancer.hp,45);assert.equal(D.cards.dancer.attackCost,6);
  for(const c of Object.values(D.cards))assert.equal(c.attribute,{A:'F',B:'D',C:'R'}[c.family]);
  assert.notEqual(D.actCharacters[0].id,D.cards.dancer.id);
});
test('invalid profiles are rejected without silently clamping',()=>{
  assert.throws(()=>new Game({flowStart:90,flowCap:80}),/上限/);
  assert.throws(()=>new Game({seed:1.5}),/整数/);
  assert.throws(()=>new Game({temperature:0}),/范围/);
});
test('ABC placement respects phase, ownership and atomic payment',()=>{
  const g=new Game({}),id=hand(g,'dawn'),before=JSON.stringify(g.run);
  assert.equal(g.dispatch('deploy',{id,node:'home'}).ok,false);assert.equal(JSON.stringify(g.run),before);
  command(g,'advance');const state=JSON.stringify(g.run);
  assert.equal(g.dispatch('deploy',{id,node:'boss'}).ok,false);assert.equal(JSON.stringify(g.run),state);
  command(g,'deploy',{id,node:'home'});assert.equal(g.run.resources.flow,55);
});
test('B tendencies accumulate once per daily Morning entry, never during preview',()=>{
  const g=ready(),[a,b]=pair(g);assert.deepEqual(g.card(b).field,[1,3]);
  g.preview(a);g.preview(a);assert.deepEqual(g.card(b).field,[1,3]);
  for(let i=0;i<3;i++)command(g,'advance');
  assert.deepEqual(g.card(b).field,[3,4]);const rng=g.run.rng;
  g.preview(a);g.preview(a);assert.equal(g.run.rng,rng);assert.deepEqual(g.card(b).field,[3,4]);
  for(let i=0;i<3;i++)command(g,'advance');assert.deepEqual(g.card(b).field,[5,5]);
});
test('calendar rolls day seven into an Era and Era seven into a Chapter',()=>{
  const g=new Game({});
  for(let i=0;i<7*3;i++)command(g,'advance');
  assert.deepEqual(g.run.calendar,{chapter:1,era:2,day:1,phase:0,absoluteDay:8});
  for(let i=0;i<42*3;i++)command(g,'advance');
  assert.deepEqual(g.run.calendar,{chapter:2,era:1,day:1,phase:0,absoluteDay:50});
});
test('daily economy respects caps and card passive starts only after deployment',()=>{
  const g=ready();pair(g);g.run.resources.flow=0;g.run.resources.calc=0;
  command(g,'advance');command(g,'advance');
  assert.equal(g.run.resources.flow,42);assert.equal(g.run.resources.calc,25);
  for(let i=0;i<3;i++)command(g,'advance');
  assert.equal(g.run.resources.flow,80);assert.equal(g.run.resources.calc,50);
});
test('preview is pure and seeded synthesis uses its exact probability fields',()=>{
  const a=ready(),b=ready(),[aa,ab]=pair(a),[ba]=pair(b);
  const before=JSON.stringify(a.run),preview=a.preview(aa);assert.equal(JSON.stringify(a.run),before);
  assert.ok(Math.abs(preview.candidates.reduce((n,c)=>n+c.probability,0)-1)<1e-12);
  const one=command(a,'synthesize',{id:aa}).result,two=command(b,'synthesize',{id:ba}).result;
  assert.deepEqual(one,two);assert.deepEqual(one.candidates,preview.candidates);assert.equal(one.success,true);
  assert.equal(a.card(aa).zone,'discard');assert.equal(a.card(ab).zone,'discard');
  assert.equal(a.card(one.createdId).zone,'world');assert.equal(a.card(one.createdId).node,'home');
  assert.equal(a.run.hand.includes(one.createdId),false);
});
test('synthesis failure consumes participating A/B and costs, grants no card or compensation',()=>{
  const g=ready({faith:0,entropy:100}),[a,b]=pair(g),count=g.run.cards.length,flow=g.run.resources.flow;
  const r=command(g,'synthesize',{id:a}).result;assert.equal(r.success,false);
  assert.equal(g.run.cards.length,count);assert.equal(g.card(a).zone,'discard');assert.equal(g.card(b).zone,'discard');
  assert.equal(g.run.resources.flow,flow-g.run.profile.synthesisFlow);
});
test('rejected synthesis consumes no material, money or random state',()=>{
  const g=ready(),[a]=pair(g);g.run.resources.flow=0;const before=JSON.stringify(g.run);
  assert.equal(g.dispatch('synthesize',{id:a}).ok,false);assert.equal(JSON.stringify(g.run),before);
});
test('faith modifier follows source boundary values, including the printed jump at 80',()=>{
  for(const [x,value] of [[0,-33],[20,-15],[40,-4],[80,4],[95,10],[100,15]])assert.equal(faithBonus(x),value);
  assert.ok(faithBonus(79.999)<3); // The printed 0.175 segment approaches 3, not 4.
});
test('choosing an attached B expands the complete A/B movement group',()=>{
  const g=ready(),[a,b]=pair(g),p=g.movePreview([b],'fire');assert.deepEqual(new Set(p.ids),new Set([a,b]));
  const before=g.run.resources.calc;command(g,'move',{ids:[b],node:'fire'});
  assert.equal(g.card(a).node,'fire');assert.equal(g.card(b).node,'fire');assert.equal(g.run.resources.calc,before-2);
});
test('a rejected member prevents movement and payment for the entire group',()=>{
  const g=ready(),[a,b]=pair(g);g.run.mobility.cherry.mode='immovable';const before=JSON.stringify(g.run);
  assert.equal(g.dispatch('move',{ids:[a],node:'fire'}).ok,false);assert.equal(JSON.stringify(g.run),before);
  assert.equal(g.card(b).node,'home');
});
test('conditional movement validates faith and terrain per card',()=>{
  const g=ready(),[a]=pair(g,'earth','reed');
  g.run.resources.faith=40;assert.equal(g.dispatch('move',{ids:[a],node:'fire'}).ok,false);
  g.run.resources.faith=60;command(g,'move',{ids:[a],node:'fire'});command(g,'move',{ids:[a],node:'plain'});
  const before=JSON.stringify(g.run);assert.equal(g.dispatch('move',{ids:[a],node:'gate',mode:'act'}).ok,false);assert.equal(JSON.stringify(g.run),before);
});
test('adjacency is not a path and locked story edges remain locked',()=>{
  const g=ready(),id=dancer(g);let before=JSON.stringify(g.run);
  assert.equal(g.dispatch('move',{ids:[id],node:'plain'}).ok,false);assert.equal(JSON.stringify(g.run),before);
  command(g,'move',{ids:[id],node:'story'});before=JSON.stringify(g.run);
  assert.equal(g.dispatch('move',{ids:[id],node:'forge'}).ok,false);assert.equal(JSON.stringify(g.run),before);
  completeDialogue(g,'story');command(g,'move',{ids:[id],node:'forge'});
  completeDialogue(g,'forge');assert.equal(g.run.nodes.forge.used,true);
  assert.equal(g.dispatch('operation',{node:'forge'}).ok,false);
});
test('inspection and movement preview mutate no gameplay state',()=>{
  const g=ready(),id=dancer(g),before=JSON.stringify(g.run);
  gameReadOnly(g,id);assert.equal(JSON.stringify(g.run),before);
  function gameReadOnly(game,card){game.movePreview([card],'fire');game.enemyDefinitions('boss');}
});
test('battle entry is allowed at Dusk while sandbox operations stay frozen',()=>{
  const g=ready(),id=dancer(g);approach(g,id);command(g,'advance');const time=clone(g.run.calendar);
  command(g,'move',{ids:[id],node:'gate',mode:'act'});
  assert.equal(g.dispatch('advance').ok,false);assert.equal(g.dispatch('deploy',{id:hand(g,'dawn'),node:'home'}).ok,false);
  for(let i=0;i<20;i++)g.tick(.05,{x:1});
  assert.deepEqual(g.run.calendar,time);
});
test('Encounter uses a real next-ready timeline with consecutive enemy actions',()=>{
  const g=ready();enter(g);const order=[];
  for(let i=0;i<8;i++){
    const u=g.battle.units.find(u=>u.id===g.battle.active);order.push(u.side);
    command(g,u.side==='player'?'guard':'enemy');
  }
  assert.ok(order.some((side,i)=>side==='ruler'&&order[i+1]==='ruler'));
});
test('Encounter allows one basic skill per eligible player action, independent of ordinary action',()=>{
  const g=ready();enter(g);command(g,'skill',{id:'wind',target:g.battle.active});
  const before=JSON.stringify(g.run);assert.equal(g.dispatch('skill',{id:'cut',target:'enemy-0'}).ok,false);assert.equal(JSON.stringify(g.run),before);
  command(g,'guard');assert.equal(g.dispatch('skill',{id:'cut',target:'enemy-0'}).ok,false);
});
test('basic cards are illegal in sandbox and ACT outside Tab',()=>{
  const g=ready();assert.equal(g.dispatch('skill',{id:'cut'}).ok,false);enter(g,'act');
  const before=JSON.stringify(g.run);assert.equal(g.dispatch('skill',{id:'cut'}).ok,false);assert.equal(JSON.stringify(g.run),before);
});
test('ACT consumes its independent roster and terrain disables a trait, not the character',()=>{
  const g=ready();const id=enter(g,'act'),p=g.battle.units.find(u=>u.side==='player');
  assert.equal(p.id,'act-traveller');assert.notEqual(p.id,id);assert.equal(p.atk,g.run.profile.actAtk);assert.equal(p.speed,g.run.profile.actSpeed);
  assert.equal(gameCardStillInSource(),true);
  function gameCardStillInSource(){return g.card(id).node==='plain';}
});
test('ACT Tab slows simulation to one tenth and retains velocity but rejects attack/move input',()=>{
  const g=ready();enter(g,'act');g.tick(.05,{x:1});
  const p=g.battle.units.find(u=>u.side==='player');command(g,'tab');
  const x=p.x,t=g.battle.elapsed;g.tick(.05,{x:-1,attack:true});
  assert.ok(Math.abs(g.battle.elapsed-t-.005)<1e-10);assert.ok(p.x>x);assert.ok(p.vx>0);assert.equal(p.attackTimer,0);
});
test('ACT can use multiple skills within Tab and full depletion recycles into stable shared slots',()=>{
  const g=ready();enter(g,'act');const slots=clone(g.run.skills.slots);command(g,'tab');
  for(const s of D.skills)command(g,'skill',{id:s.id});
  assert.deepEqual(g.run.skills.slots,slots);assert.equal(g.run.skills.available.length,5);assert.equal(g.run.skills.discard.length,0);assert.equal(g.run.skills.cycles,1);
  command(g,'skill',{id:'wind'});assert.equal(g.run.skills.available.length,4);
});
test('true damage stays 3 against C; basic physical damage applies C resistance before armor',()=>{
  const g=ready();enter(g);const caster=g.battle.units.find(u=>u.side==='player'),enemy=g.battle.units.find(u=>u.side==='ruler'),hp=enemy.hp;
  enemy.def=100;g.damage(caster,enemy,3,true,true);assert.equal(enemy.hp,hp-3);
  enemy.def=1;const before=enemy.hp;g.damage(caster,enemy,10,false,true);assert.equal(enemy.hp,before-4);
});
test('Encounter and ACT heal bindings differ without changing shared card identity',()=>{
  const a=ready();enter(a);const faith=a.run.resources.faith;command(a,'skill',{id:'light',target:a.battle.active});assert.equal(a.run.resources.faith,faith+1);
  const b=ready();enter(b,'act');const f=b.run.resources.faith;command(b,'tab');command(b,'skill',{id:'light'});assert.equal(b.run.resources.faith,f);
  assert.deepEqual(a.run.skills.slots,b.run.skills.slots);
});
test('rain hits both sides and guard/shield are consumed through the damage path',()=>{
  const g=ready();enter(g,'act');const p=g.battle.units.find(u=>u.side==='player');
  command(g,'tab');const hp=p.hp;command(g,'skill',{id:'rain'});assert.ok(p.hp<hp);
  command(g,'skill',{id:'mantra'});const e=g.battle.units.find(u=>u.side==='ruler'),now=p.hp;g.damage(e,p,20,true);assert.equal(p.hp,now);assert.equal(p.block,0);
});
test('both modes exit with exact complete pre-entry snapshots, including fees/RNG/skill discard/roster/log',()=>{
  for(const mode of ['encounter','act']){
    const g=ready(),id=dancer(g);approach(g,id);const before=JSON.stringify(g.run);
    command(g,'move',{ids:[id],node:'gate',mode});if(mode==='act')command(g,'tab');command(g,'skill',{id:'light',target:mode==='encounter'?g.battle.active:undefined});
    command(g,'exit');assert.equal(JSON.stringify(g.run),before);assert.equal(g.battle,null);
  }
});
test('actual ACT defeat restores complete snapshot rather than ending campaign',()=>{
  const g=ready({actHP:1,enemyAtk:30}),id=dancer(g);approach(g,id);const before=JSON.stringify(g.run);
  command(g,'move',{ids:[id],node:'gate',mode:'act'});
  for(let i=0;i<600&&g.battle;i++)g.tick(.05,{});
  assert.equal(g.battle,null);assert.equal(JSON.stringify(g.run),before);assert.equal(g.run.outcome,null);
});
test('actual Encounter defeat restores complete snapshot',()=>{
  const g=ready({enemyAtk:40}),id=dancer(g);g.card(id).hp=1;approach(g,id);const before=JSON.stringify(g.run);
  command(g,'move',{ids:[id],node:'gate',mode:'encounter'});command(g,'guard');command(g,'enemy');
  assert.equal(g.battle,null);assert.equal(JSON.stringify(g.run),before);
});
test('Encounter combat commits a ruler-core victory and resource payment',()=>{
  const g=ready(),id=enter(g,'encounter',true),before=g.run.resources.flow;
  for(let i=0;i<30&&g.battle;i++){const b=g.battle,u=b.units.find(u=>u.id===b.active);if(u.side==='player')command(g,'attack',{targets:b.units.filter(v=>v.side==='ruler'&&v.hp>0).map(v=>v.id)});else command(g,'enemy');}
  assert.equal(g.battle,null);assert.equal(g.run.outcome.kind,'core');assert.equal(g.card(id).node,'boss');assert.ok(g.run.resources.flow<before);assert.equal(g.run.battlesWon[0].mode,'encounter');
});
test('real-time actions can complete ACT without a hidden win command',()=>{
  const g=ready();enter(g,'act',true);
  for(let i=0;i<2000&&g.battle;i++)g.tick(.05,{attack:true});
  assert.equal(g.battle,null);assert.equal(g.run.outcome?.kind,'core');assert.equal(g.run.battlesWon[0]?.mode,'act');
});
test('war-balance victory follows captures independently of ruler-core HP',()=>{
  const g=ready({warGoal:20}),id=dancer(g);command(g,'move',{ids:[id],node:'fire'});command(g,'move',{ids:[id],node:'plain'});
  assert.equal(g.run.outcome.kind,'balance');assert.equal(g.run.core.ruler,g.run.profile.coreHP);
});
test('safe-boundary save/load retains the full run and rejects malformed/incompatible data',()=>{
  const g=ready(),id=dancer(g);command(g,'move',{ids:[id],node:'story'});completeDialogue(g,'story');
  const saved=g.save(),loaded=new Game();loaded.load(saved);assert.deepEqual(loaded.run,g.run);
  const before=JSON.stringify(loaded.run);assert.throws(()=>loaded.load('oops'));assert.equal(JSON.stringify(loaded.run),before);
  const corrupt=JSON.parse(saved);corrupt.run.hand.push('missing-id');assert.throws(()=>loaded.load(JSON.stringify(corrupt)),/分区/);
  const incompatible=JSON.parse(saved);incompatible.version=99;assert.throws(()=>loaded.load(JSON.stringify(incompatible)),/版本/);
});
test('battle cannot be saved and failed configuration commands are atomic',()=>{
  const g=ready();enter(g,'act');assert.throws(()=>g.save(),/安全边界/);const before=JSON.stringify(g.run);
  assert.equal(g.dispatch('settings',{character:'traveller',weapon:'spear'}).ok,false);assert.equal(JSON.stringify(g.run),before);
});
test('a source sandbox active ability uses command/payment and enforces its Era limit',()=>{
  const g=ready(),id=hand(g,'banquet');command(g,'deploy',{id,node:'home'});const flow=g.run.resources.flow;
  command(g,'ability',{id});assert.equal(g.run.resources.flow,flow-4);const before=JSON.stringify(g.run);
  assert.equal(g.dispatch('ability',{id,reverse:true}).ok,false);assert.equal(JSON.stringify(g.run),before);
});

test('synthesis creates C at the moved pair destination with one arrival and no second deployment fee',()=>{
  const g=ready(),[a,b]=pair(g);command(g,'move',{ids:[b],node:'fire'});
  const flow=g.run.resources.flow,calc=g.run.resources.calc,handBefore=clone(g.run.hand),count=g.run.cards.length;
  let arrivals=0;const onPlayed=g.onPlayed.bind(g);g.onPlayed=c=>{arrivals++;onPlayed(c);};
  const result=command(g,'synthesize',{id:a}).result,c=g.card(result.createdId);
  assert.equal(result.success,true);assert.equal(result.node,'fire');assert.equal(c.node,'fire');assert.equal(c.zone,'world');
  assert.equal(c.deployedDay,g.run.calendar.absoluteDay);assert.equal(arrivals,1);assert.equal(g.run.cards.length,count+1);
  assert.deepEqual(g.run.hand,handBefore);assert.equal(g.run.resources.flow,flow-g.run.profile.synthesisFlow);
  assert.equal(g.run.resources.calc,calc-g.run.profile.synthesisCalc);assert.equal(g.card(a).zone,'discard');assert.equal(g.card(b).zone,'discard');
  const before=JSON.stringify(g.run);assert.equal(g.dispatch('deploy',{id:c.id,node:'fire'}).ok,false);assert.equal(JSON.stringify(g.run),before);
  const loaded=new Game();loaded.load(g.save());assert.deepEqual(loaded.run,g.run);assert.equal(loaded.card(c.id).node,'fire');
});
test('failed synthesis after movement still consumes inputs with no world or hand reward',()=>{
  const g=ready({faith:0,entropy:100}),[a,b]=pair(g);command(g,'move',{ids:[a],node:'fire'});
  const before=clone(g.run),result=command(g,'synthesize',{id:a}).result;
  assert.equal(result.success,false);assert.equal(result.node,'fire');assert.equal(result.createdId,null);
  assert.equal(g.run.cards.length,before.cards.length);assert.deepEqual(g.run.hand,before.hand);
  assert.equal(g.run.resources.flow,before.resources.flow-g.run.profile.synthesisFlow);
  assert.equal(g.run.resources.calc,before.resources.calc-g.run.profile.synthesisCalc);
  assert.equal(g.run.resources.entropy,before.resources.entropy);assert.equal(g.card(a).zone,'discard');assert.equal(g.card(b).zone,'discard');
  assert.equal(g.run.cards.some(c=>c.zone==='world'&&c.node==='fire'),false);
});
test('world-card factory validates the node before allocating a stable ID',()=>{
  const g=ready(),before=JSON.stringify(g.run);
  assert.throws(()=>g.createCard('vitruvian','world','missing'),/节点/);assert.equal(JSON.stringify(g.run),before);
  assert.throws(()=>g.createCard('vitruvian','missing'),/分区/);assert.equal(JSON.stringify(g.run),before);
});
test('home protagonist display is independent of ACT, FPS, ABC and gameplay resources',()=>{
  const g=ready(),before=clone(g.run),display={character:'keeper-b',outfit:'formal',weapon:'sword',pose:'salute'};
  command(g,'home-display',display);assert.deepEqual(g.run.homeDisplay,display);
  for(const key of ['roster','fpsRoster','cards','resources','calendar','skills','mobility'])assert.deepEqual(g.run[key],before[key],key);
  assert.equal(g.run.log.length,before.log.length+1);
  const unchanged=JSON.stringify(g.run);command(g,'home-display',display);assert.equal(JSON.stringify(g.run),unchanged);
  command(g,'settings',{character:'scout',weapon:'spear',fpsRoster:{character:'fps-observer',weapon:'carbine'}});
  assert.deepEqual(g.run.homeDisplay,display);assert.deepEqual(g.run.cards,before.cards);
});
test('home display rejects cross-catalog identities and incompatible poses atomically',()=>{
  const g=ready(),base=clone(g.run.homeDisplay);
  for(const change of [{character:'traveller'},{character:'dancer'},{outfit:'missing'},{weapon:'blade'},{weapon:'spear',pose:'salute'},{pose:'missing'}]){
    const before=JSON.stringify(g.run);assert.equal(g.dispatch('home-display',{...base,...change}).ok,false);assert.equal(JSON.stringify(g.run),before);
  }
  assert.equal(g.dispatch('home-display',{character:'keeper-b'}).ok,false);
});
test('ACT uses the selected recruit name and palette while FPS remains a selection-only independent roster',()=>{
  const g=ready(),home=clone(g.run.homeDisplay);
  command(g,'settings',{fpsRoster:{character:'fps-observer',weapon:'carbine'}});
  assert.equal(g.run.roster.character,'traveller');command(g,'settings',{character:'scout'});
  assert.deepEqual(g.run.fpsRoster,{character:'fps-observer',weapon:'carbine'});
  const id=dancer(g);approach(g,id);const before=JSON.stringify(g.run);
  assert.equal(g.dispatch('move',{ids:[id],node:'gate',mode:'fps'}).ok,false);assert.equal(JSON.stringify(g.run),before);
  command(g,'move',{ids:[id],node:'gate',mode:'act'});
  const actor=g.battle.units.find(u=>u.side==='player'),def=D.actCharacters.find(c=>c.id==='scout');
  assert.equal(actor.id,'act-scout');assert.equal(actor.name,def.name);assert.equal(actor.palette,def.palette);
  assert.deepEqual(g.run.homeDisplay,home);command(g,'exit');assert.equal(JSON.stringify(g.run),before);
});
test('invalid FPS configuration rolls back all ACT and mobility changes in the same command',()=>{
  const g=ready();for(const fpsRoster of [{character:'keeper-a',weapon:'rifle'},{character:'fps-observer',weapon:'sword'},null]){
    const before=JSON.stringify(g.run);
    assert.equal(g.dispatch('settings',{character:'scout',weapon:'spear',fpsRoster,mobility:{dawn:{mode:'immovable',minFaith:50,forbiddenTerrain:''}}}).ok,false);
    assert.equal(JSON.stringify(g.run),before);
  }
});
test('new selections save and restore; version-one saves migrate only absent new fields',()=>{
  const g=ready();command(g,'home-display',{character:'keeper-b',outfit:'combat',weapon:'spear',pose:'ready'});
  command(g,'settings',{character:'scout',fpsRoster:{character:'fps-observer',weapon:'carbine'}});
  const save=g.save(),loaded=new Game();loaded.load(save);assert.deepEqual(loaded.run,g.run);
  const legacy=JSON.parse(save);delete legacy.run.homeDisplay;delete legacy.run.fpsRoster;loaded.load(JSON.stringify(legacy));
  assert.deepEqual(loaded.run.homeDisplay,{character:'keeper-a',outfit:'travel',weapon:'sword',pose:'idle'});
  assert.deepEqual(loaded.run.fpsRoster,{character:'fps-vanguard',weapon:'rifle'});assert.equal(loaded.run.roster.character,'scout');
  for(const [field,value] of [['homeDisplay',null],['homeDisplay',{character:'dancer',outfit:'travel',weapon:'sword',pose:'idle'}],['homeDisplay',{character:'keeper-a',outfit:'travel',weapon:'spear',pose:'salute'}],['fpsRoster',{}],['fpsRoster',{character:'traveller',weapon:'rifle'}]]){
    const corrupt=JSON.parse(save);corrupt.run[field]=value;const before=JSON.stringify(loaded.run);
    assert.throws(()=>loaded.load(JSON.stringify(corrupt)));assert.equal(JSON.stringify(loaded.run),before);
  }
});
test('watchman dialogue opens and branches without rewards, then cancellation preserves the exact run',()=>{
  const g=ready(),id=dancer(g);command(g,'move',{ids:[id],node:'story'});const before=JSON.stringify(g.run);
  command(g,'operation',{node:'story'});assert.deepEqual(g.dialogue,{nodeId:'story',lineId:'intro'});assert.equal(JSON.stringify(g.run),before);
  command(g,'dialogue-next');command(g,'dialogue-next',{choiceId:'ask'});assert.equal(g.dialogue.lineId,'road');assert.equal(JSON.stringify(g.run),before);
  command(g,'dialogue-next');command(g,'dialogue-next',{choiceId:'leave'});assert.equal(g.dialogue,null);assert.equal(JSON.stringify(g.run),before);
  command(g,'operation',{node:'story'});command(g,'dialogue-cancel');assert.equal(g.dialogue,null);assert.equal(JSON.stringify(g.run),before);
});
test('only the explicit completion choice grants story resources and road unlock, exactly once',()=>{
  const g=ready(),id=dancer(g);command(g,'move',{ids:[id],node:'story'});g.run.resources.calc=5;const before=JSON.stringify(g.run);
  command(g,'operation',{node:'story'});
  assert.equal(g.dispatch('dialogue-next',{choiceId:'accept'}).ok,false);assert.equal(g.dialogue.lineId,'intro');assert.equal(JSON.stringify(g.run),before);
  command(g,'dialogue-next');assert.equal(g.dispatch('dialogue-next').ok,false);assert.equal(JSON.stringify(g.run),before);
  command(g,'dialogue-next',{choiceId:'accept'});assert.equal(g.dialogue,null);assert.equal(g.run.resources.calc,5+g.run.profile.storyCalc);
  assert.equal(g.run.storyMet,true);assert.equal(g.run.nodes.story.used,true);assert.deepEqual(g.run.unlocks,['story-road']);
  const completed=JSON.stringify(g.run);assert.equal(g.dispatch('dialogue-next',{choiceId:'accept'}).ok,false);
  assert.equal(g.dispatch('operation',{node:'story'}).ok,false);assert.equal(JSON.stringify(g.run),completed);
  const loaded=new Game();loaded.load(g.save());assert.equal(loaded.dispatch('operation',{node:'story'}).ok,false);assert.deepEqual(loaded.run,g.run);
});
test('forge dialogue cancellation grants nothing; completing grants supply and consumes the event once',()=>{
  const g=ready(),id=dancer(g);approach(g,id,true);g.run.resources.flow=0;const before=JSON.stringify(g.run);
  command(g,'operation',{node:'forge'});command(g,'dialogue-next');command(g,'dialogue-next',{choiceId:'inspect'});
  command(g,'dialogue-cancel');assert.equal(JSON.stringify(g.run),before);assert.equal(g.run.nodes.forge.used,false);
  completeDialogue(g,'forge');assert.equal(g.run.nodes.forge.used,true);assert.equal(g.run.resources.flow,g.run.profile.forgeFlow);
  const complete=JSON.stringify(g.run);assert.equal(g.dispatch('operation',{node:'forge'}).ok,false);assert.equal(JSON.stringify(g.run),complete);
});
test('active dialogue rejects world changes, battle entry, save and load through the service',()=>{
  const g=ready(),id=dancer(g);command(g,'move',{ids:[id],node:'story'});const save=g.save();command(g,'operation',{node:'story'});
  const before=JSON.stringify({run:g.run,dialogue:g.dialogue,battle:g.battle,snapshot:g.snapshot});
  for(const [type,payload] of [['advance'],['move',{ids:[id],node:'plain'}],['move',{ids:[id],node:'boss',mode:'act'}],['operation',{node:'story'}],['settings',{character:'scout'}],['home-display',{character:'keeper-b',outfit:'formal',weapon:'sword',pose:'idle'}],['deploy',{id:hand(g,'earth'),node:'home'}]]){
    assert.equal(g.dispatch(type,payload).ok,false,type);assert.equal(JSON.stringify({run:g.run,dialogue:g.dialogue,battle:g.battle,snapshot:g.snapshot}),before);
  }
  assert.throws(()=>g.save(),/对话/);assert.throws(()=>g.load(save),/对话/);assert.throws(()=>g.start({}),/对话/);
  assert.throws(()=>g.startBattle('act',{target:'gate',ids:[id]}),/对话/);assert.throws(()=>g.advance(),/对话/);
  assert.equal(JSON.stringify({run:g.run,dialogue:g.dialogue,battle:g.battle,snapshot:g.snapshot}),before);
});
test('dialogue transaction restores its cursor and all run fields if reward application fails',()=>{
  const g=ready(),id=dancer(g);command(g,'move',{ids:[id],node:'story'});command(g,'operation',{node:'story'});command(g,'dialogue-next');
  const before=JSON.stringify({run:g.run,dialogue:g.dialogue}),gain=g.gain;g.gain=()=>{throw new Error('injected grant failure');};
  assert.equal(g.dispatch('dialogue-next',{choiceId:'accept'}).ok,false);assert.equal(JSON.stringify({run:g.run,dialogue:g.dialogue}),before);
  g.gain=gain;command(g,'dialogue-next',{choiceId:'accept'});assert.equal(g.dialogue,null);assert.equal(g.run.unlocks.filter(v=>v==='story-road').length,1);
});
