"""UE Python: seed placed Chen/dummy Blueprints in L_ChenACT from existing Content.

Run once with -run=pythonscript -script=<this file>; reruns validate and retain authored
instances, materials and transforms. No Unity source or Artifacts input is used.
"""
import unreal as u
from pathlib import Path
import shutil

ROOT = '/Game/Sandbox/ACT/Character/ChenQianyu'
MAP = '/Game/Sandbox/ACT/Training/Maps/L_ChenACT'
lib = u.EditorAssetLibrary
actors = u.get_editor_subsystem(u.EditorActorSubsystem)
level = u.get_editor_subsystem(u.LevelEditorSubsystem)
OWNER = 'Padma.SceneParticipants'


def props(obj, **values):
    for key, value in values.items():
        obj.set_editor_property(key, value)


def presentation(**values):
    result = u.PadmaCombatPresentation()
    props(result, **values)
    return result


def blueprint(path, parent, spec):
    if lib.does_asset_exist(path):
        bp = u.load_asset(path)
        assert lib.get_metadata_tag(bp, 'PadmaAuthor') == OWNER, path
    else:
        factory = u.BlueprintFactory()
        factory.set_editor_property('parent_class', parent)
        bp = u.AssetToolsHelpers.get_asset_tools().create_asset(path.rsplit('/', 1)[1], path.rsplit('/', 1)[0], u.Blueprint, factory)
        assert bp, path
        u.BlueprintEditorLibrary.compile_blueprint(bp)
        u.get_default_object(bp.generated_class()).set_editor_property('scene_spec', spec)
        lib.set_metadata_tag(bp, 'PadmaAuthor', OWNER)
        assert lib.save_loaded_asset(bp, False)
    return bp.generated_class()


backup = Path(u.Paths.project_dir()).resolve() / 'Artifacts/ChenSceneParticipants/L_ChenACT.before.umap'
backup.parent.mkdir(parents=True, exist_ok=True)
if not backup.exists():
    shutil.copy2(Path(u.Paths.project_content_dir()) / 'Sandbox/ACT/Training/Maps/L_ChenACT.umap', backup)
assert level.load_level(MAP), MAP
labs = [a for a in actors.get_all_level_actors() if isinstance(a, u.PadmaACTMeleeLab)]
assert len(labs) == 1, 'Expected one ACT lab'
lab = labs[0]
definition = lab.get_editor_property('character_definition')
assert definition, 'Missing character definition'
mesh = lab.get_editor_property('training_dummy_mesh')
assert mesh, 'Import the UE wooden dummy assets first'
player_spec = u.PadmaCombatUnitSpec()
props(player_spec, id='chen-research', definition_id='chen-act-research', display_name='Chen Qianyu',
      health=1000, max_health=1000, attack=20, speed=160, attack_range=180,
      presentation=presentation(act_definition=definition))
player_class = blueprint(ROOT + '/Blueprints/BP_ChenACTPlaced', definition.get_editor_property('character_class'), player_spec)
dummy_spec = u.PadmaCombatUnitSpec()
props(dummy_spec, id='wooden-dummy', player=False, health=1000, max_health=1000, speed=160,
      attack=5, attack_range=180, can_pursue_in_act=False,
      presentation=presentation(model=mesh))
dummy_class = blueprint('/Game/Sandbox/ACT/Training/WoodenDummy/Blueprints/BP_ACTWoodenDummy', u.PadmaCombatUnit, dummy_spec)

player = lab.get_editor_property('scene_player')
if not player:
    player = actors.spawn_actor_from_class(player_class, u.Vector(0, 0, 90))
    props(player, scene_spec=player_spec, tags=[u.Name(OWNER)])
    player.apply_scene_presentation()
    player.set_actor_label('Chen_ACT_Placed_编辑器调材质')
    player.set_folder_path('ACT/Participants')
    lab.set_editor_property('scene_player', player)

targets = list(lab.get_editor_property('scene_targets'))
if not targets:
    for profile in lab.get_editor_property('training_targets'):
        spec = u.PadmaCombatUnitSpec()
        props(spec, id=profile.get_editor_property('id'), definition_id=profile.get_editor_property('id'),
              display_name=profile.get_editor_property('display_name'), player=False,
              health=profile.get_editor_property('max_health'), max_health=profile.get_editor_property('max_health'),
              attack=5, speed=160, attack_range=180, defense=profile.get_editor_property('defense'),
              magic_defense=profile.get_editor_property('magic_defense'), attribute=profile.get_editor_property('attribute'),
              execution_immune=profile.get_editor_property('execution_immune'), can_pursue_in_act=False,
              presentation=presentation(model=mesh))
        offset = profile.get_editor_property('location_offset')
        target = actors.spawn_actor_from_class(dummy_class, u.Vector(offset.x, offset.y, offset.z + 90))
        props(target, scene_spec=spec, tags=[u.Name(OWNER)])
        target.apply_scene_presentation()
        target.get_editor_property('capsule_component').set_capsule_size(22, 88)
        target.get_editor_property('mesh').set_relative_transform(lab.get_editor_property('training_dummy_mesh_transform'), False, True)
        target.set_actor_label('WoodenDummy_' + str(profile.get_editor_property('id')))
        target.set_folder_path('ACT/Participants')
        targets.append(target)
    lab.set_editor_property('scene_targets', targets)
assert len(targets) == 3 and len(set(targets)) == 3
assert all(t and t.get_editor_property('mesh').get_editor_property('skeletal_mesh_asset') for t in [player] + targets)
preview_tag = u.Name('Padma.EditorPreviewInitialized')
for actor in [player] + targets:
    if preview_tag not in actor.tags:
        component = actor.get_editor_property('mesh')
        if actor == player:
            component.set_update_animation_in_editor(True)
        else:
            component.override_animation_data(lab.get_editor_property('training_dummy_hit_animation'), False, False, 0.0, 1.0)
        actor.tags = list(actor.tags) + [preview_tag]
lab.set_editor_property('require_scene_participants', True)
assert level.save_current_level()
u.log('PADMA_SCENE_PARTICIPANTS_SAVED: placed player + 3 targets; no runtime replacement fallback')
