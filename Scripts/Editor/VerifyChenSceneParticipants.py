"""Read-only fresh UE process check of saved map, Blueprint defaults and package dependencies.
Usage: UnrealEditor-Cmd <uproject> -run=pythonscript -script=<this file> -NullRHI -unattended.
"""
import json
from pathlib import Path
import unreal as u

ROOT = '/Game/Sandbox/ACT/Character/ChenQianyu'
MAP = '/Game/Sandbox/ACT/Training/Maps/L_ChenACT'
assert u.get_editor_subsystem(u.LevelEditorSubsystem).load_level(MAP)
actors = u.get_editor_subsystem(u.EditorActorSubsystem).get_all_level_actors()
labs = [a for a in actors if isinstance(a, u.PadmaACTMeleeLab)]
assert len(labs) == 1
lab = labs[0]
assert lab.get_editor_property('require_scene_participants')
player = lab.get_editor_property('scene_player')
targets = list(lab.get_editor_property('scene_targets'))
assert player and len(targets) == 3 and len(set(targets + [player])) == 4
participants = targets + [player]
assert len([a for a in actors if isinstance(a, u.PadmaCombatUnit)]) == 4
rows = []
for actor in participants:
    mesh = actor.get_editor_property('mesh')
    assert mesh.get_editor_property('skeletal_mesh_asset')
    assert mesh.get_editor_property('visible')
    spec = actor.get_editor_property('scene_spec')
    assert str(spec.get_editor_property('id')) != 'None'
    cdo = u.get_default_object(actor.get_class())
    assert str(cdo.get_editor_property('scene_spec').get_editor_property('id')) != 'None', 'Blueprint SceneSpec did not persist'
    rows.append({'actor': actor.get_path_name(), 'class': actor.get_class().get_path_name(),
                 'mesh': mesh.get_editor_property('skeletal_mesh_asset').get_path_name(),
                 'transform': str(actor.get_actor_transform()), 'mesh_transform': str(mesh.get_relative_transform()),
                 'materials': [m.get_path_name() if m else None for m in mesh.get_materials()]})
weapons = [c for c in player.get_components_by_class(u.StaticMeshComponent)
           if any(str(t).startswith('Padma.SceneWeapon.') for t in c.component_tags)]
definition = player.get_editor_property('scene_spec').get_editor_property('presentation').get_editor_property('act_definition')
assert len(weapons) == len(definition.get_editor_property('melee_profile').get_editor_property('weapons'))
assert player.get_editor_property('mesh').get_editor_property('anim_class')
actor_subsystem = u.get_editor_subsystem(u.EditorActorSubsystem)
for template in (player, targets[0]):
    # A newly dropped Blueprint must be visible too, not just the migrated instances.
    probe = actor_subsystem.spawn_actor_from_class(template.get_class(), u.Vector(0, 0, -10000))
    try:
        assert probe.get_editor_property('mesh').get_editor_property('skeletal_mesh_asset'), 'New Blueprint placement is empty'
    finally:
        actor_subsystem.destroy_actor(probe)
registry = u.AssetRegistryHelpers.get_asset_registry()
options = u.AssetRegistryDependencyOptions()
pending = [MAP]
seen = set()
while pending:
    package = pending.pop()
    if package in seen:
        continue
    seen.add(package)
    assert 'artifacts' not in package.lower() and 'unity' not in package.lower(), package
    if package.startswith('/Game/'):
        pending.extend(str(p) for p in registry.get_dependencies(package, options))
out = Path(u.Paths.project_dir()).resolve() / 'Artifacts/ChenSceneParticipants'
if '-PadmaFinalLayoutCapture' in u.SystemLibrary.get_command_line():
    out = Path(u.Paths.project_dir()).resolve() / 'Artifacts/ACTFinalLayout'
out.mkdir(parents=True, exist_ok=True)
(out / 'verified.json').write_text(json.dumps({'status': 'passed', 'participants': rows,
    'weapon_count': len(weapons), 'package_dependency_count': len(seen), 'packages': sorted(seen),
    'editor_world_only': True, 'gpu_visual_acceptance': False}, ensure_ascii=False, indent=2), encoding='utf8')
u.log('PADMA_SCENE_PARTICIPANTS_VERIFIED: saved actors, Blueprint defaults, weapons and dependency graph')

# Optional rendered editor-world proof, never starts PIE and never saves viewport changes.
# Use UnrealEditor -ExecCmds="py <this file>" -PadmaCapturePreview -RenderOffscreen.
if '-PadmaCapturePreview' in u.SystemLibrary.get_command_line():
    import time
    editor = u.get_editor_subsystem(u.UnrealEditorSubsystem)
    location = u.Vector(500, -680, 330)
    editor.set_level_viewport_camera_info(location, u.MathLibrary.find_look_at_rotation(location, u.Vector(130, 20, 95)))
    started = time.monotonic()
    capture = None
    def tick(delta):
        global capture
        elapsed = time.monotonic() - started
        if capture is None and elapsed > 20:
            capture = u.AutomationLibrary.take_high_res_screenshot(1600, 900, str(out / 'editor-preview.png'))
        if elapsed > 30:
            u.unregister_slate_post_tick_callback(handle)
            u.SystemLibrary.quit_editor()
    handle = u.register_slate_post_tick_callback(tick)
