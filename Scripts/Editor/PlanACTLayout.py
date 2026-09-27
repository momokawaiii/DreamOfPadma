"""Offline, read-only Content planning from AuditACTAssets inventory; emits per-file ledger.
Run with Python from any directory. No Unreal asset is moved by this script.
"""
import json
from pathlib import Path

PROJECT = Path(__file__).resolve().parents[2]
OUT = PROJECT / 'Artifacts/ACTFinalLayout'
assert not (OUT/'before-migration.zip').exists(), 'Migration already started; preserve original plan/ledger'
ROOT = '/Game/Sandbox/ACT'
CHEN = ROOT + '/Character/ChenQianyu'
inventory = json.loads((OUT / 'inventory.json').read_text(encoding='utf8'))
retired = {CHEN + '/Attack01/' + n for n in
           ['AM_Chen_Attack01', 'DA_Chen_Attack01', 'DA_Chen_Attack01_Character', 'DT_Chen_ACTSlots']}
retired.add(CHEN + '/Centimeter/BP_Chen_Fuyao_Preview')
art_folders = {'Material': 'Materials', 'MaterialInstanceConstant': 'Materials',
               'MaterialFunction': 'Materials/Functions', 'MaterialParameterCollection': 'Materials/Parameters',
               'Texture2D': 'Textures', 'StaticMesh': 'Meshes', 'SkeletalMesh': 'Meshes',
               'Skeleton': 'Skeletons', 'PhysicsAsset': 'Physics', 'NiagaraSystem': 'Niagara/Systems'}

def destination(a):
    p, c = a['path'], a['class']
    n = p.rsplit('/', 1)[1]
    if p in retired:
        return None, 'Retire: obsolete saved single-attack fixture / preview; recoverable from backup'
    if p.startswith(ROOT + '/Training/WoodenDummy/'):
        base = ROOT + '/Training/WoodenDummy/'
        folder = 'Blueprints' if c == 'Blueprint' else 'Animation/Sequences' if c == 'AnimSequence' else 'Art/' + art_folders[c]
        return base + folder + '/' + n, 'Training target: ' + c
    if '/Rendering/' in p:
        return p.replace(CHEN + '/Rendering', ROOT + '/Training'), 'Authored lighting / ground / calibration'
    if c == 'World':
        return ROOT + '/Training/Maps/' + n, 'Placed Chen, dummy, lab controller, camera and toon environment'
    if n == 'M_LabFloor':
        return ROOT + '/Training/Environment/Materials/' + n, 'Lab floor material still assigned by map'
    if '/Weapons/Fuyao/' in p or n == 'M_Fuyao_Normal_Preview_WeaponDissolve':
        return ROOT + '/Weapon/Fuyao/Art/' + art_folders[c] + '/' + n, 'Fuyao weapon: ' + c
    if c == 'PadmaACTWeaponDefinition':
        return ROOT + '/Weapon/Fuyao/AbilitySystem/' + n, 'Weapon presentation definition (no Buff grant implementation yet)'
    if '/Weapons/ChenAccessories/' in p or n == 'M_ChenAccessories_Preview_WeaponDissolve':
        return CHEN + '/Art/Equipment/' + art_folders[c] + '/' + n, 'Character offhand sword / scabbard: ' + c
    if c == 'AnimBlueprint':
        return CHEN + '/Animation/AnimBlueprints/' + n, 'Locomotion graph and Montage slot'
    if c == 'Blueprint':
        return CHEN + '/Blueprints/' + n, 'Scene-authored character presentation / defaults'
    if c == 'AnimSequence':
        folder = 'Runtime' if n.endswith(('_Runtime', '_IP')) else 'Imported'
        return CHEN + '/Animation/Sequences/' + folder + '/' + n, 'Animation: ' + ('root-motion or in-place runtime variant' if folder == 'Runtime' else 'retained source / direct-use clip')
    if c == 'AnimMontage':
        return CHEN + '/Animation/Montages/' + n, 'Action timeline: animation segments, sections, AN/ANS windows and FX'
    if c == 'PadmaACTCameraDefinition':
        return CHEN + '/Presentation/Camera/' + n, 'Camera tuning'
    if c == 'PadmaACTSkillDefinition':
        return CHEN + '/AbilitySystem/Abilities/' + n, 'GAS action tuning, cost, cooldown, montage and transitions'
    if c.startswith('PadmaACT') or c == 'DataTable':
        return CHEN + '/AbilitySystem/' + n, 'Character ability catalog / binding table / equipment profile: ' + c
    if '/Actions/FX/' in p or '/Attack01/V2/' in p:
        folder = 'Niagara/Systems' if c == 'NiagaraSystem' else 'Niagara/' + art_folders[c]
        # These are independently authored blade-contact variants, not byte-identical duplicates.
        # Keep the distinction semantic rather than a version/history directory.
        if '/Attack01/V2/' in p:
            folder += '/BladeContact'
        return CHEN + '/Art/' + folder + '/' + n, 'Skill VFX resource: ' + c + '; retained exact authored object/settings'
    return CHEN + '/Art/' + art_folders[c] + '/' + n, 'Character rendering resource: ' + c

rows = []
for a in inventory['assets']:
    target, purpose = destination(a)
    rows.append(dict(a, target=target, purpose=purpose, operation='archive' if target is None else 'keep' if target == a['path'] else 'move'))
targets = [r['target'] for r in rows if r['target']]
collisions = {t: [r['path'] for r in rows if r['target'] == t] for t in set(targets) if targets.count(t) > 1}
assert not collisions, 'Destination collisions: ' + json.dumps(collisions)
for a in rows:
    if a['path'] in retired:
        assert all(r in retired for r in a['referencers']), 'Retirement has retained inbound reference: ' + a['path']
file_packages = {'/Game/' + Path(f['file']).relative_to('Content').with_suffix('').as_posix() for f in inventory['files']}
assert file_packages == {a['path'] for a in rows}, 'Inventory/file coverage mismatch'
(OUT / 'plan.json').write_text(json.dumps({'assets': rows, 'files': inventory['files']}, ensure_ascii=False, indent=2), encoding='utf8')
lines = ['# ACT per-file migration ledger', '', 'Every source package was loaded by UE. References include hard/soft/package-management dependencies.',
         'No identical-file or semantic-equivalence assumption is used to merge imported/runtime clips or VFX resources.', '',
         '| Source package | Type / purpose | Disposition / target | Inbound references |', '|---|---|---|---|']
for r in rows:
    lines.append('| `' + r['path'] + '` | ' + r['purpose'] + ' | ' + r['operation'] + ': `' + str(r['target'] or 'backup only') + '` | ' + str(len(r['referencers'])) + ' |')
(OUT / 'AssetLedger.md').write_text('\n'.join(lines) + '\n', encoding='utf8')
print(json.dumps({'assets': len(rows), 'moves': sum(r['operation'] == 'move' for r in rows), 'archive': len(retired)}))
