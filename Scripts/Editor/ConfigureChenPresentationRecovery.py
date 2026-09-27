"""UE -ExecutePythonScript: patch only sheath handoff, perfect-dodge locomotion gates and plunge clearance.

Source evidence: Artifacts/ChenQianyu/Actions/PresentationRecovery; no PIE is started.
"""
import json
import sys
import traceback
from pathlib import Path
import unreal as u

ROOT='/Game/Sandbox/ACT/Character/ChenQianyu'
PROJECT=Path(u.Paths.project_dir()).resolve()
OUT=PROJECT/'Artifacts/ChenQianyu/Actions/PresentationRecovery'
report={'status':'started'}

try:
    OUT.mkdir(parents=True,exist_ok=True)
    sys.path.insert(0,str(PROJECT/'Scripts/Editor'))
    from ConfigureChenMovementTransitions import apply_perfect_dodge
    report['perfect_dodge']=apply_perfect_dodge()
    montage=u.load_asset(ROOT+'/Animation/Montages/AM_Chen_SheatheWeapon')
    releases=[]
    for event in u.AnimationLibrary.get_animation_notify_events(montage):
        notify=event.get_editor_property('notify')
        if isinstance(notify,u.AnimNotify_PadmaACTWeaponPresentation):
            release=notify.get_editor_property('weapon_index')==2 and not notify.get_editor_property('drawn')
            notify.set_editor_property('release_combat_idle',release)
            if release:releases.append(u.AnimationLibrary.get_anim_notify_event_trigger_time(event))
    assert len(releases)==1 and releases[0]<montage.get_play_length()-.3,releases
    assert u.EditorAssetLibrary.save_loaded_asset(montage,False)
    plunge=u.load_asset(ROOT+'/AbilitySystem/Abilities/DA_Chen_Plunge')
    # Source GameplayMiscSetting.minPlungingAttackHeight=0.6 m, transported to UE centimeters.
    plunge.set_editor_property('min_plunge_height',60.0)
    assert u.EditorAssetLibrary.save_loaded_asset(plunge,False)
    report.update(status='saved',neutral_idle_release_seconds=releases[0],min_plunge_height_cm=plunge.get_editor_property('min_plunge_height'),gameplay_tests_run=False)
except Exception:
    report.update(status='error',error=traceback.format_exc());u.log_error(report['error'])
finally:
    (OUT/'author.json').write_text(json.dumps(report,indent=2),encoding='utf8')
if report['status']=='error':raise RuntimeError(report['error'])
