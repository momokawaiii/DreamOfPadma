"""Create editable, intentionally incomplete ACT templates without overwriting user content."""
import unreal

ROOT = "/Game/Padma/MVP/Definitions/ACTCharacterCards"
tools = unreal.AssetToolsHelpers.get_asset_tools()


def create_data(name, cls, fields):
    path = ROOT + "/" + name
    if unreal.EditorAssetLibrary.does_asset_exist(path):
        existing = unreal.load_asset(path)
        if not isinstance(existing, cls):
            raise RuntimeError("Wrong existing template type: " + path)
        return existing
    factory = unreal.DataAssetFactory()
    factory.set_editor_property("data_asset_class", cls)
    asset = tools.create_asset(name, ROOT, cls, factory)
    if not asset:
        raise RuntimeError("Cannot create " + path)
    for key, value in fields.items():
        asset.set_editor_property(key, value)
    if not unreal.EditorAssetLibrary.save_loaded_asset(asset):
        raise RuntimeError("Cannot save " + path)
    return asset


created_catalog = not unreal.EditorAssetLibrary.does_asset_exist(ROOT + "/DA_ACTAuthoring_Template")
table_path = ROOT + "/DT_ACTSkills_Template"
if unreal.EditorAssetLibrary.does_asset_exist(table_path):
    table = unreal.load_asset(table_path)
    if (not isinstance(table, unreal.DataTable)
            or table.get_editor_property("row_struct") != unreal.load_object(None, "/Script/DreamOfPadma.PadmaACTSkillRow")):
        raise RuntimeError("Wrong existing skill-table template type")
else:
    factory = unreal.DataTableFactory()
    factory.set_editor_property("struct", unreal.load_object(None, "/Script/DreamOfPadma.PadmaACTSkillRow"))
    table = tools.create_asset("DT_ACTSkills_Template", ROOT, unreal.DataTable, factory)
    if not table or not unreal.EditorAssetLibrary.save_loaded_asset(table):
        raise RuntimeError("Cannot create/save ACT table template")

skill = create_data("DA_ACTSkill_Template", unreal.PadmaACTSkillDefinition, {
    "display_name": "待填写 ACT 技能",
})
character = create_data("DA_ACTCharacter_Template", unreal.PadmaACTCharacterDefinition, {
    "display_name": "待填写 ACT 角色",
    "skill_table": table,
})
weapon = create_data("DA_ACTWeapon_Template", unreal.PadmaACTWeaponDefinition, {
    "display_name": "待填写 ACT 武器",
})
catalog = create_data("DA_ACTAuthoring_Template", unreal.PadmaACTAuthoringCatalog, {
    "characters": [character],
    "weapons": [weapon],
})
report = catalog.validate_catalog()
if created_catalog and report.get_editor_property("valid"):
    raise RuntimeError("Empty templates must not be certified complete.")
for error in report.get_editor_property("errors"):
    unreal.log("[TASK-036 expected draft diagnostic] " + str(error))
unreal.log("[TASK-036] ACT templates available; existing authored content preserved. Catalog valid=" + str(report.get_editor_property("valid")))
