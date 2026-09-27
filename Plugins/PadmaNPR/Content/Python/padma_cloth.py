"""Explicit editor operation: bake a Cloth Profile into an existing project MI.

UE Python console:
    import padma_cloth
    padma_cloth.apply_profile(profile_path, material_instance_path, slot_name)
No world or mesh is modified. Profile remains the artistic source of truth.
"""
import unreal


def apply_profile(profile_path, material_instance_path, slot_name):
    assets = unreal.EditorAssetLibrary
    material_editing = unreal.MaterialEditingLibrary
    profile = assets.load_asset(profile_path)
    instance = assets.load_asset(material_instance_path)
    if not isinstance(profile, unreal.PadmaClothProfile):
        raise ValueError("Expected a PadmaClothProfile")
    if not isinstance(instance, unreal.MaterialInstanceConstant):
        raise ValueError("Expected a MaterialInstanceConstant")
    values, error = unreal.PadmaClothLibrary.get_cloth_parameters(profile, slot_name)
    if error:
        raise ValueError(error)
    names = {str(name) for name in material_editing.get_vector_parameter_names(instance)}
    if ("NPR_ReferenceControls" in names) != ("NPR_ReferenceControls" in {str(n) for n in values}):
        raise ValueError("Profile response and material variant do not match")
    missing = {str(name) for name in values} - names
    if missing:
        raise ValueError("Missing Cloth parameters: " + ", ".join(sorted(missing)))
    with unreal.ScopedEditorTransaction("Apply Padma Cloth Profile"):
        instance.modify()
        for name, value in values.items():
            material_editing.set_material_instance_vector_parameter_value(instance, name, value)
        material_editing.update_material_instance(instance)
    if not assets.save_loaded_asset(instance, False):
        raise RuntimeError("Profile applied in memory but MI could not be saved")
    unreal.log("Applied Cloth Profile to " + material_instance_path)
    return instance
