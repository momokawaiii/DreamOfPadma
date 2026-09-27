"""User-requested UE camera tuning; preserve source orbit and lock behavior."""
import json
from pathlib import Path
import unreal as u

TUNING={'mouse_yaw_sensitivity':.6,'min_zoom':.4,'max_zoom':1.2,
        'zoom_step':.1,'zoom_damping':.16,'collision_release_damping':.25}

def apply_tuning(profile):
    for key,value in TUNING.items():profile.set_editor_property(key,value)

def apply():
    profile=u.load_asset('/Game/Sandbox/ACT/Character/ChenQianyu/Presentation/Camera/DA_ACTCamera_CHEN')
    assert profile
    apply_tuning(profile)
    assert u.EditorAssetLibrary.save_loaded_asset(profile,False)
    out=Path(u.Paths.project_dir())/'Artifacts/ChenQianyu/Actions/Camera/InputTuning'
    out.mkdir(parents=True,exist_ok=True)
    (out/'configured.json').write_text(json.dumps(TUNING,indent=2))

if __name__=='__main__':apply()
