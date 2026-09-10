"""UE Python commandlet: create model/scene preview assets; never overwrite existing content."""
import unreal

ROOT = "/Game/Padma/MVP/Presentation/ModelPreview"
MAP = ROOT + "/L_ModelAuthoringPreview"
tools = unreal.AssetToolsHelpers.get_asset_tools()


def data(name, cls, fields):
    path = ROOT + "/" + name
    if unreal.EditorAssetLibrary.does_asset_exist(path):
        existing = unreal.load_asset(path)
        if not isinstance(existing, cls):
            raise RuntimeError("Wrong existing asset type: " + path)
        return existing, False
    factory = unreal.DataAssetFactory()
    factory.set_editor_property("data_asset_class", cls)
    asset = tools.create_asset(name, ROOT, cls, factory)
    if not asset:
        raise RuntimeError("Cannot create: " + path)
    for key, value in fields.items():
        asset.set_editor_property(key, value)
    if not unreal.EditorAssetLibrary.save_loaded_asset(asset):
        raise RuntimeError("Cannot save: " + path)
    return asset, True


def key(domain, identity):
    value = unreal.PadmaPresentationKey()
    value.set_editor_property("domain", domain)
    value.set_editor_property("definition_id", identity)
    return value


def entry(domain, identity, field, asset):
    value = unreal.PadmaPresentationCatalogEntry()
    value.set_editor_property("key", key(domain, identity))
    value.set_editor_property(field, asset)
    return value


def transform(location=(0, 0, 0), scale=(1, 1, 1)):
    value = unreal.Transform()
    value.set_editor_property("translation", unreal.Vector(*location))
    value.set_editor_property("scale3d", unreal.Vector(*scale))
    return value


character, _ = data("DA_Model_Character_Template", unreal.PadmaModelDefinition, {
    "visual_definition_id": "Visual.Template.Character",
    "display_name": "待导入角色模型",
})
building, _ = data("DA_Model_Building_Placeholder", unreal.PadmaModelDefinition, {
    "visual_definition_id": "Visual.Preview.Building",
    "display_name": "工事模型占位",
    "static_model": unreal.load_asset("/Engine/BasicShapes/Cube"),
})
ground, _ = data("DA_Model_Ground_Placeholder", unreal.PadmaModelDefinition, {
    "visual_definition_id": "Visual.Preview.Ground",
    "display_name": "地表模型占位",
    "static_model": unreal.load_asset("/Engine/BasicShapes/Plane"),
})
garrison, _ = data("DA_Model_Garrison_Placeholder", unreal.PadmaModelDefinition, {
    "visual_definition_id": "Visual.Preview.Garrison",
    "display_name": "驻军模型占位",
    "static_model": unreal.load_asset("/Engine/BasicShapes/Sphere"),
})
source = unreal.PadmaPresentationSource
catalog, catalog_created = data("DA_ModelPreview_Catalog", unreal.PadmaPresentationCatalog, {
    "entries": [
        entry(source.MODEL, str(garrison.get_editor_property("visual_definition_id")), "model", garrison),
        entry(source.MODEL, str(building.get_editor_property("visual_definition_id")), "model", building),
        entry(source.MODEL, str(ground.get_editor_property("visual_definition_id")), "model", ground),
        entry(source.MODEL, str(character.get_editor_property("visual_definition_id")), "model", character),
    ]
})

slots = []
# These are preview geometry, not movement distance, gameplay scale or node layout.
for identity, place in [
    ("Garrison", transform((0, 0, 110), (0.9, 0.9, 2.2))),
    ("Building", transform((260, 180, 90), (2.4, 2.4, 1.8))),
    ("Ground", transform((0, 0, 0), (14, 12, 1))),
]:
    value = unreal.PadmaSceneSlot()
    value.set_editor_property("slot_id", identity)
    value.set_editor_property("transform", place)
    slots.append(value)
scene, scene_created = data("DA_NodeScene_Preview", unreal.PadmaNodeSceneDefinition, {
    "scene_definition_id": "Scene.Preview.Home",
    "slots": slots,
})
map_created = not unreal.EditorAssetLibrary.does_asset_exist(MAP)
if map_created:
    levels = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
    actors = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    if not levels.new_level(MAP):
        raise RuntimeError("Cannot create preview map")
    world = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_editor_world()
    world.get_world_settings().set_editor_property("default_game_mode", unreal.PadmaModelPreviewGameMode)
    preview = actors.spawn_actor_from_class(unreal.PadmaNodePreview, unreal.Vector())
    preview.set_actor_label("Home - Model Authoring Preview")
    preview.set_editor_property("preview_node_id", "Node.Preview.Home")
    preview.set_editor_property("scene_definition", scene)
    preview.set_editor_property("catalog", catalog)
    bindings = []
    definitions = [
        ("Garrison", "Preview.Instance.Garrison", source.MODEL, str(garrison.get_editor_property("visual_definition_id"))),
        ("Building", "Preview.Instance.Building", source.MODEL, str(building.get_editor_property("visual_definition_id"))),
        ("Ground", "Preview.Instance.Ground", source.MODEL, str(ground.get_editor_property("visual_definition_id"))),
    ]
    for slot, instance, domain, identity in definitions:
        binding = unreal.PadmaPreviewBinding()
        binding.set_editor_property("slot_id", slot)
        binding.set_editor_property("entity_instance_id", instance)
        binding.set_editor_property("definition", key(domain, identity))
        bindings.append(binding)
    preview.set_editor_property("preview_bindings", bindings)
    actors.spawn_actor_from_class(unreal.DirectionalLight, unreal.Vector(0, 0, 500), unreal.Rotator(-55, -30, 0))
    actors.spawn_actor_from_class(unreal.SkyLight, unreal.Vector(0, 0, 300))
    actors.spawn_actor_from_class(unreal.PlayerStart, unreal.Vector(0, -500, 250))
    if not levels.save_current_level():
        raise RuntimeError("Cannot save preview map")
    if scene_created:
        scene.set_editor_property("level", world)
        if not unreal.EditorAssetLibrary.save_loaded_asset(scene):
            raise RuntimeError("Cannot save scene level reference")
# Re-run validation on the saved, current user-authored map every time.
# Loading/rebuilding happens in this commandlet process; do not save its transient Actors.
levels = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
actors = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
if not levels.load_level(MAP):
    raise RuntimeError("Cannot load existing preview map")
previews = [actor for actor in actors.get_all_level_actors() if isinstance(actor, unreal.PadmaNodePreview)]
if len(previews) != 1:
    raise RuntimeError("Preview map must contain exactly one PadmaNodePreview host")
preview = previews[0]
if not preview.rebuild_preview():
    raise RuntimeError("Preview failed to resolve: " + str(preview.get_editor_property("diagnostics")))
if map_created and len(preview.get_editor_property("presenters")) != 3:
    raise RuntimeError("New fixture expected three native projection Actors")
if catalog_created:
    result = catalog.resolve(key(source.MODEL, str(garrison.get_editor_property("visual_definition_id"))), "None")
    if not result.get_editor_property("visual_ready"):
        raise RuntimeError("Garrison model failed to resolve")
unreal.log("[TASK-040] Model assets and native node preview available. Existing assets preserved; new_map=" + str(map_created))
