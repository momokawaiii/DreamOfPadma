(function (root) {
  'use strict';
  const D = typeof module !== 'undefined' && module.exports ? require('./data.js') : root.PadmaData;
  const esc = value => String(value ?? '').replace(/[&<>"']/g, c => ({'&':'&amp;','<':'&lt;','>':'&gt;','"':'&quot;',"'":'&#39;'}[c]));
  const choice = (action, id, label, selected, disabled = false) => '<button data-action="'+action+'" data-choice="'+id+'" class="display-choice '+(selected?'selected':'')+'" '+(disabled?'disabled':'')+'>'+label+'</button>';
  const find = (items,id) => items.find(item=>item.id===id) || items[0];
  const color = (value,fallback) => /^#[0-9a-f]{6}$/i.test(value || '') ? value : fallback;

  // Composable offline illustration: clothes, weapons and poses are independent layers.
  function portrait(character, outfit = 'travel', weapon = 'sword', pose = 'idle', mini = false) {
    const coat=color(character.palette,'#557c78'), hair=color(character.hair,'#343e4c');
    const accent=outfit==='formal'?'#c8a666':outfit==='combat'?'#91aaa7':'#d1c3a2';
    const sleeve=outfit==='formal'?'#e9e2ce':coat, hem=outfit==='combat'?310:365;
    let gear='';
    if(weapon==='spear') gear='<path d="M300 372L303 111" stroke="#b59663" stroke-width="5"/><path d="M294 115L304 68L314 115L304 104Z" fill="#dbe2df" stroke="#8a9d99"/>';
    else if(['rifle','carbine'].includes(weapon)) gear='<g transform="rotate(-24 269 225)"><path d="M216 219H325V237H216Z" fill="#434e58"/><path d="M325 223H354V229H325Z" fill="#bac4c4"/><path d="M234 236L245 259H259L249 236" fill="#4f5c65"/><path d="M214 218L192 207V241L216 235" fill="#736f66"/><path d="M271 215V202H298V215" fill="none" stroke="#677d82" stroke-width="4"/></g>';
    else gear='<g transform="'+(pose==='ready'?'rotate(-56 286 222)':'')+'"><path d="M288 241L291 359L298 377L303 359L300 241Z" fill="#dce5df" stroke="#9caeae"/><path d="M278 239H312M295 210V239" stroke="#c5a76d" stroke-width="7"/><circle cx="295" cy="205" r="5" fill="#c5a76d"/></g>';
    return '<svg class="character-illustration '+(mini?'mini':'')+'" viewBox="0 0 460 480" role="img" aria-label="'+esc(character.name+'，'+outfit+'，'+weapon+'，'+pose)+'">'+
      '<ellipse cx="230" cy="438" rx="90" ry="17" fill="#244248" opacity=".2"/>'+gear+
      '<g transform="'+(pose==='ready'?'translate(-7 0) rotate(-4 230 250)':'')+'">'+
      '<path d="M199 296L203 412L215 427H230L226 292M238 291L241 415L261 427H276L267 408L265 295" fill="#344654"/>'+
      '<path d="M204 389L202 427H232V391M242 389L243 427H278L268 410L266 389" fill="#293a43"/>'+
      '<path d="M211 157L179 177L159 238L174 263L192 222L189 '+hem+'Q230 '+(hem+28)+' 276 '+hem+'L265 220L282 245L301 224L279 176L249 157Z" fill="'+coat+'" stroke="#304d58" stroke-width="2"/>'+
      '<path d="M211 157L229 187L249 157L250 310L229 334L209 310Z" fill="'+(outfit==='combat'?'#43525a':'#eee7d5')+'"/>'+
      '<path d="M208 155L194 175L212 252L229 187M250 155L265 176L247 252L229 187" fill="'+accent+'"/>'+
      '<path d="M190 279H265V292H190Z" fill="#3a424a"/><path d="M218 278H238V294H218Z" fill="#b8a175"/>'+
      (outfit==='formal'?'<path d="M194 296L206 353M263 296L254 353" stroke="#d1ae6a" stroke-width="3"/><path d="M188 190Q207 207 221 200" fill="none" stroke="#d1ae6a" stroke-width="3"/>':'')+
      (outfit==='combat'?'<path d="M192 224H218V263H192ZM243 223H267V261H243Z" fill="#73837c" stroke="#344a51"/><path d="M190 305L174 337L192 333" fill="#839b90"/>':'')+
      (pose==='salute'?'<path d="M178 185L158 222L218 205" fill="none" stroke="'+sleeve+'" stroke-width="23" stroke-linejoin="round"/><path d="M211 205L229 197" stroke="#e7c5aa" stroke-width="12" stroke-linecap="round"/>':'<path d="M178 186L157 249L168 273" fill="none" stroke="'+sleeve+'" stroke-width="22" stroke-linecap="round"/><path d="M168 268L170 282" stroke="#e7c5aa" stroke-width="12" stroke-linecap="round"/>')+
      '<path d="M274 189L290 231" stroke="'+sleeve+'" stroke-width="22" stroke-linecap="round"/><path d="M290 228L298 225" stroke="#e7c5aa" stroke-width="13" stroke-linecap="round"/>'+
      '<path d="M219 138V164L230 174L242 162V138Z" fill="#dfbca1"/>'+
      '<path d="M200 93Q230 68 261 98L257 135Q251 156 230 158Q206 148 202 131Z" fill="#efd1b5"/>'+
      '<path d="M198 119L190 94Q199 63 232 64Q270 64 275 98L262 127L257 100L237 89L226 113L221 91L203 119L205 139L195 126Z" fill="'+hair+'"/>'+
      '<path d="M196 111L189 167L206 151L208 127M262 111L275 155L254 144L254 125" fill="'+hair+'"/>'+
      '<path d="M208 124H219M239 124H250" stroke="#465662" stroke-width="3"/><path d="M225 143Q232 146 239 142" fill="none" stroke="#b38377" stroke-width="2"/>'+
      '<path d="M207 157L228 180L249 157L248 180L230 195L208 178Z" fill="'+accent+'"/>'+
      '</g></svg>';
  }
  function stage(character,outfit,weapon,pose,caption) {
    return '<div class="character-stage"><div class="stage-orbit orbit-one"></div><div class="stage-orbit orbit-two"></div><span class="stage-word">PADMA</span><div class="stage-floor"></div>'+portrait(character,outfit,weapon,pose)+'<div class="stage-caption"><span class="eyebrow">'+esc(caption)+'</span><h2>'+esc(character.name)+'</h2><p>占位造型 · 服装、武器、姿势独立切换</p></div></div>';
  }
  function header(tab) {
    return '<div class="home-heading"><div><span class="eyebrow">SANCTUARY / 归处</span><h2>归处 · 编队与展示</h2></div><button data-action="home-back">← 返回沙盘</button></div><nav class="home-tabs" aria-label="归处栏目">'+
      [['details','节点细节','这里放置了什么'],['protagonist','主角展示','坐镇角色与换装'],['modes','其他模式','ACT / FPS 招募编队']].map(([id,name,desc])=>'<button data-action="home-tab" data-tab="'+id+'" class="'+(tab===id?'active':'')+'"><strong>'+name+'</strong><span>'+desc+'</span></button>').join('')+'</nav>';
  }
  function protagonist(run,ui) {
    const p=ui.homeDraft, character=find(D.homeCharacters,p.character), weapon=find(D.homeWeapons,p.weapon), equipped=find(D.homeCharacters,run.homeDisplay.character);
    const dirty=JSON.stringify(p)!==JSON.stringify(run.homeDisplay);
    return '<div class="dressing-layout"><aside class="recruit-list"><span class="eyebrow">坐镇归处</span><h3>主角名册</h3><p>当前坐镇：'+esc(equipped.name)+'</p>'+D.homeCharacters.map(c=>'<button class="recruit-card '+(p.character===c.id?'selected':'')+'" data-action="home-character" data-choice="'+c.id+'">'+portrait(c,'travel','sword','idle',true)+'<span>'+esc(c.name)+'<small>'+(run.homeDisplay.character===c.id?'正在坐镇':'可选主角')+'</small></span></button>').join('')+'<div class="identity-note">主角留在归处。<br>ACT / FPS 使用各自招募的手下。</div></aside>'+stage(character,p.outfit,p.weapon,p.pose,'PROTAGONIST · 主角')+
      '<aside class="wardrobe"><span class="eyebrow">APPEARANCE</span><h3>展示换装</h3><p class="hint">先预览，再保存。换装不消耗沙盘资源。</p><h4>服装</h4><div class="display-options">'+D.homeOutfits.map(o=>choice('home-outfit',o.id,esc(o.name),p.outfit===o.id)).join('')+'</div><h4>武器品种</h4><div class="display-options">'+D.homeWeapons.map(w=>choice('home-weapon',w.id,esc(w.name),p.weapon===w.id)).join('')+'</div><h4>展示姿势</h4><div class="display-options">'+D.homePoses.map(pose=>choice('home-pose',pose.id,esc(pose.name),p.pose===pose.id,!weapon.poses.includes(pose.id))).join('')+'</div><p class="hint">'+esc(weapon.name)+'支持 '+weapon.poses.map(id=>find(D.homePoses,id).name).join('、')+'。</p><div class="display-commit"><span>'+(dirty?'有尚未保存的预览':'已与坐镇配置一致')+'</span><button class="primary" data-action="home-apply" '+(run.outcome?'disabled':'')+'>保存坐镇与展示配置</button><button class="text-button" data-action="home-reset">还原预览</button></div></aside></div>';
  }
  function modes(run,ui) {
    const mode=ui.otherMode, isAct=mode==='act', characters=isAct?D.actCharacters:D.fpsCharacters, weapons=isAct?D.weapons:D.fpsWeapons;
    const draft=ui.modeDraft[mode], character=find(characters,draft.character), current=isAct?run.roster:run.fpsRoster;
    const weaponType=isAct?(draft.weapon==='blade'?'sword':'spear'):draft.weapon;
    return '<div class="mode-heading"><div class="mode-toggle">'+choice('home-mode','act','ACT 编队',isAct)+choice('home-mode','fps','FPS 编队',!isAct)+'</div><p>'+(isAct?'本局 ACT 将使用这里保存的招募角色与武器。':'FPS 可先选人和武器预览，尚未开放战斗。')+'</p></div><div class="dressing-layout"><aside class="recruit-list"><span class="eyebrow">RECRUITED FOLLOWERS</span><h3>'+mode.toUpperCase()+' 招募名册</h3>'+characters.map(c=>'<button class="recruit-card '+(draft.character===c.id?'selected':'')+'" data-action="follower-character" data-choice="'+c.id+'">'+portrait(c,'combat',weaponType,'idle',true)+'<span>'+esc(c.name)+'<small>'+(current.character===c.id?'当前编队':'可选手下')+'</small></span></button>').join('')+'<div class="identity-note">独立于归处主角与地图 ABC 卡牌。</div></aside>'+stage(character,'combat',weaponType,ui.followerPose,mode.toUpperCase()+' · 招募手下')+'<aside class="wardrobe"><span class="eyebrow">LOADOUT</span><h3>出战配置</h3><h4>武器</h4><div class="display-options">'+weapons.map(w=>choice('follower-weapon',w.id,esc(w.name),draft.weapon===w.id)).join('')+'</div><h4>姿势预览</h4><div class="display-options">'+choice('follower-pose','idle','待命',ui.followerPose==='idle')+choice('follower-pose','ready','备战',ui.followerPose==='ready')+'</div><div class="recruit-description">'+(isAct?esc(character.description):'远程招募角色占位。这里保存 FPS 自己的选择，不覆盖 ACT 编队。')+'</div><div class="display-commit"><button data-action="follower-apply" class="primary" '+(run.outcome?'disabled':'')+'>保存 '+mode.toUpperCase()+' 编队</button><span>'+(isAct?'通过地图上的 ACT 入口出战':'当前仅用于编队验证')+'</span></div></aside></div>';
  }
  function details(run) {
    const cards=run.cards.filter(c=>c.zone==='world'&&c.node==='home'), protagonist=find(D.homeCharacters,run.homeDisplay.character);
    return '<div class="home-details"><div class="node-overview"><span class="eyebrow">HOME / NODE CONTENTS</span><h2>归处当前放置</h2><p>与沙盘使用同一份节点状态。查看清单不会移动、部署或创建卡牌。</p><div class="node-facts"><div><small>控制者</small><strong>己方</strong></div><div><small>核心生命</small><strong>'+run.core.player+' / '+run.profile.coreHP+'</strong></div><div><small>坐镇角色</small><strong>'+esc(protagonist.name)+'</strong></div><div><small>ABC 卡牌</small><strong>'+cards.length+' 张</strong></div></div><button data-action="scene">查看地块场景 ↗</button><p class="hint">“地块场景”展示地面和放置物；上方“主角展示”进入专用角色舞台。</p></div><div class="node-inventory"><h3>节点实体清单</h3><div class="node-item"><span class="node-token">⌂</span><div><strong>归处核心</strong><p>设施 · 不参与人物编队</p></div></div><div class="node-item"><span class="node-token">✧</span><div><strong>'+esc(protagonist.name)+'</strong><p>坐镇主角 · 与 ABC 卡库、招募名册独立</p></div></div>'+cards.map(c=>{const d=D.cards[c.defId];return '<div class="node-item"><span class="node-token">'+d.icon+'</span><div><strong>'+d.family+' · '+esc(d.name)+'</strong><p>'+esc(c.id)+(c.attachedTo?' · 附着于 '+c.attachedTo:'')+(d.family==='C'?' · 生命 '+c.hp+' / '+d.hp:'')+'</p></div><button data-action="card-info" data-id="'+c.id+'" class="text-button">详情</button></div>';}).join('')+(!cards.length?'<p class="hint">当前归处没有已部署的 ABC 卡。</p>':'')+'</div></div>';
  }
  function home(run,ui) { return header(ui.homeTab)+(ui.homeTab==='details'?details(run):ui.homeTab==='modes'?modes(run,ui):protagonist(run,ui)); }
  function dialogue(game) {
    const state=game.dialogue, story=D.dialogues[state.nodeId], line=story.lines[state.lineId];
    const actor=state.nodeId==='story'?{name:'守望人',palette:'#4e7476',hair:'#c4c4b6'}:{name:'熔炉守候者',palette:'#896954',hair:'#443d39'};
    return '<div class="story-scene '+(state.nodeId==='forge'?'forge-story':'')+'"><div class="story-location">'+esc(story.title)+'<small>原型占位剧情 · 可替换台词与演出</small></div><div class="story-scenery">'+(state.nodeId==='forge'?'♜':'❧')+'</div>'+portrait(actor,'travel',state.nodeId==='forge'?'sword':'spear','idle')+'<div class="story-dialogue"><span class="story-speaker">'+esc(line.speaker)+'</span><p>'+esc(line.text)+'</p><div class="story-choices">'+(line.choices?line.choices.map(c=>'<button data-action="dialogue-choice" data-choice="'+c.id+'" class="'+(c.complete?'primary':'')+'">'+esc(c.label)+'</button>').join(''):'<button class="primary" data-action="dialogue-next">下一句 →</button>')+'</div></div></div><p class="hint">完成最终选项后才结算事件；中途离开不会消耗这次事件，也不会发放资源。</p>';
  }
  const api={home,portrait,dialogue};
  if(typeof module!=='undefined'&&module.exports)module.exports=api;else root.PadmaHome=api;
})(typeof globalThis!=='undefined'?globalThis:this);
