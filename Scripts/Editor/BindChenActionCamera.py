"""Bind sampled Unity R camera and dragon fade. No map or gameplay timing edits."""
import unreal as u,json
from pathlib import Path
P=Path(u.Paths.project_dir()).resolve();O=P/'Artifacts/ChenQianyu/Actions/FX';L=u.EditorAssetLibrary
data=json.loads((O/'Review/camera-samples.json').read_text())
definition=u.load_asset('/Game/Sandbox/ACT/Character/ChenQianyu/AbilitySystem/Abilities/DA_Chen_LieFengShuang')
assert definition
samples=[]
for s in data['samples']:
 rotation=u.MathLibrary.make_rot_from_xz(u.Vector(*s['forward']),u.Vector(*s['up']))
 samples.append(u.PadmaACTCameraSample(time=s['time'],position=u.Vector(*s['position']),rotation=rotation,vertical_fov=s['vertical_fov']))
definition.set_editor_property('camera_samples',samples)
definition.set_editor_property('camera_duration',data['action_duration'])
definition.set_editor_property('camera_ease_out',data['ease_out'])
assert L.save_loaded_asset(definition,False)
m=u.load_asset('/Game/Sandbox/ACT/Character/ChenQianyu/Art/Niagara/Materials/M_Chen_Dragon');ml=u.MaterialEditingLibrary
m.set_editor_property('blend_mode',u.BlendMode.BLEND_MASKED)
if not ml.get_material_property_input_node(m,u.MaterialProperty.MP_OPACITY_MASK):
 fade=ml.create_material_expression(m,u.MaterialExpressionScalarParameter);fade.set_editor_property('parameter_name','SourceOpacity');fade.set_editor_property('default_value',1.)
 dither=ml.create_material_expression(m,u.MaterialExpressionMaterialFunctionCall)
 assert dither.set_material_function(u.load_asset('/Engine/Functions/Engine_MaterialFunctions02/Utility/DitherTemporalAA'))
 assert ml.connect_material_expressions(fade,'',dither,'Alpha Threshold')
 assert ml.connect_material_property(dither,'',u.MaterialProperty.MP_OPACITY_MASK)
ml.recompile_material(m);assert L.save_loaded_asset(m,False)
mesh=u.load_asset('/Game/Sandbox/ACT/Character/ChenQianyu/Art/Niagara/Meshes/SK_Chen_Dragon')
slots=mesh.get_editor_property('materials');assert len(slots)>0
for index,slot in enumerate(slots):
 slot.set_editor_property('material_interface',m);slots[index]=slot
mesh.set_editor_property('materials',slots);assert L.save_loaded_asset(mesh,False)
(O/'Review/camera-bound.json').write_text(json.dumps({'asset':definition.get_path_name(),'samples':len(samples),'duration':data['action_duration'],'source':data['source'],'binding':data['binding'],'fade':'source dragon motion end to source renderer stop; UE dither adaptation'},indent=2))
u.SystemLibrary.quit_editor()
