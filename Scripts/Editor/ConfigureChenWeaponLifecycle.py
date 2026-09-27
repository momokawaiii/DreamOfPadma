"""UE Editor -ExecutePythonScript: configure existing Content weapon lifecycle assets.
Missing assets require explicit offline import/authoring. Existing tuning is retained.
Does not run PIE.
Pass -PadmaValidateOnly on the UE command line to check prerequisites without authoring.
"""
import json
import traceback
from pathlib import Path
import unreal as u

ROOT = '/Game/Sandbox/ACT/Character/ChenQianyu'
OUT = Path(u.Paths.project_saved_dir()).resolve() / 'ChenAuthoring/WeaponLifecycle'
lib = u.EditorAssetLibrary
OWNER = 'ChenWeaponLifecycle20260920'
VALIDATE_ONLY = u.SystemLibrary.parse_param(u.SystemLibrary.get_command_line(), 'PadmaValidateOnly')
report = {'status': 'started', 'validate_only': VALIDATE_ONLY, 'animations': [], 'montages': [], 'materials': []}


def existing_owned(path):
    obj = u.load_asset(path)
    if not obj:
        raise RuntimeError('Missing required Content asset: ' + path + '; restore/import it explicitly before running this script.')
    assert lib.get_metadata_tag(obj, 'PadmaAuthor') == OWNER, 'Unowned asset: ' + path
    return obj


def save(obj):
    assert lib.save_loaded_asset(obj, False), obj.get_path_name()


try:
    OUT.mkdir(parents=True, exist_ok=True)
    mesh = u.load_asset(ROOT + '/Art/Meshes/SK_Chen_FullCharacter_CM')
    if not mesh:raise RuntimeError('Missing required Content mesh: ' + ROOT + '/Art/Meshes/SK_Chen_FullCharacter_CM')
    skeleton = mesh.get_editor_property('skeleton')
    if not skeleton:raise RuntimeError('Missing skeleton on ' + mesh.get_path_name())
    montages = {}
    for name, clip in [('DrawWeapon', 'A_actor_chen_idle_to_battle'), ('SheatheWeapon', 'A_actor_chen_battle_to_idle')]:
        path = ROOT + '/Animation/Sequences/Imported/AS_Chen_' + name + '_CM'
        anim = existing_owned(path)
        if not isinstance(anim, u.AnimSequence):raise RuntimeError('Expected AnimSequence: ' + path)
        if anim.get_editor_property('skeleton') != skeleton:raise RuntimeError('Skeleton mismatch: ' + path)
        report['animations'].append({'asset': path, 'seconds': anim.get_play_length(), 'source': clip})
        path = ROOT + '/Animation/Montages/AM_Chen_' + name
        montage = existing_owned(path)
        if not isinstance(montage, u.AnimMontage):raise RuntimeError('Expected AnimMontage: ' + path)
        if montage.get_editor_property('skeleton') != skeleton:raise RuntimeError('Skeleton mismatch: ' + path)
        segments = [segment for slot in montage.get_editor_property('slot_anim_tracks')
                    for segment in slot.get_editor_property('anim_track').get_editor_property('anim_segments')]
        if not segments:raise RuntimeError('Missing animation segments: ' + path)
        for segment in segments:
            reference = segment.get_editor_property('anim_reference')
            if not reference or reference.get_editor_property('skeleton') != skeleton:
                raise RuntimeError('Missing or incompatible animation reference: ' + path)
        montages[name] = montage
        report['montages'].append(path)
    character = u.load_asset(ROOT + '/AbilitySystem/DA_ACTCharacter_CHEN')
    if not character:raise RuntimeError('Missing required Content character: ' + ROOT + '/AbilitySystem/DA_ACTCharacter_CHEN')
    profile = character.get_editor_property('melee_profile')
    if not profile:raise RuntimeError('Missing melee_profile on ' + character.get_path_name())
    for key in ['draw_weapon_montage', 'sheathe_weapon_montage']:
        reference = profile.get_editor_property(key)
        if not isinstance(reference, u.AnimMontage) or reference.get_editor_property('skeleton') != skeleton:
            raise RuntimeError('Missing or incompatible ' + key + ' on ' + profile.get_path_name())
    weapons = profile.get_editor_property('weapons')
    if not weapons:raise RuntimeError('Missing weapons on ' + profile.get_path_name())
    for index, weapon in enumerate(weapons):
        if not weapon.get_editor_property('mesh'):
            raise RuntimeError('Missing mesh for weapon ' + str(index))
        materials = weapon.get_editor_property('presentation_materials')
        if not materials or any(not material for material in materials):
            raise RuntimeError('Missing authored presentation materials for weapon ' + str(index))
        report['materials'].append({'weapon': index, 'existing': [material.get_path_name() for material in materials]})
    report.update(equipment=profile.get_path_name(), gameplay_tests_run=False,
                  timing={key: profile.get_editor_property(key) for key in ['drawn_hold_seconds', 'stowed_hold_seconds', 'weapon_dissolve_seconds']})
    u.log('Padma preflight PASS ConfigureChenWeaponLifecycle: animations, Montage references, skeletons, weapon meshes and materials validated.')
    if VALIDATE_ONLY:
        report['status'] = 'validated'
        u.log('PadmaValidateOnly: ConfigureChenWeaponLifecycle skipped authoring and asset saves.')
    else:
        world = u.get_editor_subsystem(u.UnrealEditorSubsystem).get_editor_world()
        u.SystemLibrary.execute_console_command(world, 'Padma.ChenActions.ConfigureWeaponMontages')
        for montage in montages.values():
            save(montage)
        save(profile)
        report['status'] = 'saved'
except Exception:
    report.update(status='error', error=traceback.format_exc())
    u.log_error('ConfigureChenWeaponLifecycle FAILED: ' + report['error'])
finally:
    (OUT / ('validate.json' if VALIDATE_ONLY else 'author.json')).write_text(json.dumps(report, indent=2), encoding='utf-8')
if report['status'] == 'error':
    raise RuntimeError(report['error'])
