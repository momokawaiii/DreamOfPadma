(function (root) {
  'use strict';
  const copy = value => JSON.parse(JSON.stringify(value));
  const esc = value => String(value ?? '').replace(/[&<>"']/g, c => ({'&':'&amp;','<':'&lt;','>':'&gt;','"':'&quot;',"'":'&#39;'}[c]));
  const num = value => Number(value || 0).toFixed(1).replace(/\.0$/, '');
  const sortReady = (a, b) => a.ready - b.ready || a.id.localeCompare(b.id);

  // Prediction uses copies: repeated names are valid in a next-ready timeline.
  function nextTurns(battle, count = 6) {
    const queue = battle.units.filter(u => u.hp > 0).map(copy), result = [];
    if (!queue.length) return result;
    for (let i = 0; i < count; i++) {
      queue.sort(sortReady);
      const unit = queue[0];
      result.push({ id: unit.id, name: unit.name, side: unit.side, time: unit.ready });
      unit.ready += 100 / unit.speed;
    }
    return result;
  }

  function effectText(effect) {
    if (effect.kind === 'heal') return effect.amount ? '生命 +' + num(effect.amount) : '生命已满';
    if (effect.kind === 'shield') return '归墟 +' + num(effect.amount);
    if (effect.kind === 'block') return '抗衡 +' + num(effect.amount);
    if (effect.kind === 'recoil') return '反伤 −' + num(effect.amount);
    if (effect.blocked) return '抗衡抵挡 · 消耗 1 层';
    const parts = [];
    if (effect.absorbed) parts.push('归墟吸收 ' + num(effect.absorbed));
    if (effect.amount) parts.push((effect.isTrue ? '真伤 ' : '生命 ') + '−' + num(effect.amount));
    return parts.join(' / ') || '未损失生命';
  }

  function actionText(receipt) {
    const lookup = id => receipt.before.units.find(u => u.id === id)?.name || id;
    const targets = [...new Set(receipt.effects.filter(e => e.kind !== 'recoil').map(e => e.targetId))];
    return (receipt.source?.kind === 'card' ? '基础卡 · ' + receipt.actionLabel : lookup(receipt.actorId) + ' · ' + receipt.actionLabel) + ' → ' + (targets.map(lookup).join('、') || '场景');
  }

  // Milliseconds affect presentation only. The command has already resolved.
  function plan(receipt) {
    const groups = [];
    for (const effect of receipt.effects) {
      const wave = effect.wave || 0;
      let group = groups.find(g => g.wave === wave);
      if (!group) { group = { wave, effects: [] }; groups.push(group); }
      group.effects.push(copy(effect));
    }
    if (!groups.length) groups.push({ wave: 0, effects: [] });
    const steps = [{ at: 0, phase: 'windup', effects: [] }];
    groups.forEach((group, i) => {
      steps.push({ at: 340 + i * 1100, phase: 'launch', ...group });
      steps.push({ at: 740 + i * 1100, phase: 'impact', ...group });
    });
    const settle = 740 + (groups.length - 1) * 1100 + 1050;
    steps.push({ at: settle, phase: 'settle', effects: [] });
    steps.push({ at: settle + (receipt.ended ? 1050 : 220), phase: 'done', effects: [] });
    return steps;
  }

  function applyEffects(view, effects) {
    for (const effect of effects) {
      const target = view.units.find(u => u.id === effect.targetId);
      if (!target) continue;
      for (const key of ['hp','shield','block']) {
        if (Number.isFinite(effect[key + 'After'])) target[key] = effect[key + 'After'];
      }
    }
    return view;
  }

  class Playback {
    constructor({ schedule = (fn, ms) => root.setTimeout(fn, ms), unschedule = id => root.clearTimeout(id) } = {}) {
      // Window timer methods must keep their host receiver, not this Playback.
      this.schedule = schedule; this.unschedule = unschedule;
      this.generation = 0; this.timers = []; this.busy = false;
    }
    cancel() {
      this.generation++;
      this.timers.forEach(id => this.unschedule(id)); this.timers = [];
      this.busy = false;
    }
    play(receipt, { speed = 1, onStep = () => {}, onDone = () => {}, onError = () => {} } = {}) {
      this.cancel();
      const token = this.generation, detached = copy(receipt), view = copy(receipt.before);
      const rate = speed === 2 ? 2 : 1;
      this.busy = true;
      const fail = error => {
        if (token !== this.generation) return;
        this.cancel();
        onError(error, detached);
      };
      const deliver = step => {
        if (!this.busy || token !== this.generation) return;
        try {
          if (step.phase === 'done') {
            this.busy = false; this.timers = [];
            onDone(detached); return;
          }
          if (step.phase === 'impact') applyEffects(view, step.effects);
          onStep(step, view, detached);
        } catch (error) {
          fail(error);
        }
      };
      try {
        for (const step of plan(detached)) {
          if (step.at === 0) deliver(step);
          else if (token === this.generation && this.busy) this.timers.push(this.schedule(() => deliver(step), step.at / rate));
        }
      } catch (error) {
        fail(error);
      }
    }
  }

  function silhouette(unit) {
    const enemy = unit.side === 'ruler';
    return '<svg viewBox="0 0 120 134" class="unit-silhouette" aria-hidden="true"><ellipse cx="60" cy="123" rx="38" ry="8" fill="currentColor" opacity=".15"/><path d="M60 37L31 117Q60 132 89 117L72 54Z" fill="currentColor" opacity=".8"/><path d="M57 49L44 108L66 119L80 111L67 56" fill="currentColor"/><path d="M48 57L29 83M70 57L90 80" stroke="currentColor" stroke-width="9" stroke-linecap="round"/><circle cx="60" cy="30" r="15" fill="#dac3a1"/><path d="M44 30Q41 5 61 8Q79 10 77 35L67 21L49 29Z" fill="currentColor"/>' + (enemy ? '<path d="M91 39V116M83 52H99M88 37L92 21L96 37Z" fill="currentColor" stroke="currentColor" stroke-width="3"/><path d="M24 64L39 72V91L29 104L18 91V73Z" fill="#b39b77" stroke="currentColor" stroke-width="2"/>' : '<path d="M89 70L102 22L106 21L96 74Z" fill="#e9e7cb" stroke="currentColor" stroke-width="2"/><path d="M83 70L102 77M91 74L88 85" stroke="currentColor" stroke-width="4"/><path d="M50 13L35 7L41 25" fill="currentColor"/>') + '</svg>';
  }

  function unitStats(u) {
    return '<div class="hp"><i style="width:' + Math.max(0,u.hp/u.maxHP*100) + '%"></i></div><div class="unit-hp">生命 <strong>' + num(u.hp) + '</strong> / ' + num(u.maxHP) + '</div><div class="unit-buffs"><span>◈ 归墟 <b>' + num(u.shield) + '</b></span><span>◇ 抗衡 <b>' + num(u.block) + '</b></span></div>';
  }

  function unitDetails(unit, definition = {}, options = {}) {
    const enemy=unit.side==='ruler', d=definition || {}, kind=(d.kind==='building'?'建筑':'角色');
    const stat=(label,value)=>'<div><dt>'+esc(label)+'</dt><dd>'+esc(value)+'</dd></div>';
    const ability=(title,tag,description)=>'<article class="detail-ability"><div><h4>'+esc(title)+'</h4><span>'+esc(tag)+'</span></div><p>'+esc(description)+'</p></article>';
    let abilities=ability('普通攻击','已接入 · '+(enemy?'自动行动':'角色主动'),enemy?'行动时攻击我方第一个存活单位，使用当前攻击 '+num(unit.atk)+' 结算物理伤害。':'消耗 '+num(unit.attackCost)+' 心流，使用当前攻击 '+num(unit.atk)+' 对选定敌人结算物理伤害。最多 '+unit.maxTargets+' 个不同目标；完成后结束本次行动。');
    if(!enemy)abilities+=ability('防御','已接入 · 通用动作','自身归墟 +2，结束本次行动；不消耗心流。');
    if(unit.defId==='vitruvian') {
      abilities+=ability('食烟火','已接入 · 沙盘被动','繁荣／欲望卡登场时，永久攻击 +1；已有增益计入当前攻击。');
      abilities+=ability('完美肉体','已接入 · 沙盘主动','消耗 1 心流，使己方已部署的非核心 C 卡各恢复 1 生命。本战斗界面不可使用。');
      abilities+=ability('黄金分割','尚未接入','当前原型未配置完整技能效果，暂不可使用。');
    }
    if(unit.defId==='dancer')abilities+=ability('七重纱舞','尚未接入','当前原型未配置完整技能效果，暂不可使用。');
    if(unit.defId==='door')abilities+=ability('命中效果','已接入 · 普攻附带','普攻造成实际伤害时，双方信仰各减少 1。');
    const level=unit.level ?? d.level;
    const status=unit.hp<=0?'已倒下':options.active===unit.id?'当前行动单位':'等待行动';
    const stats=stat('生命',num(unit.hp)+' / '+num(unit.maxHP))+stat('当前攻击',num(unit.atk))+stat('防御',num(unit.def))+stat('归墟',num(unit.shield))+stat('抗衡',num(unit.block))+stat('行动速度',num(unit.speed))+stat('下次行动时间',num(unit.ready))+stat('普攻心流',enemy?'敌方自动行动':num(unit.attackCost))+stat('普攻目标上限',enemy?'1':String(unit.maxTargets))+stat('行动状态',status)+stat('本次已行动',unit.acted?'是':'否')+stat('核心单位',unit.core?'是':'否');
    return '<div class="unit-detail-layout '+(enemy?'enemy-detail':'')+'"><aside class="detail-portrait"><span class="eyebrow">'+(enemy?'王庭 · 守军':'我方 · 造物')+'</span><div class="detail-portrait-art">'+silhouette(unit)+'</div><span class="portrait-placeholder">占位立绘</span><h3>'+esc(unit.name)+'</h3><div class="detail-level">'+(level!=null?'Lv. '+esc(level):'Lv. — <small>等级未配置</small>')+'</div><div class="detail-tags"><span>'+esc(unit.family)+' / '+esc(unit.attribute)+'</span><span>'+kind+'</span><span>'+status+'</span></div></aside><div class="detail-content"><section><div class="detail-section-head"><h3>当前属性</h3><span>行动时间 '+num(options.actionTime||0)+'</span></div><dl class="detail-stats">'+stats+'</dl></section><section><div class="detail-section-head"><h3>角色技能</h3><span>能力与使用范围</span></div>'+abilities+'</section><section class="detail-catalog"><h3>卡牌档案</h3><div class="detail-tags">'+Object.entries(d.tags||{}).map(([key,value])=>'<span>'+esc(key)+' '+esc(value)+'</span>').join('')+'</div><p>'+esc(d.coverage||'HTML 占位守军；目前仅接入普通攻击，其他专属技能未配置。')+'</p><p class="detail-source">'+esc(d.page||'HTML 试玩配置')+(d.target?' · 合成向量 '+d.target.map(num).join(' / '):'')+'</p></section><p class="detail-note">基础技能手牌由卡牌／场景触发，独立于此角色技能。'+(options.busy?'此处为打开时的画面数据，本次动画继续，下一行动等待关闭详情。':'查看期间不开始下一次敌方行动；关闭后继续战斗。')+'</p></div></div>';
  }

  function unitCard(unit, battle, options) {
    const active = battle.active === unit.id, enemy = unit.side === 'ruler';
    const cardPlay = options.busy && options.receipt?.source?.kind === 'card';
    const targeted = enemy ? options.targets.includes(unit.id) : options.ally === unit.id;
    const label = unit.hp <= 0 ? '已倒下' : active ? (cardPlay ? '当前出牌窗口' : options.busy ? '正在行动' : enemy ? '敌方即将行动' : '轮到你了') : targeted ? (enemy ? '攻击目标' : '支援目标') : enemy ? '敌方单位' : '我方单位';
    const canAttack=options.attackTargeting && enemy && unit.hp>0 && !options.busy;
    return '<button class="combat-unit ' + (enemy ? 'enemy ' : '') + (active ? (cardPlay ? 'window-owner ' : 'acting ') : '') + (targeted ? 'targeted ' : '') + (canAttack?'attack-legal ':'') + (unit.hp <= 0 ? 'dead' : '') + '" data-action="target" data-target="' + esc(unit.id) + '" data-unit-id="' + esc(unit.id) + '" aria-keyshortcuts="I" aria-label="' + esc(label + '：' + unit.name + '，生命 ' + num(unit.hp)) + '。长按、右键或按 I 查看详情"><span class="unit-state">' + (canAttack&&!targeted?'点击作为攻击目标':label) + '</span><span class="unit-portrait">' + silhouette(unit) + '<span class="unit-crest">' + (enemy ? '♜' : '✧') + '</span></span><h3>' + esc(unit.name) + '</h3><div class="unit-vitals">' + unitStats(unit) + '</div><small>攻 ' + num(unit.atk) + ' · 防 ' + num(unit.def) + '</small><span class="unit-inspect-hint">长按查看详情</span><span class="unit-floats" aria-hidden="true"></span></button>';
  }

  function render(battle, options = {}) {
    const o = { targets: [], ally: null, busy: false, receipt: null, speed: 1, ...options };
    const active = battle.units.find(u => u.id === battle.active), ours = active?.side === 'player';
    const cardPlay = o.busy && o.receipt?.source?.kind === 'card';
    const title = cardPlay ? '基础卡展开 · ' + o.receipt.actionLabel : o.busy ? '正在行动 · ' + active.name : (ours ? '轮到你了 · ' : '敌方即将行动 · ') + active.name;
    const sub = o.busy ? actionText(o.receipt) : ours ? '普攻：先点普通攻击，再点敌人打出。长按双方角色查看详情。基础卡松手后须确认。' : '留意高亮单位和攻击轨迹；长按双方角色可查看详情。';
    return '<div class="encounter-view ' + (o.busy ? 'is-playing' : '') + '"><div class="turn-banner ' + (ours ? 'player-turn' : 'enemy-turn') + '" role="status" aria-live="polite"><span class="turn-mark">' + (ours ? '✧' : '♜') + '</span><div><small>ENCOUNTER · 第 ' + battle.actionCount + ' 次行动</small><h3 id="turnTitle">' + esc(title) + '</h3><p id="actionCaption">' + esc(sub) + '</p></div><div class="playback-option"><span>' + (o.busy ? '动作播放中' : '播放速度') + '</span><button data-action="fx-speed" ' + (o.busy ? 'disabled' : '') + ' aria-label="切换战斗动画速度，当前 ' + o.speed + ' 倍">' + o.speed + '×</button></div></div><div class="battle-shell"><div class="timeline"><span class="eyebrow">行动顺序 →<small>存活单位预测</small></span>' + nextTurns(battle).map((turn, i) => '<span class="turn ' + (i === 0 ? 'current ' : '') + (turn.side === 'ruler' ? 'enemy-turn' : '') + '"><small>' + (i === 0 ? '当前' : '随后 ' + i) + ' · ' + turn.time.toFixed(2) + '</small><b>' + esc(turn.name) + '</b></span>').join('') + '</div><div class="encounter-arena" id="encounterArena" tabindex="0" aria-label="战场，全体基础卡可拖入此处并确认"><div class="arena-side"><div class="side-label">我方 · 造物</div><div class="side">' + battle.units.filter(u => u.side === 'player').map(u => unitCard(u,battle,o)).join('') + '</div></div><div class="arena-divider" aria-hidden="true">VS</div><div class="arena-side"><div class="side-label">王庭 · 守军</div><div class="side">' + battle.units.filter(u => u.side === 'ruler').map(u => unitCard(u,battle,o)).join('') + '</div></div>' + (cardPlay ? '<div class="card-cast-stage"><small>基础技能 · 卡牌展开</small><span>' + esc(({cut:'╱',wind:'≈',mantra:'◇',light:'✦',rain:'⋮'})[o.receipt.abilityId] || '✧') + '</span><strong>' + esc(o.receipt.actionLabel) + '</strong></div>' : '') + '<svg id="encounterTrails" class="encounter-trails" aria-hidden="true"></svg></div><div class="action-readout" id="actionReadout" role="status" aria-live="polite">' + esc(o.lastAction || '箭头表示攻击方向 · 数字显示实际扣血 · 归墟 / 抗衡分别反馈') + '</div></div></div>';
  }

  function environmentMarkup(id, effects, point, area, speed) {
    let svg = '';
    const targets = [...new Set(effects.map(effect => effect.targetId))];
    for (const [index,targetId] of targets.entries()) {
      const p = point(targetId);
      if (!p) continue;
      if (id === 'rain') {
        // Deterministic visual scatter; never consumes the gameplay random stream.
        for (let i=0;i<5;i++) {
          const startX=p.x+(i-2)*29-35, startY=-35-i*15, dx=30+(i%2)*15, dy=p.y-startY+(i%2)*12;
          svg += '<g class="fishbone-fall" style="--fall-x:'+dx+'px;--fall-y:'+dy+'px;--fish-time:'+(400/speed)+'ms;--fish-delay:'+((i*13+index*7)/speed)+'ms"><g transform="translate('+startX+' '+startY+') rotate(-12)" fill="none" stroke="#a19baf" stroke-width="2.5" stroke-linecap="round"><path d="M0 0V39M0 9L-9 4M0 9L9 4M0 18L-10 12M0 18L10 12M0 27L-8 22M0 27L8 22M0 39L-8 46M0 39L8 46"/><path d="M-6-4L0-12L6-4L0 2Z" fill="#e7dfcc"/></g></g>';
        }
        svg += '<ellipse class="sky-impact" cx="'+p.x+'" cy="'+(p.y+52)+'" rx="43" ry="13" fill="none" stroke="#8b82a2" stroke-width="2"/>';
      } else if (id === 'cut') {
        svg += '<g class="spell-slash" stroke="#a278b5" stroke-linecap="round"><path d="M'+(p.x-35)+' '+(p.y+44)+' L'+(p.x+35)+' '+(p.y-44)+'" stroke-width="8"/><path d="M'+(p.x-17)+' '+(p.y+50)+' L'+(p.x+41)+' '+(p.y-22)+'" stroke-width="2"/></g>';
      } else if (id === 'light') {
        svg += '<g class="spell-light"><path d="M'+(p.x-24)+' 0L'+(p.x-37)+' '+(p.y+45)+'H'+(p.x+37)+'L'+(p.x+24)+' 0Z" fill="#e5d58f" opacity=".35"/><path d="M'+p.x+' '+(p.y-26)+'V'+(p.y+26)+'M'+(p.x-26)+' '+p.y+'H'+(p.x+26)+'" stroke="#fff5c8" stroke-width="5"/></g>';
      } else {
        svg += '<g class="spell-aura" style="color:'+(id==='mantra'?'#b59652':'#63a398')+'"><circle cx="'+p.x+'" cy="'+p.y+'" r="40" fill="none" stroke="currentColor" stroke-width="3"/><circle cx="'+p.x+'" cy="'+p.y+'" r="48" fill="none" stroke="currentColor" stroke-width="1" stroke-dasharray="'+(id==='mantra'?'6 12':'36 14')+'"/><text x="'+p.x+'" y="'+(p.y+11)+'" text-anchor="middle" font-size="33" fill="currentColor">'+(id==='mantra'?'◇':'≈')+'</text></g>';
      }
    }
    return svg;
  }

  // All geometry is measured against the current viewport; no fixed card positions.
  function paint(container, step, view, receipt, { speed = 1 } = {}) {
    const find = id => [...container.querySelectorAll('[data-unit-id]')].find(el => el.dataset.unitId === id);
    const cardPlay = receipt.source?.kind === 'card';
    const actor = cardPlay ? null : find(receipt.actorId), arena = container.querySelector('#encounterArena');
    const layer = container.querySelector('#encounterTrails');
    if (!arena || !layer) return;
    const lookup = id => view.units.find(u => u.id === id)?.name || id;
    const title = container.querySelector('#turnTitle'), caption = container.querySelector('#actionCaption'), readout = container.querySelector('#actionReadout');
    if (step.phase === 'windup') {
      actor?.classList.add('fx-casting');
      readout.textContent = actionText(receipt);
    }
    if (step.phase === 'launch') {
      for (const el of container.querySelectorAll('[data-unit-id]')) {
        el.classList.remove('fx-hit','fx-support','fx-recoil');
        el.querySelector('.unit-floats').innerHTML = '';
      }
      layer.innerHTML = '';
      const area = arena.getBoundingClientRect();
      layer.setAttribute('viewBox', '0 0 ' + area.width + ' ' + area.height);
      layer.style.setProperty('--travel', (400 / speed) + 'ms');
      const point = id => {
        const element = find(id)?.querySelector('.unit-portrait');
        if (!element) return null;
        const r = element.getBoundingClientRect();
        return { x: r.left + r.width / 2 - area.left, y: r.top + r.height * 0.46 - area.top };
      };
      const seen = new Set();
      if (cardPlay) {
        layer.innerHTML = environmentMarkup(receipt.abilityId, step.effects, point, area, speed);
        container.querySelector('.card-cast-stage')?.classList.add('spell-released');
      }
      for (const effect of step.effects) {
        if (cardPlay) break;
        if (effect.kind === 'recoil') continue;
        const key = effect.sourceId + ':' + effect.targetId;
        if (seen.has(key)) continue;
        seen.add(key);
        const start = point(effect.sourceId), end = point(effect.targetId);
        if (!start || !end) continue;
        const support = ['heal','shield','block'].includes(effect.kind), color = support ? '#3c9e92' : effect.isTrue ? '#9a66bd' : '#bf7544';
        if (effect.sourceId === effect.targetId) {
          layer.innerHTML += '<circle class="fx-self-ring" cx="' + end.x + '" cy="' + end.y + '" r="42" fill="none" stroke="' + color + '" stroke-width="3"/>';
          continue;
        }
        const path = 'M ' + start.x + ' ' + start.y + ' Q ' + ((start.x+end.x)/2) + ' ' + (Math.min(start.y,end.y)-65) + ' ' + end.x + ' ' + end.y;
        layer.innerHTML += '<g style="color:' + color + '"><path class="fx-route" d="' + path + '" fill="none" stroke="currentColor" stroke-width="2" stroke-dasharray="5 6"/><path class="fx-trace" d="' + path + '" fill="none" stroke="currentColor" stroke-width="4" pathLength="1"/><circle class="fx-particle" r="7" fill="currentColor"><animateMotion dur="' + (0.4/speed) + 's" fill="freeze" path="' + path + '"/></circle><path d="M ' + (end.x-6) + ' ' + (end.y-10) + ' L ' + end.x + ' ' + end.y + ' L ' + (end.x+10) + ' ' + (end.y-3) + '" fill="none" stroke="currentColor" stroke-width="3"/></g>';
      }
      if (receipt.abilityId === 'rain') {
        arena.classList.remove('fx-raining'); void arena.offsetWidth; arena.classList.add('fx-raining');
        caption.textContent = actionText(receipt) + ' · 第 ' + (step.wave + 1) + ' / 3 波（包括我方）';
      }
    }
    if (step.phase === 'impact') {
      actor?.classList.remove('fx-casting');
      const byTarget = new Map();
      for (const effect of step.effects) {
        if (!byTarget.has(effect.targetId)) byTarget.set(effect.targetId, []);
        byTarget.get(effect.targetId).push(effect);
      }
      for (const [id, effects] of byTarget) {
        const element = find(id), unit = view.units.find(u => u.id === id);
        if (!element || !unit) continue;
        const support = effects.every(e => ['heal','shield','block'].includes(e.kind));
        element.classList.add(support ? 'fx-support' : 'fx-hit');
        element.querySelector('.hp i').style.width = Math.max(0,unit.hp/unit.maxHP*100) + '%';
        element.querySelector('.unit-hp').innerHTML = '生命 <strong>' + num(unit.hp) + '</strong> / ' + num(unit.maxHP);
        element.querySelector('.unit-buffs').innerHTML = '<span>◈ 归墟 <b>' + num(unit.shield) + '</b></span><span>◇ 抗衡 <b>' + num(unit.block) + '</b></span>';
        element.querySelector('.unit-floats').innerHTML = effects.map(e => '<span class="damage-float ' + (['heal','shield','block'].includes(e.kind) ? 'support' : e.isTrue ? 'true-damage' : '') + '">' + esc(effectText(e)) + '</span>').join('');
        element.setAttribute('aria-label', unit.name + '，生命 ' + num(unit.hp) + '，' + effects.map(effectText).join('，'));
        if (unit.hp <= 0) {
          element.classList.add('fx-fallen');
          element.querySelector('.unit-state').textContent = '已倒下';
        }
      }
      readout.textContent = step.effects.map(e => (cardPlay ? receipt.actionLabel : lookup(e.sourceId)) + ' → ' + lookup(e.targetId) + '：' + effectText(e)).join('；');
    }
    if (step.phase === 'settle') {
      arena.classList.remove('fx-raining');
      if (receipt.ended) {
        title.textContent = receipt.ended === 'victory' ? '战斗胜利 · 即将提交战果' : '战斗失败 · 即将返回并回滚';
        caption.textContent = '最后一击已结算，正在返回沙盘。';
      } else {
        caption.textContent = receipt.after.active === (receipt.actionOwnerId || receipt.actorId) && receipt.after.actionCount === receipt.before.actionCount ? '技能已生效，请继续普攻或防御。' : '本次行动结束，准备下一位行动者。';
      }
    }
  }

  const api = { nextTurns, effectText, actionText, plan, applyEffects, Playback, render, paint, environmentMarkup, unitDetails };
  if (typeof module !== 'undefined' && module.exports) module.exports = api; else root.PadmaEncounterFX = api;
})(typeof globalThis !== 'undefined' ? globalThis : this);
