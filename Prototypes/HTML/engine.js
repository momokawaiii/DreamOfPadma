(function (root) {
  'use strict';
  const D = typeof module !== 'undefined' && module.exports ? require('./data.js') : root.PadmaData;
  const clone = value => JSON.parse(JSON.stringify(value));
  const clamp = (v, lo, hi) => Math.min(hi, Math.max(lo, v));
  const need = (condition, text) => { if (!condition) throw new Error(text); };
  // Command-scoped presentation data never enters run state, saves or rollback.
  const encounterPresentations = new WeakMap();
  const combatStatus = unit => ({ hp: unit.hp, shield: unit.shield, block: unit.block });
  const nodeDef = id => D.nodes.find(n => n.id === id);
  const defaultHomeDisplay = () => ({ character: 'keeper-a', outfit: 'travel', weapon: 'sword', pose: 'idle' });
  const defaultFpsRoster = () => ({ character: 'fps-vanguard', weapon: 'rifle' });
  function validateHomeDisplay(value) {
    need(value && D.homeCharacters.some(c => c.id === value.character), '未知归处主角');
    need(D.homeOutfits.some(c => c.id === value.outfit), '未知主角服装');
    const weapon = D.homeWeapons.find(w => w.id === value.weapon);
    need(weapon, '未知主角展示武器');
    need(D.homePoses.some(p => p.id === value.pose) && weapon.poses.includes(value.pose), '展示姿态与武器不兼容');
    return { character: value.character, outfit: value.outfit, weapon: value.weapon, pose: value.pose };
  }
  function validateFpsRoster(value) {
    need(value && D.fpsCharacters.some(c => c.id === value.character), '未知 FPS 角色');
    need(D.fpsWeapons.some(w => w.id === value.weapon), '未知 FPS 武器');
    return { character: value.character, weapon: value.weapon };
  }
  function profile(input = {}) {
    const p = { ...D.defaults, ...input };
    for (const [key,, ,min,max] of D.profileFields) {
      need(typeof p[key] === 'number' && Number.isFinite(p[key]) && p[key] >= min && p[key] <= max, '参数超出范围：' + key);
    }
    for (const key of ['seed','dailyDraw']) need(Number.isInteger(p[key]), key + ' 必须是整数');
    need(p.flowStart <= p.flowCap && p.calcStart <= p.calcCap, '初始资源不能超过上限');
    return Object.fromEntries(D.profileFields.map(([key]) => [key, p[key]]));
  }
  function faithBonus(x) {
    if (x >= 95) return 10 + x - 95;
    if (x >= 80) return 4 + 0.4 * (x - 80);
    if (x >= 40) return -4 + 0.175 * (x - 40);
    if (x >= 20) return -15 + 0.55 * (x - 20);
    return -33 + 0.9 * x;
  }
  class Game {
    constructor(p) { this.run = null; this.battle = null; this.snapshot = null; this.dialogue = null; this.notice = ''; if (p) this.start(p); }
    start(p = {}) {
      need(!this.dialogue, '请先结束或取消对话，再开始新局');
      const cfg = profile(p);
      this.run = { version: D.version, profile: cfg, rng: cfg.seed >>> 0, sequence: 0,
        calendar: { chapter: 1, era: 1, day: 1, phase: 0, absoluteDay: 1 },
        resources: { flow: cfg.flowStart, calc: cfg.calcStart, faith: cfg.faith, entropy: cfg.entropy },
        rulerFaith: cfg.faith, core: { player: cfg.coreHP, ruler: cfg.coreHP }, war: 0, outcome: null,
        cards: [], deck: [], hand: [], discard: [], nodes: Object.fromEntries(D.nodes.map(n => [n.id, { owner: n.owner, used: false, visited: n.id === 'home' }])),
        unlocks: [], storyMet: false, synthesisHistory: [], battlesWon: [], log: [], nextId: 1,
        skills: { slots: D.skills.map(s => s.id), available: D.skills.map(s => s.id), discard: [], cycles: 0 },
        roster: { character: 'traveller', weapon: 'blade', hp: cfg.actHP },
        homeDisplay: defaultHomeDisplay(), fpsRoster: defaultFpsRoster(),
        mobility: Object.fromEntries(Object.keys(D.cards).map(id => [id, { mode: 'movable', minFaith: 50, forbiddenTerrain: '' }])) };
      // Editable HTML fixture examples, not source-card mobility definitions.
      this.run.mobility.door.mode = 'immovable';
      this.run.mobility.reed = { mode: 'conditional', minFaith: 50, forbiddenTerrain: '山地' };
      for (const id of ['dawn','cherry','dancer','earth','reed','banquet','cleopatra']) this.createCard(id, 'hand');
      for (const id of ['dawn','cherry','earth','reed','banquet','cleopatra','dawn','cherry','dancer','earth','reed','banquet','cleopatra','vitruvian']) this.createCard(id, 'deck');
      this.battle = null; this.snapshot = null; this.dialogue = null;
      this.emit('新局开始：HTML 临时试玩参数；第 1 天黎明。进入晌午后部署 A、附着 B。');
      this.notice = '试玩已开始';
      return this.run;
    }
    createCard(defId, zone, node = null) {
      const d = D.cards[defId]; need(d, '未知卡牌');
      need(['world','hand','deck','discard'].includes(zone), '未知卡牌分区');
      need(zone !== 'world' || nodeDef(node), '世界卡牌需要有效节点');
      const c = { id: 'card-' + this.run.nextId++, defId, zone, node: zone === 'world' ? node : null, attachedTo: null,
        field: d.field ? [...d.field] : null, hp: d.hp || d.stability || 1, atk: d.atk || 0, deployedDay: zone === 'world' ? this.run.calendar.absoluteDay : null, lastAccumulatedDay: 0, activeEra: '' };
      this.run.cards.push(c);
      if (zone === 'world') this.onPlayed(c);
      else this.run[zone].push(c.id);
      return c;
    }
    card(id) { const c = this.run.cards.find(c => c.id === id); need(c, '找不到卡牌实例'); return c; }
    emit(text, kind = 'info') { this.run.log.push({ id: ++this.run.sequence, text, kind }); if (this.run.log.length > 120) this.run.log.shift(); }
    random() { this.run.rng = (Math.imul(this.run.rng, 1664525) + 1013904223) >>> 0; return this.run.rng / 4294967296; }
    shuffle(items) { const a = [...items]; for (let i = a.length - 1; i > 0; --i) { const j = Math.floor(this.random() * (i + 1)); [a[i],a[j]] = [a[j],a[i]]; } return a; }
    pay(flow = 0, calc = 0) {
      const r = this.run.resources; need(flow >= 0 && calc >= 0, '费用不能为负');
      need(r.flow >= flow && r.calc >= calc, '资源不足：需要心流 ' + flow + '、计算力 ' + calc + '；可等待次日恢复。');
      r.flow -= flow; r.calc -= calc;
    }
    gain(flow = 0, calc = 0) { const r = this.run.resources, p = this.run.profile; r.flow = clamp(r.flow + flow, 0, p.flowCap); r.calc = clamp(r.calc + calc, 0, p.calcCap); }
    world(morning = false, allowDialogue = false) {
      need(this.run, '请先开始新局'); need(!this.battle, '战斗中沙盘时间冻结，不能执行沙盘操作');
      need(allowDialogue || !this.dialogue, '请先结束或取消对话，再执行沙盘操作');
      need(!this.run.outcome, '本局已结束，请保存记录或开始新局');
      if (morning) need(this.run.calendar.phase === 1, '此沙盘操作在晌午执行；点击推进阶段。');
    }
    dispatch(type, payload = {}) {
      if (!this.run) return { ok: false, text: '请先开始新局' };
      const previous = clone({ run: this.run, battle: this.battle, snapshot: this.snapshot, dialogue: this.dialogue });
      const observedBattle = this.battle;
      const presentation = observedBattle && observedBattle.mode === 'encounter' && ['attack','guard','enemy','skill'].includes(type) ? {
        mode: 'encounter', actorId: type === 'skill' ? null : observedBattle.active,
        actionOwnerId: observedBattle.active,
        source: type === 'skill' ? { kind: 'card', id: payload && payload.id } : { kind: 'unit', id: observedBattle.active },
        actionLabel: type === 'skill' ? (D.skills.find(s => s.id === (payload && payload.id)) || {}).name : type === 'guard' ? '防御' : '普攻',
        abilityId: type === 'skill' ? payload && payload.id : null,
        before: clone(observedBattle), after: null, ended: null, effects: []
      } : null;
      if (presentation) encounterPresentations.set(this, presentation);
      try {
        need(!this.dialogue || ['dialogue-next','dialogue-cancel'].includes(type), '请先结束或取消对话，再执行其他操作');
        let result;
        switch (type) {
          case 'advance': result = this.advance(); break;
          case 'deploy': result = this.deploy(payload.id, payload.node, payload.anchor); break;
          case 'synthesize': result = this.synthesize(payload.id); break;
          case 'move': result = this.move(payload.ids, payload.node, payload.mode); break;
          case 'operation': result = this.operation(payload.node); break;
          case 'dialogue-next': result = this.dialogueNext(payload.choiceId); break;
          case 'dialogue-cancel': result = this.dialogueCancel(); break;
          case 'home-display': result = this.homeDisplay(payload); break;
          case 'ability': result = this.ability(payload.id, payload.reverse); break;
          case 'attack': result = this.encounterAttack(payload.targets); break;
          case 'guard': result = this.encounterGuard(); break;
          case 'enemy': result = this.enemyAction(); break;
          case 'skill': result = this.skill(payload.id, payload.target); break;
          case 'tab': need(this.battle, '基础技能只能在局部战斗使用'); this.battle.tab = !this.battle.tab; result = this.battle.tab ? '技能库已打开' : '技能库已关闭'; break;
          case 'exit': need(this.battle, '当前不在战斗'); this.finish(false, '主动退出'); result = this.notice; break;
          case 'settings': result = this.settings(payload); break;
          default: throw new Error('未知命令：' + type);
        }
        this.notice = typeof result === 'string' ? result : (result && result.text) || '操作完成';
        if (presentation) presentation.after = clone(observedBattle);
        return { ok: true, text: this.notice, result, ...(presentation ? { presentation } : {}) };
      } catch (error) {
        this.run = previous.run; this.battle = previous.battle; this.snapshot = previous.snapshot; this.dialogue = previous.dialogue;
        this.notice = error.message;
        return { ok: false, text: error.message };
      } finally {
        encounterPresentations.delete(this);
      }
    }
    setZone(c, zone) {
      for (const name of ['deck','hand','discard']) this.run[name] = this.run[name].filter(id => id !== c.id);
      c.zone = zone;
      if (['deck','hand','discard'].includes(zone)) { this.run[zone].push(c.id); c.node = null; c.attachedTo = null; }
    }
    onPlayed(c) {
      const d = D.cards[c.defId];
      for (const unit of this.run.cards.filter(v => v.zone === 'world')) {
        if (unit.defId === 'vitruvian' && (d.tags.繁荣 || d.tags.欲望)) {
          unit.atk += 1; this.emit('食烟火：' + unit.id + ' 永久攻击 +1');
        }
        if (unit.defId === 'cherry' && unit.attachedTo && d.kind === 'person') this.gain(1);
      }
    }
    deploy(id, node, anchor) {
      this.world(true); const c = this.card(id), d = D.cards[c.defId];
      need(c.zone === 'hand', '只能部署手牌'); need(nodeDef(node), '未知地块'); need(this.run.nodes[node].owner === 'player', '只能在己方地块部署');
      if (d.family === 'B') {
        const a = this.card(anchor); need(a.zone === 'world' && a.node === node && D.cards[a.defId].family === 'A', '请先选择此地块的 A 作为附着对象');
        need(!this.run.cards.some(v => v.attachedTo === a.id), '本版合成界面仅覆盖一张 A 附着一张 B；多 B 合成尚未接入');
        c.attachedTo = a.id;
      }
      this.pay(d.family === 'C' ? this.run.profile.deployC : d.cost);
      this.setZone(c, 'world'); c.node = node; c.deployedDay = this.run.calendar.absoluteDay; this.onPlayed(c);
      this.emit(d.name + ' ' + (d.family === 'B' ? '附着到 ' + D.cards[this.card(anchor).defId].name : '部署于 ' + nodeDef(node).name));
      return '已部署 ' + d.name;
    }
    advance() {
      this.world(); const t = this.run.calendar, p = this.run.profile;
      if (t.phase < 2) t.phase++;
      else {
        t.phase = 0; t.day++; t.absoluteDay++;
        if (t.day > 7) { t.day = 1; t.era++; this.emit('时代推进'); }
        if (t.era > 7) { t.era = 1; t.chapter++; this.emit('章节推进'); }
        let extra = this.run.nodes.fire.owner === 'player' ? p.fireIncome : 0;
        for (const c of this.run.cards.filter(v => v.zone === 'world')) {
          const d = D.cards[c.defId], age = t.absoluteDay - c.deployedDay;
          if (d.dailyFlow && (!d.dailyLimit || age <= d.dailyLimit)) extra += d.dailyFlow;
          if (c.defId === 'banquet' && age > 0 && age % 3 === 0) this.run.core.player = Math.min(p.coreHP, this.run.core.player + 1);
          if (d.family === 'C') c.hp = Math.min(d.hp, c.hp + 1);
        }
        this.run.roster.hp = Math.min(p.actHP, this.run.roster.hp + 1);
        const flowGain = p.flowCap * p.dailyFraction + extra;
        const calcGain = p.calcCap * Math.max(0.5, p.dailyFraction * (1 - this.run.resources.entropy / 200));
        this.gain(flowGain, calcGain);
        let drawn = 0;
        while (drawn < p.dailyDraw && this.run.deck.length) { this.setZone(this.card(this.run.deck[0]), 'hand'); drawn++; }
        this.emit('黎明结算：心流恢复 ' + flowGain.toFixed(1) + '、计算力恢复 ' + calcGain.toFixed(1) + '（不超上限），抽取 ' + drawn + ' 张 ABC。');
        if (!this.run.deck.length) this.emit('ABC 牌堆已空：本版不自动回收 ABC 弃牌；完整耗尽规则待确认。', 'note');
      }
      if (t.phase === 1) {
        for (const b of this.run.cards.filter(c => c.zone === 'world' && c.attachedTo)) {
          if (b.lastAccumulatedDay === t.absoluteDay) continue;
          const a = this.card(b.attachedTo), tendency = D.cards[a.defId].tendency;
          b.field = b.field.map((v, i) => v + tendency[i]); b.lastAccumulatedDay = t.absoluteDay;
          this.emit(D.cards[b.defId].name + ' 晌午累加场域 → [' + b.field.join(', ') + ']');
        }
      }
      this.emit('进入第 ' + t.day + ' 天 · ' + D.phases[t.phase]); return D.phases[t.phase];
    }
    preview(id) {
      const a = this.card(id); need(a.zone === 'world' && D.cards[a.defId].family === 'A', '请选择已部署的 A');
      const attached = this.run.cards.filter(c => c.attachedTo === a.id && c.zone === 'world');
      need(attached.length === 1, '需要 A 与一张附着 B');
      const b = attached[0], ad = D.cards[a.defId], bd = D.cards[b.defId], combined = {};
      for (const d of [ad,bd]) for (const [key,value] of Object.entries(d.tags)) combined[key] = (combined[key] || 0) + value / 100;
      const candidates = Object.values(D.cards).filter(c => c.family === 'C')
        .map(c => ({ id: c.id, score: Object.entries(c.tags).reduce((sum,[tag,w]) => sum + (combined[tag] || 0) * w / 100, 0) }))
        .filter(c => c.score > 0);
      need(candidates.length, '本版没有共同标签候选；无标签兜底规则尚未接入');
      const max = Math.max(...candidates.map(c => c.score)), temp = this.run.profile.temperature;
      const sum = candidates.reduce((s,c) => s + Math.exp((c.score - max) / temp), 0);
      for (const c of candidates) {
        c.probability = Math.exp((c.score - max) / temp) / sum;
        const target = D.cards[c.id].target, r = this.run.resources;
        c.distance = Math.abs(b.field[0] - target[0]) + Math.abs(b.field[1] - target[1]);
        c.affinity = ad.affinity; c.entropy = -Math.pow(r.entropy / 10, 1.5); c.faith = faithBonus(r.faith);
        c.rawSuccess = (100 - c.distance + c.affinity + c.entropy + c.faith) / 100;
        c.success = clamp(c.rawSuccess, 0, 1);
      }
      return { a: a.id, b: b.id, field: [...b.field], candidates, flow: this.run.profile.synthesisFlow, calc: this.run.profile.synthesisCalc };
    }
    synthesize(id) {
      this.world(true); const preview = this.preview(id);
      const node = this.card(preview.a).node;
      need(nodeDef(node) && this.card(preview.b).node === node, '合成材料必须位于同一有效节点');
      this.pay(preview.flow, preview.calc);
      const selectionRoll = this.random(); let at = 0, selected = preview.candidates.at(-1);
      for (const candidate of preview.candidates) { at += candidate.probability; if (selectionRoll < at) { selected = candidate; break; } }
      const successRoll = this.random(), success = successRoll < selected.success;
      const cherry = this.card(preview.b).defId === 'cherry';
      this.setZone(this.card(preview.a), 'discard'); this.setZone(this.card(preview.b), 'discard');
      let createdId = null;
      if (success) {
        createdId = this.createCard(selected.id, 'world', node).id;
        if (cherry) this.run.resources.entropy = Math.max(0, this.run.resources.entropy - 3);
      }
      const record = { ...preview, selected: selected.id, success, selectionRoll, successRoll, node, createdId };
      this.run.synthesisHistory.push(record);
      const text = (success ? '合成成功：' + D.cards[selected.id].name + ' 已直接出现于' + nodeDef(node).name : '合成失败：无奖励，费用不退') + '；A、B 已进入弃牌堆。';
      this.emit(text, success ? 'success' : 'note'); return { ...record, text };
    }
    group(ids) {
      need(Array.isArray(ids) && ids.length, '先在右侧选择要移动的卡牌');
      const all = new Set(ids);
      for (const id of [...all]) {
        const c = this.card(id); if (c.attachedTo) all.add(c.attachedTo);
      }
      for (const c of this.run.cards) if (c.attachedTo && all.has(c.attachedTo)) all.add(c.id);
      return [...all].map(id => this.card(id));
    }
    movePreview(ids, target) {
      need(nodeDef(target), '未知目标地块'); const group = this.group(ids), source = group[0].node;
      need(group.every(c => c.zone === 'world' && c.node === source), '整组卡牌必须在同一来源地块');
      need(this.run.nodes[source].owner === 'player', '来源地块不属于己方');
      const edge = D.edges.find(e => e[0] === source && e[1] === target);
      need(edge, '没有这条预设单向道路，六边形相邻不代表可通行');
      need(!edge[2] || this.run.unlocks.includes(edge[2]), '这条道路尚未解锁：先完成驿站事件');
      let calc = 0;
      const checks = [];
      for (const c of group) {
        const d = D.cards[c.defId], policy = this.run.mobility[c.defId]; let reason = '';
        if (!policy || !['movable','immovable','conditional'].includes(policy.mode)) reason = '移动规则未配置';
        else if (policy.mode === 'immovable') reason = '不可移动';
        else if (policy.mode === 'conditional' && this.run.resources.faith < policy.minFaith) reason = '需要信仰 ≥ ' + policy.minFaith;
        else if (policy.mode === 'conditional' && policy.forbiddenTerrain === nodeDef(target).terrain) reason = '禁止进入' + policy.forbiddenTerrain;
        const cost = d.family === 'A' ? this.run.profile.moveA : d.family === 'B' ? this.run.profile.moveB : d.kind === 'building' ? this.run.profile.moveBuilding : this.run.profile.movePerson;
        checks.push({ id: c.id, name: d.name, ok: !reason, reason, calc: cost }); calc += cost;
      }
      return { source, target, ids: group.map(c => c.id), checks, calc, enemy: this.run.nodes[target].owner === 'ruler' };
    }
    move(ids, target, mode) {
      this.world(); const p = this.movePreview(ids, target);
      need(p.checks.every(c => c.ok), p.checks.filter(c => !c.ok).map(c => c.name + '：' + c.reason).join('；'));
      // Combat entry is legal in every phase. Peaceful movement follows the Morning action window.
      if (!p.enemy) need(this.run.calendar.phase === 1, '非战斗移动在晌午执行');
      if (p.enemy) {
        need(['encounter','act'].includes(mode), '请选择 Encounter 或 ACT 试玩入口');
        if (mode === 'encounter') need(p.ids.some(id => D.cards[this.card(id).defId].family === 'C' && this.card(id).hp > 0), '本版 Encounter 需移动组中有 C 单位；ACT 使用总设置中的独立角色');
      }
      const snapshot = clone(this.run); this.pay(0, p.calc);
      if (p.enemy) { this.snapshot = snapshot; this.startBattle(mode, p); return '进入 ' + (mode === 'act' ? 'ACT' : 'Encounter') + '，日历已冻结'; }
      for (const id of p.ids) this.card(id).node = target;
      this.capture(target); this.emit('移动完成：' + p.ids.length + ' 张卡 → ' + nodeDef(target).name + '，计算力 −' + p.calc);
      return '已移动到 ' + nodeDef(target).name;
    }
    capture(id) {
      const n = this.run.nodes[id]; n.visited = true;
      if (n.owner !== 'player') {
        n.owner = 'player';
        this.run.war += nodeDef(id).type === 'gate' ? this.run.profile.gateWar : this.run.profile.captureWar;
        if (id === 'boss') this.run.core.ruler = Math.max(0, this.run.core.ruler - this.run.profile.coreStrike);
        this.emit('占领 ' + nodeDef(id).name + '，战局天平 ' + this.run.war, 'success');
      }
      this.evaluateOutcome();
    }
    evaluateOutcome() {
      if (this.run.core.ruler <= 0) this.run.outcome = { kind: 'core', text: '王庭核心归零 · 本局胜利' };
      else if (this.run.war >= this.run.profile.warGoal) this.run.outcome = { kind: 'balance', text: '战局天平抵达玩家边界 · 本局胜利' };
    }
    operation(id) {
      this.world(true); const n = this.run.nodes[id]; need(n && n.owner === 'player', '先占领此地块');
      need(!n.used, '此地块的一次性事件已经使用');
      const script = D.dialogues[id]; need(script && script.lines[script.start], '此地块没有可用的一次性操作');
      this.dialogue = { nodeId: id, lineId: script.start };
      return '对话已开始；完成确认前不结算奖励';
    }
    dialogueNext(choiceId) {
      need(this.dialogue, '当前没有进行中的对话'); this.world(true, true);
      const { nodeId, lineId } = this.dialogue, script = D.dialogues[nodeId], line = script && script.lines[lineId];
      need(line, '无效对话节点');
      if (line.choices) {
        const choice = line.choices.find(c => c.id === choiceId); need(choice, '请选择当前对话提供的选项');
        if (choice.cancel) return this.dialogueCancel();
        if (choice.complete) {
          const result = this.#completeOperation(nodeId);
          this.dialogue = null; return result;
        }
        need(choice.next && script.lines[choice.next], '对话选项没有有效后继');
        this.dialogue.lineId = choice.next;
      } else {
        need(choiceId === undefined, '当前对话不接受选项');
        need(line.next && script.lines[line.next], '对话没有有效后继');
        this.dialogue.lineId = line.next;
      }
      return '对话继续';
    }
    dialogueCancel() {
      need(this.dialogue, '当前没有进行中的对话'); this.dialogue = null;
      return '已离开对话；没有领取奖励或消耗事件';
    }
    #completeOperation(id) {
      this.world(true, true);
      const line = this.dialogue && D.dialogues[this.dialogue.nodeId]?.lines[this.dialogue.lineId];
      need(this.dialogue && this.dialogue.nodeId === id && line?.choices?.some(c => c.complete), '地块事件需要在对话选项中确认');
      const n = this.run.nodes[id]; need(n && n.owner === 'player', '先占领此地块');
      need(!n.used, '此地块的一次性事件已经使用');
      if (id === 'story') { n.used = true; this.run.storyMet = true; this.run.unlocks.push('story-road'); this.gain(0, this.run.profile.storyCalc); this.emit('守望人的援助：获得计算力，驿站 → 熔炉道路解锁。好感 Buff 尚未配置。'); }
      else if (id === 'forge') { n.used = true; this.gain(this.run.profile.forgeFlow); this.emit('旧日熔炉使用完毕，转为普通地块。'); }
      else throw new Error('此地块没有可用的一次性操作');
      return '地块事件已结算';
    }
    homeDisplay(input) {
      this.world(); const value = validateHomeDisplay(input);
      if (JSON.stringify(value) === JSON.stringify(this.run.homeDisplay)) return '展示配置未改变';
      this.run.homeDisplay = value;
      this.emit('归处主角展示已更新；服装、武器与姿态仅用于外观展示');
      return '主角展示已应用';
    }
    ability(id, reverse) {
      this.world(true); const c = this.card(id); need(c.zone === 'world', '技能需要已部署卡牌');
      if (c.defId === 'vitruvian') {
        this.pay(1); for (const ally of this.run.cards.filter(v => v.zone === 'world' && D.cards[v.defId].family === 'C')) ally.hp = Math.min(D.cards[ally.defId].hp, ally.hp + 1);
        this.emit('完美肉体：己方已部署非核心 C 恢复 1 生命');
      } else if (c.defId === 'banquet') {
        const era = this.run.calendar.chapter + ':' + this.run.calendar.era;
        need(c.activeEra !== era, '希律一诺本时代已经使用');
        if (reverse) { this.pay(0,4); this.gain(4); } else { this.pay(4); this.gain(0,4); }
        c.activeEra = era; this.emit('希律一诺：4 ' + (reverse ? '计算力换心流' : '心流换计算力'));
      } else throw new Error('此卡的沙盘主动技能尚未接入本版');
      return '沙盘主动技能已生效';
    }
    settings(input) {
      this.world();
      const character = Object.hasOwn(input, 'character') ? input.character : this.run.roster.character;
      const weapon = Object.hasOwn(input, 'weapon') ? input.weapon : this.run.roster.weapon;
      const fpsRoster = Object.hasOwn(input, 'fpsRoster') ? validateFpsRoster(input.fpsRoster) : this.run.fpsRoster;
      need(D.actCharacters.some(c => c.id === character), '未知 ACT 角色'); need(D.weapons.some(w => w.id === weapon), '未知武器');
      for (const [id, policy] of Object.entries(input.mobility || {})) {
        need(D.cards[id] && ['movable','immovable','conditional','unconfigured'].includes(policy.mode), '无效移动配置');
        need(Number.isFinite(policy.minFaith) && policy.minFaith >= 0 && policy.minFaith <= 100, '移动信仰条件须为 0–100');
        need(['','山地','平原','城镇','林地','丘陵'].includes(policy.forbiddenTerrain), '无效地形条件');
      }
      this.run.roster.character = character; this.run.roster.weapon = weapon; this.run.fpsRoster = clone(fpsRoster);
      Object.assign(this.run.mobility, clone(input.mobility || {}));
      this.emit('全局配置已更新：ACT 阵容、FPS 展示编队与卡牌移动规则'); return '配置已应用';
    }
    enemyDefinitions(target) {
      const p = this.run.profile, n = nodeDef(target);
      return Array.from({ length: n.enemies || 0 }, (_, i) => ({
        id: 'enemy-' + i, name: (target === 'boss' ? '王庭守卫' : '关隘守卫') + ' ' + (i + 1),
        side: 'ruler', family: 'C', attribute: 'R', hp: target === 'boss' ? p.bossHP : p.enemyHP,
        maxHP: target === 'boss' ? p.bossHP : p.enemyHP, atk: p.enemyAtk, def: p.enemyDef,
        speed: p.enemySpeed + i, ready: 100 / (p.enemySpeed + i), shield: 0, block: 0, acted: false, core: false,
        x: 570 + i * 110, y: 170 + i * 140, vx: 0, vy: 0, attackTimer: 0.8 + i * 0.5, windup: 0
      }));
    }
    startBattle(mode, movement) {
      this.world(); need(['encounter','act'].includes(mode), '本版仅提供 Encounter 或 ACT 战斗，FPS 仅可选角');
      const p = this.run.profile, enemies = this.enemyDefinitions(movement.target);
      let players;
      if (mode === 'encounter') {
        players = movement.ids.map(id => this.card(id)).filter(c => D.cards[c.defId].family === 'C').map(c => {
          const d = D.cards[c.defId]; return { id: c.id, defId: c.defId, name: d.name, side: 'player', family: 'C', attribute: d.attribute, hp: c.hp, maxHP: d.hp, atk: c.atk, def: d.def, attackCost: d.attackCost, maxTargets: d.maxTargets, speed: p.playerSpeed, ready: 100 / p.playerSpeed, shield: 0, block: 0, acted: false, core: false };
        });
      } else {
        need(this.run.roster.hp > 0, 'ACT 出战角色没有生命'); const weapon = D.weapons.find(w => w.id === this.run.roster.weapon), character = D.actCharacters.find(c => c.id === this.run.roster.character), terrain = nodeDef(movement.target).terrain;
        need(character && weapon, 'ACT 阵容配置无效');
        players = [{ id: 'act-' + character.id, defId: character.id, name: character.name, palette: character.palette, side: 'player', family: 'ACT', attribute: null, hp: this.run.roster.hp, maxHP: p.actHP, atk: p.actAtk * weapon.damage, def: p.actDef, shield: 0, block: 0, core: false, x: 160, y: 245, vx: 0, vy: 0, range: p.actRange * weapon.range, speed: p.actSpeed * (terrain === '平原' ? 1.15 : 1), interval: p.actCooldown * weapon.interval, attackTimer: 0, flash: 0 }];
      }
      this.battle = { mode, movement: clone(movement), units: [...players,...enemies], elapsed: 0, actionTime: 0, actionCount: 0, active: null, cardUsed: false, tab: false, page: 0, log: [], effects: [], result: null };
      this.emit('进入 ' + mode.toUpperCase() + ' @ ' + nodeDef(movement.target).name + '；快照包含移动前资源、卡库、地图、随机流、阵容。');
      if (mode === 'encounter') this.nextActor();
    }
    battleLog(text) { this.battle.log.push(text); if (this.battle.log.length > 30) this.battle.log.shift(); }
    nextActor() {
      const b = this.battle, alive = b.units.filter(u => u.hp > 0).sort((a,c) => a.ready - c.ready || a.id.localeCompare(c.id));
      const active = alive[0]; if (!active) return;
      b.active = active.id; b.actionTime = active.ready; b.cardUsed = false; active.acted = false; b.actionCount++;
      this.battleLog(active.name + ' 行动（时间 ' + active.ready.toFixed(2) + '）');
    }
    activePlayer() {
      need(this.battle && this.battle.mode === 'encounter', '此操作只用于 Encounter');
      const u = this.battle.units.find(u => u.id === this.battle.active);
      need(u && u.side === 'player' && u.hp > 0, '请等待我方行动条到达'); return u;
    }
    recordEncounterEffect(kind, source, target, before, details = {}) {
      const presentation = encounterPresentations.get(this);
      if (!presentation) return;
      const amount = kind === 'shield' ? target.shield - before.shield : kind === 'block' ? target.block - before.block : Math.abs(target.hp - before.hp);
      presentation.effects.push({
        kind, sourceId: source.id, targetId: target.id, wave: 0, amount, isTrue: false, absorbed: 0, blocked: false,
        hpBefore: before.hp, hpAfter: target.hp, shieldBefore: before.shield, shieldAfter: target.shield,
        blockBefore: before.block, blockAfter: target.block, ...details
      });
    }
    damage(source, target, amount, isTrue = false, basic = false, wave = 0) {
      if (target.hp <= 0) return 0;
      let value = amount;
      if (!isTrue) {
        let factor = 1;
        if (basic && target.family === 'C') factor = 0.5;
        else if (source.attribute && target.attribute && this.run.calendar.era !== 6) {
          const beats = { D:'F', F:'R', R:'D' };
          if (beats[source.attribute] === target.attribute) factor = 2;
          else if (beats[target.attribute] === source.attribute) factor = 0.5;
        }
        value = amount * factor - target.def;
        if (value <= 0) {
          value = 1;
          if (!basic && source.hp > 0) {
            const sourceBefore = combatStatus(source);
            source.hp = Math.max(0, source.hp - 1);
            this.recordEncounterEffect('recoil', source, source, sourceBefore, { wave });
          }
        }
      }
      const before = combatStatus(target), blocked = target.block > 0;
      if (target.block > 0) { target.block--; value = 0; }
      const absorbed = Math.min(target.shield, value); target.shield -= absorbed; value -= absorbed;
      target.hp = Math.max(0, target.hp - value);
      this.recordEncounterEffect('damage', source, target, before, { wave, isTrue, absorbed, blocked });
      this.battleLog(source.name + ' → ' + target.name + '：' + value.toFixed(1) + (isTrue ? ' 真伤' : ' 伤害') + (absorbed ? '，护盾吸收 ' + absorbed : ''));
      if (this.battle.mode === 'act') this.battle.effects.push({ x: target.x, y: target.y, text: '-' + value.toFixed(1), life: 0.8, side: target.side });
      return value;
    }
    endAction(unit) {
      unit.acted = true; unit.ready += 100 / unit.speed;
      if (!this.checkBattleEnd()) this.nextActor();
    }
    encounterAttack(targets = []) {
      const u = this.activePlayer();
      need(Array.isArray(targets) && targets.length && targets.length <= u.maxTargets, '选择 1–' + u.maxTargets + ' 个不同敌人');
      need(new Set(targets).size === targets.length, '不能重复选择同一目标');
      const enemies = targets.map(id => this.battle.units.find(v => v.id === id));
      need(enemies.every(v => v && v.side !== u.side && v.hp > 0), '攻击目标无效'); this.pay(u.attackCost);
      for (const target of enemies) {
        const dealt = this.damage(u, target, u.atk);
        if (u.defId === 'door' && dealt > 0) { this.run.resources.faith = Math.max(0, this.run.resources.faith - 1); this.run.rulerFaith = Math.max(0, this.run.rulerFaith - 1); }
      }
      this.endAction(u); return this.battle ? '普攻已结算' : this.notice;
    }
    encounterGuard() {
      const u = this.activePlayer(), before = combatStatus(u); u.shield += 2;
      this.recordEncounterEffect('shield', u, u, before);
      this.battleLog(u.name + ' 防御：归墟 +2（HTML 通用动作）'); this.endAction(u); return '防御并结束本次行动';
    }
    enemyAction() {
      need(this.battle && this.battle.mode === 'encounter', '当前不是 Encounter');
      const u = this.battle.units.find(v => v.id === this.battle.active); need(u && u.side === 'ruler', '尚未轮到敌人');
      const target = this.battle.units.find(v => v.side === 'player' && v.hp > 0); need(target, '没有存活目标');
      this.damage(u, target, u.atk); this.endAction(u); return this.battle ? '敌方行动已结算' : this.notice;
    }
    skill(id, targetId) {
      need(this.battle, '基础技能只能在局部战斗使用'); const b = this.battle, skill = D.skills.find(s => s.id === id); need(skill, '未知技能');
      need(this.run.skills.available.includes(id), '此技能在基础技能弃牌堆；全部耗尽后洗回');
      let actionOwner;
      if (b.mode === 'encounter') { actionOwner = this.activePlayer(); need(!b.cardUsed, '本次行动已使用一张基础技能卡'); }
      else { need(b.tab, 'ACT 中请先按 Tab 打开技能库'); actionOwner = b.units.find(u => u.side === 'player' && u.hp > 0); }
      let target = b.units.find(u => u.id === targetId);
      // Encounter previews must explicitly name their target; ACT keeps its own binding/input contract.
      if (b.mode === 'act' && !target) target = skill.target === 'ally' ? actionOwner : b.units.find(u => u.side === 'ruler' && u.hp > 0);
      if (skill.target !== 'all') need(target && target.hp > 0 && !target.core && (skill.target === 'ally' ? target.side === 'player' : target.side === 'ruler'), '技能目标阵营或状态不合法');
      this.pay(skill.flow, skill.calc);
      if (b.mode === 'encounter') this.applyEncounterSkill(id, target);
      else this.applyActSkill(id, actionOwner, target);
      b.cardUsed = true;
      this.run.skills.available = this.run.skills.available.filter(v => v !== id); this.run.skills.discard.push(id);
      if (!this.run.skills.available.length) {
        this.run.skills.available = this.shuffle(this.run.skills.discard); this.run.skills.discard = []; this.run.skills.cycles++;
        this.battleLog('基础技能弃牌堆耗尽后洗回；五个共享槽位身份保持不变');
      }
      this.battleLog('打出 ' + skill.name);
      const ended = this.checkBattleEnd();
      // Environmental friendly fire can defeat the action owner while another ally remains.
      if (!ended && b.mode === 'encounter' && actionOwner.hp <= 0) this.nextActor();
      return this.battle ? skill.name + ' 已生效' : this.notice;
    }
    applyEncounterSkill(id, target) {
      // A hand card has no unit caster or inherited unit attributes/attack modifiers.
      const source = { id: null, name: D.skills.find(skill => skill.id === id).name, attribute: null };
      if (id === 'cut') this.damage(source, target, 3, true, true);
      if (id === 'wind') { const before = combatStatus(target); target.shield += 2; this.recordEncounterEffect('shield', source, target, before); }
      if (id === 'mantra') { const before = combatStatus(target); target.block += 1; this.recordEncounterEffect('block', source, target, before); }
      if (id === 'light') {
        const before = combatStatus(target); target.hp = Math.min(target.maxHP, target.hp + 5);
        this.recordEncounterEffect('heal', source, target, before);
        if (!target.acted) this.run.resources.faith = Math.min(100, this.run.resources.faith + 1);
      }
      if (id === 'rain') for (let i = 0; i < 3; i++) for (const unit of this.battle.units) if (!unit.core) this.damage(source, unit, 2, false, true, i);
    }
    applyActSkill(id, caster, target) {
      // Separate binding: ACT heal has no Encounter turn/faith condition.
      if (id === 'cut') this.damage(caster, target, 3, true, true);
      if (id === 'wind') target.shield += 2;
      if (id === 'mantra') target.block += 1;
      if (id === 'light') target.hp = Math.min(target.maxHP, target.hp + 5);
      if (id === 'rain') for (let i = 0; i < 3; i++) for (const unit of this.battle.units) if (!unit.core) this.damage(caster, unit, 2, false, true);
    }
    checkBattleEnd() {
      if (!this.battle.units.some(u => u.side === 'player' && u.hp > 0)) { this.finish(false, '局部战斗失败'); return true; }
      if (!this.battle.units.some(u => u.side === 'ruler' && u.hp > 0)) { this.finish(true, '局部战斗胜利'); return true; }
      return false;
    }
    finish(won, reason) {
      const b = this.battle, target = b.movement.target;
      const presentation = encounterPresentations.get(this);
      if (presentation) presentation.ended = won ? 'victory' : 'defeat';
      if (!won) {
        this.run = clone(this.snapshot); this.notice = reason + '：已完整恢复入战前状态（包括移动费用）。';
        // Deliberately no run log append here: exact rollback includes prior log.
      } else {
        for (const id of b.movement.ids) {
          const c = this.card(id), unit = b.units.find(u => u.id === id);
          if (unit) { c.hp = unit.hp; if (unit.hp <= 0) { this.setZone(c, 'discard'); continue; } }
          c.node = target;
        }
        if (b.mode === 'act') this.run.roster.hp = b.units.find(u => u.side === 'player').hp;
        this.run.battlesWon.push({ mode: b.mode, node: target, actions: b.actionCount, seconds: b.elapsed });
        this.capture(target); this.emit(reason + '，提交战果并返回沙盘', 'success'); this.notice = reason + '，已返回沙盘';
      }
      this.battle = null; this.snapshot = null;
    }
    tick(dt, input = {}) {
      if (!this.battle || this.battle.mode !== 'act') return;
      const b = this.battle; dt = clamp(Number(dt) || 0, 0, 0.05) * (b.tab ? 0.1 : 1); b.elapsed += dt;
      const player = b.units.find(u => u.side === 'player'), p = this.run.profile;
      if (!b.tab) {
        const x = clamp(input.x || 0,-1,1), y = clamp(input.y || 0,-1,1), length = Math.hypot(x,y) || 1;
        player.vx = x / length * player.speed; player.vy = y / length * player.speed;
      }
      player.x = clamp(player.x + player.vx * dt, 35, 765); player.y = clamp(player.y + player.vy * dt, 55, 435);
      player.attackTimer = Math.max(0, player.attackTimer - dt); player.flash = Math.max(0, player.flash - dt);
      if (!b.tab && input.attack && player.attackTimer <= 0) {
        const targets = b.units.filter(u => u.side === 'ruler' && u.hp > 0 && Math.hypot(u.x-player.x,u.y-player.y) <= player.range).sort((a,c) => Math.hypot(a.x-player.x,a.y-player.y) - Math.hypot(c.x-player.x,c.y-player.y));
        if (targets[0]) { this.damage(player, targets[0], player.atk); player.attackTimer = player.interval; player.flash = 0.18; }
      }
      for (const enemy of b.units.filter(u => u.side === 'ruler' && u.hp > 0)) {
        const dx = player.x - enemy.x, dy = player.y - enemy.y, distance = Math.hypot(dx,dy) || 1;
        enemy.attackTimer -= dt;
        if (enemy.windup > 0) {
          enemy.windup -= dt;
          if (enemy.windup <= 0) { if (distance < 68) this.damage(enemy, player, enemy.atk); enemy.attackTimer = p.enemyInterval; }
        } else if (distance < 58 && enemy.attackTimer <= 0) enemy.windup = 0.55;
        else if (distance > 48) { enemy.vx = dx / distance * 80; enemy.vy = dy / distance * 80; enemy.x += enemy.vx * dt; enemy.y += enemy.vy * dt; }
      }
      for (const effect of b.effects) { effect.life -= dt; effect.y -= 20 * dt; } b.effects = b.effects.filter(e => e.life > 0);
      this.checkBattleEnd();
    }
    save() { this.worldForSave(); return JSON.stringify({ app: 'padma-html', version: D.version, run: this.run }); }
    worldForSave() { need(this.run, '没有本局状态'); need(!this.battle, '请先结束或退出战斗，再在沙盘安全边界保存'); need(!this.dialogue, '请先结束或取消对话，再在沙盘安全边界保存'); }
    load(text) {
      need(!this.battle, '战斗中不能读档；请先退出并回滚到沙盘安全边界');
      need(!this.dialogue, '对话中不能读档；请先结束或取消对话');
      let data; try { data = JSON.parse(text); } catch (_) { throw new Error('存档不是合法 JSON'); }
      need(data && data.app === 'padma-html' && data.version === D.version && data.run && data.run.version === D.version, '存档格式或版本不兼容');
      const r = data.run; profile(r.profile);
      need(Array.isArray(r.cards) && r.cards.length <= 2000 && Array.isArray(r.log), '存档卡牌或日志无效');
      const ids = new Set(r.cards.map(c => c.id));
      need(ids.size === r.cards.length && r.cards.every(c => D.cards[c.defId] && ['hand','deck','discard','world'].includes(c.zone)), '存档卡牌定义／ID 无效');
      for (const zone of ['hand','deck','discard']) need(Array.isArray(r[zone]) && new Set(r[zone]).size === r[zone].length && r[zone].every(id => ids.has(id) && r.cards.find(c => c.id === id).zone === zone) && r.cards.filter(c => c.zone === zone).length === r[zone].length, '存档卡库分区不一致');
      need(r.cards.every(c => c.zone !== 'world' || (nodeDef(c.node) && (!c.attachedTo || r.cards.some(a => a.id === c.attachedTo && a.zone === 'world' && a.node === c.node && D.cards[a.defId].family === 'A')))), '存档节点／附着引用无效');
      need(r.calendar && ['chapter','era','day','absoluteDay'].every(k => Number.isInteger(r.calendar[k]) && r.calendar[k] >= 1) && r.calendar.era <= 7 && r.calendar.day <= 7 && [0,1,2].includes(r.calendar.phase), '存档日历无效');
      need(r.resources && ['flow','calc','faith','entropy'].every(k => Number.isFinite(r.resources[k]) && r.resources[k] >= 0), '存档资源无效');
      need(D.nodes.every(n => r.nodes[n.id] && ['player','neutral','ruler'].includes(r.nodes[n.id].owner)), '存档地图无效');
      need(r.skills && JSON.stringify(r.skills.slots) === JSON.stringify(D.skills.map(s => s.id)) && Array.isArray(r.skills.available) && Array.isArray(r.skills.discard), '存档共享槽位不兼容');
      const skillIds = [...r.skills.available,...r.skills.discard]; need(skillIds.length === 5 && new Set(skillIds).size === 5 && skillIds.every(id => D.skills.some(s => s.id === id)), '存档基础技能卡库无效');
      need(r.roster && D.actCharacters.some(c => c.id === r.roster.character) && D.weapons.some(w => w.id === r.roster.weapon) && Number.isFinite(r.roster.hp) && r.roster.hp > 0, '存档 ACT 阵容无效');
      // Narrow additive migration for the original version-1 prototype saves.
      // Missing new fields get defaults; present but malformed fields reject the load.
      if (!Object.hasOwn(r, 'homeDisplay')) r.homeDisplay = defaultHomeDisplay();
      if (!Object.hasOwn(r, 'fpsRoster')) r.fpsRoster = defaultFpsRoster();
      validateHomeDisplay(r.homeDisplay); validateFpsRoster(r.fpsRoster);
      need(Number.isInteger(r.rng) && r.rng >= 0 && Number.isInteger(r.nextId) && r.nextId > 0 && Number.isFinite(r.war) && r.core && ['player','ruler'].every(k => Number.isFinite(r.core[k]) && r.core[k] >= 0) && Array.isArray(r.unlocks) && Array.isArray(r.synthesisHistory) && Array.isArray(r.battlesWon) && r.mobility, '存档本局字段无效');
      this.run = clone(r); this.battle = null; this.snapshot = null; this.dialogue = null; this.notice = '已继续保存的本局'; return this.run;
    }
  }
  const api = { Game, faithBonus, profile, clone };
  if (typeof module !== 'undefined' && module.exports) module.exports = api; else root.PadmaEngine = api;
})(typeof globalThis !== 'undefined' ? globalThis : this);
