"""UE -ExecutePythonScript: wire the existing Content combat-idle animation.
Missing assets require explicit offline import; authored animation/timing values are retained.
Pass -PadmaValidateOnly on the UE command line to check prerequisites without authoring.
"""
import json
import traceback
from pathlib import Path
import unreal as u

ROOT='/Game/Sandbox/ACT/Character/ChenQianyu'
OUT=Path(u.Paths.project_saved_dir()).resolve()/'ChenAuthoring/WeaponCombatIdle'
lib=u.EditorAssetLibrary
VALIDATE_ONLY=u.SystemLibrary.parse_param(u.SystemLibrary.get_command_line(),'PadmaValidateOnly')
report={'status':'started','validate_only':VALIDATE_ONLY,'validated_assets':[]}

def required_asset(path):
    obj=u.load_asset(path)
    if not obj:raise RuntimeError('Missing required Content asset: '+path+'; restore/import it explicitly before running this script.')
    report['validated_assets'].append(path)
    return obj

try:
    OUT.mkdir(parents=True,exist_ok=True)
    bp_path=ROOT+'/Animation/AnimBlueprints/ABP_ACT_CHEN'
    mesh=required_asset(ROOT+'/Art/Meshes/SK_Chen_FullCharacter_CM')
    skeleton=mesh.get_editor_property('skeleton')
    if not skeleton:raise RuntimeError('Missing skeleton on '+mesh.get_path_name())
    path=ROOT+'/Animation/Sequences/Imported/AS_Chen_CombatIdle_CM'
    anim=required_asset(path)
    if not isinstance(anim,u.AnimSequence):raise RuntimeError('Expected AnimSequence: '+path)
    if anim.get_editor_property('skeleton')!=skeleton:raise RuntimeError('Skeleton mismatch: '+path)
    bp=required_asset(bp_path)
    if not isinstance(bp,u.AnimBlueprint) or bp.get_editor_property('target_skeleton')!=skeleton:
        raise RuntimeError('Expected AnimBlueprint with matching skeleton: '+bp_path)
    neutral=required_asset(ROOT+'/Animation/Sequences/Imported/AS_Chen_IdleBase_CM')
    if not isinstance(neutral,u.AnimSequence) or neutral.get_editor_property('skeleton')!=skeleton:
        raise RuntimeError('Expected neutral idle AnimSequence with matching skeleton: '+neutral.get_path_name())
    character=required_asset(ROOT+'/AbilitySystem/DA_ACTCharacter_CHEN')
    equipment=character.get_editor_property('melee_profile')
    if not equipment:raise RuntimeError('Missing melee_profile on '+character.get_path_name())
    report.update(animation=path,seconds=anim.get_play_length(),frames=u.AnimationLibrary.get_num_frames(anim),
                  source='A_actor_chen_battle_loop',source_sha256=lib.get_metadata_tag(anim,'PadmaSourceSha256'),abp=bp_path,
                  timing={key:equipment.get_editor_property(key) for key in ['drawn_hold_seconds','stowed_hold_seconds','weapon_dissolve_seconds']},
                  gameplay_tests_run=False)
    u.log('Padma preflight PASS ConfigureChenCombatIdle: assets, skeletons, ABP and melee_profile validated.')
    if VALIDATE_ONLY:
        report['status']='validated'
        u.log('PadmaValidateOnly: ConfigureChenCombatIdle skipped authoring and asset saves.')
    else:
        world=u.get_editor_subsystem(u.UnrealEditorSubsystem).get_editor_world()
        u.SystemLibrary.execute_console_command(world,'Padma.ChenActions.ConfigureCombatIdle')
        blend_nodes=list(u.AnimationLibrary.get_nodes_of_class(bp,u.AnimGraphNode_BlendListByBool))
        assert len(blend_nodes)==1,'Combat idle selector missing'
        assert lib.save_loaded_asset(bp,False)
        report['status']='saved'
except Exception:
    report.update(status='error',error=traceback.format_exc());u.log_error('ConfigureChenCombatIdle FAILED: '+report['error'])
finally:
    (OUT/('validate.json' if VALIDATE_ONLY else 'author.json')).write_text(json.dumps(report,indent=2),encoding='utf8')
if report['status']=='error':raise RuntimeError(report['error'])
