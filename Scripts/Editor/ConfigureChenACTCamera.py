"""Author the shared ACT character camera profile and lock marker; preserve all action/BP/ABP data."""
import json
from pathlib import Path
import unreal as u

def apply():
 project=Path(u.Paths.project_dir()).resolve()
 source=json.loads((project/'Scripts/Editor/ChenACTCameraSource.json').read_text(encoding='utf-8-sig'))
 root='/Game/Sandbox/ACT/Character/ChenQianyu/Presentation/Camera'
 character_root='/Game/Sandbox/ACT/Character/ChenQianyu'
 lib=u.EditorAssetLibrary
 tools=u.AssetToolsHelpers.get_asset_tools()
 profile=lib.load_asset(root+'/DA_ACTCamera_CHEN')
 if not profile:
  factory=u.DataAssetFactory();factory.set_editor_property('data_asset_class',u.PadmaACTCameraDefinition)
  profile=tools.create_asset('DA_ACTCamera_CHEN',root,u.PadmaACTCameraDefinition,factory)
 assert profile
 for k,v in source['parameters'].items():profile.set_editor_property(k,v)
 # Keep the user's mouse/zoom tuning when rebuilding the source-derived profile.
 import sys
 sys.path.insert(0,str(project/'Scripts/Editor'))
 from ConfigureChenCameraInput import apply_tuning
 apply_tuning(profile)
 profile.set_editor_property('orbit',[u.PadmaACTOrbitSample(**s) for s in source['orbit']])
 material=lib.load_asset(character_root+'/Art/Materials/M_ACTLockMarker')
 if not material:
  material=tools.create_asset('M_ACTLockMarker',character_root+'/Art/Materials',u.Material,u.MaterialFactoryNew())
  material.set_editor_property('shading_model',u.MaterialShadingModel.MSM_UNLIT)
  material.set_editor_property('blend_mode',u.BlendMode.BLEND_MASKED)
  material.set_editor_property('two_sided',True)
  ml=u.MaterialEditingLibrary
  uv=ml.create_material_expression(material,u.MaterialExpressionTextureCoordinate)
  custom=ml.create_material_expression(material,u.MaterialExpressionCustom)
  custom.set_editor_property('code','float r=length(UV-0.5); return step(0.32,r)*step(r,0.45);')
  custom.set_editor_property('output_type',u.CustomMaterialOutputType.CMOT_FLOAT1)
  input_uv=u.CustomInput();input_uv.set_editor_property('input_name','UV')
  custom.set_editor_property('inputs',[input_uv])
  assert ml.connect_material_expressions(uv,'',custom,'UV')
  assert ml.connect_material_property(custom,'',u.MaterialProperty.MP_OPACITY_MASK)
  color=ml.create_material_expression(material,u.MaterialExpressionConstant3Vector)
  color.set_editor_property('constant',u.LinearColor(.05,1.,.85,1.))
  assert ml.connect_material_property(color,'',u.MaterialProperty.MP_EMISSIVE_COLOR)
  ml.recompile_material(material)
 assert lib.save_loaded_asset(material,False)
 profile.set_editor_property('lock_marker_material',material)
 assert lib.save_loaded_asset(profile,False)
 character=lib.load_asset(character_root+'/AbilitySystem/DA_ACTCharacter_CHEN')
 assert character
 character.set_editor_property('camera_profile',profile)
 assert lib.save_loaded_asset(character,False)
 out=project/'Artifacts/ChenQianyu/Actions/Camera';out.mkdir(parents=True,exist_ok=True)
 (out/'author.json').write_text(json.dumps({'profile':profile.get_path_name(),'character':character.get_path_name(),'source':source['source'],'samples':len(source['orbit'])},indent=2),encoding='utf-8')

if __name__=='__main__':
 try:apply()
 finally:u.SystemLibrary.quit_editor()
