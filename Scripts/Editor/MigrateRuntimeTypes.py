"""TASK-047: validate/load old reflected references, then resave project-owned fixture assets.

Run in UE Python after building the renamed runtime. See Docs/Architecture/RuntimeNaming.md.
No asset path/ID moves. All tables and maps are validated before the first save.
"""
import json
import os
import shutil
import unreal

project = unreal.Paths.convert_relative_path_to_full(unreal.Paths.project_dir())
with open(os.path.join(project, "Scripts", "Migrations", "RuntimeNaming047.json"), encoding="utf-8") as stream:
    migration = json.load(stream)
if migration.get("version") != 1 or migration.get("task") != "TASK-047":
    raise RuntimeError("Unsupported runtime naming migration manifest")
root = migration["preserved"]["contentRoot"]
if root != "/Game/Padma/MVP/Playable":
    raise RuntimeError("Migration may only resave its declared project fixture root")
renamed_structs = {entry["new"] for entry in migration["redirects"] if entry["kind"] == "Struct"}
content_directory = os.path.join(project, "Content", "Padma", "MVP", "Playable")
backup_directory = os.path.join(project, "Artifacts", "TASK-047", "content-before")
for directory, _, files in os.walk(content_directory):
    for name in files:
        if not name.endswith((".uasset", ".umap")):
            continue
        original = os.path.join(directory, name)
        backup = os.path.join(backup_directory, os.path.relpath(original, content_directory))
        if not os.path.exists(backup):
            os.makedirs(os.path.dirname(backup), exist_ok=True)
            shutil.copy2(original, backup)
pending = []
tables = {}
for path in unreal.EditorAssetLibrary.list_assets(root, recursive=True, include_folder=False):
    asset = unreal.load_asset(path)
    if not asset:
        raise RuntimeError("Cannot load existing asset; nothing has been saved: " + path)
    if isinstance(asset, unreal.DataTable):
        row = asset.get_editor_property("row_struct")
        if not row:
            raise RuntimeError("Table lost its row type: " + path)
        rows = unreal.DataTableFunctionLibrary.export_data_table_to_json_string(asset)
        if not isinstance(rows, str):
            raise RuntimeError("Cannot snapshot authored rows: " + path)
        tables[path] = {"rowStruct": row.get_path_name(), "rows": json.loads(rows)}
        if row.get_path_name() in renamed_structs:
            pending.append(asset)
    elif isinstance(asset, unreal.PadmaContentCatalog):
        if asset.validate_catalog() != "":
            raise RuntimeError("Existing content catalog failed validation; nothing has been saved")
        pending.append(asset)

levels = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
editor = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
maps = [root + "/Maps/L_PadmaWorld", root + "/Maps/L_PadmaBattle"]
for path in maps:
    if not levels.load_level(path):
        raise RuntimeError("Cannot load existing map: " + path)
    mode = editor.get_editor_world().get_world_settings().get_editor_property("default_game_mode")
    if mode != unreal.PadmaGameMode.static_class():
        raise RuntimeError("Map GameMode redirect failed; preserved: " + path)

for asset in pending:
    if not unreal.EditorAssetLibrary.save_loaded_asset(asset, only_if_is_dirty=False):
        raise RuntimeError("Unable to resave reflected references: " + asset.get_path_name())
for path in maps:
    if not levels.load_level(path) or not levels.save_current_level():
        raise RuntimeError("Unable to resave map reference: " + path)

for path, before in tables.items():
    table = unreal.load_asset(path)
    after = json.loads(unreal.DataTableFunctionLibrary.export_data_table_to_json_string(table))
    if after != before["rows"]:
        raise RuntimeError("Authored row values changed during type migration: " + path)
    if before["rowStruct"] in renamed_structs:
        tags = unreal.EditorAssetLibrary.get_tag_values(path)
        if before["rowStruct"] not in str(tags.get("RowStructure", "")):
            raise RuntimeError("RowStructure picker tag did not update: " + path)

output = os.path.join(project, "Artifacts", "TASK-047")
os.makedirs(output, exist_ok=True)
with open(os.path.join(output, "asset-migration.json"), "w", encoding="utf-8") as stream:
    json.dump({"task": "TASK-047", "resavedAssets": [a.get_path_name() for a in pending],
               "maps": maps, "tables": tables}, stream, ensure_ascii=False, indent=2)
unreal.log("[TASK-047] Runtime type migration passed: %d tables unchanged; %d assets and 2 maps resaved; picker tags current"
           % (len(tables), len(pending)))
