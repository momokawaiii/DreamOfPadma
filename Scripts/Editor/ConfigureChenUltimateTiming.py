"""UE Python: apply R's source animation segments, keeping existing notify starts.
Run independently of the full action/gameplay authoring scripts.
"""
import json
from pathlib import Path
import unreal as u

def apply():
    m=u.load_asset('/Game/Sandbox/ACT/Character/ChenQianyu/Animation/Montages/AM_Chen_LieFengShuang')
    assert m
    events=u.AnimationLibrary.get_animation_notify_events(m)
    before=[str(e.get_editor_property('notify_state_class') or e.get_editor_property('notify')) for e in events]
    world=u.EditorLevelLibrary.get_editor_world()
    u.SystemLibrary.execute_console_command(world,'Padma.ChenActions.RetimeUltimate')
    assert abs(u.AnimationLibrary.get_sequence_length(m)-8.8)<.001
    after=[str(e.get_editor_property('notify_state_class') or e.get_editor_property('notify')) for e in u.AnimationLibrary.get_animation_notify_events(m)]
    assert before==after, 'Existing notify objects changed'
    assert u.EditorAssetLibrary.save_loaded_asset(m,False)
    out=Path(u.Paths.project_dir())/'Artifacts/ChenQianyu/Actions/FX/RQRepair'
    out.mkdir(parents=True,exist_ok=True)
    (out/'r-timing.json').write_text(json.dumps(dict(length=8.8,notify_objects_preserved=len(after),
        source_fps=30,segments=[[0,0,77/30,1],[77/30,122/30,133/30,.5],[99/30,133/30,139/30,1.5],[103/30,139/30,10,1]],
        source='chr_0005_chen_ultimate_skill.json',final_contact=103/30),indent=2))

if __name__=='__main__':
    try: apply()
    finally: u.SystemLibrary.quit_editor()
