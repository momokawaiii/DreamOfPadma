"""UE Editor -ExecutePythonScript: apply only Chen stowed mounts and action draw flags.
See TASK-056 for source provenance and the explicit reset boundary.
"""
import json
from pathlib import Path
import unreal as u

def apply():
    project = Path(u.Paths.project_dir()).resolve()
    source = json.loads((project/'Scripts/Editor/ChenWeaponStowSource.json').read_text(encoding='utf-8-sig'))
    character = u.load_asset('/Game/Sandbox/ACT/Character/ChenQianyu/AbilitySystem/DA_ACTCharacter_CHEN')
    assert character, 'Chen character definition missing'
    profile = character.get_editor_property('melee_profile')
    assert profile, 'Chen melee profile missing'
    weapons = list(profile.get_editor_property('weapons'))
    assert len(weapons) == 3, 'Expected independent main sword, offhand sword and scabbard'
    for row in source['weapons']:
        weapon = weapons[row['index']]
        weapon.set_editor_property('stowed_socket', row['bone'])
        tr = u.Transform(location=u.Vector(*row['location_cm']), rotation=u.Quat(*row['quaternion_xyzw']).rotator(), scale=u.Vector(1,1,1))
        weapon.set_editor_property('stowed_transform', tr)
    profile.set_editor_property('weapons', weapons)
    assert u.EditorAssetLibrary.save_loaded_asset(profile, False)
    actions = []
    table = character.get_editor_property('skill_table')
    rows = json.loads(u.DataTableFunctionLibrary.export_data_table_to_json_string(table))
    for row in rows:
        action = u.load_asset(row['Definition'])
        assert action, 'Skill definition missing: '+str(row)
        draw = action.get_editor_property('action_kind') not in [u.PadmaACTActionKind.JUMP, u.PadmaACTActionKind.DODGE, u.PadmaACTActionKind.PERFECT_DODGE]
        action.set_editor_property('draw_weapons', draw)
        assert u.EditorAssetLibrary.save_loaded_asset(action, False)
        actions.append({'path':action.get_path_name(), 'draw_weapons':draw})
    out = project/'Artifacts/ChenQianyu/Actions/WeaponStow'
    out.mkdir(parents=True,exist_ok=True)
    (out/'author.json').write_text(json.dumps({'profile':profile.get_path_name(),'source':source,'actions':actions},indent=2),encoding='utf-8')

if __name__ == '__main__':
    try:
        apply()
    finally:
        u.SystemLibrary.quit_editor()
