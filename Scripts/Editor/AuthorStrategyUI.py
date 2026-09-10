"""Import the TASK-049 illustrated card atlas. Run in the UE Python commandlet.
See Docs/Production/Tasks/TASK-049-Native-Strategy-HUD.md. Reruns preserve existing art.
"""
import os
import unreal

source = os.path.join(unreal.Paths.project_dir(), "Content/Padma/UI/SourceArt/T_CardIllustrations.png")
destination = "/Game/Padma/UI/Textures/T_CardIllustrations"
if not os.path.isfile(source):
    raise RuntimeError("Missing card atlas source: " + source)
if not unreal.EditorAssetLibrary.does_asset_exist(destination):
    task = unreal.AssetImportTask()
    task.filename = source
    task.destination_path = "/Game/Padma/UI/Textures"
    task.destination_name = "T_CardIllustrations"
    task.automated = True
    task.replace_existing = False
    task.save = False
    unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([task])
    texture = unreal.load_asset(destination)
    if not isinstance(texture, unreal.Texture2D):
        raise RuntimeError("Card atlas import did not create a Texture2D")
    texture.set_editor_property("compression_settings", unreal.TextureCompressionSettings.TC_EDITOR_ICON)
    texture.set_editor_property("lod_group", unreal.TextureGroup.TEXTUREGROUP_UI)
    texture.set_editor_property("srgb", True)
    if not unreal.EditorAssetLibrary.save_loaded_asset(texture):
        raise RuntimeError("Failed to save card atlas")
else:
    texture = unreal.load_asset(destination)
    if not isinstance(texture, unreal.Texture2D):
        raise RuntimeError("Existing card atlas has wrong asset type")
unreal.log("[TASK-049] UI atlas ready: " + texture.get_path_name())

# TASK-050: reusable nine-slice skins. Existing imports remain untouched on rerun.
for name in ("T_HUDPanel", "T_HUDButton"):
    source = os.path.join(unreal.Paths.project_dir(), "Content/Padma/UI/SourceArt", name + ".png")
    destination = "/Game/Padma/UI/Textures/" + name
    if not os.path.isfile(source):
        raise RuntimeError("Missing UI skin source: " + source)
    if not unreal.EditorAssetLibrary.does_asset_exist(destination):
        task = unreal.AssetImportTask()
        task.filename = source
        task.destination_path = "/Game/Padma/UI/Textures"
        task.destination_name = name
        task.automated = True
        task.replace_existing = False
        task.save = False
        unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([task])
        texture = unreal.load_asset(destination)
        if not isinstance(texture, unreal.Texture2D):
            raise RuntimeError("UI skin import failed: " + name)
        texture.set_editor_property("compression_settings", unreal.TextureCompressionSettings.TC_EDITOR_ICON)
        texture.set_editor_property("lod_group", unreal.TextureGroup.TEXTUREGROUP_UI)
        texture.set_editor_property("srgb", True)
        if not unreal.EditorAssetLibrary.save_loaded_asset(texture):
            raise RuntimeError("Failed to save skin: " + name)
    texture = unreal.load_asset(destination)
    if not isinstance(texture, unreal.Texture2D):
        raise RuntimeError("Existing UI skin has wrong asset type: " + name)
    # Slate box margins use the resource dimensions rather than Brush.ImageSize.
    # Original PNGs stay intact; compact UE resources keep corners out of the text.
    texture.set_editor_property("max_texture_size", 256 if name == "T_HUDPanel" else 512)
    if not unreal.EditorAssetLibrary.save_loaded_asset(texture):
        raise RuntimeError("Failed to save UI resource sizing: " + name)
    unreal.log("[TASK-050] UI skin ready: " + destination)
