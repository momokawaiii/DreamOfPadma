const test = require('node:test');
const assert = require('node:assert/strict');
const fs = require('node:fs');
const vm = require('node:vm');
const { CardInput, legalTarget } = require('./card-input.js');
const D = require('./data.js');

function fixture() {
  const context = {
    battle: { mode: 'encounter', active: 'c-1', actionCount: 3, cardUsed: false, units: [
      { id: 'c-1', side: 'player', hp: 20, core: false },
      { id: 'c-2', side: 'player', hp: 10, core: false },
      { id: 'enemy-0', side: 'ruler', hp: 16, core: false },
      { id: 'enemy-1', side: 'ruler', hp: 16, core: false },
      { id: 'core', side: 'ruler', hp: 30, core: true },
      { id: 'dead', side: 'ruler', hp: 0, core: false }
    ] },
    skills: D.skills, available: D.skills.map(skill => skill.id), blocked: false
  };
  const plays = [], changes = [], notices = [];
  const input = new CardInput({ getContext: () => context, onPlay: command => plays.push(command), onChange: state => changes.push(state), onNotice: text => notices.push(text) });
  return { input, context, plays, changes, notices };
}
const enemy = { unitId: 'enemy-0', arena: true };

test('browser UMD export loads without DOM or timer APIs', () => {
  const context = vm.createContext({});
  vm.runInContext(fs.readFileSync(require.resolve('./card-input.js'), 'utf8'), context);
  assert.equal(typeof context.PadmaCardInput.CardInput, 'function');
  assert.equal(typeof context.PadmaCardInput.legalTarget, 'function');
});

test('click or keyboard selection only arms; explicit target preview still needs confirmation', () => {
  const { input, context, plays } = fixture(), before = structuredClone(context);
  assert.equal(input.select('cut'), true);
  assert.equal(input.state.awaitingConfirm, false);
  assert.equal(input.confirm(), false);
  assert.equal(input.commit(enemy), true);
  assert.equal(input.state.awaitingConfirm, true);
  assert.equal(input.state.targetId, 'enemy-0');
  assert.deepEqual(plays, []);
  assert.deepEqual(context, before);
  assert.equal(input.confirm(), true);
  assert.deepEqual(plays, [{ id: 'cut', target: 'enemy-0' }]);
  assert.equal(input.state.skillId, null);
  assert.equal(input.confirm(), false);
  assert.deepEqual(context, before);
});

test('drag starts at six pixels; valid release previews and confirmation submits once', () => {
  const { input, plays, context } = fixture(), before = structuredClone(context);
  assert.equal(input.begin('cut', 7, 50, 100), true);
  input.move(7, 50, 95);
  assert.equal(input.state.dragging, false);
  input.move(7, 50, 94);
  assert.equal(input.state.dragging, true);
  input.move(7, 50, 100);
  assert.equal(input.state.dragging, true);
  assert.equal(input.release(7, enemy), true);
  assert.equal(input.state.dragging, false);
  assert.equal(input.state.pointerId, null);
  assert.equal(input.state.awaitingConfirm, true);
  assert.deepEqual(plays, []);
  assert.deepEqual(context, before);
  assert.equal(input.release(7, enemy), false);
  assert.equal(input.confirm(), true);
  assert.equal(input.confirm(), false);
  assert.equal(plays.length, 1);
});

test('pointer tap below drag threshold stays armed without staging its release hit', () => {
  const { input, plays } = fixture();
  input.begin('cut', 0, 10, 30);
  input.move(0, 13, 33);
  input.release(0, enemy);
  assert.equal(input.state.skillId, 'cut');
  assert.equal(input.state.awaitingConfirm, false);
  assert.equal(input.confirm(), false);
  assert.deepEqual(plays, []);
});

test('wrong pointer cannot move, release, or replace an existing gesture', () => {
  const { input } = fixture();
  input.begin('cut', 1, 10, 30);
  const before = input.state;
  assert.equal(input.move(2, 10, 0), false);
  assert.equal(input.release(2, enemy), false);
  assert.equal(input.begin('rain', 2, 20, 40), false);
  assert.deepEqual(input.state, before);
  input.move(1, 10, 0);
  input.release(1, enemy);
  assert.equal(input.state.awaitingConfirm, true);
});

test('invalid drops cancel without gameplay changes or commands', () => {
  for (const hit of [undefined, { arena: false, unitId: 'enemy-0' }, { arena: true }, { arena: true, unitId: 'c-1' }, { arena: true, unitId: 'dead' }, { arena: true, unitId: 'core' }, { arena: true, unitId: 'missing' }]) {
    const { input, context, plays } = fixture(), before = structuredClone(context);
    input.begin('cut', 1, 20, 100);
    input.move(1, 20, 20);
    assert.equal(input.release(1, hit), false);
    assert.equal(input.state.skillId, null);
    assert.deepEqual(plays, []);
    assert.deepEqual(context, before);
  }
});

test('single-target skill side, core and life checks apply to click previews', () => {
  const { input, context, plays } = fixture();
  for (const skillId of ['wind', 'mantra', 'light']) {
    input.select(skillId);
    assert.equal(input.commit(enemy), false);
    assert.equal(input.state.skillId, skillId);
    assert.equal(input.commit({ arena: true, unitId: 'c-2' }), true);
    assert.equal(input.state.targetId, 'c-2');
    context.battle.units[1].core = true;
    assert.equal(input.confirm(), false);
    context.battle.units[1].core = false;
  }
  assert.deepEqual(plays, []);
});

test('Rain stages an arena-wide preview, requires confirmation and sends no unit target', () => {
  for (const hit of [{ arena: true }, enemy, { arena: true, unitId: 'c-1' }]) {
    const { input, plays } = fixture();
    input.select('rain');
    assert.equal(input.commit(hit), true);
    assert.equal(input.state.awaitingConfirm, true);
    assert.equal(input.state.targetId, null);
    assert.deepEqual(plays, []);
    assert.equal(input.confirm(), true);
    assert.deepEqual(plays, [{ id: 'rain' }]);
  }
  const { input } = fixture();
  input.select('rain');
  assert.equal(input.commit({ arena: false }), false);
  assert.equal(input.state.awaitingConfirm, false);
});

test('invalid explicit target clears staged confirmation and permits retry', () => {
  const { input, plays } = fixture();
  input.select('cut');
  input.commit(enemy);
  assert.equal(input.commit({ arena: true, unitId: 'c-1' }), false);
  assert.equal(input.state.skillId, 'cut');
  assert.equal(input.state.targetId, null);
  assert.equal(input.state.awaitingConfirm, false);
  assert.equal(input.confirm(), false);
  assert.equal(input.commit({ arena: true, unitId: 'enemy-1' }), true);
  input.confirm();
  assert.deepEqual(plays, [{ id: 'cut', target: 'enemy-1' }]);
});

test('new card selection or gesture clears any prior target confirmation', () => {
  const { input, plays } = fixture();
  input.select('cut');
  input.commit(enemy);
  input.select('wind');
  assert.equal(input.state.awaitingConfirm, false);
  assert.equal(input.confirm(), false);
  input.commit({ arena: true, unitId: 'c-1' });
  input.begin('cut', 1, 20, 100);
  assert.equal(input.state.awaitingConfirm, false);
  assert.deepEqual(plays, []);
});

test('battle identity, actor and action-count changes invalidate an armed action window', () => {
  const changes = [
    context => { context.battle = structuredClone(context.battle); },
    context => { context.battle.active = 'c-2'; },
    context => { context.battle.actionCount++; }
  ];
  for (const change of changes) for (const stage of ['release', 'commit', 'confirm']) {
    const { input, context, plays } = fixture();
    input.begin('cut', 1, 20, 100);
    input.move(1, 20, 20);
    if (stage === 'confirm') input.release(1, enemy);
    change(context);
    assert.equal(stage === 'release' ? input.release(1, enemy) : stage === 'commit' ? input.commit(enemy) : input.confirm(), false);
    assert.equal(input.state.skillId, null);
    assert.deepEqual(plays, []);
  }
});

test('playback locks, card lifecycle and action eligibility are rechecked at arm and confirmation', () => {
  const changes = [
    context => { context.blocked = true; },
    context => { context.available = []; },
    context => { context.battle.cardUsed = true; },
    context => { context.battle.active = 'enemy-0'; },
    context => { context.battle.units[0].hp = 0; },
    context => { context.battle.mode = 'act'; },
    context => { context.battle = null; },
    context => { context.skills = []; }
  ];
  for (const change of changes) {
    const { input, context, plays } = fixture();
    input.select('cut');
    input.commit(enemy);
    change(context);
    assert.equal(input.confirm(), false);
    assert.equal(input.select('cut'), false);
    assert.equal(input.state.skillId, null);
    assert.deepEqual(plays, []);
  }
});

test('changed or removed target is rejected on confirmation', () => {
  for (const change of [unit => { unit.hp = 0; }, unit => { unit.side = 'player'; }, unit => { unit.core = true; }, unit => { unit.id = 'removed'; }]) {
    const { input, context, plays } = fixture();
    input.select('cut');
    input.commit(enemy);
    change(context.battle.units[2]);
    assert.equal(input.confirm(), false);
    assert.equal(input.state.skillId, null);
    assert.deepEqual(plays, []);
  }
});

test('cancel supports Escape, pointer-cancel and scene exit without command or stale pointer release', () => {
  for (const phase of ['armed', 'drag', 'confirm']) {
    const { input, plays } = fixture();
    input.begin('cut', 1, 20, 100);
    if (phase !== 'armed') input.move(1, 20, 20);
    if (phase === 'confirm') input.release(1, enemy);
    assert.equal(input.cancel(), true);
    assert.equal(input.cancel(), false);
    assert.equal(input.release(1, enemy), false);
    assert.equal(input.confirm(), false);
    assert.deepEqual(plays, []);
  }
});

test('preview state is copied; clear happens before command callback and prevents reentrant double confirmation', () => {
  const { input, plays } = fixture();
  input.select('cut');
  input.commit(enemy);
  const snapshot = input.state;
  snapshot.targetId = 'enemy-1';
  snapshot.awaitingConfirm = false;
  input.onPlay = command => {
    assert.equal(input.state.skillId, null);
    assert.equal(input.confirm(), false);
    plays.push(command);
  };
  input.confirm();
  assert.deepEqual(plays, [{ id: 'cut', target: 'enemy-0' }]);
});

test('illegal context during movement cancels a stale gesture', () => {
  const { input, context, plays } = fixture();
  input.begin('cut', 1, 20, 100);
  context.battle.actionCount++;
  assert.equal(input.move(1, 20, 20), false);
  assert.equal(input.state.skillId, null);
  assert.equal(input.release(1, enemy), false);
  assert.deepEqual(plays, []);
});

test('target helper validates supported scope and mode without mutating arguments', () => {
  const { context } = fixture(), before = structuredClone(context);
  assert.equal(legalTarget(context.battle, D.skills[0], enemy), true);
  assert.equal(legalTarget(context.battle, { target: 'unknown' }, enemy), false);
  assert.equal(legalTarget(null, D.skills[0], enemy), false);
  assert.equal(legalTarget({ ...context.battle, mode: 'act' }, D.skills[0], enemy), false);
  assert.equal(legalTarget(context.battle, null, enemy), false);
  assert.deepEqual(context, before);
});
