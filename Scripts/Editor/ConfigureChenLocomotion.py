"""Author Chen root-motion variants and Idle/Walk/Run graph from existing UE assets.
Missing clips require explicit offline import; never reads an extraction cache.
"""
import unreal as u
import json, shutil, traceback
from pathlib import Path
root='/Game/Sandbox/ACT/Character/ChenQianyu'
dest=root+'/Animation/Sequences/Imported'
lib=u.EditorAssetLibrary
out=Path(u.Paths.project_dir()).resolve()/'Artifacts/ChenQianyu/Actions/Locomotion'
out.mkdir(parents=True,exist_ok=True)
def props(obj,**values):
    for k,v in values.items():obj.set_editor_property(k,v)
def copy(source,target,root_motion=False):
    a=lib.load_asset(target) if lib.does_asset_exist(target) else lib.duplicate_asset(source,target)
    assert a,(source,target)
    props(a,enable_root_motion=root_motion,force_root_lock=True,root_motion_root_lock=u.RootMotionRootLock.ANIM_FIRST_FRAME)
    assert lib.save_loaded_asset(a,False)
    return a
report={}
try:
    backup=out/'ABP_ACT_CHEN.before.uasset'
    if not backup.exists():shutil.copy2(Path(u.Paths.project_content_dir())/'Sandbox/ACT/Character/ChenQianyu/Animation/AnimBlueprints/ABP_ACT_CHEN.uasset',backup)
    path=dest+'/AS_Chen_RunBase_CM'
    if not lib.does_asset_exist(path):
        raise RuntimeError('Missing required Content animation: '+path+'; restore/import it explicitly before configuring locomotion.')
    run=lib.load_asset(path)
    assert u.AnimationLibrary.get_num_frames(run)==40
    assert abs(run.get_play_length()-2/3)<1e-4
    runtime=root+'/Animation/Sequences/Runtime'
    copy(path,runtime+'/AS_Chen_RunBase_IP')
    copy(root+'/Animation/Sequences/Imported/AS_Chen_WalkBase_CM',runtime+'/AS_Chen_WalkBase_IP')
    names=['Attack01','Attack02','Attack03','Attack04','Attack05','GuiQiongYu','JianTianHe','LieFengShuang','Execution','Dodge','PerfectDodge','Plunge','Jump']
    # Walk segments through reflection, using the sequences referenced by each Montage.
    variants={}
    for name in names:
        m=lib.load_asset(root+'/Animation/Montages/AM_Chen_'+name)
        for slot in m.get_editor_property('slot_anim_tracks'):
            for segment in slot.get_editor_property('anim_track').get_editor_property('anim_segments'):
                source=segment.get_editor_property('anim_reference')
                base=source.get_name().removesuffix('_CM').removesuffix('_Runtime')
                target=runtime+'/'+base+'_Runtime'
                rm=name not in ['Dodge','PerfectDodge','Plunge','Jump']
                a=copy(source.get_path_name(),target,rm)
                variants[target]=rm
    u.SystemLibrary.execute_console_command(None,'Padma.ChenActions.ConfigureLocomotion')
    bp=lib.load_asset(root+'/Animation/AnimBlueprints/ABP_ACT_CHEN')
    assert lib.save_loaded_asset(bp,False)
    for name in names:assert lib.save_loaded_asset(lib.load_asset(root+'/Animation/Montages/AM_Chen_'+name),False)
    d=lib.load_asset(root+'/AbilitySystem/DA_ACTCharacter_CHEN')
    d.set_editor_property('run_speed',450.0)
    assert lib.save_loaded_asset(d,False)
    report.update(status='saved',variants=variants,run_frames=40,run_speed=d.get_editor_property('run_speed'),walk_speed=d.get_editor_property('walk_speed'))
except Exception:
    report.update(status='error',error=traceback.format_exc());u.log_error(report['error'])
finally:
    (out/'author.json').write_text(json.dumps(report,indent=2),encoding='utf-8')
    u.SystemLibrary.quit_editor()
