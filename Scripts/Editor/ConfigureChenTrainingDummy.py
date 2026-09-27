"""Save Chen's editable training defaults without changing combat timing or character Blueprints."""
import unreal as u
import json
from pathlib import Path

try:
    root='/Game/Sandbox/ACT/Character/ChenQianyu'
    skill=u.load_asset(root+'/AbilitySystem/Abilities/DA_Chen_Execution')
    assert skill
    skill.set_editor_property('damage_scale',1.0)
    assert u.EditorAssetLibrary.save_loaded_asset(skill,False)
    level=u.get_editor_subsystem(u.LevelEditorSubsystem)
    assert level.load_level('/Game/Sandbox/ACT/Training/Maps/L_ChenACT')
    labs=[a for a in u.get_editor_subsystem(u.EditorActorSubsystem).get_all_level_actors() if isinstance(a,u.PadmaACTMeleeLab)]
    assert len(labs)==1
    labs[0].set_editor_property('dummy_max_health',10000.0)
    labs[0].set_editor_property('dummy_follows_player',False)
    assert level.save_current_level()
    out=Path(u.Paths.project_dir()).resolve()/'Artifacts/ChenQianyu/Actions/Training'
    out.mkdir(parents=True,exist_ok=True)
    (out/'author.json').write_text(json.dumps({'execution_damage_scale':skill.get_editor_property('damage_scale'),
        'dummy_max_health':labs[0].get_editor_property('dummy_max_health'),'dummy_follows_player':labs[0].get_editor_property('dummy_follows_player')},indent=2),encoding='utf-8')
finally:u.SystemLibrary.quit_editor()
