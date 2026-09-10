const test = require('node:test');
const assert = require('node:assert/strict');
const D = require('./data.js');
const { Game, clone } = require('./engine.js');

function command(game, type, payload) {
  const result = game.dispatch(type, payload);
  assert.equal(result.ok, true, result.text);
  return result;
}
function enter(profile = {}, mode = 'encounter', partySize = 1) {
  const game = new Game(profile);
  command(game, 'advance');
  const id = game.run.cards.find(card => card.zone === 'hand' && card.defId === 'dancer').id;
  command(game, 'deploy', { id, node: 'home' });
  const ids = [id];
  for (let i = 1; i < partySize; i++) {
    const additional = game.createCard('dancer', 'hand').id;
    command(game, 'deploy', { id: additional, node: 'home' }); ids.push(additional);
  }
  for (const node of ['fire', 'plain']) command(game, 'move', { ids, node });
  const snapshot = clone(game.run);
  command(game, 'move', { ids, node: 'gate', mode });
  return { game, id, snapshot, player: game.battle.units.find(unit => unit.side === 'player'), enemies: game.battle.units.filter(unit => unit.side === 'ruler') };
}
const state = game => JSON.stringify({ run: game.run, battle: game.battle, snapshot: game.snapshot });

test('attack receipt identifies the actor, each target and the actual next-ready actor', () => {
  const { game, id, enemies } = enter();
  const before = clone(game.battle), flow = game.run.resources.flow;
  const receipt = command(game, 'attack', { targets: enemies.map(unit => unit.id) }).presentation;
  assert.equal(receipt.mode, 'encounter'); assert.equal(receipt.actorId, id);
  assert.equal(receipt.actionOwnerId, id); assert.deepEqual(receipt.source, { kind: 'unit', id });
  assert.equal(receipt.actionLabel, '普攻'); assert.equal(receipt.abilityId, null); assert.equal(receipt.ended, null);
  assert.deepEqual(receipt.before, before); assert.deepEqual(receipt.after, game.battle);
  assert.equal(receipt.after.active, 'enemy-1');
  assert.deepEqual(receipt.effects.map(effect => [effect.kind, effect.sourceId, effect.targetId, effect.wave, effect.amount]), [
    ['damage', id, 'enemy-0', 0, 9], ['damage', id, 'enemy-1', 0, 9]
  ]);
  assert.equal(game.run.resources.flow, flow - D.cards.dancer.attackCost);
});

test('true damage reports actual clamped loss instead of overkill', () => {
  const { game, id, enemies } = enter();
  enemies[0].hp = 2; enemies[0].def = 100;
  const receipt = command(game, 'skill', { id: 'cut', target: enemies[0].id }).presentation;
  assert.equal(receipt.abilityId, 'cut'); assert.equal(receipt.actionLabel, D.skills.find(skill => skill.id === 'cut').name);
  assert.equal(receipt.actorId, null); assert.equal(receipt.actionOwnerId, id);
  assert.deepEqual(receipt.source, { kind: 'card', id: 'cut' });
  assert.equal(receipt.after.active, id); assert.equal(receipt.after.cardUsed, true);
  assert.deepEqual(receipt.effects[0], {
    kind: 'damage', sourceId: null, targetId: 'enemy-0', wave: 0, amount: 2, isTrue: true, absorbed: 0, blocked: false,
    hpBefore: 2, hpAfter: 0, shieldBefore: 0, shieldAfter: 0, blockBefore: 0, blockAfter: 0
  });
});

test('shield absorption is separate from applied HP loss, including a fully absorbed hit', () => {
  const { game, enemies } = enter();
  enemies[0].shield = 4; enemies[1].shield = 20;
  const effects = command(game, 'attack', { targets: enemies.map(unit => unit.id) }).presentation.effects;
  assert.deepEqual(effects.map(effect => [effect.amount, effect.absorbed, effect.shieldBefore, effect.shieldAfter, effect.hpBefore, effect.hpAfter]), [
    [5, 4, 4, 0, 16, 11], [0, 9, 20, 11, 16, 16]
  ]);
});

test('a blocked hit records the consumed layer without invented HP or shield damage', () => {
  const { game, enemies } = enter();
  enemies[0].block = 2; enemies[0].shield = 3;
  const effect = command(game, 'skill', { id: 'cut', target: enemies[0].id }).presentation.effects[0];
  assert.equal(effect.blocked, true); assert.equal(effect.isTrue, true);
  assert.equal(effect.blockBefore, 2); assert.equal(effect.blockAfter, 1);
  assert.equal(effect.amount, 0); assert.equal(effect.absorbed, 0);
  assert.equal(effect.hpBefore, effect.hpAfter); assert.equal(effect.shieldBefore, effect.shieldAfter);
});

test('failed penetration records attacker recoil before the target hit, with separate identities', () => {
  const { game, id, player, enemies } = enter();
  player.hp = 0.5; enemies[0].def = 100;
  const receipt = command(game, 'attack', { targets: [enemies[0].id] }).presentation;
  const [recoil, hit] = receipt.effects;
  assert.equal(recoil.kind, 'recoil'); assert.equal(recoil.sourceId, id); assert.equal(recoil.targetId, id);
  assert.equal(recoil.amount, 0.5); assert.equal(recoil.hpBefore, 0.5); assert.equal(recoil.hpAfter, 0);
  assert.equal(hit.kind, 'damage'); assert.equal(hit.sourceId, id); assert.equal(hit.targetId, 'enemy-0'); assert.equal(hit.amount, 1);
  assert.equal(receipt.ended, 'defeat'); assert.equal(game.battle, null);
});

test('guard, wind and mantra record actual shield/block grants without ending a skill action', () => {
  for (const [type, payload, kind, amount] of [['guard', {}, 'shield', 2], ['skill', { id: 'wind' }, 'shield', 2], ['skill', { id: 'mantra' }, 'block', 1]]) {
    const { game, id } = enter();
    const receipt = command(game, type, { ...payload, target: id }).presentation, effect = receipt.effects[0];
    assert.equal(receipt.effects.length, 1); assert.equal(effect.kind, kind); assert.equal(effect.amount, amount);
    assert.equal(effect.sourceId, type === 'skill' ? null : id); assert.equal(effect.targetId, id); assert.equal(effect.hpBefore, effect.hpAfter);
    assert.equal(receipt.actorId, type === 'skill' ? null : id); assert.equal(receipt.actionOwnerId, id);
    assert.deepEqual(receipt.source, type === 'skill' ? { kind: 'card', id: payload.id } : { kind: 'unit', id });
    assert.equal(receipt.after.active === id, type === 'skill');
    assert.equal(receipt.actionLabel, type === 'guard' ? '防御' : D.skills.find(skill => skill.id === payload.id).name);
  }
});

test('healing reports the clamped HP delta and retains the existing faith rule', () => {
  for (const missing of [0, 2, 10]) {
    const { game, player } = enter();
    player.hp -= missing;
    const faith = game.run.resources.faith, receipt = command(game, 'skill', { id: 'light', target: player.id }).presentation;
    const effect = receipt.effects[0];
    assert.equal(effect.kind, 'heal'); assert.equal(effect.amount, Math.min(5, missing));
    assert.equal(effect.hpAfter - effect.hpBefore, effect.amount); assert.equal(game.run.resources.faith, faith + 1);
  }
});

test('rain receipts retain three ordered waves, friendly fire and per-wave snapshots', () => {
  const { game, id, player, enemies } = enter();
  player.block = 1; enemies[0].shield = 2;
  const receipt = command(game, 'skill', { id: 'rain' }).presentation;
  assert.equal(receipt.effects.length, 9);
  assert.deepEqual(receipt.effects.map(effect => effect.wave), [0, 0, 0, 1, 1, 1, 2, 2, 2]);
  assert.deepEqual(receipt.effects.map(effect => effect.targetId), [id, 'enemy-0', 'enemy-1', id, 'enemy-0', 'enemy-1', id, 'enemy-0', 'enemy-1']);
  assert.equal(receipt.actorId, null); assert.equal(receipt.actionOwnerId, id);
  assert.deepEqual(receipt.source, { kind: 'card', id: 'rain' });
  assert.ok(receipt.effects.every(effect => effect.sourceId === null && effect.kind === 'damage' && !effect.isTrue));
  assert.equal(receipt.effects[0].blocked, true); assert.equal(receipt.effects[3].blocked, false);
  assert.deepEqual(receipt.effects.filter(effect => effect.targetId === 'enemy-0').map(effect => [effect.amount, effect.absorbed, effect.shieldBefore, effect.shieldAfter]), [[0, 1, 2, 1], [0, 1, 1, 0], [1, 0, 0, 0]]);
});

test('rain omits later hits on already defeated units without inventing extra effects', () => {
  const { game, enemies } = enter(); enemies[0].hp = 1;
  const receipt = command(game, 'skill', { id: 'rain' }).presentation;
  assert.deepEqual(receipt.effects.filter(effect => effect.targetId === 'enemy-0').map(effect => effect.wave), [0]);
  assert.equal(receipt.effects.filter(effect => effect.targetId === 'enemy-1').length, 3);
});

test('rain skips a defeated action owner and gives the next living ally a usable action', () => {
  const { game } = enter({}, 'encounter', 2);
  const caster = game.battle.units.find(unit => unit.id === game.battle.active);
  const ally = game.battle.units.find(unit => unit.side === 'player' && unit.id !== caster.id);
  caster.hp = 1;
  const actions = game.battle.actionCount, allyHP = ally.hp;
  const receipt = command(game, 'skill', { id: 'rain' }).presentation;
  assert.equal(receipt.actorId, null); assert.equal(receipt.actionOwnerId, caster.id); assert.equal(receipt.before.active, caster.id);
  assert.equal(receipt.ended, null); assert.equal(caster.hp, 0); assert.equal(ally.hp, allyHP - 3);
  assert.equal(game.battle.active, ally.id); assert.equal(receipt.after.active, ally.id);
  assert.equal(game.battle.actionCount, actions + 1); assert.equal(game.battle.cardUsed, false);
  assert.equal(receipt.effects.filter(effect => effect.targetId === caster.id).length, 1);
  const next = command(game, 'skill', { id: 'wind', target: ally.id }).presentation;
  assert.equal(next.actorId, null); assert.equal(next.actionOwnerId, ally.id); assert.equal(next.effects[0].targetId, ally.id);
  command(game, 'guard');
  assert.notEqual(game.battle.active, caster.id);
});

test('a surviving rain action owner retains the same action even with another ally present', () => {
  const { game } = enter({}, 'encounter', 2);
  const actorId = game.battle.active, actions = game.battle.actionCount;
  const receipt = command(game, 'skill', { id: 'rain' }).presentation;
  assert.equal(receipt.after.active, actorId); assert.equal(game.battle.active, actorId);
  assert.equal(game.battle.actionCount, actions); assert.equal(game.battle.cardUsed, true);
  assert.equal(game.dispatch('skill', { id: 'wind' }).ok, false);
  command(game, 'guard'); assert.equal(game.battle.actionCount, actions + 1);
});

test('a lethal rain settlement never advances the action selector after ending battle', () => {
  const { game } = enter();
  game.battle.units.find(unit => unit.id === game.battle.active).hp = 1;
  let advances = 0;
  const nextActor = game.nextActor.bind(game);
  game.nextActor = () => { advances++; return nextActor(); };
  const receipt = command(game, 'skill', { id: 'rain' }).presentation;
  assert.equal(receipt.ended, 'defeat'); assert.equal(game.battle, null); assert.equal(advances, 0);
});

test('enemy action has its own source identity and does not change action scheduling', () => {
  const { game, id } = enter();
  command(game, 'guard');
  const first = command(game, 'enemy').presentation, second = command(game, 'enemy').presentation;
  assert.equal(first.actorId, 'enemy-1'); assert.equal(first.effects[0].sourceId, 'enemy-1'); assert.equal(first.effects[0].targetId, id);
  assert.equal(first.actionOwnerId, 'enemy-1'); assert.deepEqual(first.source, { kind: 'unit', id: 'enemy-1' });
  assert.equal(first.effects[0].absorbed, 2); assert.equal(first.effects[0].amount, 1);
  assert.equal(first.after.active, 'enemy-0'); assert.equal(second.actorId, 'enemy-0'); assert.equal(second.after.active, id);
});

test('the final victory hit remains available after commit clears the battle', () => {
  const { game, id } = enter({ enemyHP: 2 });
  const receipt = command(game, 'attack', { targets: ['enemy-0', 'enemy-1'] }).presentation;
  assert.equal(game.battle, null); assert.equal(game.snapshot, null); assert.equal(receipt.ended, 'victory');
  assert.equal(receipt.before.units.filter(unit => unit.hp > 0 && unit.side === 'ruler').length, 2);
  assert.ok(receipt.after.units.filter(unit => unit.side === 'ruler').every(unit => unit.hp === 0));
  assert.deepEqual(receipt.effects.map(effect => effect.amount), [2, 2]);
  assert.equal(game.card(id).node, 'gate'); assert.equal(game.run.battlesWon.length, 1);
  const saved = JSON.parse(game.save());
  assert.equal(Object.hasOwn(saved, 'presentation'), false); assert.equal(Object.hasOwn(saved.run, 'presentation'), false);
});

test('the final defeat hit survives exact restoration of every pre-entry run field', () => {
  const { game, id, snapshot } = enter({ enemyAtk: 40 });
  game.battle.units.find(unit => unit.id === id).hp = 1;
  command(game, 'skill', { id: 'light', target: id }); command(game, 'guard');
  const receipt = command(game, 'enemy').presentation;
  assert.equal(receipt.ended, 'defeat'); assert.equal(receipt.effects[0].amount, 6);
  assert.equal(receipt.after.units.find(unit => unit.id === id).hp, 0);
  assert.equal(game.battle, null); assert.equal(game.snapshot, null); assert.deepEqual(game.run, snapshot);
});

test('a lethal basic skill keeps its receipt after card consumption and victory settlement', () => {
  const { game } = enter();
  game.battle.units.find(unit => unit.id === 'enemy-0').hp = 0;
  game.battle.units.find(unit => unit.id === 'enemy-1').hp = 2;
  const receipt = command(game, 'skill', { id: 'cut', target: 'enemy-1' }).presentation;
  assert.equal(receipt.ended, 'victory'); assert.equal(receipt.after.cardUsed, true);
  assert.equal(receipt.effects[0].amount, 2); assert.equal(receipt.effects[0].hpAfter, 0);
  assert.equal(game.battle, null); assert.equal(game.run.skills.discard.includes('cut'), true);
});

test('rain that defeats both sides retains the existing defeat-first outcome and exact rollback', () => {
  const { game, snapshot } = enter();
  for (const unit of game.battle.units) unit.hp = 1;
  const receipt = command(game, 'skill', { id: 'rain' }).presentation;
  assert.equal(receipt.ended, 'defeat'); assert.equal(receipt.effects.length, 3);
  assert.ok(receipt.effects.every(effect => effect.wave === 0 && effect.amount === 1));
  assert.ok(receipt.after.units.every(unit => unit.hp === 0)); assert.deepEqual(game.run, snapshot);
});

test('receipt snapshots and effects cannot mutate live state or one another', () => {
  const { game } = enter();
  const receipt = command(game, 'skill', { id: 'cut', target: 'enemy-0' }).presentation;
  const current = state(game);
  receipt.before.units[0].hp = 0; receipt.after.units[0].hp = 0; receipt.effects[0].hpAfter = -100;
  receipt.after.movement.ids.length = 0; receipt.before.log.push('presentation only');
  receipt.source.id = 'presentation-card'; receipt.source.kind = 'presentation-only'; receipt.actionOwnerId = null;
  assert.equal(state(game), current);
  const frozen = clone(receipt); command(game, 'guard'); assert.deepEqual(receipt, frozen);
});

test('invalid commands and a failure after damage publish no effects and restore the transaction', () => {
  const { game } = enter();
  command(game, 'skill', { id: 'wind', target: game.battle.active });
  for (const [type, payload] of [['attack', { targets: ['enemy-0', 'enemy-0'] }], ['skill', { id: 'cut' }], ['skill', null], ['enemy', {}]]) {
    const before = state(game), result = game.dispatch(type, payload);
    assert.equal(result.ok, false); assert.equal(Object.hasOwn(result, 'presentation'), false); assert.equal(state(game), before);
  }
  const before = state(game), original = game.endAction;
  game.endAction = () => { throw new Error('injected post-damage failure'); };
  const result = game.dispatch('attack', { targets: ['enemy-0'] });
  assert.equal(result.ok, false); assert.equal(Object.hasOwn(result, 'presentation'), false); assert.equal(state(game), before);
  game.endAction = original;
  const next = command(game, 'attack', { targets: ['enemy-1'] }).presentation;
  assert.deepEqual(next.effects.map(effect => effect.targetId), ['enemy-1']);
});

test('exit, fresh starts and ACT actions never return a stale Encounter receipt', () => {
  const { game } = enter(); command(game, 'guard');
  assert.equal(Object.hasOwn(command(game, 'exit'), 'presentation'), false);
  game.start({}); assert.equal(Object.hasOwn(command(game, 'advance'), 'presentation'), false);
  const act = enter({}, 'act').game;
  assert.equal(Object.hasOwn(command(act, 'tab'), 'presentation'), false);
  for (const skill of D.skills) assert.equal(Object.hasOwn(command(act, 'skill', { id: skill.id }), 'presentation'), false);
  const player = act.battle.units.find(unit => unit.side === 'player'), enemy = act.battle.units.find(unit => unit.side === 'ruler');
  const effectsBefore = act.battle.effects.length; act.damage(player, enemy, 3, true, true);
  assert.equal(act.battle.effects.length, effectsBefore + 1);
});

test('every single-target Encounter card rejects missing, unknown, wrong-side, defeated and core targets atomically', () => {
  for (const skill of D.skills.filter(skill => skill.target !== 'all')) {
    for (const invalid of ['missing', 'null', 'unknown', 'wrong-side', 'dead', 'core']) {
      const { game, player, enemies } = enter();
      const target = skill.target === 'ally' ? player : enemies[0];
      const payload = { id: skill.id };
      if (invalid === 'null') payload.target = null;
      if (invalid === 'unknown') payload.target = 'unknown-unit';
      if (invalid === 'wrong-side') payload.target = skill.target === 'ally' ? enemies[0].id : player.id;
      if (invalid === 'dead' || invalid === 'core') {
        payload.target = target.id;
        if (invalid === 'dead') target.hp = 0;
        else target.core = true;
      }
      const before = state(game), result = game.dispatch('skill', payload);
      assert.equal(result.ok, false, skill.id + ': ' + invalid);
      assert.equal(Object.hasOwn(result, 'presentation'), false);
      assert.equal(state(game), before, 'invalid target must not pay, discard, advance or mutate battle');
    }
  }
});

test('all single-target cards affect only the explicitly chosen unit and never attribute the effect to the action owner', () => {
  for (const skill of D.skills.filter(skill => skill.target !== 'all')) {
    const { game, enemies } = enter({}, 'encounter', 2);
    const owner = game.battle.active;
    const target = skill.target === 'ally' ? game.battle.units.find(unit => unit.side === 'player' && unit.id !== owner) : enemies[1];
    if (skill.id === 'light') target.hp -= 10;
    const receipt = command(game, 'skill', { id: skill.id, target: target.id }).presentation;
    assert.equal(receipt.actorId, null); assert.equal(receipt.actionOwnerId, owner);
    assert.deepEqual(receipt.source, { kind: 'card', id: skill.id });
    assert.deepEqual(receipt.effects.map(effect => [effect.sourceId, effect.targetId]), [[null, target.id]]);
    assert.equal(receipt.after.active, owner); assert.equal(receipt.after.cardUsed, true);
    assert.deepEqual(game.run.skills.discard, [skill.id]);
    const before = state(game), again = game.dispatch('skill', { id: skill.id, target: target.id });
    assert.equal(again.ok, false); assert.equal(state(game), before);
  }
});

test('environmental rain is invariant to the action owner attributes and attack while retaining C physical resistance', () => {
  const results = [];
  for (const [attribute, atk] of [[null, 0], ['F', 1], ['D', 20], ['R', 1000]]) {
    const { game, player, enemies } = enter();
    player.attribute = attribute; player.atk = atk; player.def = 0;
    enemies[0].family = 'B'; enemies[0].attribute = 'D'; enemies[0].def = 0;
    enemies[1].attribute = 'F'; enemies[1].def = 0;
    const logLength = game.battle.log.length;
    const receipt = command(game, 'skill', { id: 'rain' }).presentation;
    assert.ok(receipt.effects.every(effect => effect.sourceId === null && effect.kind === 'damage'));
    assert.deepEqual(receipt.effects.map(effect => effect.amount), [1, 2, 1, 1, 2, 1, 1, 2, 1]);
    const hitLogs = receipt.after.log.slice(logLength).filter(line => line.includes(' → '));
    assert.equal(hitLogs.length, 9);
    assert.ok(hitLogs.every(line => line.startsWith(D.skills.find(skill => skill.id === 'rain').name + ' → ')));
    results.push(receipt.effects);
  }
  for (const result of results.slice(1)) assert.deepEqual(result, results[0]);
});

test('a basic-card source never borrows true-damage attribution or modifiers from the current C unit', () => {
  const { game, player, enemies } = enter();
  player.attribute = 'F'; player.atk = 999;
  enemies[1].attribute = 'R'; enemies[1].def = 999;
  const receipt = command(game, 'skill', { id: 'cut', target: enemies[1].id }).presentation;
  assert.equal(receipt.effects[0].amount, 3); assert.equal(receipt.effects[0].sourceId, null);
  assert.ok(receipt.after.log.some(line => line.startsWith(D.skills.find(skill => skill.id === 'cut').name + ' → ' + enemies[1].name)));
  assert.equal(receipt.after.log.some(line => line.startsWith(player.name + ' → ')), false);
});

test('ordinary attacks reject absent targets and keep exactly the selected target list and unit provenance', () => {
  const { game, player } = enter();
  const before = state(game), absent = game.dispatch('attack');
  assert.equal(absent.ok, false); assert.equal(state(game), before); assert.equal(Object.hasOwn(absent, 'presentation'), false);
  const receipt = command(game, 'attack', { targets: ['enemy-1'] }).presentation;
  assert.deepEqual(receipt.source, { kind: 'unit', id: player.id });
  assert.equal(receipt.actorId, player.id); assert.equal(receipt.actionOwnerId, player.id);
  assert.deepEqual(receipt.effects.map(effect => [effect.sourceId, effect.targetId]), [[player.id, 'enemy-1']]);
  assert.equal(receipt.after.units.find(unit => unit.id === 'enemy-0').hp, 16);
});

test('ACT retains its pre-existing fallback target binding independently of explicit Encounter targeting', () => {
  const { game, player, enemies } = enter({}, 'act');
  command(game, 'tab');
  const hp = enemies[0].hp;
  const attack = command(game, 'skill', { id: 'cut', target: 'unknown-act-target' });
  assert.equal(enemies[0].hp, hp - 3); assert.equal(Object.hasOwn(attack, 'presentation'), false);
  command(game, 'skill', { id: 'wind', target: 'unknown-act-target' });
  assert.equal(player.shield, 2);
});
