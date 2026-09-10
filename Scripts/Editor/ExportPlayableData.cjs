'use strict';
// Export accepted HTML fixtures to UE DataTable JSON. Usage: node Scripts/Editor/ExportPlayableData.cjs [--check] [--out path]
const fs = require('node:fs');
const path = require('node:path');
const D = require('../../Prototypes/HTML/data.js');
const ROOT = '/Game/Padma/MVP/Playable/Definitions';
const PROVENANCE = 'TASK-046 temporary UE Demo / Prototypes/HTML/data.js; user-authorized 2026-09-09, not final balance';
const effects = {
  cut: { Magnitude: 3, RepeatCount: 1, bTrueDamage: true },
  wind: { Magnitude: 2, RepeatCount: 1 },
  mantra: { Magnitude: 1, RepeatCount: 1 },
  light: { Magnitude: 5, RepeatCount: 1 },
  rain: { Magnitude: 2, RepeatCount: 3 }
};
function row(id, fields) { return { Name: id, Id: id, ...fields }; }
function vec(values) { return { X: values?.[0] ?? 0, Y: values?.[1] ?? 0 }; }
function card(c) {
  const conditional = c.id === 'reed';
  return row(c.id, {
    Family: c.family, Attribute: c.attribute, Kind: c.kind || '', DisplayName: c.name,
    Icon: c.icon, SourcePage: c.page, Coverage: c.coverage, ModelId: 'card.' + c.id,
    Cost: c.cost ?? 0, Stability: c.stability ?? 0, Affinity: c.affinity ?? 0,
    DailyFlow: c.dailyFlow ?? 0, DailyLimit: c.dailyLimit ?? 0,
    Attack: c.atk ?? 0, Defense: c.def ?? 0, Health: c.hp ?? 0,
    AttackCost: c.attackCost ?? 0, MaxTargets: c.maxTargets ?? 1,
    Tendency: vec(c.tendency), Field: vec(c.field), Target: vec(c.target), Tags: c.tags,
    Mobility: { bConfigured: true, Policy: conditional ? 'Conditional' : c.id === 'door' ? 'Immovable' : 'Movable',
      ConditionId: conditional ? 'Demo.ExplicitMobility' : '' },
    MinFaith: 50, ForbiddenTerrain: c.id === 'reed' ? '山地' : '', Provenance: PROVENANCE
  });
}
function skill(s, mode) {
  return row(s.id, {
    DisplayName: s.name, Icon: s.icon, SourcePage: s.page, Target: s.target,
    FlowCost: s.flow, CalculationCost: s.calc, CostSource: s.costSource,
    Description: s[mode], EffectId: s.id, bTrueDamage: false, ...effects[s.id],
    bFaithIfUnacted: mode === 'encounter' && s.id === 'light',
    DefinitionId: mode === 'act' ? s.id : '',
    Provenance: PROVENANCE + '; effect values from engine.js apply' + (mode === 'act' ? 'Act' : 'Encounter') + 'Skill'
  });
}
function display(values, domain) {
  return values.map(v => row(v.id, {
    Domain: domain, DisplayName: v.name, Palette: v.palette || '', Hair: v.hair || '',
    Description: v.description || '', CompatiblePoseIds: v.poses || [],
    ModelId: domain + '.' + v.id, Provenance: PROVENANCE
  }));
}
function exportData() {
  const tables = {};
  function table(name, struct, rows) { tables[name] = { rowStruct: {"Card": "PadmaCardDefinitionRow", "Node": "PadmaWorldNodeDefinitionRow", "Edge": "PadmaWorldEdgeDefinitionRow", "Profile": "PadmaBalanceParameterRow", "ACTCharacter": "PadmaACTCharacterDefinitionRow", "ACTWeapon": "PadmaACTWeaponDefinitionRow", "Display": "PadmaAppearanceDefinitionRow", "FPSCharacter": "PadmaFPSCharacterDefinitionRow", "FPSWeapon": "PadmaFPSWeaponDefinitionRow", "Skill": "PadmaSkillEffectRowBase", "EncounterSkill": "PadmaEncounterSkillEffectRow", "ACTSkill": "PadmaACTSkillEffectRow", "Dialogue": "PadmaDialogueLineRow"}[struct], rows }; }
  const cards = Object.values(D.cards);
  for (const family of ['A','B','C']) table('Cards_' + family, 'Card', cards.filter(c => c.family === family).map(card));
  table('Nodes', 'Node', D.nodes.map(n => row(n.id, {
    DisplayName: n.name, Subtitle: n.subtitle, Type: n.type, Terrain: n.terrain,
    Position: { X: n.x, Y: n.y }, Icon: n.icon, InitialOwner: n.owner,
    EnemyCount: n.enemies ?? 0, Description: n.description, Provenance: PROVENANCE,
    Stage: 0, AnchorKind: 'None', NPCId: 'None', DialogueId: 'None', CompletionFlag: 'None',
    bHabitable: true, bTraversable: true, bWilderness: false, Elevation: 0, HexQ: 0, HexR: 0
  })));
  table('Edges', 'Edge', D.edges.map((e,i) => row('edge.' + String(i).padStart(2, '0'), {
    From: e[0], To: e[1], UnlockFlag: e[2] || '', Provenance: PROVENANCE
  })));
  table('Profiles', 'Profile', D.profileFields.map(f => row(f[0], {
    DisplayName: f[1], Value: f[2], Minimum: f[3], Maximum: f[4], Provenance: PROVENANCE
  })));
  table('EncounterSkills', 'EncounterSkill', D.skills.map(s => skill(s, 'encounter')));
  table('ACTSkills', 'ACTSkill', D.skills.map(s => skill(s, 'act')));
  table('ACTCharacters', 'ACTCharacter', D.actCharacters.map(c => row(c.id, {
    DisplayName: c.name, Palette: c.palette, Trait: c.trait, Description: c.description,
    ModelId: 'act.character.' + c.id, DefinitionId: c.id,
    SpeedBonusTerrain: '平原', TraitDisabledTerrain: '山地', TerrainSpeedMultiplier: 1.15,
    Provenance: PROVENANCE + '; explicit LightStep fixture from engine.js'
  })));
  table('ACTWeapons', 'ACTWeapon', D.weapons.map(w => row(w.id, {
    DisplayName: w.name, DamageMultiplier: w.damage, RangeMultiplier: w.range,
    IntervalMultiplier: w.interval, ModelId: 'act.weapon.' + w.id, DefinitionId: w.id, Provenance: PROVENANCE
  })));
  for (const [tableName, domain, values] of [
    ['HomeCharacters','home.character',D.homeCharacters], ['HomeOutfits','home.outfit',D.homeOutfits],
    ['HomeWeapons','home.weapon',D.homeWeapons], ['HomePoses','home.pose',D.homePoses]
  ]) table(tableName, 'Display', display(values, domain));
  table('FPSCharacters', 'FPSCharacter', D.fpsCharacters.map(c => row(c.id, {
    DisplayName: c.name, Palette: c.palette, ModelId: 'fps.character.' + c.id, Provenance: PROVENANCE
  })));
  table('FPSWeapons', 'FPSWeapon', D.fpsWeapons.map(w => row(w.id, {
    DisplayName: w.name, ModelId: 'fps.weapon.' + w.id,
    Coverage: 'UE Demo preparation/catalog only; FPS execution is not implemented.', Provenance: PROVENANCE
  })));
  table('Dialogues', 'Dialogue', Object.entries(D.dialogues).flatMap(([id,d]) =>
    Object.entries(d.lines).map(([line,l]) => row(id + '.' + line, {
      DialogueId: id, LineId: line, Title: d.title, bStart: d.start === line,
      Speaker: l.speaker, Text: l.text, Next: l.next || '',
      Choices: (l.choices || []).map(c => ({
        Id: c.id, Label: c.label, Next: c.next || '', bComplete: !!c.complete, bCancel: !!c.cancel
      }))
    }))));
  return {
    version: 1, sourceVersion: D.version, provenance: PROVENANCE, assetRoot: ROOT,
    order: { cards: cards.map(c => c.id), nodes: D.nodes.map(n => n.id), skills: D.skills.map(s => s.id),
      eras: D.eras, phases: D.phases },
    tables
  };
}
function check(data) {
  for (const [name, table] of Object.entries(data.tables)) {
    if (!table.rows.length) throw new Error('Empty exported table: ' + name);
    const ids = new Set();
    for (const r of table.rows) {
      if (!r.Id || r.Id !== r.Name || ids.has(r.Id)) throw new Error('Invalid duplicate exported identity: ' + name);
      ids.add(r.Id);
    }
  }
  if (Object.keys(D.cards).length !== ['Cards_A','Cards_B','Cards_C'].reduce((n,t) => n + data.tables[t].rows.length, 0))
    throw new Error('Card export coverage mismatch');
  if (D.skills.some(s => !effects[s.id])) throw new Error('New HTML skill needs explicit effect configuration');
  if (data.tables.Profiles.rows.length !== D.profileFields.length) throw new Error('Profile export coverage mismatch');
}
if (require.main === module) {
  const args = process.argv.slice(2), data = exportData();
  check(data);
  if (!args.includes('--check')) {
    const at = args.indexOf('--out');
    if (at >= 0 && !args[at + 1]) throw new Error('--out needs a filename');
    const target = path.resolve(at >= 0 ? args[at + 1] : path.join(__dirname, '../../Artifacts/TASK-046/PlayableData.json'));
    fs.mkdirSync(path.dirname(target), { recursive: true });
    fs.writeFileSync(target, JSON.stringify(data, null, 2) + '\n', 'utf8');
    process.stdout.write('Exported temporary UE Demo content: ' + target + '\n');
  }
  process.stdout.write('Checked ' + Object.keys(data.tables).length + ' tables, ' + data.order.cards.length + ' cards, '
    + data.tables.Profiles.rows.length + ' profile parameters, 5 distinct skills per mode.\n');
}
module.exports = { exportData, check };
