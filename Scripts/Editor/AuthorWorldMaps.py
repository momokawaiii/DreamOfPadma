"""Create the anchored map authoring kit; preserve existing assets on rerun.
Run after building DreamOfPadmaEditor. See Docs/Content/WorldMapAuthoring.md.
"""
import json
import os
import re
import shutil
import unreal

ROOT = "/Game/Padma/World"
PLAYABLE = "/Game/Padma/MVP/Playable/Definitions"
PROJECT = unreal.Paths.convert_relative_path_to_full(unreal.Paths.project_dir())
EVIDENCE = os.path.join(PROJECT, "Artifacts", "TASK-048")
os.makedirs(EVIDENCE, exist_ok=True)
TOOLS = unreal.AssetToolsHelpers.get_asset_tools()
created = []


def save(asset):
    if not unreal.EditorAssetLibrary.save_loaded_asset(asset):
        raise RuntimeError("Cannot save " + asset.get_path_name())


def value(cls, **fields):
    result = cls()
    for key, field in fields.items():
        result.set_editor_property(key, field)
    return result


def display_text(raw):
    # DataTable export writes FText history syntax; Python string assignment is literal.
    text = str(raw)
    if text.startswith(("NSLOCTEXT(", "LOCTEXT(", "INVTEXT(")):
        strings = re.findall(r'"(?:\\.|[^"\\])*"', text)
        if strings:
            return json.loads(strings[-1])
    return text


def data(folder, name, cls, fields):
    path = folder + "/" + name
    if unreal.EditorAssetLibrary.does_asset_exist(path):
        result = unreal.load_asset(path)
        if not isinstance(result, cls):
            raise RuntimeError("Wrong existing asset class: " + path)
        return result, False
    factory = unreal.DataAssetFactory()
    factory.set_editor_property("data_asset_class", cls)
    result = TOOLS.create_asset(name, folder, cls, factory)
    if not result:
        raise RuntimeError("Cannot create " + path)
    for key, field in fields.items():
        result.set_editor_property(key, field)
    created.append(path)
    return result, True


def table(name, struct, rows):
    folder = ROOT + "/Maps/Story"
    path = folder + "/" + name
    expected = unreal.load_object(None, "/Script/DreamOfPadma." + struct)
    if unreal.EditorAssetLibrary.does_asset_exist(path):
        result = unreal.load_asset(path)
        if not isinstance(result, unreal.DataTable) or result.get_editor_property("row_struct") != expected:
            raise RuntimeError("Wrong table schema: " + path)
        return result
    factory = unreal.DataTableFactory()
    factory.set_editor_property("struct", expected)
    result = TOOLS.create_asset(name, folder, unreal.DataTable, factory)
    if not unreal.DataTableFunctionLibrary.fill_data_table_from_json_string(result, json.dumps(rows, ensure_ascii=False)):
        raise RuntimeError("Table import failed: " + path)
    save(result)
    created.append(path)
    return result


checkpoints = table("DT_FirstRegion_Checkpoints", "PadmaStoryCheckpointRow", [{
    "Name": "watcher-gate", "Id": "watcher-gate", "NodeId": "gate", "NPCId": "watcher",
    "bRequireCompletion": True, "MatchedFlag": "watcher-aid", "MissedFlag": "watcher-absent",
    "MatchedDialogueId": "gate-met", "MissedDialogueId": "gate-missed"
}])
dialogues = table("DT_FirstRegion_Branches", "PadmaDialogueLineRow", [{
    "Name": identity + ".start", "Id": identity + ".start", "DialogueId": identity,
    "LineId": "start", "Title": "关隘 · 人物线索（试玩剧情）", "bStart": True,
    "Speaker": "旅途记录", "Text": text, "Next": "None",
    "Choices": [{"Id": "finish", "Label": "记录线索，继续旅途", "Next": "None", "bComplete": True, "bCancel": False}]
} for identity, text in [
    ("gate-met", "你曾完成守望人的嘱托。他留下的线索为此处的主线提供了另一种可能。此分支已在抵达关隘时记录。"),
    ("gate-missed", "抵达关隘时，你尚未完成守望人的嘱托。旅途沿另一条线索继续。之前绕路并不会提前判定错过。")
]])

catalog = unreal.load_asset(PLAYABLE + "/DA_PlayableCatalog")
if not catalog:
    raise RuntimeError("Create the playable content before authoring maps.")


def rows_from_tables(property_name):
    rows = []
    for ref in catalog.get_editor_property(property_name):
        loaded = ref if isinstance(ref, unreal.DataTable) else unreal.load_asset(str(ref))
        rows.extend(json.loads(unreal.DataTableFunctionLibrary.export_data_table_to_json_string(loaded)))
    return rows


map_asset, new_map = data(ROOT + "/Maps/Definitions", "DA_FirstRegion_Map", unreal.PadmaWorldMapAsset, {})
if new_map:
    nodes = []
    for row in rows_from_tables("node_tables"):
        position = row["Position"]
        node = value(unreal.PadmaWorldNodeDefinitionRow, id=row["Id"], display_name=display_text(row["DisplayName"]),
                     subtitle=display_text(row["Subtitle"]), type=row["Type"], terrain=row["Terrain"],
                     position=unreal.Vector2D(position["X"], position["Y"]), icon=row["Icon"],
                     initial_owner=row["InitialOwner"], enemy_count=row["EnemyCount"], description=display_text(row["Description"]))
        if row["Id"] == "story":
            node.set_editor_property("anchor_kind", unreal.PadmaMapAnchorKind.MAIN_STORY)
            node.set_editor_property("npc_id", "watcher")
            node.set_editor_property("dialogue_id", "story")
            node.set_editor_property("completion_flag", "story-road")
        if row["Id"] == "forge":
            node.set_editor_property("anchor_kind", unreal.PadmaMapAnchorKind.FIXED_NPC)
            node.set_editor_property("npc_id", "smith")
            node.set_editor_property("dialogue_id", "forge")
        nodes.append(node)
    edges = [value(unreal.PadmaWorldEdgeDefinitionRow, id=r["Id"], from_=r["From"], to=r["To"], unlock_flag=r["UnlockFlag"])
             for r in rows_from_tables("edge_tables")]
    layout = value(unreal.PadmaMapLayout, map_id="first-region", seed=12345, home_node="home", boss_node="boss", nodes=nodes, edges=edges)
    settings = value(unreal.PadmaMapGenerationSettings, branch_count_min=1, branch_count_max=3,
                     branch_length_min=1, branch_length_max=1, branch_spread=120.0,
                     terrain_choices=["平原", "林地", "丘陵"])
    map_asset.set_editor_property("template", layout)
    map_asset.set_editor_property("generation_settings", settings)
    map_asset.set_editor_property("checkpoint_tables", [checkpoints])
    map_asset.import_checkpoint_tables()
    if not map_asset.get_editor_property("last_validation_succeeded"):
        raise RuntimeError(str(map_asset.get_editor_property("validation_error")))
    map_asset.generate()
# Repair only literal localization history accidentally imported by the first script version.
for property_name in ("template", "generated_layout"):
    layout = map_asset.get_editor_property(property_name)
    nodes = list(layout.get_editor_property("nodes"))
    for node in nodes:
        for field in ("display_name", "subtitle", "description"):
            raw = node.get_editor_property(field)
            if display_text(raw) != str(raw):
                node.set_editor_property(field, display_text(raw))
    layout.set_editor_property("nodes", nodes)
    map_asset.set_editor_property(property_name, layout)
map_asset.validate()
if not map_asset.get_editor_property("last_validation_succeeded"):
    raise RuntimeError(str(map_asset.get_editor_property("validation_error")))
save(map_asset)

material_path = ROOT + "/Maps/Presentation/M_MapFoliage"
foliage = unreal.load_asset(material_path) if unreal.EditorAssetLibrary.does_asset_exist(material_path) else None
if not foliage:
    foliage = TOOLS.create_asset("M_MapFoliage", ROOT + "/Maps/Presentation", unreal.Material, unreal.MaterialFactoryNew())
    color = unreal.MaterialEditingLibrary.create_material_expression(foliage, unreal.MaterialExpressionConstant3Vector)
    color.set_editor_property("constant", unreal.LinearColor(.085, .24, .13, 1))
    unreal.MaterialEditingLibrary.connect_material_property(color, "", unreal.MaterialProperty.MP_BASE_COLOR)
    unreal.MaterialEditingLibrary.recompile_material(foliage)
    save(foliage)
if not foliage.get_editor_property("used_with_instanced_static_meshes"):
    foliage.set_editor_property("used_with_instanced_static_meshes", True)
    unreal.MaterialEditingLibrary.recompile_material(foliage)
    save(foliage)

vertex_path = ROOT + "/Maps/Presentation/M_MapVertexColor"
vertex_material = unreal.load_asset(vertex_path) if unreal.EditorAssetLibrary.does_asset_exist(vertex_path) else None
if not vertex_material:
    vertex_material = TOOLS.create_asset("M_MapVertexColor", ROOT + "/Maps/Presentation", unreal.Material, unreal.MaterialFactoryNew())
    vertex = unreal.MaterialEditingLibrary.create_material_expression(vertex_material, unreal.MaterialExpressionVertexColor)
    # UE 5.8 vertex-color first output is unnamed; RGB silently fails to connect.
    if not unreal.MaterialEditingLibrary.connect_material_property(vertex, "", unreal.MaterialProperty.MP_BASE_COLOR):
        raise RuntimeError("Cannot connect map vertex colors")
    fill = unreal.MaterialEditingLibrary.create_material_expression(vertex_material, unreal.MaterialExpressionMultiply)
    fill.set_editor_property("const_b", .22)
    unreal.MaterialEditingLibrary.connect_material_expressions(vertex, "", fill, "A")
    unreal.MaterialEditingLibrary.connect_material_property(fill, "", unreal.MaterialProperty.MP_EMISSIVE_COLOR)
    roughness = unreal.MaterialEditingLibrary.create_material_expression(vertex_material, unreal.MaterialExpressionConstant)
    roughness.set_editor_property("r", .9)
    unreal.MaterialEditingLibrary.connect_material_property(roughness, "", unreal.MaterialProperty.MP_ROUGHNESS)
    unreal.MaterialEditingLibrary.recompile_material(vertex_material)
    save(vertex_material)

theme, new_theme = data(ROOT + "/Maps/Presentation", "DA_FirstRegion_Theme", unreal.PadmaMapVisualTheme, {})
if new_theme:
    terrains = [value(unreal.PadmaTerrainVisual, terrain_id=name, color=unreal.LinearColor(*color, 1), height=height)
                for name, color, height in [("城镇", (.32,.35,.27), 24), ("平原", (.24,.38,.24), 14),
                                            ("林地", (.13,.28,.19), 26), ("丘陵", (.39,.35,.24), 55), ("山地", (.34,.38,.40), 90)]]
    bindings = []
    for node in map_asset.get_editor_property("template").get_editor_property("nodes"):
        identity = str(node.get_editor_property("id"))
        binding = value(unreal.PadmaMapNodeVisualBinding, node_id=identity)
        if identity in ("story", "forge"):
            model, _ = data(ROOT + "/Maps/Models", "DA_MapNPC_" + identity, unreal.PadmaModelDefinition, {
                "visual_definition_id": "map.npc." + identity, "display_name": "地图人物模型 · " + identity,
                "static_model": unreal.load_asset("/Engine/BasicShapes/Cylinder"),
                "model_transform": unreal.Transform(scale=unreal.Vector(.25,.25,.9))})
            save(model)
            binding.set_editor_property("npc_model", model)
        bindings.append(binding)
    theme.set_editor_property("terrain_visuals", terrains)
    theme.set_editor_property("node_bindings", bindings)
    theme.set_editor_property("vertex_color_material", vertex_material)
    theme.set_editor_property("decoration_meshes", [unreal.load_asset("/Engine/BasicShapes/Cone")])
    theme.set_editor_property("decoration_material", foliage)
    theme.set_editor_property("decoration_count", 200)
    save(theme)

old_vertex = theme.get_editor_property("vertex_color_material")
if not old_vertex or "MVP/Playable/Presentation/M_PadmaVertexColor" in str(old_vertex):
    theme.set_editor_property("vertex_color_material", vertex_material)
    save(theme)

catalog_file = os.path.join(PROJECT, "Content", "Padma", "MVP", "Playable", "Definitions", "DA_PlayableCatalog.uasset")
backup = os.path.join(EVIDENCE, "catalog-before-map.uasset")
if not os.path.exists(backup):
    shutil.copy2(catalog_file, backup)
if not catalog.get_editor_property("world_map"):
    catalog.set_editor_property("world_map", map_asset)
if not catalog.get_editor_property("map_visual_theme"):
    catalog.set_editor_property("map_visual_theme", theme)
current_dialogues = list(catalog.get_editor_property("dialogue_tables"))
if dialogues not in current_dialogues:
    current_dialogues.append(dialogues)
    catalog.set_editor_property("dialogue_tables", current_dialogues)
save(catalog)

levels = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
actors = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
preview_map = ROOT + "/Maps/Editing/L_MapAuthoring"
if not unreal.EditorAssetLibrary.does_asset_exist(preview_map):
    if not levels.new_level(preview_map):
        raise RuntimeError("Cannot create map authoring level")
    host = actors.spawn_actor_from_class(unreal.PadmaMapAuthoringPreview, unreal.Vector())
    host.set_actor_label("Map Authoring - select and Rebuild Preview")
    host.set_editor_property("map", map_asset)
    host.set_editor_property("theme", theme)
    actors.spawn_actor_from_class(unreal.DirectionalLight, unreal.Vector(0, 0, 600), unreal.Rotator(-55, -30, 0))
    actors.spawn_actor_from_class(unreal.SkyLight, unreal.Vector(0, 0, 400))
    if not levels.save_current_level():
        raise RuntimeError("Cannot save authoring level")
if not levels.load_level(preview_map):
    raise RuntimeError("Cannot load authoring level")
hosts = [a for a in actors.get_all_level_actors() if isinstance(a, unreal.PadmaMapAuthoringPreview)]
if len(hosts) != 1:
    raise RuntimeError("Expected one editor map host")
hosts[0].rebuild_preview()
world = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_editor_world()
handles = unreal.GameplayStatics.get_all_actors_of_class(world, unreal.PadmaMapNodeHandle)
if len(handles) != len(map_asset.get_editor_property("template").get_editor_property("nodes")):
    raise RuntimeError("Editor preview failed: " + str(hosts[0].get_editor_property("status")))
hosts[0].clear_preview()
if any(unreal.GameplayStatics.get_all_actors_of_class(world, cls) for cls in (unreal.PadmaWorldMapActor, unreal.PadmaWorldNodeActor, unreal.PadmaMapNodeHandle)):
    raise RuntimeError("Transient map preview actors leaked after clear")
with open(os.path.join(EVIDENCE, "map-authoring.json"), "w", encoding="utf-8") as stream:
    json.dump({"created": created, "map": map_asset.get_path_name(), "theme": theme.get_path_name(), "previewHandles": len(handles)}, stream, ensure_ascii=False, indent=2)
unreal.log("[TASK-048] Map authoring passed; definitions, checkpoints, theme and transient editor handles ready")
