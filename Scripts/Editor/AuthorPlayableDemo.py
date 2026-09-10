"""Create the native playable content, presentation materials and two maps.
Run ExportPlayableData.cjs first, then execute this script using UE Python.
Existing assets/maps are preserved and checked; no defaults or third-party files are overwritten.
"""
import os
import runpy
import unreal

PROJECT=unreal.Paths.convert_relative_path_to_full(unreal.Paths.project_dir())
runpy.run_path(os.path.join(PROJECT,"Scripts","Editor","AuthorPlayableContent.py"),run_name="__main__")
TOOLS=unreal.AssetToolsHelpers.get_asset_tools()
ROOT="/Game/Padma/MVP/Playable"

def material(name,vertex,color):
    folder=ROOT+"/Presentation"
    path=folder+"/"+name
    if unreal.EditorAssetLibrary.does_asset_exist(path):
        result=unreal.load_asset(path)
        if not isinstance(result,unreal.Material):
            raise RuntimeError("Existing material has wrong type: "+path)
        return result
    result=TOOLS.create_asset(name,folder,unreal.Material,unreal.MaterialFactoryNew())
    if not result:
        raise RuntimeError("Cannot create material: "+path)
    result.set_editor_property("two_sided",True)
    expression=unreal.MaterialEditingLibrary.create_material_expression(result,unreal.MaterialExpressionVertexColor if vertex else unreal.MaterialExpressionConstant3Vector)
    if not vertex:
        expression.set_editor_property("constant",unreal.LinearColor(*color))
    unreal.MaterialEditingLibrary.connect_material_property(expression,"RGB" if vertex else "",unreal.MaterialProperty.MP_BASE_COLOR)
    roughness=unreal.MaterialEditingLibrary.create_material_expression(result,unreal.MaterialExpressionConstant)
    roughness.set_editor_property("r",.9)
    unreal.MaterialEditingLibrary.connect_material_property(roughness,"",unreal.MaterialProperty.MP_ROUGHNESS)
    unreal.MaterialEditingLibrary.recompile_material(result)
    if not unreal.EditorAssetLibrary.save_loaded_asset(result):raise RuntimeError("Material save failed: "+path)
    return result

material("M_PadmaVertexColor",True,(.1,.2,.15,1))
material("M_PadmaGround",False,(.055,.075,.065,1))
levels=unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
for name in ["L_PadmaWorld","L_PadmaBattle"]:
    path=ROOT+"/Maps/"+name
    if unreal.EditorAssetLibrary.does_asset_exist(path):
        if not levels.load_level(path):raise RuntimeError("Cannot open existing playable map: "+path)
        world=unreal.UnrealEditorSubsystem.get_editor_world(unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem))
        if world.get_world_settings().get_editor_property("default_game_mode")!=unreal.PadmaGameMode.static_class():
            raise RuntimeError("Existing map GameMode differs; preserved: "+path)
        unreal.log("[TASK-046] Preserved existing map "+path)
        continue
    if not levels.new_level(path):
        raise RuntimeError("Cannot create map: "+path)
    world=unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_editor_world()
    world.get_world_settings().set_editor_property("default_game_mode",unreal.PadmaGameMode.static_class())
    if not levels.save_current_level():raise RuntimeError("Cannot save map: "+path)
    unreal.log("[TASK-046] Created map "+path)
levels.load_level(ROOT+"/Maps/L_PadmaWorld")
unreal.log("[TASK-046] Native playable maps ready. Open L_PadmaWorld and Play.")
