"""TASK-046 content-only UE Python authoring.
First run: node Scripts/Editor/ExportPlayableData.cjs
Then run this file in the UE editor Python commandlet. Optional PADMA_PLAYABLE_DATA selects export JSON.
Existing definitions are preserved; the catalog only appends missing generated art hooks.
This script creates no levels and never edits ThirdParty.
"""
import json
import os
import unreal

ROOT = "/Game/Padma/MVP/Playable/Definitions"
PROJECT = unreal.Paths.convert_relative_path_to_full(unreal.Paths.project_dir())
INPUT = os.environ.get("PADMA_PLAYABLE_DATA", os.path.join(PROJECT, "Artifacts", "TASK-046", "PlayableData.json"))
if not os.path.isfile(INPUT):
    raise RuntimeError("Missing PlayableData.json. Run node Scripts/Editor/ExportPlayableData.cjs first: " + INPUT)
with open(INPUT, "r", encoding="utf-8") as stream:
    source = json.load(stream)
if source.get("version") != 1 or source.get("assetRoot") != ROOT:
    raise RuntimeError("Unsupported/misrouted playable content export")
tools = unreal.AssetToolsHelpers.get_asset_tools()
created = []


def save(asset):
    if not unreal.EditorAssetLibrary.save_loaded_asset(asset):
        raise RuntimeError("Cannot save asset: " + asset.get_path_name())


def data(name, cls, fields, folder=ROOT):
    path = folder + "/" + name
    if unreal.EditorAssetLibrary.does_asset_exist(path):
        asset = unreal.load_asset(path)
        if not isinstance(asset, cls):
            raise RuntimeError("Existing asset has wrong type: " + path)
        return asset
    factory = unreal.DataAssetFactory()
    factory.set_editor_property("data_asset_class", cls)
    asset = tools.create_asset(name, folder, cls, factory)
    if not asset:
        raise RuntimeError("Cannot create Data Asset: " + path)
    for key, value in fields.items():
        asset.set_editor_property(key, value)
    save(asset)
    created.append(path)
    return asset


def table(name, row_struct, rows):
    folder = ROOT + "/Tables"
    path = folder + "/DT_" + name
    expected = unreal.load_object(None, "/Script/DreamOfPadma." + row_struct)
    if not expected:
        raise RuntimeError("Missing native row type; build the Editor target first: " + row_struct)
    if unreal.EditorAssetLibrary.does_asset_exist(path):
        asset = unreal.load_asset(path)
        if not isinstance(asset, unreal.DataTable) or asset.get_editor_property("row_struct") != expected:
            raise RuntimeError("Existing table has wrong row type: " + path)
        return asset
    factory = unreal.DataTableFactory()
    factory.set_editor_property("struct", expected)
    asset = tools.create_asset("DT_" + name, folder, unreal.DataTable, factory)
    if not asset or not unreal.DataTableFunctionLibrary.fill_data_table_from_json_string(
            asset, json.dumps(rows, ensure_ascii=False)):
        raise RuntimeError("Cannot create/import table: " + path)
    save(asset)
    created.append(path)
    return asset


def native_value(cls, **fields):
    value = cls()
    for key, field in fields.items():
        value.set_editor_property(key, field)
    return value


def transform(scale):
    value = unreal.Transform()
    value.set_editor_property("scale3d", unreal.Vector(*scale))
    return value


def safe_name(identity):
    return identity.replace(".", "_").replace("-", "_")


def mesh(path):
    value = unreal.load_asset(path)
    if not value:
        raise RuntimeError("Required engine placeholder is missing: " + path)
    return value


cube = mesh("/Engine/BasicShapes/Cube")
cylinder = mesh("/Engine/BasicShapes/Cylinder")
cone = mesh("/Engine/BasicShapes/Cone")
plane = mesh("/Engine/BasicShapes/Plane")
models = {}
model_bindings = []


def model(identity, display_name, shape, scale=(1, 1, 1)):
    if identity in models:
        return models[identity]
    asset = data("DA_Model_" + safe_name(identity), unreal.PadmaModelDefinition, {
        "visual_definition_id": identity,
        "display_name": display_name + " · UE Demo 可替换模型占位",
        "static_model": shape,
        "model_transform": transform(scale),
    }, ROOT + "/Models")
    models[identity] = asset
    model_bindings.append(native_value(unreal.PadmaModelBinding, id=identity, definition=asset))
    return asset


tables = {name: table(name, spec["rowStruct"], spec["rows"]) for name, spec in source["tables"].items()}
for family in ("A", "B", "C"):
    for row in source["tables"]["Cards_" + family]["rows"]:
        shape = cube if row["Kind"] == "building" else cylinder if family == "C" else cone if family == "A" else plane
        scale = (1.2, 1.0, 1.3) if row["Kind"] == "building" else (0.55, 0.55, 1.55) if family == "C" else (0.6, 0.6, 0.6)
        model(row["ModelId"], row["DisplayName"], shape, scale)

for name in ("ACTCharacters", "HomeCharacters", "FPSCharacters"):
    for row in source["tables"][name]["rows"]:
        model(row["ModelId"], row["DisplayName"], cylinder, (0.6, 0.6, 1.65))
for name in ("ACTWeapons", "HomeWeapons", "FPSWeapons"):
    for row in source["tables"][name]["rows"]:
        model(row["ModelId"], row["DisplayName"], cube, (0.15, 0.15, 1.2))
for row in source["tables"]["HomeOutfits"]["rows"]:
    model(row["ModelId"], row["DisplayName"], cone, (0.8, 0.8, 1.1))
for row in source["tables"]["HomePoses"]["rows"]:
    # Optional Pose soft AnimSequence hook is on this binding. No animation is invented.
    model(row["ModelId"], row["DisplayName"], cylinder, (0.6, 0.6, 1.65))
model("ground.default", "地表", plane, (7, 7, 1))
model("guard", "王庭守卫", cylinder, (0.65, 0.65, 1.7))

skill_assets = {}
skill_bindings = []
binding_rows = []
for index, row in enumerate(source["tables"]["ACTSkills"]["rows"]):
    identity = row["Id"]
    asset = data("DA_ACTSkill_" + identity, unreal.PadmaACTSkillDefinition, {
        "definition_id": identity,
        "display_name": row["DisplayName"],
        "ability_implementation_id": "Demo.ACT." + identity,
    }, ROOT + "/ACT")
    skill_assets[identity] = asset
    skill_bindings.append(native_value(unreal.PadmaACTSkillBinding, id=identity, definition=asset))
    binding_rows.append({
        "Name": identity, "SkillId": identity, "ActivationBindingId": "Slot" + str(index + 1),
        "Definition": asset.get_path_name()
    })
act_binding_table = table("ACTAuthoringBindings", "PadmaACTSkillRow", binding_rows)
character_bindings = []
for row in source["tables"]["ACTCharacters"]["rows"]:
    restriction = native_value(unreal.PadmaACTTraitTerrainRestriction,
                               trait_id=row["Trait"], disallowed_terrain_ids=[row["TraitDisabledTerrain"]])
    # Static ModelId placeholder renders until the user supplies a skeletal mesh/AnimBP here.
    asset = data("DA_ACTCharacter_" + row["Id"], unreal.PadmaACTCharacterDefinition, {
        "definition_id": row["Id"], "display_name": row["DisplayName"],
        "skill_table": act_binding_table, "trait_ids": [row["Trait"]],
        "terrain_restrictions": [restriction],
    }, ROOT + "/ACT")
    character_bindings.append(native_value(unreal.PadmaACTCharacterBinding, id=row["Id"], definition=asset))

weapon_bindings = []
for row in source["tables"]["ACTWeapons"]["rows"]:
    asset = data("DA_ACTWeapon_" + row["Id"], unreal.PadmaACTWeaponDefinition, {
        "definition_id": row["Id"], "display_name": row["DisplayName"], "static_model": cube,
    }, ROOT + "/ACT")
    weapon_bindings.append(native_value(unreal.PadmaACTWeaponBinding, id=row["Id"], definition=asset))

effects = [native_value(unreal.PadmaEffectBinding, id=row["Id"])
           for row in source["tables"]["EncounterSkills"]["rows"]]
effects.append(native_value(unreal.PadmaEffectBinding, id="environment.default"))
attack_effects = [native_value(unreal.PadmaEffectBinding, id=identity)
                 for identity in ["attack"] + [row["Id"] for row in source["tables"]["Cards_C"]["rows"]]]
effects.extend(attack_effects)
node_bindings = []
for row in source["tables"]["Nodes"]["rows"]:
    building = ""
    if row["Type"] != "normal":
        building = "node.building." + row["Id"]
        model(building, row["DisplayName"], cube, (1.5, 1.5, 1.2))
    node_bindings.append(native_value(unreal.PadmaNodePresentationBinding, node_id=row["Id"],
                                     ground_model_id="ground.default", building_model_id=building,
                                     effect_id="environment.default"))

catalog_fields = {
    "content_version": 1, "provenance": source["provenance"],
    "card_tables": [tables["Cards_A"], tables["Cards_B"], tables["Cards_C"]],
    "models": model_bindings, "act_character_definitions": character_bindings,
    "act_weapon_definitions": weapon_bindings, "act_skill_definitions": skill_bindings,
    "effects": effects, "node_presentation": node_bindings,
    "card_order": source["order"]["cards"], "node_order": source["order"]["nodes"],
    "skill_order": source["order"]["skills"], "eras": source["order"]["eras"], "phases": source["order"]["phases"],
}
for prop, name in (
    ("node_tables", "Nodes"), ("edge_tables", "Edges"), ("profile_tables", "Profiles"),
    ("encounter_skill_tables", "EncounterSkills"), ("act_skill_tables", "ACTSkills"),
    ("act_character_tables", "ACTCharacters"), ("act_weapon_tables", "ACTWeapons"),
    ("home_character_tables", "HomeCharacters"), ("home_outfit_tables", "HomeOutfits"),
    ("home_weapon_tables", "HomeWeapons"), ("home_pose_tables", "HomePoses"),
    ("fps_character_tables", "FPSCharacters"), ("fps_weapon_tables", "FPSWeapons"),
    ("dialogue_tables", "Dialogues"),
):
    catalog_fields[prop] = [tables[name]]
catalog = data("DA_PlayableCatalog", unreal.PadmaContentCatalog, catalog_fields)
# Existing catalogs retain every authored field and binding. Add only newly required
# guard/attack hooks, matching FName's case-insensitive identity rules.
appended = 0
for prop, required in (
    ("models", [binding for binding in model_bindings if str(binding.get_editor_property("id")) == "guard"]),
    ("effects", attack_effects),
):
    current = list(catalog.get_editor_property(prop))
    identities = {str(binding.get_editor_property("id")).casefold() for binding in current}
    additions = []
    for binding in required:
        identity = str(binding.get_editor_property("id")).casefold()
        if identity not in identities:
            additions.append(binding)
            identities.add(identity)
    if additions:
        catalog.set_editor_property(prop, current + additions)
        appended += len(additions)
if appended:
    save(catalog)
    unreal.log("[TASK-046] Appended " + str(appended) + " missing guard/attack art hooks; existing bindings preserved.")
unreal.log("[TASK-046] validate_catalog Python signature: " + str(catalog.validate_catalog.__doc__))
result = catalog.validate_catalog()
unreal.log("[TASK-046] validate_catalog result=" + repr(result) + "; type=" + type(result).__name__)
# UE 5.8 PyGenUtil::PackReturnValues removes a leading bool when an out parameter
# follows it: false becomes None; true becomes the FString Error (empty on success).
if result is None:
    raise RuntimeError("Current authored catalog was rejected by native validation; existing assets were preserved. "
                       "UE Python returned None and suppresses the out Error when the native bool is false.")
if not isinstance(result, str) or result:
    raise RuntimeError("Unexpected catalog validation output; existing assets were preserved: " + repr(result))
unreal.log("[TASK-046] Temporary UE Demo catalog validated. Created " + str(len(created))
           + " assets; existing authored assets preserved. ACT skeletal models remain optional import hooks.")
