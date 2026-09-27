"""Import the original R dragon mesh with sampled generic source animation (centimeters)."""
import unreal as u,json,traceback
from pathlib import Path
P=Path(u.Paths.project_dir()).resolve();O=P/'Artifacts/ChenQianyu/Actions/FX';ROOT='/Game/Sandbox/ACT/Character/ChenQianyu/Art/Niagara/Meshes';L=u.EditorAssetLibrary;T=u.AssetToolsHelpers.get_asset_tools()
def main():
 # Existing authored dragon assets are canonical. Never recreate a second skeleton/animation.
 mesh=u.load_asset(ROOT+'/SK_Chen_Dragon')
 if mesh:
  anim=u.load_asset('/Game/Sandbox/ACT/Character/ChenQianyu/Animation/Sequences/Imported/SK_Chen_Dragon_Anim')
  assert anim and anim.get_editor_property('skeleton')==mesh.get_editor_property('skeleton')
  u.log('Existing dragon Content verified; no cache import or material rebuild.')
  return
 assert u.SystemLibrary.parse_param(u.SystemLibrary.get_command_line(),'PadmaOfflineReimport'), 'Missing dragon: restore Content or explicitly request offline reimport with -PadmaOfflineReimport'
 u.SystemLibrary.execute_console_command(None,'Interchange.FeatureFlags.Import.FBX 0')
 options=u.FbxImportUI()
 for k,v in dict(automated_import_should_detect_type=False,mesh_type_to_import=u.FBXImportType.FBXIT_SKELETAL_MESH,import_as_skeletal=True,import_mesh=True,import_animations=True,import_materials=False,import_textures=False,create_physics_asset=False).items():options.set_editor_property(k,v)
 for k,v in dict(import_uniform_scale=1.,use_t0_as_ref_pose=False).items():options.skeletal_mesh_import_data.set_editor_property(k,v)
 for k,v in dict(import_uniform_scale=1.,use_default_sample_rate=False,custom_sample_rate=60).items():options.anim_sequence_import_data.set_editor_property(k,v)
 task=u.AssetImportTask()
 for k,v in dict(filename=str(O/'dragon-bake/centimeters/SK_Chen_Dragon.fbx'),destination_path=ROOT,destination_name='SK_Chen_Dragon',automated=True,replace_existing=True,save=True,options=options,factory=u.FbxFactory()).items():task.set_editor_property(k,v)
 T.import_asset_tasks([task]);mesh=u.load_asset(ROOT+'/SK_Chen_Dragon');assert isinstance(mesh,u.SkeletalMesh),task.imported_object_paths
 animations=[u.load_asset(p) for p in L.list_assets(ROOT) if isinstance(u.load_asset(p),u.AnimSequence)];assert len(animations)==1
 anim=animations[0];assert abs(anim.get_play_length()-2.45)<.02
 skeleton=mesh.get_editor_property('skeleton')
 assert L.rename_loaded_asset(skeleton,'/Game/Sandbox/ACT/Character/ChenQianyu/Art/Niagara/Skeletons/SK_Chen_Dragon_Skeleton')
 assert L.rename_loaded_asset(anim,'/Game/Sandbox/ACT/Character/ChenQianyu/Animation/Sequences/Imported/SK_Chen_Dragon_Anim')
 assert L.save_loaded_asset(mesh.get_editor_property('skeleton'),False)
 assert L.save_loaded_asset(anim,False)
 import sys
 sys.path.insert(0,str(P/'Scripts/Editor'))
 from PadmaChenDragonMaterial import restore_dragon_material
 restore_dragon_material()
 report={'status':'saved','mesh':mesh.get_path_name(),'animation':anim.get_path_name(),'duration':anim.get_play_length(),'source':'P_chen_exhibit_03_lonng / A_actor_chen_long_ani_01','shader':'UE DefaultLit approximation; original base/normal/emission/metal-spec-shadow-smooth textures and source emission tint/intensity'}
 (O/'dragon.json').write_text(json.dumps(report,indent=2))
main()
