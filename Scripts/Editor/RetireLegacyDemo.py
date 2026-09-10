"""Guarded six-asset retirement. Usage: Docs/Content/WorldMapAuthoring.md."""
import json
import shutil
from pathlib import Path
import unreal

project = Path(unreal.Paths.project_dir()).resolve()
backup = project / "Artifacts/TASK-048/legacy-backup"
root = "/Game/Padma/MVP/Presentation/ModelPreview"
catalog_path, map_path = root + "/DA_ModelPreview_Catalog", root + "/L_ModelAuthoringPreview"
old = ["/Game/Padma/Demo/World/DemoSandbox", "/Game/Padma/Demo/Encounter/L_DemoEncounter",
       "/Game/Padma/Demo/Definitions/Characters/DA_Demo_VitruvianMan"]
old += ["/Game/Padma/Demo/Definitions/Skills/DA_Demo_" + x for x in ("GoldenRatio", "MortalSustenance", "PerfectBody")]
registry = unreal.AssetRegistryHelpers.get_asset_registry()
registry.search_all_assets(True)
options = unreal.AssetRegistryDependencyOptions(include_soft_package_references=True, include_hard_package_references=True, include_searchable_names=True, include_soft_management_references=True, include_hard_management_references=True)
if "-PadmaRetireLegacyDemo" not in unreal.SystemLibrary.get_command_line():
    raise RuntimeError("Audit only: explicit -PadmaRetireLegacyDemo is required to migrate/delete the exact legacy packages")

for package in old + [catalog_path, map_path]:
    for extension in (".uasset", ".umap"):
        relative = "Content/" + package.removeprefix("/Game/") + extension
        source, target = (project / relative).resolve(), backup / relative
        if not source.is_relative_to(project / "Content"):
            raise RuntimeError("Path escaped Content")
        if source.exists() and not target.exists():
            target.parent.mkdir(parents=True, exist_ok=True)
            shutil.copy2(source, target)

legacy = getattr(unreal.PadmaPresentationSource, "DEMO_CHARACTER", None)
catalog = unreal.load_asset(catalog_path)
if catalog and legacy is not None:
    entries = list(catalog.get_editor_property("entries"))
    for entry in entries:
        key = entry.get_editor_property("key")
        if key.get_editor_property("domain") != legacy:
            continue
        source = unreal.load_asset(old[2])
        model_path = root + "/DA_Model_Garrison_Placeholder"
        model = unreal.load_asset(model_path) if unreal.EditorAssetLibrary.does_asset_exist(model_path) else None
        if not model:
            factory = unreal.DataAssetFactory()
            factory.set_editor_property("data_asset_class", unreal.PadmaModelDefinition)
            model = unreal.AssetToolsHelpers.get_asset_tools().create_asset("DA_Model_Garrison_Placeholder", root, unreal.PadmaModelDefinition, factory)
            model.set_editor_property("visual_definition_id", "Visual.Preview.Garrison")
            model.set_editor_property("display_name", source.get_editor_property("display_name"))
            model.set_editor_property("static_model", source.get_editor_property("placeholder_mesh"))
            if not unreal.EditorAssetLibrary.save_loaded_asset(model):
                raise RuntimeError("Cannot save replacement model")
        key.set_editor_property("domain", unreal.PadmaPresentationSource.MODEL)
        key.set_editor_property("definition_id", "Visual.Preview.Garrison")
        entry.set_editor_property("key", key)
        entry.set_editor_property("model", model)
        entry.set_editor_property("demo_character", None)
    catalog.set_editor_property("entries", entries)
    if not unreal.EditorAssetLibrary.save_loaded_asset(catalog):
        raise RuntimeError("Cannot save preview catalog")
    levels = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
    if not levels.load_level(map_path):
        raise RuntimeError("Cannot load preview map")
    for actor in unreal.get_editor_subsystem(unreal.EditorActorSubsystem).get_all_level_actors():
        if isinstance(actor, unreal.PadmaNodePreview):
            bindings = list(actor.get_editor_property("preview_bindings"))
            for binding in bindings:
                key = binding.get_editor_property("definition")
                if key.get_editor_property("domain") == legacy:
                    key.set_editor_property("domain", unreal.PadmaPresentationSource.MODEL)
                    key.set_editor_property("definition_id", "Visual.Preview.Garrison")
                    binding.set_editor_property("definition", key)
            actor.set_editor_property("preview_bindings", bindings)
    if not levels.save_current_level():
        raise RuntimeError("Cannot save preview bindings")

registry.scan_paths_synchronous(["/Game/Padma"], force_rescan=True)
for package in old:
    external = [str(x) for x in (registry.get_referencers(package, options) or []) if str(x) not in old]
    if external:
        raise RuntimeError("External referencers prevent retirement: " + package + str(external))
removed = []
for package in old:
    if unreal.EditorAssetLibrary.does_asset_exist(package):
        if not unreal.EditorAssetLibrary.delete_asset(package):
            raise RuntimeError("Cannot retire " + package)
        removed.append(package)
remaining = [str(x.package_name) for x in (registry.get_assets_by_path("/Game/Padma/Demo", recursive=True) or [])]
if remaining:
    raise RuntimeError("Unexpected remaining Demo assets: " + str(remaining))
disk_residuals = [str(project / "Content" / (package.removeprefix("/Game/") + extension))
                  for package in old for extension in (".umap", ".uasset")
                  if (project / "Content" / (package.removeprefix("/Game/") + extension)).exists()]
if disk_residuals:
    raise RuntimeError("UE registry entries removed but disk files remain; compare backups before cleanup: " + str(disk_residuals))
report = project / "Artifacts/TASK-048/legacy-retirement.json"
report.parent.mkdir(parents=True, exist_ok=True)
report.write_text(json.dumps({"removed": removed, "remaining": remaining, "backup": str(backup)}, indent=2), encoding="utf-8")
unreal.log("[TASK-048] Legacy retirement passed; %d assets removed; preview migrated" % len(removed))
