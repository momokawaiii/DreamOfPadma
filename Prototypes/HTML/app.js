(function () {
  'use strict';
  const D = window.PadmaData, E = window.PadmaEngine, H = window.PadmaHome, F = window.PadmaEncounterFX, I = window.PadmaCardInput, game = new E.Game();
  const $ = id => document.getElementById(id), esc = value => String(value ?? '').replace(/[&<>"']/g, c => ({'&':'&amp;','<':'&lt;','>':'&gt;','"':'&quot;',"'":'&#39;'}[c]));
  const STORE = 'padma-html-rule-prototype-v1';
  const ui = { node: 'home', selected: [], card: null, filter: 'all', mode: 'act', target: null, targets: [], ally: null, profile: { ...D.defaults }, settingsTab: 'act', outcomeSeen: false, attackPulse: 0, movePoint: null, saved: false, homeTab: null, homeDraft: null, modeDraft: null, otherMode: 'act', followerPose: 'idle', presentation: null, fxSpeed: 1, lastAction: '', selectionTurn: null, dropHover: null, suppressSkillClick: null, attackTargeting: false, attackCount: 1, inspectId: null, suppressUnitClick: null };
  const keys = new Set(); let toastTimer, enemyTimer, unitHold, lastFrame = 0, lastHud = 0;
  const playback = new F.Playback();
  const cardInput = new I.CardInput({
    getContext: () => ({ battle: game.battle, skills: D.skills, available: game.run?.skills.available || [], blocked: !!ui.presentation || $('dialog').open }),
    onChange: state => updateCardAim(state),
    onPlay: payload => dispatch('skill', payload),
    onNotice: text => toast(text, true)
  });
  const inBattle = () => !!(game.battle || ui.presentation);
  function cancelPlayback() { cancelUnitHold(false); cardInput.cancel(); ui.attackTargeting=false; ui.targets=[]; ui.suppressSkillClick=null; playback.cancel(); ui.presentation = null; clearTimeout(enemyTimer); keys.clear(); }
  const button = (action, text, extra = '') => '<button data-action="' + action + '" ' + extra + '>' + text + '</button>';
  const name = id => D.cards[id].name, num = v => Number(v).toFixed(1).replace(/\.0$/, '');
  function toast(text, error = false) { $('toast').textContent = text; $('toast').className = 'toast' + (error ? ' error' : ''); $('toast').hidden = false; clearTimeout(toastTimer); toastTimer = setTimeout(() => $('toast').hidden = true, error ? 6500 : 4200); }
  function dialog(title, body, foot = '') {
    $('dialog').classList.remove('story-modal','unit-detail-modal'); ui.inspectId=null;
    $('dialogContent').innerHTML = '<div class="dialog-head"><h2>' + esc(title) + '</h2>' + button('close','✕','aria-label="关闭弹窗"') + '</div><div class="dialog-body">' + body + '</div>' + (foot ? '<div class="dialog-foot">' + foot + '</div>' : '');
    if (!$('dialog').open) $('dialog').showModal();
    keys.clear(); ui.movePoint = null;
  }
  function close() {
    const wasUnitInfo=!!ui.inspectId; ui.inspectId=null;
    if (game.dialogue) {
      const result = game.dispatch('dialogue-cancel');
      if (!result.ok) { toast(result.text, true); return; }
      render();
    }
    $('dialog').close();
    if(wasUnitInfo) scheduleEnemy();
  }
  function showUnitDetails(id) {
    const b=ui.presentation?.view || game.battle;
    const unit=b?.mode==='encounter' && b.units.find(u=>u.id===id);
    if(!unit)return;
    clearTimeout(enemyTimer);
    dialog(unit.name+' · 角色详情',F.unitDetails(E.clone(unit),D.cards[unit.defId],{active:b.active,actionTime:b.actionTime,busy:!!ui.presentation}),button('close','返回战斗','class="primary"'));
    ui.inspectId=id; $('dialog').classList.add('unit-detail-modal');
  }
  function cancelUnitHold(resume=true) {
    if(!unitHold)return;
    clearTimeout(unitHold.timer);unitHold.element.classList.remove('inspecting-hold');unitHold=null;
    if(resume)scheduleEnemy();
  }
  function scheduleEnemy() {
    clearTimeout(enemyTimer);
    const b=game.battle;
    if(!b || b.mode!=='encounter' || ui.presentation || unitHold || (ui.inspectId && $('dialog').open) || b.units.find(u=>u.id===b.active)?.side!=='ruler')return;
    const expectedActor=b.active, expectedCount=b.actionCount;
    enemyTimer=setTimeout(()=>{
      if(!ui.presentation && !unitHold && !(ui.inspectId && $('dialog').open) && game.battle===b && b.active===expectedActor && b.actionCount===expectedCount)dispatch('enemy');
    },950/ui.fxSpeed);
  }
  function resetHomeDrafts() {
    ui.homeDraft = E.clone(game.run.homeDisplay);
    ui.modeDraft = { act: { character: game.run.roster.character, weapon: game.run.roster.weapon }, fps: E.clone(game.run.fpsRoster) };
  }
  function openHome(tab = 'protagonist') {
    if (!game.run || inBattle() || game.dialogue) return;
    ui.node = 'home'; ui.homeTab = tab; resetHomeDrafts(); render();
    $('homeScreen').scrollIntoView({ block: 'start' });
  }
  function inspectNode(id) {
    ui.node = id; ui.card = null; $('mapTip').hidden = true;
    // Keep the clicked SVG node alive so a native double-click can reach it.
    for (const node of $('map').querySelectorAll('[data-node]')) node.classList.toggle('selected', node.dataset.node === id);
    renderInspector(); renderHand();
  }
  function showDialogue() {
    if (!game.dialogue) return;
    const story = D.dialogues[game.dialogue.nodeId];
    dialog(story.title, H.dialogue(game), button('dialogue-leave', '暂时离开'));
    $('dialog').classList.add('story-modal');
  }
  function actName() { return game.run ? D.actCharacters.find(c => c.id === game.run.roster.character).name : ''; }
  function dispatch(type, payload) {
    if (ui.presentation && (type !== 'exit' || !game.battle)) return { ok: false, text: '请等待本次动作播放结束' };
    if (['attack','guard','enemy','skill','move','exit'].includes(type)) { cancelUnitHold(false); cardInput.cancel(); ui.attackTargeting=false; ui.targets=[]; }
    if (type === 'exit') cancelPlayback();
    const oldBattle = !!game.battle, beforeSequence = game.run?.sequence, result = game.dispatch(type, payload);
    if (!result.ok) { if (inBattle()) render(); toast(result.text, true); return result; }
    if (game.run.sequence !== beforeSequence) { ui.saved = false; $('saveStatus').textContent = '本局有未保存操作'; }
    if (type === 'move') {
      ui.card = null; ui.selected = [];
      if (!game.battle) ui.node = payload.node;
      else { cancelPlayback(); ui.lastAction = ''; ui.selectionTurn = null; ui.target = 'enemy-0'; ui.targets = []; ui.ally = null; keys.clear(); }
    }
    if (oldBattle && !game.battle) { ui.node = game.run.battlesWon.at(-1)?.node || ui.node; keys.clear(); ui.movePoint = null; }
    if (result.presentation) {
      clearTimeout(enemyTimer); keys.clear();
      playback.play(result.presentation, {
        speed: ui.fxSpeed,
        onStep(step, view, receipt) {
          ui.presentation = { view, receipt };
          if (step.phase === 'windup') render();
          F.paint($('battle'), step, view, receipt, { speed: ui.fxSpeed });
        },
        onDone(receipt) {
          ui.lastAction = F.actionText(receipt) + '；' + receipt.effects.map(e => (receipt.before.units.find(u => u.id === e.targetId)?.name || e.targetId) + '：' + F.effectText(e)).join('；');
          ui.presentation = null; render();
          if (receipt.ended) toast(game.notice, receipt.ended === 'defeat');
        },
        onError(error, receipt) {
          // The engine already committed this action. Only restore its view.
          ui.presentation = null;
          ui.lastAction = F.actionText(receipt) + '（动画中断，结果已结算）';
          console.error('Encounter presentation failed', error);
          render();
          toast('动画播放异常，已恢复操作；本次效果已结算，不会重复扣费。' + (receipt.ended ? game.notice : ''), true);
        }
      });
      return result;
    }
    render();
    if (!['tab','enemy','operation','dialogue-next'].includes(type)) toast(!game.battle && oldBattle ? game.notice : result.text);
    return result;
  }
  function resource(label, value, suffix = '', cls = '') { return '<div class="resource ' + cls + '"><small>' + label + '</small><strong>' + value + '</strong> <em>' + suffix + '</em></div>'; }
  function renderResources() {
    if (!game.run) { $('resources').innerHTML = ''; return; }
    const r = game.run, p = r.profile, t = r.calendar;
    $('resources').innerHTML = resource('第 ' + t.chapter + ' 章 · ' + D.eras[t.era-1] + '时代', '第 ' + t.day + ' 天 · ' + D.phases[t.phase], inBattle() ? '时间冻结' : '') +
      resource('心流',num(r.resources.flow),' / '+p.flowCap) + resource('计算力',num(r.resources.calc),' / '+p.calcCap) +
      resource('信仰',num(r.resources.faith)) + resource('全局熵',num(r.resources.entropy)) +
      resource('核心 · 我方 / 王庭',num(r.core.player) + ' / ' + num(r.core.ruler)) +
      '<div class="resource war-resource"><small>战局天平</small><strong>' + num(r.war) + '</strong><em> / ' + p.warGoal + '</em><div class="meter"><i style="width:' + Math.min(100,r.war/p.warGoal*100) + '%"></i></div></div>';
  }
  function nextStep() {
    const r = game.run;
    if (r.outcome) return r.outcome.text + '。可以保存本局记录，或修改参数再开一局。';
    if (r.battlesWon.length) return '战果已提交。继续向王庭归处移动，或占领支路地块把战局天平推至 ' + r.profile.warGoal + '。';
    if (r.synthesisHistory.some(s => s.success)) return '合成的 C 已在 A/B 所在节点直接登场。勾选它，再点目标地块 → 右侧“移动到此处”。归处可双击打开主角展示与编队。';
    const a = r.cards.find(c => c.zone === 'world' && D.cards[c.defId].family === 'A');
    if (!a) return '进入晌午 → 点击手牌“佛罗伦萨的曙光” → 在归处部署。随后选择“山樱一梦”附着。';
    const b = r.cards.find(c => c.attachedTo === a.id);
    if (!b) return '点击 B“山樱一梦”，在右侧选中已部署的 A 并附着。';
    if (!b.lastAccumulatedDay) return '推进到次日晌午，让 A 的倾向向 B 累加一次；再点击 A 下的“预览合成”。';
    return 'A 的倾向已累加，点击地块内 A 下方“预览合成”查看概率，再执行合成。';
  }
  function renderMap() {
    const r = game.run; let svg = '<defs><pattern id="hexes" width="100" height="86" patternUnits="userSpaceOnUse"><path d="M25 1 L75 1 L99 43 L75 85 L25 85 L1 43Z" fill="none" stroke="#d4ddc4" stroke-width="1"/></pattern><marker id="arrow" viewBox="0 0 10 10" refX="9" refY="5" markerWidth="6" markerHeight="6" orient="auto-start-reverse"><path d="M0 0 L10 5 L0 10" fill="#9ba98d"/></marker><filter id="shadow"><feDropShadow dx="0" dy="6" stdDeviation="8" flood-opacity=".08"/></filter></defs>' +
      '<rect width="1080" height="610" fill="url(#hexes)" opacity=".35"/><path d="M-60 480Q200 230 390 555T1180 500" fill="none" stroke="#bacfca" stroke-width="23" opacity=".5"/><path d="M-60 480Q200 230 390 555T1180 500" fill="none" stroke="#d0ddd5" stroke-width="12"/>' +
      '<text x="55" y="75" fill="#9da98d" font-family="Georgia" font-size="12" letter-spacing="5">THE FIRST DREAM</text><text x="985" y="85" fill="#a8b29b" font-size="38">✧</text>';
    for (const edge of D.edges) {
      const a = D.nodes.find(n=>n.id===edge[0]), b = D.nodes.find(n=>n.id===edge[1]), len = Math.hypot(b.x-a.x,b.y-a.y), x = (b.x-a.x)/len, y = (b.y-a.y)/len, open = !edge[2] || r.unlocks.includes(edge[2]);
      svg += '<path d="M'+(a.x+x*80)+' '+(a.y+y*48)+' L'+(b.x-x*90)+' '+(b.y-y*50)+'" stroke="'+(open?'#a1ae8f':'#bbbea9')+'" stroke-width="2" '+(!open?'stroke-dasharray="5 6"':'')+' marker-end="url(#arrow)" fill="none"/>';
      if (!open) svg += '<text x="'+(a.x+b.x)/2+'" y="'+((a.y+b.y)/2-8)+'" text-anchor="middle" fill="#899379" font-size="11">剧情解锁</text>';
    }
    for (const n of D.nodes) {
      const state = r.nodes[n.id], own = r.cards.filter(c=>c.zone==='world' && c.node===n.id), enemy = state.owner==='ruler' ? n.enemies || 0 : 0;
      const top = state.owner==='player'?'#c6d7ad':state.owner==='ruler'?'#d8c5ac':'#dce1c4', bottom = state.owner==='player'?'#92ab7b':state.owner==='ruler'?'#b5997b':'#b1b998', ink = state.owner==='ruler'?'#8c594a':'#546e50';
      svg += '<g class="map-node '+(ui.node===n.id?'selected':'')+'" data-node="'+n.id+'" tabindex="0" role="button" aria-label="'+esc(n.name+'，'+(state.owner==='ruler'?'王庭':state.owner==='player'?'己方':'中立')+'，卡牌 '+(own.length+enemy))+'" transform="translate('+n.x+' '+n.y+')">'+
        '<polygon points="-84,-18 -43,-52 43,-52 84,-18 84,1 43,47 -43,47 -84,1" fill="'+bottom+'" filter="url(#shadow)"/>'+
        '<polygon class="tile-top" points="-84,-18 -43,-52 43,-52 84,-18 43,28 -43,28" fill="'+top+'" stroke="#aab995" stroke-width="1.5"/>'+
        '<ellipse cx="0" cy="-10" rx="34" ry="10" fill="#72865d" opacity=".13"/><text y="-10" text-anchor="middle" font-size="42" fill="'+ink+'" font-family="Georgia">'+n.icon+'</text>'+
        '<text y="66" text-anchor="middle" font-size="16" font-weight="600" fill="#3d5443">'+n.name+'</text><text y="86" text-anchor="middle" font-size="10" fill="#859079">'+n.terrain+' · '+(enemy?'敌方 '+enemy+' 张':own.length?'己方 '+own.length+' 张':state.owner==='player'?'己方领地':'空置')+'</text>'+(n.id==='home'?'<text y="103" text-anchor="middle" font-size="10" fill="#587360">双击 · 归处编队</text>':'')+
        (ui.selected.some(id=>r.cards.some(c=>c.id===id && c.node===n.id))?'<circle cx="61" cy="-38" r="11" fill="#355e4b"/><text x="61" y="-34" text-anchor="middle" font-size="12" fill="#fff">✓</text>':'')+'</g>';
    }
    $('map').innerHTML = svg;
  }
  function coverage(d) { return '<span class="source-tag">'+esc(d.page)+' · 属性按 A→F / B→D / C→R 修订</span><p class="hint">'+esc(d.coverage)+'</p>'; }
  function renderInspector() {
    const r=game.run, n=D.nodes.find(n=>n.id===ui.node), state=r.nodes[n.id], own=r.cards.filter(c=>c.zone==='world' && c.node===n.id);
    let body='<div class="inspector-top"><span class="eyebrow">地块信息 · '+n.terrain+'</span><h2>'+n.name+'</h2><span class="badge '+(state.owner==='ruler'?'red':'')+'">'+({player:'己方领地',ruler:'王庭领地',neutral:'中立地块'}[state.owner])+'</span><span class="badge gold">敌情全部公开</span></div><div class="inspector-body"><p>'+n.description+'</p>'+button('scene','查看地块场景 ↗','class="text-button"')+'<h3 style="margin-top:16px">己方卡牌 · '+own.length+'</h3>';
    if (n.id === 'home') body += '<div class="home-entry">'+button('home-open','进入归处编队 ↗','class="primary"')+'<p class="hint">或双击地图上的归处。节点细节 / 主角展示 / 其他模式。</p></div>';
    if (!own.length) body+='<p class="hint">此地块暂无己方卡牌。</p>';
    for (const c of own) {
      const d=D.cards[c.defId], mob=r.mobility[c.defId], attached=!!c.attachedTo;
      body+='<div class="unit-row"><label><input type="checkbox" data-select="'+c.id+'" '+(ui.selected.includes(c.id)?'checked':'')+' aria-label="选择移动 '+esc(d.name)+'"> <strong>'+d.family+' · '+d.name+'</strong></label><small>'+ (attached?'↳ 附着 '+name(game.card(c.attachedTo).defId)+' · 场域 ['+c.field.join(', ')+']': d.family==='C'?'攻 '+num(c.atk)+' / 防 '+d.def+' / 生命 '+num(c.hp)+' / '+d.hp:'稳定性 '+d.stability+' · 倾向 ['+d.tendency.join(', ')+']')+'</small><small>移动：'+({movable:'可移动',immovable:'不可移动',conditional:'条件移动',unconfigured:'未配置'}[mob.mode])+' · '+c.id+'</small><div class="mini-actions">'+button('card-info','查看卡牌','data-id="'+c.id+'"');
      if (d.family==='A') body+=button('synth-preview','预览合成','data-id="'+c.id+'"');
      if (['vitruvian','banquet'].includes(c.defId)) body+=button('ability',c.defId==='vitruvian'?'完美肉体':'希律一诺','data-id="'+c.id+'"');
      body+='</div></div>';
    }
    if (state.owner==='ruler') {
      body+='<h3 style="margin-top:16px">敌方卡牌 · 全部公开</h3>';
      for(const enemy of game.enemyDefinitions(n.id)) body+='<div class="unit-row"><strong>C · '+enemy.name+'</strong><small>R · 攻 '+enemy.atk+' / 防 '+enemy.def+' / 生命 '+enemy.hp+'</small><small>技能：普攻。无附着卡、无隐藏援军。HTML 临时单位。</small></div>';
    }
    if (['story','forge'].includes(n.id)) body+=button('operation',state.used?'事件已完成':n.id==='story'?'与守望人交谈':'使用一次熔炉补给','data-node-id="'+n.id+'" '+(state.owner!=='player'||state.used?'disabled':''));
    if (ui.card) {
      const c=game.card(ui.card), d=D.cards[c.defId]; body+='<div class="card-detail"><h3>选中的手牌 · '+d.name+'</h3>'+coverage(d);
      if(c.zone==='hand') {
        if(d.family==='B') { const anchors=own.filter(c=>D.cards[c.defId].family==='A'); body+='<label class="hint">附着目标<select id="anchor">'+(anchors.length?anchors.map(a=>'<option value="'+a.id+'">'+name(a.defId)+' · '+a.id+'</option>').join(''):'<option value="">先部署一张 A</option>')+'</select></label>'; }
        body+=button('deploy',(d.family==='B'?'附着到所选 A':'部署到 '+n.name)+' · 心流 '+(d.family==='C'?r.profile.deployC:d.cost),'class="primary" '+(state.owner!=='player'?'disabled':''));
      } else body+='<span class="badge">'+(c.zone==='discard'?'此卡已在弃牌堆':'此卡已部署')+'</span>';
      body+='</div>';
    }
    body+='</div><div class="move-box"><h3>移动到此处</h3>';
    let preview, error='';
    try { preview=game.movePreview(ui.selected,n.id); } catch(e) { error=e.message; }
    if(preview) {
      for(const c of preview.checks) body+='<div class="check '+(!c.ok?'bad':'')+'">'+(c.ok?'✓ ':'× ')+c.name+' · '+(c.ok?'计算力 '+c.calc:c.reason)+'</div>';
      body+='<p class="hint">合计计算力 '+preview.calc+'。整组校验，任何一张不通过都不移动、不扣费。</p>';
      if(preview.enemy) body+='<div class="mode-choices">'+button('choose-mode','ACT 试玩','data-mode="act" class="'+(ui.mode==='act'?'primary':'')+'"')+button('choose-mode','Encounter 试玩','data-mode="encounter" class="'+(ui.mode==='encounter'?'primary':'')+'"')+'</div><p class="hint">本版手动选模式；正式路由待共同确定。</p>';
    } else body+='<p class="hint">'+esc(error)+'</p>';
    body+=button('move',preview&&preview.enemy?'移动并进入 '+(ui.mode==='act'?'ACT':'Encounter'):'移动到此处','class="primary" '+(!preview||!preview.checks.every(c=>c.ok)||r.outcome?'disabled':''));
    body+='</div>'; $('inspector').innerHTML=body;
  }
  function renderHand() {
    const r=game.run;
    $('handTabs').innerHTML=[['all','全部手牌'],['A','A · 根源'],['B','B · 媒介'],['C','C · 造物'],['discard','ABC 弃牌堆']].map(([id,label])=>button('filter',label,'data-filter="'+id+'" class="'+(ui.filter===id?'active':'')+'"')).join('');
    $('deckCount').textContent='手牌 '+r.hand.length+' · 牌堆 '+r.deck.length+' · ABC 弃牌 '+r.discard.length+' · 基础技能弃牌 '+r.skills.discard.length;
    const cards=r.cards.filter(c=>c.zone===(ui.filter==='discard'?'discard':'hand') && (['all','discard'].includes(ui.filter)||D.cards[c.defId].family===ui.filter));
    $('hand').innerHTML=cards.map(c=>{
      const d=D.cards[c.defId]; return '<button class="hand-card '+(ui.card===c.id?'chosen':'')+'" data-action="select-card" data-id="'+c.id+'"><div class="card-top"><span>'+d.family+' / '+d.attribute+'</span><span>心流 '+(d.family==='C'?r.profile.deployC:d.cost)+'</span></div><div class="card-icon">'+d.icon+'</div><h3>'+d.name+'</h3><p>'+ (d.family==='A'?'倾向 '+d.tendency.join(' / '):d.family==='B'?'场域 '+d.field.join(' / '):'攻 '+d.atk+' · 防 '+d.def+' · 生命 '+d.hp)+'</p><p>'+d.page+' · 部分效果已接入</p></button>';
    }).join('')||'<div class="empty">此分类暂无卡牌。</div>';
  }
  function render() {
    clearTimeout(enemyTimer); renderResources();
    for (const el of document.querySelectorAll('.masthead nav button, footer button')) {
      el.disabled = inBattle();
      el.title = inBattle() ? '请先结束或退出战斗，再使用沙盘菜单' : '';
    }
    $('welcome').hidden=!!game.run; $('world').hidden=!game.run||inBattle(); $('battle').hidden=!inBattle();
    $('homeScreen').hidden = !game.run || inBattle() || !ui.homeTab;
    if(!game.run) return;
    if(inBattle()) { renderBattle(); return; }
    if (ui.homeTab) {
      $('world').hidden = true;
      $('homeScreen').innerHTML = H.home(game.run, ui);
      if (game.dialogue) showDialogue();
      return;
    }
    if(ui.card && !game.run.cards.some(c=>c.id===ui.card)) ui.card=null;
    ui.selected=ui.selected.filter(id=>game.run.cards.some(c=>c.id===id&&c.zone==='world'));
    $('nextStep').textContent=nextStep(); $('advance').textContent='推进到'+D.phases[(game.run.calendar.phase+1)%3]+' →'; $('advance').disabled=!!game.run.outcome;
    renderMap(); renderInspector(); renderHand();
    $('selectionBar').innerHTML=ui.selected.length?'<span><strong>已选择 '+game.group(ui.selected).length+' 张卡</strong> · '+game.group(ui.selected).map(c=>name(c.defId)).join('、')+'</span>'+button('clear-selection','清除选择','class="text-button"'):'<span>先在右侧勾选卡牌，再点击目的地。选择附着 B 会自动带上 A 与全部附着 B。</span>';
    $('recentLog').innerHTML=game.run.log.slice(-3).reverse().map(l=>'<p class="'+esc(l.kind)+'">'+esc(l.text)+'</p>').join('');
    if(game.run.outcome&&!ui.outcomeSeen) { ui.outcomeSeen=true; showOutcome(); }
    if (game.dialogue) showDialogue();
  }
  function showOutcome() {
    const r=game.run; dialog('本局完成','<div class="outcome"><div class="symbol">✧</div><h2>'+r.outcome.text+'</h2><p>第 '+r.calendar.day+' 天 · '+D.eras[r.calendar.era-1]+'时代<br>合成 '+r.synthesisHistory.length+' 次 · 战斗胜利 '+r.battlesWon.length+' 次 · 天平 '+r.war+'</p></div><div class="notice">这是一轮真实命令结算。可以保存本局、检查日志，或换一种模式与参数再玩一次。尚未接入的 PDF 技能不算已验收。</div>',button('close','查看沙盘')+button('save','保存本局','class="primary"'));
  }
  function showProfile() {
    dialog('HTML 试玩参数','<div class="notice warn">仅用于 HTML，所有字段可调整；不会导入 UE。PDF 已明确的卡牌面板与费用放在图鉴中，不由此表覆盖。修改下列数值会在“按此配置开始新局”时生效。</div><div class="profile-grid">'+D.profileFields.map(([key,label,,min,max])=>'<label>'+label+'<input type="number" data-profile="'+key+'" value="'+ui.profile[key]+'" min="'+min+'" max="'+max+'" step="'+(['temperature','dailyFraction','actCooldown','enemyInterval'].includes(key)?'0.01':'1')+'"></label>').join('')+'</div><p class="hint">数值源：mygame 的每日至少 50% 恢复、移动参考值、合成计算力 3 已作为起点，其余为 HTML 测试值。无债务、无被动倒计时；由“推进阶段”驱动沙盘。</p>',button('defaults','恢复临时默认值')+button('start-profile','按此配置开始新局','class="primary"'));
  }
  function start(useProfile) {
    if (ui.presentation) return;
    cancelPlayback(); ui.lastAction = '';
    if(useProfile) for(const el of document.querySelectorAll('[data-profile]')) ui.profile[el.dataset.profile]=Number(el.value);
    try { game.start(ui.profile); } catch(e) { toast(e.message,true); return; }
    Object.assign(ui,{node:'home',selected:[],card:null,filter:'all',outcomeSeen:false,saved:false,target:null,targets:[],movePoint:null,homeTab:null,homeDraft:null,modeDraft:null});
    keys.clear(); close(); $('saveStatus').textContent='新局尚未保存'; render(); toast('进入晌午后，可以部署手牌。');
  }
  function showHelp() {
    dialog('第一局怎么走','<div class="notice">入口就是当前页面。双击仓库 Prototypes/HTML/index.html 也能离线打开，无需 UE 或服务器。</div><ol><li><strong>开局经营：</strong>点击“推进到晌午”。点手牌 A“佛罗伦萨的曙光”，在右侧部署到归处；点 B“山樱一梦”，附着到这个 A。</li><li><strong>合成：</strong>推进到黄昏 → 次日黎明 → 晌午。A 下点“预览合成”，检查场域、候选和概率，再执行。A、B 成败都弃置；成功的 C 直接在 A/B 当前所在节点登场，不进手牌、不再扣部署费用。</li><li><strong>部署与移动：</strong>可先把 A/B 整组移动到薪火台再合成，检查 C 是否直接出现在薪火台。也可部署手牌“希律的舞女”，勾选其移动框。点击薪火台，右侧点“移动到此处”。重复走到风蚀原。</li><li><strong>地块经营：</strong>双击归处可进入节点细节／主角展示／其他模式，主角与 ACT/FPS 手下独立。旧日熔炉与守望人先进入对话，完成最终选项再发放一次补给或解锁道路。另一条路可以去关隘；驿站的事件可解锁直达熔炉道路。</li><li><strong>ACT：</strong>保持默认 ACT 入口，移动入战。WASD／方向键或点击场地移动，空格／“普攻”攻击近处敌人。Tab 打开技能库，1–5 打出技能；再次 Tab 关闭。敌人红圈蓄力时可以走开。</li><li><strong>Encounter：</strong>换一局或下场选择 Encounter。顶部标出行动者；先点击敌人选择普攻目标（舞女最多两个）。基础卡从底部拖到目标，松手预览，再确认释放；“雨”拖到战场并确认，会影响双方。每个我方行动最多一张基础卡，随后普攻或防御。观察攻击轨迹、受击与数字，动画结束后继续；右上可切换 1× / 2× 播放速度。</li><li><strong>胜利与验证：</strong>攻克王庭，核心归零即胜；分兵占领其他地块可达天平胜利。战斗中“退出并回滚”会退回入战前，包括移动费用。</li></ol><p>保存／继续位于顶栏；保存发生在沙盘。基础技能弃牌与 ABC 弃牌分开，基础技能耗尽后洗回原共享槽位。鼠标悬浮地块可预览信息，点击只查看。</p>',button('close','开始操作','class="primary"'));
  }
  function showCoverage() {
    dialog('这版已经接入什么','<div class="notice">用途：先用 HTML 对齐规则，再制作 UE。此版本是可玩的精选规则切片，尚未实现整份 PDF 的全部技能与系统；UE 原生 GAS 不在浏览器中运行。</div><h3>已接入</h3><p>三阶段与七日／七时代日历、心流／计算力恢复、ABC 部署、A/B 场域与合成、失败无奖励、预设道路与移动条件、整组原子移动、全敌情、地块事件、独立 ACT 阵容、Encounter 行动条、五张共享技能、ACT 子弹时间、战果提交／完整回滚、两种胜利、沙盘保存／继续。</p><h3>当前试玩约定，供对齐</h3><p>开局牌组、七节点地图、守卫、ACT 旅人／武器与通用防御都是 HTML 夹具。Encounter 使用速度倒数推进的行动条；不支持中断与反应窗口。同归于尽先判局部失败。Tab 再次按下关闭，本版恰好一页，滚轮仍停留第 1 页。以上均不冻结正式 UE 规则。</p><h3>仍未接入</h3><p>完整 72 张卡牌及专属技能、B 直接使用／多 B 合成、ABC 空牌堆回收、债务、时代强化与高阶克制、全局熵灾祸、统治者巡逻／威胁／增援、好感 Buff、更多整局失败条件、正式双边战斗路由、ACT 技能表导入、正式模型／动画。FPS 已有独立选角预览，但不开放战斗。</p><h3>卡牌接入清单</h3>'+Object.values(D.cards).map(d=>'<div class="coverage-card"><strong>'+d.family+' · '+d.name+'</strong>'+coverage(d)+'</div>').join(''),button('close','已了解','class="primary"'));
  }
  function showSynthesis(id) {
    try {
      const preview=game.preview(id);
      const rows=preview.candidates.map(c=>'<tr><td>'+name(c.id)+'</td><td>'+num(c.probability*100)+'%</td><td>'+c.distance+'</td><td>'+num(c.affinity)+'</td><td>'+num(c.entropy)+'</td><td>'+num(c.faith)+'</td><td><strong>'+num(c.success*100)+'%</strong></td></tr>').join('');
      dialog('合成预览','<div class="synthesis-grid"><div class="notice"><strong>'+name(game.card(preview.a).defId)+' ＋ '+name(game.card(preview.b).defId)+'</strong><br>B 当前场域：['+preview.field.join(', ')+']<br>费用：心流 '+preview.flow+' / 计算力 '+preview.calc+'</div><div class="formula">候选：标签点积 → Softmax<br>P = (100 − 距离 + 相性<br>　　+ 熵修正 + 信仰修正) / 100<br>HTML 夹具将 P 限定在 0–100%</div></div><table><thead><tr><th>候选 C</th><th>选中概率</th><th>距离</th><th>相性</th><th>熵</th><th>信仰</th><th>成功率</th></tr></thead><tbody>'+rows+'</tbody></table><p class="hint">先抽候选，再判成功。预览不消耗资源或随机数。合成参与的 A、B 无论成败都进入 ABC 弃牌堆，失败无奖励且不退费用。场域只在每天进入晌午累加一次。</p>',button('close','再等一天')+button('synthesize','执行合成','class="primary" data-id="'+id+'"'));
    } catch(e) { toast(e.message,true); }
  }
  function scene() {
    const n=D.nodes.find(n=>n.id===ui.node), cards=game.run.cards.filter(c=>c.zone==='world'&&c.node===n.id), enemies=game.run.nodes[n.id].owner==='ruler'?game.enemyDefinitions(n.id):[];
    const figures=[...(n.id==='home'?[{name:D.homeCharacters.find(c=>c.id===game.run.homeDisplay.character).name+'（坐镇）',building:false}]:[]),...cards.map(c=>({name:name(c.defId),building:D.cards[c.defId].family==='A'||D.cards[c.defId].kind==='building'})),...enemies.map(c=>({name:c.name,building:false}))];
    let art='<svg viewBox="0 0 800 240" aria-label="地块占位场景"><path d="M0 150L400 75L800 150L400 260Z" fill="#a9bf98"/><path d="M0 170L400 90L800 170" fill="none" stroke="#91ab7d"/><path d="M350 225L450 115" stroke="#d6c9a6" stroke-width="30"/>';
    figures.forEach((f,i)=>{const x=170+i*140;art+=f.building?'<g transform="translate('+x+' 92)"><path d="M-35 22L0 0L35 22V76H-35Z" fill="#f4ecdb" stroke="#a19f83"/><path d="M-46 22L0-12L46 22Z" fill="#74896b"/><path d="M-10 76V45H10V76" fill="#889778"/></g>':'<g transform="translate('+x+' 128)"><ellipse cy="54" rx="24" ry="7" fill="#779a6a" opacity=".5"/><path d="M0 0L-19 49H19Z" fill="#68866b"/><circle cy="-6" r="10" fill="#e4c6a1"/><path d="M-9 12L-26 32M9 12L27 30" stroke="#526a54" stroke-width="6"/></g>';art+='<text x="'+x+'" y="209" text-anchor="middle" fill="#334c38" font-size="11">'+esc(f.name)+'</text>';}); art+='</svg>';
    dialog(n.name+' · 地块场景','<div class="scene-preview">'+art+'<div class="scene-labels">'+n.terrain+' · '+figures.length+' 个可见占位实体</div></div><p class="hint">人物、建筑根据本局地块中的卡牌实例显示。这里是 HTML 占位场景；模型与技能表软引用的 UE Data Asset 已有前期候选成果，待 HTML 规则对齐后继续。</p>'+(!figures.length?'<p>此地块目前没有人物或建筑卡。</p>':''),button('close','返回地块信息','class="primary"'));
  }
  function showSettings(tab) {
    if(!game.run) { showProfile(); return; }
    ui.settingsTab=tab||ui.settingsTab;
    const r=game.run, tabs=[['act','ACT 阵容'],['skills','共享技能'],['mobility','移动规则'],['story','好感 / Buff'],['fps','FPS'],['codex','卡牌图鉴']];
    let body='<div class="tab-list">'+tabs.map(([id,label])=>button('settings-tab',label,'data-tab="'+id+'" class="'+(ui.settingsTab===id?'active':'')+'"')).join('')+'</div>';
    if(ui.settingsTab==='act') {
      body+='<div class="notice">ACT 角色来自战斗总设置，与沙盘的 ABC 卡牌独立。HTML 先用“试玩旅人”，模型和技能表引用待用户设计。归处主角使用独立身份，可通过双击归处打开专用展示。</div><h3>出战角色</h3><select id="actCharacter">'+D.actCharacters.map(c=>'<option value="'+c.id+'" '+(r.roster.character===c.id?'selected':'')+'>'+c.name+'</option>').join('')+'</select><p>'+D.actCharacters.find(c=>c.id===r.roster.character).description+'</p><h3>武器</h3><select id="actWeapon">'+D.weapons.map(w=>'<option value="'+w.id+'" '+(r.roster.weapon===w.id?'selected':'')+'>'+w.name+'</option>').join('')+'</select><p class="hint">练习剑：标准攻击与距离。练习长枪：攻击 ×0.85、距离 ×1.5、间隔 ×1.25。均为 HTML 临时设计。当前生命 '+num(r.roster.hp)+' / '+r.profile.actHP+'；每日恢复 1。</p>';
    } else if(ui.settingsTab==='skills') {
      body+='<p>两种模式共用以下身份与槽位，分别执行效果。打出后进独立基础技能弃牌堆，耗尽后洗回；不会占用 ABC 弃牌堆。</p><table><thead><tr><th>槽位 / 来源</th><th>Encounter 配置</th><th>ACT 配置</th></tr></thead><tbody>'+D.skills.map((s,i)=>'<tr><td>'+(i+1)+' · '+s.name+'<br><small>'+s.page+'<br>心流 '+s.flow+' / 计算力 '+s.calc+'<br>'+s.costSource+'</small></td><td>'+s.encounter+'</td><td>'+s.act+'</td></tr>').join('')+'</tbody></table><p class="hint">真伤不受克制／防御影响；基础卡对 C 的物理伤害先乘 0.5，再扣防御。本版“归墟”吸收伤害，“抗衡”消耗一层格挡一次。形意未接入。</p>';
    } else if(ui.settingsTab==='mobility') {
      body+='<div class="notice warn">逐张配置可移动／不可移动／条件移动。当前各卡的移动方式是可编辑的 HTML 夹具；不是对卡集正式规则的代定。选择 B 会自动携带 A 与全部附着 B，逐张校验后整体扣费移动。</div><table class="mobility-table"><thead><tr><th>卡牌</th><th>移动方式</th><th>最低信仰</th><th>禁入地形</th></tr></thead><tbody>'+Object.values(D.cards).map(d=>{const m=r.mobility[d.id];return '<tr><td>'+d.family+' · '+d.name+'</td><td><select data-mobility="'+d.id+'">'+[['movable','可移动'],['immovable','不可移动'],['conditional','条件移动'],['unconfigured','未配置']].map(([v,t])=>'<option value="'+v+'" '+(m.mode===v?'selected':'')+'>'+t+'</option>').join('')+'</select></td><td><input type="number" min="0" max="100" data-faith="'+d.id+'" value="'+m.minFaith+'"></td><td><select data-terrain="'+d.id+'">'+['','山地','平原','城镇','林地','丘陵'].map(t=>'<option value="'+t+'" '+(m.forbiddenTerrain===t?'selected':'')+'>'+(t||'无')+'</option>').join('')+'</select></td></tr>';}).join('')+'</tbody></table>';
    } else if(ui.settingsTab==='story') body+='<h3>守望人</h3><p>'+(r.storyMet?'已在渡鸦驿站交谈，援助与道路解锁已写入本局。':'尚未在渡鸦驿站交谈。')+'</p><div class="notice">好感阈值、对应角色、影响的卡牌类型、Buff 数值与叠加方式等待共同设计。本版不自动发放好感 Buff，也不把好感当作合成相性。</div><h3>已生效的经营效果</h3><p>'+ (r.nodes.fire.owner==='player'?'薪火台：每日心流 +'+r.profile.fireIncome:'尚未占领薪火台。')+'</p>';
    else if(ui.settingsTab==='fps') body+='<h3>FPS · 后续模式</h3><p>首版不提供 FPS 出战。未来角色、武器、技能可在此扩展；当前可玩的实时模式是 ACT。</p>';
    else body+=Object.values(D.cards).map(d=>'<div class="coverage-card"><strong>'+d.family+' · '+d.name+'</strong><p>'+Object.entries(d.tags).map(([k,v])=>k+' '+v).join(' / ')+'</p>'+coverage(d)+'</div>').join('');
    dialog('全局配置',body,button('close','关闭')+(['act','mobility'].includes(ui.settingsTab)?button('apply-settings','应用到当前局','class="primary" '+(game.battle?'disabled':'')):''));
  }
  function applySettings() {
    const input={character:$('actCharacter')?.value||game.run.roster.character,weapon:$('actWeapon')?.value||game.run.roster.weapon,mobility:{}};
    for(const el of document.querySelectorAll('[data-mobility]')) { const id=el.dataset.mobility; input.mobility[id]={mode:el.value,minFaith:Number(document.querySelector('[data-faith="'+id+'"]').value),forbiddenTerrain:document.querySelector('[data-terrain="'+id+'"]').value}; }
    if(dispatch('settings',input).ok) { if (ui.homeTab) { resetHomeDrafts(); render(); } close(); }
  }
  function cardHit(element) {
    const arena = element?.closest?.('#encounterArena');
    return { arena: !!arena, unitId: arena ? element.closest('[data-unit-id]')?.dataset.unitId : undefined };
  }
  function updateCardAim(state = cardInput.state) {
    const ghost=$('cardGhost'), skill=D.skills.find(s=>s.id===state.skillId), b=game.battle;
    if (ghost) {
      ghost.hidden=!state.dragging;
      if(state.dragging && skill) {
        ghost.style.left=state.x+'px'; ghost.style.top=state.y+'px';
        ghost.innerHTML='<span>'+esc(skill.icon)+'</span><strong>'+esc(skill.name)+'</strong><small>松手预览 · 再确认释放</small>';
      }
    }
    const arena=$('encounterArena'), panel=$('cardAimPanel');
    if(!arena || !b || b.mode!=='encounter') return;
    arena.classList.toggle('card-aiming',!!skill);
    arena.classList.toggle('aim-all',!!skill && skill.target==='all');
    for(const el of $('battle').querySelectorAll('[data-unit-id]')) {
      const unit=b.units.find(u=>u.id===el.dataset.unitId);
      const legal=!!skill && unit?.hp>0 && !unit.core && I.legalTarget(b,skill,{arena:true,unitId:el.dataset.unitId});
      el.classList.toggle('drop-legal',legal);
      el.classList.toggle('drop-picked',!!state.awaitingConfirm && legal && (skill.target==='all' || state.targetId===el.dataset.unitId));
      el.classList.toggle('drop-hover',legal && ui.dropHover===el.dataset.unitId);
    }
    for(const el of $('battle').querySelectorAll('[data-skill]')) {
      el.classList.toggle('card-selected',el.dataset.skill===state.skillId);
      el.setAttribute('aria-pressed',String(el.dataset.skill===state.skillId));
    }
    if(!panel) return;
    panel.hidden=!skill;
    if(!skill) { panel.innerHTML=''; return; }
    const targets=state.awaitingConfirm ? b.units.filter(u=>u.hp>0 && !u.core && I.legalTarget(b,skill,{arena:true,unitId:u.id}) && (skill.target==='all'||u.id===state.targetId)).map(u=>(u.side==='player'?'我方 · ':'敌方 · ')+u.name) : [];
    const help=state.awaitingConfirm ? '即将影响：'+targets.join('、') : skill.target==='all' ? '拖入战场，或点击战场空白处预览全体目标。' : '拖到'+(skill.target==='ally'?'我方':'敌方')+'单位，或点击该单位预览。';
    const html='<div><strong>'+esc(skill.name)+'</strong><span class="badge">心流 '+skill.flow+' · 计算力 '+skill.calc+'</span><p>'+esc(help)+'</p><small>'+(skill.target==='all'?'包含我方，请确认后释放。':'松手不会立即生效。')+'确认前不扣费；Esc 可取消。</small></div><div class="card-confirm-actions">'+button('confirm-card','确认释放','class="primary" '+(!state.awaitingConfirm?'disabled':''))+button('cancel-card','取消')+'</div>';
    if(panel.innerHTML!==html) panel.innerHTML=html;
  }
  function renderSkills() {
    const b=ui.presentation?.view || game.battle;
    return (b.mode==='encounter'?'<div class="hand-instruction">基础技能手牌 <span>拖到目标 → 松手预览 → 确认释放 · 也可按 1–5 选牌</span></div>':'')+'<div class="skills '+(b.mode==='encounter'?'encounter-hand':'')+'">'+D.skills.map((s,i)=>{
      const inDiscard=!game.run.skills.available.includes(s.id), turnLocked=b.mode==='encounter'&&(b.cardUsed||b.units.find(u=>u.id===b.active)?.side!=='player'), locked=!!ui.presentation||inDiscard||turnLocked;
      return '<button class="skill-card" draggable="false" aria-pressed="false" data-action="skill" data-skill="'+s.id+'" '+(locked?'disabled':'')+'><kbd>'+(i+1)+'</kbd><b>'+s.name+'</b><p>'+(b.mode==='act'?s.act:s.encounter)+'</p><span class="cost">'+(ui.presentation?'动作播放中':inDiscard?'在弃牌堆':turnLocked?'等待下一次我方行动':'心流 '+s.flow+' / 计算力 '+s.calc)+'</span></button>';
    }).join('')+'</div>';
  }
  function battleHeading(b) { return '<div class="battle-heading"><div><span class="eyebrow">局部战斗 · 沙盘时间冻结</span><h2>'+ (b.mode==='act'?'ACT':'Encounter')+' / '+D.nodes.find(n=>n.id===b.movement.target).name+'</h2></div>'+button('exit','退出并回滚','class="danger" '+(ui.presentation?.receipt.ended?'disabled':''))+'</div>'; }
  function renderBattle() {
    clearTimeout(enemyTimer);
    const b=ui.presentation?.view || game.battle;
    if (!b) return;
    if(b.mode==='act') { renderAct(); return; }
    const active=b.units.find(u=>u.id===b.active), ours=active.side==='player', busy=!!ui.presentation;
    const turnKey=b.active+':'+b.actionCount;
    if(!busy && ui.selectionTurn!==turnKey) { ui.targets=[]; ui.ally=null; ui.attackTargeting=false; ui.attackCount=1; ui.selectionTurn=turnKey; }
    ui.targets=ui.targets.filter(id=>b.units.some(u=>u.id===id&&u.hp>0&&u.side==='ruler')).slice(0,ours?active.maxTargets:0);
    const locked=!ours||busy, targetNames=ui.targets.map(id=>b.units.find(u=>u.id===id).name).join('、');
    const hint=busy?'请等待动作结束；长按角色仍可查看详情。':ui.attackTargeting?'请选择高亮敌人，'+(ui.attackCount===1?'点击即打出。':'选满 '+ui.attackCount+' 个不同目标即打出。'):ours?(b.cardUsed?'基础技能已用，请普攻或防御。':'先点普攻再选敌人；也可先使用 1 张基础技能。'):'敌方将自动行动；长按角色可查看详情。';
    const maxCount=ours?Math.min(active.maxTargets,b.units.filter(u=>u.side==='ruler'&&u.hp>0).length):1;
    ui.attackCount=Math.min(ui.attackCount,Math.max(1,maxCount));
    const countPicker=maxCount>1?'<div class="attack-count" aria-label="普攻目标数量">'+Array.from({length:maxCount},(_,i)=>button('attack-count',(i+1)+' 个目标','data-count="'+(i+1)+'" aria-pressed="'+(ui.attackCount===i+1)+'" '+(locked?'disabled':''))).join('')+'</div>':'';
    const attackPanel=ui.attackTargeting?'<div class="attack-aim-panel" role="status" aria-live="polite"><div><span class="eyebrow">普通攻击 · 选择作用单位</span><strong>'+esc(active.name)+' → '+esc(targetNames||'等待选择敌人')+'</strong><p>'+esc(hint)+' 当前 '+ui.targets.length+' / '+ui.attackCount+' · 心流 '+active.attackCost+'</p><small>取消或按 Esc 不消耗资源。长按只查看详情，不会出手。</small></div>'+button('cancel-attack','取消释放')+'</div>':'';
    $('battle').innerHTML=battleHeading(b)+F.render(b,{targets:ui.targets,ally:ui.ally,busy,attackTargeting:ui.attackTargeting,receipt:ui.presentation?.receipt,speed:ui.fxSpeed,lastAction:ui.lastAction})+'<div class="battle-controls encounter-controls">'+button('attack',(ui.attackTargeting?'正在选择目标':'普通攻击')+(ours?' · 心流 '+active.attackCost:''),'class="primary" '+(locked||ui.attackTargeting?'disabled':''))+countPicker+button('guard','防御 · 归墟 +2',locked?'disabled':'')+button('tab','Tab · 查看技能库',busy?'disabled':'')+'<span class="hint">'+esc(hint)+'</span></div>'+attackPanel+'<div id="cardAimPanel" class="card-aim-panel" role="status" aria-live="polite" hidden></div>'+renderSkills()+'<div class="battle-log">'+b.log.slice(-5).map(esc).join('<br>')+'</div>';
    updateCardAim();
    scheduleEnemy();
  }
  function renderAct() {
    const b=game.battle, terrain=D.nodes.find(n=>n.id===b.movement.target).terrain;
    $('battle').innerHTML=battleHeading(b)+'<div class="act-layout"><div class="act-canvas-wrap '+(b.tab?'bullet':'')+'"><canvas id="actCanvas" width="800" height="480" tabindex="0" aria-label="ACT 战斗场地，点击移动，WASD 移动，空格普攻"></canvas>'+(b.tab?'<div class="bullet-label">技能库 · 世界速度 1/10<br><small style="color:#c6d8bb">移动 / 普攻输入暂停，惯性继续</small></div>':'')+'</div><aside class="act-info"><span class="eyebrow">来自战斗总设置</span><h3 style="margin-top:8px">'+actName()+'</h3><p id="actStats"></p><span class="badge">'+D.weapons.find(w=>w.id===game.run.roster.weapon).name+'</span><p>'+ (terrain==='山地'?'轻步：山地停用；角色仍正常出战。':'轻步：仅平原提供移速加成。')+'</p><h3>敌方目标</h3><div id="actTargets"></div><p class="hint">所有敌人、生命和蓄力均公开。普攻选择距离最近的敌人；一刀两断使用这里选中的目标。</p></aside></div><div class="act-bottom">'+button('act-attack','普攻 · 空格','class="primary" '+(b.tab?'disabled':''))+button('tab',b.tab?'关闭技能库 · Tab':'打开技能库 · Tab')+'<p>WASD / 方向键 / 点击场地移动 · 空格普攻 · Tab + 1–5 用牌<br>红圈蓄力将要攻击，及时离开范围。</p></div>'+(b.tab?renderSkills():'<div class="notice">ACT 出战角色独立于地图卡牌。战斗内按 Tab 查看同一套五个基础技能槽位。</div>')+'<div class="battle-log" id="actLog"></div>';
    drawAct(); updateActHud();
  }
  function updateActHud() {
    if(!game.battle||game.battle.mode!=='act'||!$('actStats'))return;
    const b=game.battle,p=b.units.find(u=>u.side==='player');
    $('actStats').innerHTML='生命 '+num(p.hp)+' / '+p.maxHP+'<br>归墟 '+num(p.shield)+' · 抗衡 '+p.block+'<br>战斗时间 '+b.elapsed.toFixed(1)+' 秒';
    $('actTargets').innerHTML=b.units.filter(u=>u.side==='ruler').map(u=>button('act-target',u.name+' · '+num(u.hp)+' / '+u.maxHP,'data-target="'+u.id+'" class="target-button '+(ui.target===u.id?'targeted':'')+'" '+(u.hp<=0?'disabled':''))).join('');
    $('actLog').innerHTML=b.log.slice(-3).map(esc).join('<br>');
  }
  function drawAct() {
    const canvas=$('actCanvas');if(!canvas||!game.battle)return;const ctx=canvas.getContext('2d'),b=game.battle;
    ctx.clearRect(0,0,800,480);ctx.fillStyle='#dfe5d3';ctx.fillRect(0,0,800,480);
    ctx.strokeStyle='#cbd5bc';ctx.lineWidth=1;
    for(let x=-200;x<1000;x+=55){ctx.beginPath();ctx.moveTo(x,0);ctx.lineTo(x+230,480);ctx.stroke();}
    for(let y=0;y<480;y+=40){ctx.beginPath();ctx.moveTo(0,y);ctx.lineTo(800,y);ctx.stroke();}
    ctx.fillStyle='#b7c8a3';ctx.fillRect(0,0,800,40);ctx.fillRect(0,440,800,40);
    for(const x of [40,115,650,740]){ctx.fillStyle='#799571';ctx.beginPath();ctx.moveTo(x,6);ctx.lineTo(x-21,43);ctx.lineTo(x+21,43);ctx.fill();}
    ctx.fillStyle='#7b8d71';ctx.font='11px Microsoft YaHei';ctx.fillText('ACT 占位场景 · '+D.nodes.find(n=>n.id===b.movement.target).terrain,20,464);
    for(const u of b.units){
      if(u.hp<=0){ctx.fillStyle='#9ba78f';ctx.font='22px Georgia';ctx.fillText('×',u.x-7,u.y);continue;}
      if(u.windup>0){ctx.strokeStyle='#ba735f';ctx.fillStyle='#ba735f22';ctx.beginPath();ctx.arc(u.x,u.y,68,0,Math.PI*2);ctx.fill();ctx.stroke();}
      if(u.side==='player'&&u.flash>0){ctx.strokeStyle='#b0934b';ctx.lineWidth=5;ctx.beginPath();ctx.arc(u.x,u.y,u.range,-Math.PI*.6,Math.PI*.6);ctx.stroke();ctx.lineWidth=1;}
      ctx.fillStyle='#5d70582b';ctx.beginPath();ctx.ellipse(u.x,u.y+17,21,9,0,0,Math.PI*2);ctx.fill();
      ctx.fillStyle=u.side==='player'?'#466e55':'#986951';ctx.beginPath();ctx.moveTo(u.x,u.y-15);ctx.lineTo(u.x-15,u.y+16);ctx.lineTo(u.x+15,u.y+16);ctx.closePath();ctx.fill();
      ctx.fillStyle='#dfc5a4';ctx.beginPath();ctx.arc(u.x,u.y-20,8,0,Math.PI*2);ctx.fill();
      if(u.shield>0||u.block>0){ctx.strokeStyle='#c6a959';ctx.lineWidth=2;ctx.beginPath();ctx.arc(u.x,u.y,25,0,Math.PI*2);ctx.stroke();ctx.lineWidth=1;}
      if(ui.target===u.id){ctx.strokeStyle='#ac8b48';ctx.beginPath();ctx.arc(u.x,u.y,30,0,Math.PI*2);ctx.stroke();}
      ctx.fillStyle='#b0bba5';ctx.fillRect(u.x-30,u.y-42,60,5);ctx.fillStyle=u.side==='player'?'#3d6950':'#9a5b49';ctx.fillRect(u.x-30,u.y-42,60*Math.max(0,u.hp/u.maxHP),5);
      ctx.fillStyle='#3e5340';ctx.font='11px Microsoft YaHei';ctx.textAlign='center';ctx.fillText(u.name,u.x,u.y+37);ctx.textAlign='left';
    }
    for(const e of b.effects){ctx.globalAlpha=Math.min(1,e.life*2);ctx.fillStyle=e.side==='player'?'#965849':'#365b3f';ctx.font='bold 17px Georgia';ctx.fillText(e.text,e.x+16,e.y-22);ctx.globalAlpha=1;}
    if(ui.movePoint&&!b.tab){ctx.strokeStyle='#678768';ctx.beginPath();ctx.arc(ui.movePoint.x,ui.movePoint.y,8,0,Math.PI*2);ctx.stroke();}
  }
  function playSkill(id) {
    if (ui.presentation) return;
    const s=D.skills.find(s=>s.id===id);if(!s)return;
    const b=game.battle;
    if(b?.mode==='encounter') { if(ui.attackTargeting){ui.attackTargeting=false;ui.targets=[];renderBattle();} cardInput.select(id); return; }
    let target=s.target==='ally'?(b?.mode==='act'?b.units.find(u=>u.side==='player')?.id:ui.ally||b?.active):(b?.mode==='act'?ui.target:ui.targets[0]);
    dispatch('skill',{id,target});
  }
  document.addEventListener('click',event=>{
    const el=event.target.closest('[data-action]');if(!el)return;
    const action=el.dataset.action;
    if(action==='target' && ui.suppressUnitClick===el.dataset.target){ui.suppressUnitClick=null;return;}
    if (el.disabled || (ui.presentation && action!=='close' && (action !== 'exit' || !game.battle))) return;
    if(action==='close')close();
    else if(action==='start')start(false);
    else if(action==='start-profile')start(true);
    else if(action==='defaults'){ui.profile={...D.defaults};showProfile();}
    else if(action==='profile')showProfile();
    else if(action==='menu'){dialog('开始一场新的梦','<p>新局会使用当前临时参数。若要保留正在玩的这一局，请先使用顶栏“保存”。</p>',button('profile','调整参数')+button('start','开始新局','class="primary"'));}
    else if(action==='help')showHelp();
    else if(action==='coverage')showCoverage();
    else if(action==='settings')showSettings();
    else if(action==='settings-tab')showSettings(el.dataset.tab);
    else if(action==='apply-settings')applySettings();
    else if(action==='save'){
      try{localStorage.setItem(STORE,game.save());ui.saved=true;$('saveStatus').textContent='已保存到此浏览器';toast('已在沙盘安全边界保存本局');}catch(e){toast('保存失败：'+e.message,true);}
    } else if(action==='continue'){
      try{const saved=localStorage.getItem(STORE);if(!saved)throw new Error('此浏览器还没有存档');game.load(saved);cancelPlayback();ui.lastAction='';Object.assign(ui,{node:'home',selected:[],card:null,outcomeSeen:false,homeTab:null,homeDraft:null,modeDraft:null});close();render();$('saveStatus').textContent='已加载本地存档';toast('已继续保存的本局');}catch(e){toast(e.message,true);}
    } else if(action==='advance')dispatch('advance');
    else if(action==='filter'){ui.filter=el.dataset.filter;renderHand();}
    else if(action==='select-card'){ui.card=el.dataset.id;renderInspector();renderHand();}
    else if(action==='deploy')dispatch('deploy',{id:ui.card,node:ui.node,anchor:$('anchor')?.value});
    else if(action==='synth-preview')showSynthesis(el.dataset.id);
    else if(action==='synthesize'){const result=dispatch('synthesize',{id:el.dataset.id});if(result.ok){close();const x=result.result;if(x.success){ui.node=x.node;ui.selected=[x.createdId];ui.card=null;render();}dialog(x.success?'合成成功 · 就地登场':'合成失败','<div class="outcome"><div class="symbol">'+(x.success?D.cards[x.selected].icon:'◇')+'</div><h2>'+(x.success?name(x.selected):'没有产生造物')+'</h2><p>'+esc(x.text)+'</p></div><div class="formula">候选随机数：'+x.selectionRoll.toFixed(6)+'<br>成功随机数：'+x.successRoll.toFixed(6)+'<br>本次使用了与预览相同的计算字段。</div>',button('close',x.success?'查看生成节点':'返回沙盘','class="primary"'));}}
    else if(action==='clear-selection'){ui.selected=[];render();}
    else if(action==='choose-mode'){ui.mode=el.dataset.mode;renderInspector();}
    else if(action==='move')dispatch('move',{ids:ui.selected,node:ui.node,mode:ui.mode});
    else if(action==='operation')dispatch('operation',{node:el.dataset.nodeId});
    else if(action==='scene')scene();
    else if(action==='home-open')openHome();
    else if(action==='home-back'){ui.homeTab=null;render();}
    else if(action==='home-tab'){ui.homeTab=el.dataset.tab;render();}
    else if(action==='home-character'){ui.homeDraft.character=el.dataset.choice;render();}
    else if(action==='home-outfit'){ui.homeDraft.outfit=el.dataset.choice;render();}
    else if(action==='home-weapon'){ui.homeDraft.weapon=el.dataset.choice;const poses=D.homeWeapons.find(w=>w.id===el.dataset.choice).poses;if(!poses.includes(ui.homeDraft.pose))ui.homeDraft.pose=poses[0];render();}
    else if(action==='home-pose'){ui.homeDraft.pose=el.dataset.choice;render();}
    else if(action==='home-reset'){ui.homeDraft=E.clone(game.run.homeDisplay);render();}
    else if(action==='home-apply')dispatch('home-display',ui.homeDraft);
    else if(action==='home-mode'){ui.otherMode=el.dataset.choice;ui.followerPose='idle';render();}
    else if(action==='follower-character'){ui.modeDraft[ui.otherMode].character=el.dataset.choice;render();}
    else if(action==='follower-weapon'){ui.modeDraft[ui.otherMode].weapon=el.dataset.choice;render();}
    else if(action==='follower-pose'){ui.followerPose=el.dataset.choice;render();}
    else if(action==='follower-apply'){const p=ui.modeDraft[ui.otherMode];dispatch('settings',ui.otherMode==='act'?{character:p.character,weapon:p.weapon}:{fpsRoster:E.clone(p)});}
    else if(action==='dialogue-next'||action==='dialogue-choice'){const result=dispatch('dialogue-next',action==='dialogue-choice'?{choiceId:el.dataset.choice}:{});if(result.ok&&!game.dialogue){close();toast(result.text);}}
    else if(action==='dialogue-leave')close();
    else if(action==='card-info'){const c=game.card(el.dataset.id),d=D.cards[c.defId];dialog(d.name,coverage(d)+'<p>'+Object.entries(d.tags).map(([k,v])=>k+' '+v).join(' / ')+'</p>'+ (d.family==='C'?'<p>攻击 '+num(c.atk)+' / 防御 '+d.def+' / 生命 '+num(c.hp)+' / '+d.hp+'</p>':''),button('close','关闭'));}
    else if(action==='ability'){const c=game.card(el.dataset.id);if(c.defId==='banquet')dialog('希律一诺','<p>每时代一次；4 心流与 4 计算力相互转换，不超过各自上限。</p>',button('use-ability','心流 → 计算力','data-id="'+c.id+'"')+button('use-ability','计算力 → 心流','data-id="'+c.id+'" data-reverse="true"'));else dispatch('ability',{id:c.id});}
    else if(action==='use-ability'){if(dispatch('ability',{id:el.dataset.id,reverse:el.dataset.reverse==='true'}).ok)close();}
    else if(action==='journal')dialog('本局记录','<div>'+game.run.log.map(l=>'<p class="'+esc(l.kind)+'"><small>'+l.id+'</small> '+esc(l.text)+'</p>').join('')+'</div>',button('close','关闭'));
    else if(action==='fx-speed'){ui.fxSpeed=ui.fxSpeed===1?2:1;renderBattle();}
    else if(action==='exit')dispatch('exit');
    else if(action==='tab')dispatch('tab');
    else if(action==='attack'){
      const b=game.battle,active=b?.units.find(u=>u.id===b.active);
      if(b?.mode!=='encounter'||active?.side!=='player'||active.hp<=0)return;
      cardInput.cancel();ui.attackTargeting=true;ui.targets=[];renderBattle();
    }
    else if(action==='attack-count'){
      const b=game.battle,active=b?.units.find(u=>u.id===b.active),count=Number(el.dataset.count);
      if(b?.mode!=='encounter'||active?.side!=='player'||!Number.isInteger(count)||count<1||count>active.maxTargets)return;
      ui.attackCount=count;ui.targets=[];renderBattle();
    }
    else if(action==='cancel-attack'){ui.attackTargeting=false;ui.targets=[];renderBattle();}
    else if(action==='guard')dispatch('guard');
    else if(action==='confirm-card')cardInput.confirm();
    else if(action==='cancel-card')cardInput.cancel();
    else if(action==='skill'){if(game.battle?.mode==='encounter'&&ui.suppressSkillClick===el.dataset.skill&&event.detail>0){ui.suppressSkillClick=null;return;}playSkill(el.dataset.skill);}
    else if(action==='target'){
      if(cardInput.state.skillId){cardInput.commit({arena:true,unitId:el.dataset.target});return;}
      const b=game.battle, u=b?.units.find(u=>u.id===el.dataset.target), active=b?.units.find(u=>u.id===b.active);
      if(!ui.attackTargeting||!u||u.hp<=0||u.side!=='ruler'||active?.side!=='player')return;
      if(ui.targets.includes(u.id))ui.targets=ui.targets.filter(id=>id!==u.id);
      else ui.targets.push(u.id);
      if(ui.targets.length===ui.attackCount)dispatch('attack',{targets:[...ui.targets]});
      else renderBattle();
    }
    else if(action==='act-target'){ui.target=el.dataset.target;updateActHud();}
    else if(action==='act-attack'){if(!game.battle?.tab)ui.attackPulse=.25;}
  });
  document.addEventListener('change',event=>{
    if(event.target.dataset.select){const id=event.target.dataset.select,checked=event.target.checked;let group=[];try{group=game.group([id]).map(c=>c.id);}catch(_){}ui.selected=checked?[...new Set([...ui.selected,...group])]:ui.selected.filter(c=>!group.includes(c));render();}
  });
  $('map').addEventListener('click',event=>{const el=event.target.closest('[data-node]');if(el)inspectNode(el.dataset.node);});
  $('map').addEventListener('dblclick',event=>{const el=event.target.closest('[data-node]');if(el?.dataset.node==='home'){event.preventDefault();openHome();}});
  $('map').addEventListener('keydown',event=>{if(event.key==='Enter'||event.key===' '){const el=event.target.closest('[data-node]');if(el){event.preventDefault();if(event.altKey&&el.dataset.node==='home')openHome();else inspectNode(el.dataset.node);}}});
  $('map').addEventListener('pointerover',event=>{const el=event.target.closest('[data-node]');if(!el||!game.run)return;const n=D.nodes.find(n=>n.id===el.dataset.node),own=game.run.cards.filter(c=>c.zone==='world'&&c.node===n.id),enemies=game.run.nodes[n.id].owner==='ruler'?game.enemyDefinitions(n.id):[];$('mapTip').innerHTML='<strong>'+n.name+' · '+n.terrain+'</strong>我方：'+(own.map(c=>name(c.defId)+(c.attachedTo?'（附着）':'')).join('、')||'无')+'<br>敌方：'+(enemies.map(e=>e.name+'（攻'+e.atk+' 防'+e.def+' 生命'+e.hp+'）').join('、')||'无');$('mapTip').hidden=false;});
  $('map').addEventListener('pointerleave',()=>{$('mapTip').hidden=true;});
  $('battle').addEventListener('click',event=>{
    if(game.battle?.mode==='encounter' && cardInput.state.skillId && !event.target.closest('[data-action]') && cardHit(event.target).arena) { cardInput.commit(cardHit(event.target)); return; }
    if(event.target.id==='actCanvas'&&game.battle?.mode==='act'&&!game.battle.tab){const rect=event.target.getBoundingClientRect();ui.movePoint={x:(event.clientX-rect.left)*800/rect.width,y:(event.clientY-rect.top)*480/rect.height};event.target.focus();}
  });
  document.addEventListener('pointerdown',event=>{
    cancelUnitHold(false);
    const unitElement=event.target.closest('[data-unit-id]'), battle=ui.presentation?.view || game.battle;
    if(unitElement && event.button===0 && battle?.mode==='encounter' && !$('dialog').open && !cardInput.state.dragging){
      ui.suppressUnitClick=null; clearTimeout(enemyTimer);
      unitHold={pointerId:event.pointerId,x:event.clientX,y:event.clientY,element:unitElement};
      unitElement.classList.add('inspecting-hold');
      unitHold.timer=setTimeout(()=>{
        if(!unitHold)return;
        unitElement.classList.remove('inspecting-hold');
        ui.suppressUnitClick=unitElement.dataset.unitId;
        showUnitDetails(unitElement.dataset.unitId);
      },500);
      return;
    }
    let el=event.target.closest('[data-skill]');
    if(!el||el.disabled||event.button!==0||game.battle?.mode!=='encounter'||ui.presentation)return;
    if(ui.attackTargeting){const id=el.dataset.skill;ui.attackTargeting=false;ui.targets=[];renderBattle();el=$('battle').querySelector('[data-skill="'+id+'"]')||el;}
    if(cardInput.begin(el.dataset.skill,event.pointerId,event.clientX,event.clientY)) {
      el.focus(); el.setPointerCapture?.(event.pointerId); event.preventDefault();
      ui.suppressSkillClick=el.dataset.skill;
    }
  });
  document.addEventListener('pointermove',event=>{
    if(unitHold?.pointerId===event.pointerId && Math.hypot(event.clientX-unitHold.x,event.clientY-unitHold.y)>10)cancelUnitHold();
    if(cardInput.state.pointerId!==event.pointerId)return;
    ui.dropHover=cardHit(document.elementFromPoint(event.clientX,event.clientY)).unitId || null;
    cardInput.move(event.pointerId,event.clientX,event.clientY);
  });
  document.addEventListener('pointerup',event=>{
    if(unitHold?.pointerId===event.pointerId)cancelUnitHold();
    if(cardInput.state.pointerId!==event.pointerId)return;
    ui.suppressSkillClick=cardInput.state.skillId;ui.dropHover=null;
    cardInput.release(event.pointerId,cardHit(document.elementFromPoint(event.clientX,event.clientY)));
  });
  document.addEventListener('pointercancel',event=>{if(unitHold?.pointerId===event.pointerId)cancelUnitHold();if(cardInput.state.pointerId===event.pointerId)cardInput.cancel();});
  document.addEventListener('contextmenu',event=>{const el=event.target.closest('[data-unit-id]');if(el && (ui.presentation?.view || game.battle)?.mode==='encounter'){event.preventDefault();cancelUnitHold(false);showUnitDetails(el.dataset.unitId);}});
  document.addEventListener('scroll',()=>cancelUnitHold(),true);
  document.addEventListener('keydown',event=>{
    if($('dialog').open)return;
    if(event.key.toLowerCase()==='i' && event.target.closest?.('[data-unit-id]')){event.preventDefault();showUnitDetails(event.target.closest('[data-unit-id]').dataset.unitId);return;}
    if(event.key==='Escape'&&ui.attackTargeting){event.preventDefault();ui.attackTargeting=false;ui.targets=[];renderBattle();return;}
    if(event.key==='Escape'&&cardInput.state.skillId){event.preventDefault();cardInput.cancel();return;}
    if(ui.presentation){if(['1','2','3','4','5',' '].includes(event.key))event.preventDefault();return;}
    if($('dialog').open||!game.battle)return;
    if(game.battle.mode==='encounter' && (cardInput.state.skillId || ui.attackTargeting) && event.key==='Tab')return;
    if(game.battle.mode==='encounter' && event.key===' ' && event.target.closest?.('button'))return;
    if(game.battle.mode==='encounter'&&event.key==='Enter'&&event.target.id==='encounterArena'&&cardInput.state.skillId){event.preventDefault();cardInput.commit({arena:true});return;}
    if(event.key==='Tab'){event.preventDefault();if(!event.repeat)dispatch('tab');return;}
    if(['1','2','3','4','5'].includes(event.key)){event.preventDefault();if(!event.repeat)playSkill(D.skills[Number(event.key)-1].id);return;}
    if([' ','ArrowUp','ArrowDown','ArrowLeft','ArrowRight'].includes(event.key))event.preventDefault();
    if(event.key==='Escape'&&game.battle.tab){dispatch('tab');return;}
    keys.add(event.key.toLowerCase());
    if(!game.battle.tab&&['w','a','s','d','arrowup','arrowdown','arrowleft','arrowright'].includes(event.key.toLowerCase()))ui.movePoint=null;
  });
  document.addEventListener('keyup',event=>keys.delete(event.key.toLowerCase()));
  window.addEventListener('pagehide',cancelPlayback);
  window.addEventListener('pageshow',event=>{if(event.persisted){cancelPlayback();render();}});
  window.addEventListener('blur',()=>{cancelUnitHold();cardInput.cancel();if(ui.attackTargeting){ui.attackTargeting=false;ui.targets=[];if(inBattle())renderBattle();}keys.clear();ui.movePoint=null;});
  $('dialog').addEventListener('cancel',event=>{event.preventDefault();close();});
  $('battle').addEventListener('wheel',event=>{if(game.battle?.tab){event.preventDefault();toast('本版仅配置一页，仍为共享槽位 1–5。');}}, {passive:false});
  function frame(time) {
    const dt=Math.min(.05,Math.max(0,(time-lastFrame)/1000));lastFrame=time;
    if(game.battle?.mode==='act'){
      const p=game.battle.units.find(u=>u.side==='player');
      let x=(keys.has('d')||keys.has('arrowright')?1:0)-(keys.has('a')||keys.has('arrowleft')?1:0),y=(keys.has('s')||keys.has('arrowdown')?1:0)-(keys.has('w')||keys.has('arrowup')?1:0);
      if(ui.movePoint&&!game.battle.tab){const dx=ui.movePoint.x-p.x,dy=ui.movePoint.y-p.y,len=Math.hypot(dx,dy);if(len<8)ui.movePoint=null;else{x=dx/len;y=dy/len;}}
      ui.attackPulse=Math.max(0,ui.attackPulse-dt);
      const old=game.battle;game.tick(dt,{x,y,attack:keys.has(' ')||ui.attackPulse>0});
      if(!game.battle){keys.clear();ui.movePoint=null;ui.node=old.movement.target;render();toast(game.notice);}
      else{drawAct();if(time-lastHud>180){updateActHud();renderResources();lastHud=time;}}
    }
    requestAnimationFrame(frame);
  }
  // The app exposes no state-mutation shortcuts or hidden victory controls.
  render();requestAnimationFrame(frame);
})();
