"""Restore the four referenced dragon textures; callable from import and repair recipes."""
import json
from pathlib import Path
import unreal as u


def restore_dragon_material():
    output = Path(u.Paths.project_dir()).resolve() / 'Artifacts/ChenQianyu/Actions/FX'
    root = '/Game/Sandbox/ACT/Character/ChenQianyu/Art/Niagara'
    lib, ml, tools = u.EditorAssetLibrary, u.MaterialEditingLibrary, u.AssetToolsHelpers.get_asset_tools()
    source = json.loads(next((output / 'dragon-source/Material').glob('M_fx_chen_dragon_01_*.json')).read_text())
    props = source['m_SavedProperties']
    mat = lib.load_asset(root + '/Materials/M_Chen_Dragon')
    if not mat:
        mat = tools.create_asset('M_Chen_Dragon', root + '/Materials', u.Material, u.MaterialFactoryNew())
    ml.delete_all_material_expressions(mat)
    mat.set_editor_property('blend_mode', u.BlendMode.BLEND_MASKED)
    mat.set_editor_property('shading_model', u.MaterialShadingModel.MSM_DEFAULT_LIT)
    ml.set_material_usage(mat, u.MaterialUsage.MATUSAGE_SKELETAL_MESH)

    def node(cls, **values):
        result = ml.create_material_expression(mat, cls)
        for key, value in values.items():
            result.set_editor_property(key, value)
        return result

    def wire(a, pin, b, target):
        assert ml.connect_material_expressions(a, pin, b, target)

    def output_pin(a, pin, target):
        assert ml.connect_material_property(a, pin, target)

    textures, samples = {}, {}
    for suffix, slot in [('D', '_BaseMap'), ('N', '_BumpMap'), ('E', '_EmissionMap'), ('M', '_MetallicGlossMap')]:
        ptr = props['m_TexEnvs'][slot]['m_Texture']
        name = 'T_fx_chen_dragon_01_' + suffix
        path = next((output / 'dragon-convert/Texture2D').glob(name + '_p' + f'{ptr["m_PathID"] & ((1 << 64) - 1):016X}' + '.png'))
        tex = lib.load_asset(root + '/Textures/' + name)
        if not tex:
            task = u.AssetImportTask()
            for key, value in dict(filename=str(path), destination_path=root + '/Textures', destination_name=name,
                                   automated=True, replace_existing=False, save=True).items():
                task.set_editor_property(key, value)
            tools.import_asset_tasks([task])
            tex = lib.load_asset(root + '/Textures/' + name)
        assert tex, name
        tex.set_editor_property('srgb', suffix in ['D', 'E'])
        tex.set_editor_property('compression_settings', u.TextureCompressionSettings.TC_NORMALMAP if suffix == 'N'
                                else u.TextureCompressionSettings.TC_MASKS if suffix == 'M' else u.TextureCompressionSettings.TC_DEFAULT)
        sample = node(u.MaterialExpressionTextureSample, texture=tex)
        sample.set_editor_property('sampler_type', u.MaterialSamplerType.SAMPLERTYPE_NORMAL if suffix == 'N'
                                   else u.MaterialSamplerType.SAMPLERTYPE_MASKS if suffix == 'M' else u.MaterialSamplerType.SAMPLERTYPE_COLOR)
        assert lib.save_loaded_asset(tex, False)
        samples[suffix] = sample
        textures[slot] = {'asset': tex.get_path_name(), 'source_path_id': ptr['m_PathID'], 'png': str(path)}

    output_pin(samples['D'], 'RGB', u.MaterialProperty.MP_BASE_COLOR)
    output_pin(samples['N'], 'RGB', u.MaterialProperty.MP_NORMAL)
    # Original HGRP/CharacterNPR property label: RGBA = Metal, Spec, Shadow, Smooth.
    output_pin(samples['M'], 'R', u.MaterialProperty.MP_METALLIC)
    output_pin(samples['M'], 'G', u.MaterialProperty.MP_SPECULAR)
    rough = node(u.MaterialExpressionOneMinus)
    wire(samples['M'], 'A', rough, '')
    output_pin(rough, '', u.MaterialProperty.MP_ROUGHNESS)
    # Source B is an NPR shadow mask. Preserve it as data, not an invented AO map.
    color = props['m_Colors']['_EmissionColor']
    brightness = props['m_Floats']['_EmissionBrightness']
    tint = node(u.MaterialExpressionConstant3Vector, constant=u.LinearColor(*[color[k] * brightness for k in 'rgb']))
    glow = node(u.MaterialExpressionMultiply)
    wire(samples['E'], 'RGB', glow, 'A')
    wire(tint, '', glow, 'B')
    output_pin(glow, '', u.MaterialProperty.MP_EMISSIVE_COLOR)
    opacity = node(u.MaterialExpressionScalarParameter, parameter_name='SourceOpacity', default_value=1.)
    dither = node(u.MaterialExpressionMaterialFunctionCall)
    assert dither.set_material_function(u.load_asset('/Engine/Functions/Engine_MaterialFunctions02/Utility/DitherTemporalAA'))
    wire(opacity, '', dither, 'Alpha Threshold')
    output_pin(dither, '', u.MaterialProperty.MP_OPACITY_MASK)
    ml.recompile_material(mat)
    assert lib.save_loaded_asset(mat, False)
    mesh = lib.load_asset(root + '/Meshes/SK_Chen_Dragon')
    assert mesh
    slots = mesh.get_editor_property('materials')
    assert slots
    for index, slot in enumerate(slots):
        slot.set_editor_property('material_interface', mat)
        slots[index] = slot
    mesh.set_editor_property('materials', slots)
    assert lib.save_loaded_asset(mesh, False)
    report = {'material': mat.get_path_name(), 'textures': textures, 'emission_brightness': brightness,
              'metallic': 'M.R', 'specular': 'M.G', 'roughness': '1-M.A',
              'limitation': 'UE DefaultLit response; M.B NPR shadow mask is preserved but not mapped to UE AO.',
              'fade': 'SourceOpacity -> DitherTemporalAA -> OpacityMask; UE presentation adaptation'}
    (output / 'Review/dragon-material-restored.json').write_text(json.dumps(report, indent=2))
    return mat
