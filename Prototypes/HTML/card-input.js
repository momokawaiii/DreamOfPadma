(function (root) {
  'use strict';

  // Input previews contain no gameplay mutations. Only confirm emits a command.
  const idle = () => ({ skillId: null, dragging: false, pointerId: null, x: 0, y: 0, awaitingConfirm: false, targetId: null });
  function legalTarget(battle, skill, hit) {
    if (!battle || battle.mode !== 'encounter' || !skill || !hit || !hit.arena) return false;
    if (skill.target === 'all') return true;
    if (!['enemy', 'ally'].includes(skill.target)) return false;
    const target = (battle.units || []).find(unit => unit.id === hit.unitId);
    return !!target && target.hp > 0 && !target.core && target.side === (skill.target === 'ally' ? 'player' : 'ruler');
  }

  class CardInput {
    constructor({ getContext, onChange = () => {}, onPlay = () => {}, onNotice = () => {} }) {
      this.getContext = getContext;
      this.onChange = onChange;
      this.onPlay = onPlay;
      this.onNotice = onNotice;
      this._state = idle();
      this._window = null;
      this._start = null;
    }
    get state() { return { ...this._state }; }
    _changed() { this.onChange(this.state); }
    _context(id) {
      const context = this.getContext() || {}, battle = context.battle;
      const actor = battle && (battle.units || []).find(unit => unit.id === battle.active);
      const skill = (context.skills || []).find(candidate => candidate.id === id);
      if (context.blocked || !battle || battle.mode !== 'encounter' || !actor || actor.side !== 'player' || actor.hp <= 0 || battle.cardUsed || !skill || !(context.available || []).includes(id)) return null;
      return { battle, skill };
    }
    _current() {
      const context = this._context(this._state.skillId), window = this._window;
      if (!context || !window || context.battle !== window.battle || context.battle.active !== window.active || context.battle.actionCount !== window.actionCount) return null;
      return context;
    }
    select(id) {
      const context = this._context(id);
      if (!context) {
        this.cancel();
        this.onNotice('当前无法选择这张基础技能卡。');
        return false;
      }
      this._window = { battle: context.battle, active: context.battle.active, actionCount: context.battle.actionCount };
      this._start = null;
      this._state = { ...idle(), skillId: id };
      this._changed();
      return true;
    }
    begin(id, pointerId, x, y) {
      if (pointerId == null || !Number.isFinite(x) || !Number.isFinite(y)) return false;
      if (this._state.pointerId != null && this._state.pointerId !== pointerId) return false;
      if (!this.select(id)) return false;
      this._start = { x, y };
      Object.assign(this._state, { pointerId, x, y });
      this._changed();
      return true;
    }
    move(pointerId, x, y) {
      if (this._state.pointerId == null || pointerId !== this._state.pointerId || !Number.isFinite(x) || !Number.isFinite(y)) return false;
      if (!this._current()) return this._expired();
      const distance = (x - this._start.x) ** 2 + (y - this._start.y) ** 2;
      Object.assign(this._state, { x, y, dragging: this._state.dragging || distance >= 36 });
      this._changed();
      return true;
    }
    release(pointerId, hit) {
      if (this._state.pointerId == null || pointerId !== this._state.pointerId) return false;
      const wasDragging = this._state.dragging;
      this._start = null;
      Object.assign(this._state, { pointerId: null, dragging: false });
      if (!this._current()) return this._expired();
      if (!wasDragging) {
        this._changed();
        return true;
      }
      const context = this._current();
      if (!legalTarget(context.battle, context.skill, hit)) {
        this.cancel();
        this.onNotice('已取消拖放：请将卡牌拖到有效目标或战场。');
        return false;
      }
      return this.commit(hit);
    }
    commit(hit) {
      if (!this._state.skillId) return false;
      const context = this._current();
      if (!context) return this._expired();
      this._start = null;
      Object.assign(this._state, { pointerId: null, dragging: false, awaitingConfirm: false, targetId: null });
      if (!legalTarget(context.battle, context.skill, hit)) {
        this._changed();
        this.onNotice(context.skill.target === 'all' ? '请点击战场，预览所有非核心单位。' : '请选择一个存活的非核心' + (context.skill.target === 'ally' ? '友方' : '敌方') + '目标。');
        return false;
      }
      Object.assign(this._state, { awaitingConfirm: true, targetId: context.skill.target === 'all' ? null : hit.unitId });
      this._changed();
      return true;
    }
    confirm() {
      if (!this._state.awaitingConfirm) return false;
      const context = this._current();
      if (!context) return this._expired();
      if (!legalTarget(context.battle, context.skill, { unitId: this._state.targetId, arena: true })) {
        this.cancel();
        this.onNotice('目标已失效，请重新选择卡牌和目标。');
        return false;
      }
      const command = { id: this._state.skillId };
      if (context.skill.target !== 'all') command.target = this._state.targetId;
      this.cancel();
      this.onPlay(command);
      return true;
    }
    _expired() {
      this.cancel();
      this.onNotice('行动窗口已改变，请重新选择基础技能卡。');
      return false;
    }
    cancel() {
      const changed = this._state.skillId !== null;
      this._state = idle();
      this._window = null;
      this._start = null;
      if (changed) this._changed();
      return changed;
    }
  }

  const api = { CardInput, legalTarget };
  if (typeof module !== 'undefined' && module.exports) module.exports = api;
  else root.PadmaCardInput = api;
})(typeof globalThis !== 'undefined' ? globalThis : this);
