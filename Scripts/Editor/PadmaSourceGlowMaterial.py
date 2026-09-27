"""Apply the TASK-055 UE exposure/halo bridge after rebuilding a source material."""
import unreal as u

HALO_MATERIALS = frozenset((
    'chen_attack_01_start2_3',
    'fxbat_chen_common_hit_01_2', 'fxbat_chen_common_hit_01_6',
    'fxbat_chen_common_hit_02_3', 'fxbat_chen_common_hit_02_8',
))

def apply_source_halo_bridge(material, source_name, source_floats):
    """Idempotent graph append; caller owns compile/save. Source parameters stay intact."""
    if source_floats.get('_IgnorePostExposure') != 1:
        return material
    ml = u.MaterialEditingLibrary
    emissive = ml.get_material_property_input_node(material, u.MaterialProperty.MP_EMISSIVE_COLOR)
    if not emissive:
        raise ValueError('Rebuild source emissive graph before applying halo bridge')
    if emissive.get_editor_property('desc') == 'PadmaSourceHaloOutput':
        return material
    inverse = ml.create_material_expression(material, u.MaterialExpressionEyeAdaptationInverse)
    pin = str(ml.get_material_expression_input_names(inverse)[0])
    assert ml.connect_material_expressions(emissive, '', inverse, pin)
    gain = ml.create_material_expression(material, u.MaterialExpressionScalarParameter)
    gain.set_editor_property('parameter_name', 'SourceHaloGain')
    gain.set_editor_property('default_value', .12 if source_name in HALO_MATERIALS else 1.)
    gain.set_editor_property('desc', 'UE visual calibration after exposure compensation; source tint/intensity unchanged.')
    output = ml.create_material_expression(material, u.MaterialExpressionMultiply)
    output.set_editor_property('desc', 'PadmaSourceHaloOutput')
    assert ml.connect_material_expressions(inverse, '', output, 'A')
    assert ml.connect_material_expressions(gain, '', output, 'B')
    assert ml.connect_material_property(output, '', u.MaterialProperty.MP_EMISSIVE_COLOR)
    return material
