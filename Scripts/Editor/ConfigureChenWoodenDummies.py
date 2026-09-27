"""Bind the imported wooden dummy and hit clip to L_ChenACT; preserve combat profiles.

Run with UE5.8 -run=pythonscript -script=<absolute path>. See ChenACTRenderLab.md.
Creates a plain wood material wrapper once; subsequent runs preserve its graph.
"""
import json
from pathlib import Path
import unreal as u

ROOT='/Game/Sandbox/ACT/Training/WoodenDummy'
MAP='/Game/Sandbox/ACT/Training/Maps/L_ChenACT'
lib=u.EditorAssetLibrary
mesh=u.load_asset(ROOT+'/Art/Meshes/SK_WoodenDummy')
hit=u.load_asset(ROOT+'/Animation/Sequences/AS_WoodenDummy_Hit')
assert mesh and hit and mesh.get_editor_property('skeleton')==hit.get_editor_property('skeleton'), 'Import matching dummy mesh/skeleton/animation first'
assert hit.get_editor_property('sequence_length')>0 and not hit.get_editor_property('enable_root_motion')
material_path=ROOT+'/Art/Materials/M_WoodenDummy'
if lib.does_asset_exist(material_path):
    material=lib.load_asset(material_path)
else:
    material=u.AssetToolsHelpers.get_asset_tools().create_asset('M_WoodenDummy',ROOT+'/Art/Materials',u.Material,u.MaterialFactoryNew())
    assert material
    ml=u.MaterialEditingLibrary
    for param,texture,prop,y,sampler in [
        ('BaseColor','T_WoodenDummy_D',u.MaterialProperty.MP_BASE_COLOR,0,u.MaterialSamplerType.SAMPLERTYPE_COLOR),
        ('Normal','T_WoodenDummy_N',u.MaterialProperty.MP_NORMAL,180,u.MaterialSamplerType.SAMPLERTYPE_NORMAL),
    ]:
        node=ml.create_material_expression(material,u.MaterialExpressionTextureSampleParameter2D,-400,y)
        node.set_editor_property('parameter_name',param)
        node.set_editor_property('texture',u.load_asset(ROOT+'/Art/Textures/'+texture))
        node.set_editor_property('sampler_type',sampler)
        assert ml.connect_material_property(node,'RGB',prop)
    rough=ml.create_material_expression(material,u.MaterialExpressionScalarParameter,-400,360)
    rough.set_editor_property('parameter_name','Roughness');rough.set_editor_property('default_value',.85)
    ml.connect_material_property(rough,'',u.MaterialProperty.MP_ROUGHNESS)
    material.set_editor_property('used_with_skeletal_mesh',True)
    lib.set_metadata_tag(material,'PadmaAuthor','WoodenTrainingDummy')
    ml.recompile_material(material)
    assert lib.save_loaded_asset(material,False)

# Modify only the project copy; source files on F: remain unchanged. Avoid the
# source demonstration's global dissolve collection affecting stationary targets.
materials=mesh.get_editor_property('materials')
assert len(materials)==1
slot=materials[0];slot.set_editor_property('material_interface',material);materials[0]=slot
mesh.set_editor_property('materials',materials)
assert lib.save_loaded_asset(mesh,False)

level=u.get_editor_subsystem(u.LevelEditorSubsystem)
assert level.load_level(MAP)
labs=[a for a in u.get_editor_subsystem(u.EditorActorSubsystem).get_all_level_actors() if isinstance(a,u.PadmaACTMeleeLab)]
assert len(labs)==1
lab=labs[0];before=str(lab.get_editor_property('training_targets'))
lab.set_editor_property('training_dummy_mesh',mesh)
lab.set_editor_property('training_dummy_hit_animation',hit)
lab.set_editor_property('training_dummy_mesh_transform',u.Transform(location=u.Vector(0,0,-90),rotation=u.Rotator(),scale=u.Vector(.85,.85,.85)))
assert str(lab.get_editor_property('training_targets'))==before
assert level.save_current_level()
out=Path(u.Paths.project_dir()).resolve()/'Artifacts/TrainingDummy';out.mkdir(parents=True,exist_ok=True)
(out/'configured.json').write_text(json.dumps({'map':MAP,'mesh':mesh.get_path_name(),'hit':hit.get_path_name(),
 'duration':hit.get_editor_property('sequence_length'),'material':material_path,'training_targets_unchanged':True,'target_count':len(lab.get_editor_property('training_targets'))},ensure_ascii=False,indent=2),encoding='utf8')
u.log('WOODEN_DUMMY_CONFIGURED')
