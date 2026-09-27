"""Rebuild the existing Chen ACT environment from Toon /Game/Render/Map_Main.

Run in UE Editor Python or UnrealEditor-Cmd -run=pythonscript -script=<this file>.
Usage and baseline: Docs/Content/ChenACTRenderLab.md. Safe to run again.
"""
import json
from pathlib import Path
import unreal as u

ROOT = '/Game/Sandbox/ACT/Character/ChenQianyu'
MAP = '/Game/Sandbox/ACT/Training/Maps/L_ChenACT'
TAG = 'PadmaChenRenderLab'
ENV = '/Game/Sandbox/ACT/Training/Environment/ToonReference'
lib = u.EditorAssetLibrary
actors = u.get_editor_subsystem(u.EditorActorSubsystem)
level = u.get_editor_subsystem(u.LevelEditorSubsystem)


def marked_actor(cls, label, location, rotation=u.Rotator()):
    matches = [a for a in actors.get_all_level_actors() if a.get_actor_label() == label]
    assert len(matches) <= 1, 'Duplicate actor: ' + label
    if matches:
        actor = matches[0]
        assert isinstance(actor, cls) and u.Name(TAG) in actor.tags, 'Unowned actor: ' + label
    else:
        actor = actors.spawn_actor_from_class(cls, location, rotation)
        assert actor, label
        actor.tags = [u.Name(TAG)]
        actor.set_actor_label(label)
    actor.set_folder_path('RenderLab')
    return actor


assert lib.does_asset_exist(MAP), 'Move Chen assets to the agreed ACT directory first'
for required in ['SM_Ground', 'M_Ground_Inst', 'M_Ground_01_Inst']:
    assert lib.does_asset_exist(ENV + '/' + required), 'Missing Toon environment dependency: ' + required
assert level.load_level(MAP), MAP
labs = [a for a in actors.get_all_level_actors() if isinstance(a, u.PadmaACTMeleeLab)]
assert len(labs) == 1, 'Expected one existing ACT lab'
lab = labs[0]
definition_before = lab.get_editor_property('character_definition')
targets_before = str(lab.get_editor_property('training_targets'))
lab.set_actor_label('陈千语_ACT木桩与三渲二调试_B面板_F8重置')
lab.set_folder_path('ACT')

# Remove only the known props created by the superseded calibration recipe.
obsolete = {'调试补光_可关闭', '参考球_暗灰_线性0.02', '参考球_中灰_线性0.18', '参考球_浅灰_线性0.8'}
for actor in list(actors.get_all_level_actors()):
    if actor.get_actor_label() in obsolete:
        assert u.Name(TAG) in actor.tags, 'Refuse to remove unowned actor'
        assert actors.destroy_actor(actor)

lights = [a for a in actors.get_all_level_actors() if isinstance(a, u.DirectionalLight) and u.Name(TAG) not in a.tags]
assert len(lights) == 1, 'Expected one existing key light'
key = lights[0]
key.set_actor_label('调试主光_旋转观察明暗')
key.set_folder_path('RenderLab')
key_component = key.get_component_by_class(u.DirectionalLightComponent)
key_component.set_mobility(u.ComponentMobility.MOVABLE)
key_component.set_intensity(6)
key_component.set_light_color(u.LinearColor(1, 1, 1, 1))
key_component.set_editor_property('use_temperature', True)
key_component.set_temperature(6500)
key.set_actor_rotation(u.Rotator(-25.281711, -53.515924, -10.599011), False)

skies = [a for a in actors.get_all_level_actors() if isinstance(a, u.SkyLight)]
assert len(skies) == 1
sky = skies[0]
sky.set_actor_label('调试环境光_Toon灰色Cubemap')
sky.set_folder_path('RenderLab')
sky_component = sky.get_component_by_class(u.SkyLightComponent)
sky_component.set_mobility(u.ComponentMobility.STATIONARY)
sky_component.set_editor_property('source_type', u.SkyLightSourceType.SLS_SPECIFIED_CUBEMAP)
grey_cube = u.load_object(None, '/Engine/EngineResources/GrayLightTextureCube.GrayLightTextureCube')
assert grey_cube, 'Engine grey lighting cubemap is unavailable'
sky_component.set_cubemap(grey_cube)
sky_component.set_intensity(0.2)
sky_component.set_indirect_lighting_intensity(0.5)
sky_component.set_editor_property('volumetric_scattering_intensity', 0.5)
sky_component.set_editor_property('lower_hemisphere_is_black', True)
sky_component.set_editor_property('lower_hemisphere_color', u.LinearColor(0, 0, 0, 1))
sky_component.set_editor_property('real_time_capture', False)
sky_component.recapture_sky()

fog = marked_actor(u.ExponentialHeightFog, '调试背景_Toon浅灰高度雾', u.Vector(150, -510, -30))
fog_component = fog.get_component_by_class(u.ExponentialHeightFogComponent)
fog_component.set_fog_density(0.02)
fog_component.set_fog_height_falloff(0)
fog_component.set_editor_property('fog_inscattering_luminance', u.LinearColor(0.755208, 0.755208, 0.755208, 1))
fog_component.set_fog_max_opacity(1)
fog_component.set_start_distance(0)

pp = marked_actor(u.PostProcessVolume, '调试曝光_固定基准', u.Vector())
pp.set_editor_property('unbound', True)
pp.set_editor_property('priority', 0)
pp.set_editor_property('blend_weight', 1)
# Restore source environment overrides, without importing character rim-light PP.
settings = u.PostProcessSettings()
for name, value in {
    'auto_exposure_method': u.AutoExposureMethod.AEM_MANUAL,
    'auto_exposure_bias': 0.0,
    'auto_exposure_apply_physical_camera_exposure': False,
    'dynamic_global_illumination_method': u.DynamicGlobalIlluminationMethod.LUMEN,
    'reflection_method': u.ReflectionMethod.LUMEN,
}.items():
    settings.set_editor_property('override_' + name, True)
    settings.set_editor_property(name, value)
pp.set_editor_property('settings', settings)

# Retain the arena's existing collider at its original transform. The reference
# geometry is visual only, so it cannot alter landing, sweeps or target collision.
floors = [a for a in actors.get_all_level_actors() if a.get_name() == 'StaticMeshActor_0']
assert len(floors) == 1, 'Expected the original training floor'
floor = floors[0]
floor.set_actor_label('训练碰撞地面_仅碰撞')
floor.set_folder_path('ACT')
floor.get_component_by_class(u.StaticMeshComponent).set_visibility(False)
for label, mesh, material, scale in [
    ('调试地面_Toon细节层', ENV + '/SM_Ground', ENV + '/M_Ground_Inst', 3),
    ('调试地面_Toon大平面', '/Engine/BasicShapes/Plane', ENV + '/M_Ground_01_Inst', 100),
]:
    ground = marked_actor(u.StaticMeshActor, label, u.Vector())
    ground.set_actor_scale3d(u.Vector(scale, scale, scale))
    component = ground.get_component_by_class(u.StaticMeshComponent)
    component.set_static_mesh(lib.load_asset(mesh))
    component.set_material(0, lib.load_asset(material))
    component.set_collision_profile_name('NoCollision')
    component.set_collision_enabled(u.CollisionEnabled.NO_COLLISION)

assert lab.get_editor_property('character_definition') == definition_before
assert str(lab.get_editor_property('training_targets')) == targets_before
assert level.save_current_level()
out = Path(u.Paths.project_dir()).resolve() / 'Artifacts/ChenACTLayout'
out.mkdir(parents=True, exist_ok=True)
report = {
    'map': MAP, 'character_definition': definition_before.get_path_name(),
    'training_targets_unchanged': True, 'training_target_count': len(lab.get_editor_property('training_targets')),
    'key_light_intensity': key_component.get_editor_property('intensity'),
    'exposure': 'Manual, bias 0, physical camera exposure disabled',
    'reference': 'E:/2026ue/Toon/Content/Render/Map_Main.umap',
    'sky_intensity': sky_component.get_editor_property('intensity'),
    'environment_actors': [a.get_actor_label() for a in actors.get_all_level_actors() if u.Name(TAG) in a.tags],
    'character_materials_authored': False,
}
(out / 'render-lab.json').write_text(json.dumps(report, ensure_ascii=False, indent=2), encoding='utf-8')
u.log('CHEN_RENDER_LAB_DONE')
