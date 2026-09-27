"""UE -ExecutePythonScript: moving dissolve, full dodge input lock and staged plunge.

Leaves standing dissolve and existing damage/FX/landing notifies intact. No PIE.
"""
import json
import sys
import traceback
from pathlib import Path
import unreal as u

ROOT='/Game/Sandbox/ACT/Character/ChenQianyu'
PROJECT=Path(u.Paths.project_dir()).resolve()
OUT=PROJECT/'Artifacts/ChenQianyu/Actions/InputAndPlunge'
report={'status':'started'}
try:
    OUT.mkdir(parents=True,exist_ok=True)
    sys.path.insert(0,str(PROJECT/'Scripts/Editor'))
    from ConfigureChenMovementTransitions import apply_perfect_dodge
    report['perfect_dodge']=apply_perfect_dodge()
    equipment=u.load_asset(ROOT+'/AbilitySystem/DA_Chen_Equipment')
    equipment.set_editor_property('moving_weapon_dissolve_seconds',.3)
    assert u.EditorAssetLibrary.save_loaded_asset(equipment,False)
    skill=u.load_asset(ROOT+'/AbilitySystem/Abilities/DA_Chen_Plunge')
    # UE tuning: reduced from the prototype's 1200 cm/s. Source global 60 m/s
    # is NOT a verified Chen override and is not silently treated as a slower value.
    skill.set_editor_property('plunge_speed',600.0)
    montage=u.load_asset(ROOT+'/Animation/Montages/AM_Chen_Plunge')
    lib=u.AnimationLibrary
    track='PlungeMovement'
    if track in [str(t) for t in lib.get_animation_notify_track_names(montage)]:
        lib.remove_animation_notify_events_by_track(montage,track)
    else:lib.add_animation_notify_track(montage,track)
    # Original chr_0005_chen_plunging_attack_start: Start [0,19)/30, then Loop.
    lib.add_animation_notify_event(montage,track,19/30,u.AnimNotify_PadmaACTPlungeDescend)
    assert u.EditorAssetLibrary.save_loaded_asset(montage,False)
    # Persist the event before enabling suspended gravity; a failed author pass stays playable.
    skill.set_editor_property('plunge_hold_until_notify',True)
    assert u.EditorAssetLibrary.save_loaded_asset(skill,False)
    report.update(status='saved',moving_dissolve_seconds=equipment.get_editor_property('moving_weapon_dissolve_seconds'),
                  standing_dissolve_seconds=equipment.get_editor_property('weapon_dissolve_seconds'),
                  plunge_preparation_seconds=19/30,plunge_speed_cm_s=skill.get_editor_property('plunge_speed'),
                  plunge_speed_provenance='UE reduced-speed tuning; original global 60 m/s is not confirmed Chen override',
                  gameplay_tests_run=False)
except Exception:
    report.update(status='error',error=traceback.format_exc());u.log_error(report['error'])
finally:
    (OUT/'author.json').write_text(json.dumps(report,indent=2),encoding='utf8')
if report['status']=='error':raise RuntimeError(report['error'])
