"""Run in UE 5.8 with PythonScriptPlugin and CascadeToNiagaraConverter enabled. SourceRoot via PADMA_CHEN_SOURCE env."""
import unreal as u, json, os, math, traceback
from pathlib import Path
R=Path(u.Paths.project_dir()).resolve()/'Artifacts/ChenQianyu/Actions/FX'
S=R/'SourceAssets';dest='/Game/Sandbox/ACT/Character/ChenQianyu/Art/Niagara'
lib=u.EditorAssetLibrary;at=u.AssetToolsHelpers.get_asset_tools();ml=u.MaterialEditingLibrary
f=u.FXConverterUtilitiesLibrary;Cat=u.ScriptExecutionCategory;Typ=u.NiagaraScriptInputType
report={'status':'started','systems':[],'limitations':['Unity custom shader/particle features are reconstructed, not byte-identical. Mesh prefab transform is baked at spawn; original rotation/noise/subUV/trail/custom-data behavior requires visual refinement.']}
def asset(name,cls,factory):
 folder=dest+('/Systems' if cls==u.NiagaraSystem else '/Materials' if cls==u.Material else '/Meshes')
 p=folder+'/'+name
 return lib.load_asset(p) if lib.does_asset_exist(p) else at.create_asset(name,folder,cls,factory)
def imp(file,name,mesh=False):
 folder=dest+('/Meshes' if mesh else '/Textures')
 p=folder+'/'+name
 if lib.does_asset_exist(p):return lib.load_asset(p)
 t=u.AssetImportTask();t.filename=str(S/file);t.destination_path=folder;t.destination_name=name;t.automated=True;t.save=True
 if mesh:
  o=u.FbxImportUI();o.automated_import_should_detect_type=False;o.mesh_type_to_import=u.FBXImportType.FBXIT_STATIC_MESH;o.import_materials=False;o.import_textures=False;o.static_mesh_import_data.import_uniform_scale=1.;o.static_mesh_import_data.combine_meshes=True;o.static_mesh_import_data.auto_generate_collision=False;t.options=o;t.factory=u.FbxFactory()
 at.import_asset_tasks([t]);assert t.get_objects(),file;return t.get_objects()[0]
textures={}
def tex(e):
 name=e['Name']+'_p'+f'{e["PathID"]&((1<<64)-1):016X}'
 if name not in textures:textures[name]=imp(e['source_png'],name)
 return textures[name]
def expression(mat,cls,**props):
 e=ml.create_material_expression(mat,cls)
 for k,v in props.items():e.set_editor_property(k,v)
 return e
def wire(a,out,b,pin=''):assert ml.connect_material_expressions(a,out,b,pin),(a,out,b,pin)
def material(p):
 source=p['materials'][0];d=source['data']['m_SavedProperties'];tx=source['textures'];fl=d['m_Floats'];colors=d['m_Colors']
 mat=asset('M_'+p['ue_name'],u.Material,u.MaterialFactoryNew());ml.delete_all_material_expressions(mat)
 mat.set_editor_property('blend_mode',u.BlendMode.BLEND_ADDITIVE);mat.set_editor_property('two_sided',True);mat.set_editor_property('shading_model',u.MaterialShadingModel.MSM_UNLIT)
 for usage in [u.MaterialUsage.MATUSAGE_NIAGARA_SPRITES,u.MaterialUsage.MATUSAGE_NIAGARA_RIBBONS,u.MaterialUsage.MATUSAGE_NIAGARA_MESH_PARTICLES]:ml.set_material_usage(mat,usage)
 main=tx.get('_MainTex') or tx.get('_MaskTex') or tx.get('_RefractTex')
 if main:sample=expression(mat,u.MaterialExpressionTextureSample,texture=tex(main));rgb=(sample,'RGB');alpha=(sample,'A')
 else:sample=expression(mat,u.MaterialExpressionConstant3Vector,constant=u.LinearColor(1,1,1,1));rgb=(sample,'');one=expression(mat,u.MaterialExpressionConstant,r=1.);alpha=(one,'')
 tint=colors.get('_TintColor',{'r':1,'g':1,'b':1,'a':1});c=expression(mat,u.MaterialExpressionConstant3Vector,constant=u.LinearColor(tint['r'],tint['g'],tint['b'],1))
 mul=expression(mat,u.MaterialExpressionMultiply);wire(*rgb,mul,'A');wire(c,'',mul,'B')
 pc=expression(mat,u.MaterialExpressionParticleColor);glow=expression(mat,u.MaterialExpressionMultiply);wire(mul,'',glow,'A');wire(pc,'RGB',glow,'B');assert ml.connect_material_property(glow,'',u.MaterialProperty.MP_EMISSIVE_COLOR)
 mask=tx.get('_MaskTex')
 if mask and mask!=main:
  ms=expression(mat,u.MaterialExpressionTextureSample,texture=tex(mask));a=expression(mat,u.MaterialExpressionMultiply);wire(*alpha,a,'A');wire(ms,'R',a,'B');alpha=(a,'')
 opacity=expression(mat,u.MaterialExpressionMultiply);wire(*alpha,opacity,'A');wire(pc,'A',opacity,'B');assert ml.connect_material_property(opacity,'',u.MaterialProperty.MP_OPACITY)
 ml.recompile_material(mat);lib.save_loaded_asset(mat);return mat
def args(path):return u.CreateScriptContextArgs(f.create_asset_data(path))
def module(ctx,name,path,cat):return ctx.find_or_add_module_script(name,args(path),cat)
def setp(ctx,key,value):assert ctx.set_parameter(key,value),key
def enum(path,value):return f.create_script_input_enum('/Niagara/Enums/'+path+'.'+path,value)
def basic(ctx,duration,count,delay=0.,rate=None):
 state=module(ctx,'EmitterState','/Niagara/Modules/Emitter/EmitterState.EmitterState',Cat.EMITTER_UPDATE)
 setp(state,'Life Cycle Mode',enum('ENiagaraEmitterLifeCycleMode','Self'));setp(state,'Loop Behavior',enum('ENiagara_EmitterStateOptions','Once'));setp(state,'Loop Duration',f.create_script_input_float(max(duration,.01)))
 if rate is None and count>0:
  spawn=module(ctx,'Burst','/Niagara/Modules/Emitter/SpawnBurst_Instantaneous.SpawnBurst_Instantaneous',Cat.EMITTER_UPDATE)
  setp(spawn,'Spawn Count',f.create_script_input_int(count));setp(spawn,'Spawn Time',f.create_script_input_float(delay))
 elif rate is not None:
  spawn=module(ctx,'Rate','/Niagara/Modules/Emitter/SpawnRate.SpawnRate',Cat.EMITTER_UPDATE);setp(spawn,'SpawnRate',f.create_script_input_float(rate))
 module(ctx,'ParticleState','/Niagara/Modules/Update/Lifetime/ParticleState.ParticleState',Cat.PARTICLE_UPDATE)
 init=module(ctx,'Initialize','/Niagara/Modules/Spawn/Initialization/V2/InitializeParticle.InitializeParticle',Cat.PARTICLE_SPAWN)
 return init
def curve_input(points):
 keys=[]
 for t,v in points:
  key=u.RichCurveKeyBP();key.set_editor_property('time',t);key.set_editor_property('value',v);key.set_editor_property('interp_mode',u.RichCurveInterpMode.RCIM_LINEAR);keys.append(key)
 di=f.create_float_curve_di(keys);sc=f.create_script_context(args('/Niagara/DynamicInputs/ValueFromCurve/FloatFromCurve.FloatFromCurve'))
 setp(sc,'FloatCurve',f.create_script_input_di(di));setp(sc,'CurveIndex',f.create_script_input_linked_parameter('Particles.NormalizedAge',Typ.FLOAT));return f.create_script_input_dynamic(sc,Typ.FLOAT)
def color_input(p):
 cm=p['particle']['ColorModule'];init=p['particle']['InitialModule']['startColor']['maxColor']
 if cm['enabled'] and cm['gradient']['minMaxState']==1:
  g=cm['gradient']['maxGradient'];rgb=[]
  for channel in 'rgb':rgb.append([(g['ctime'+str(i)]/65535,g['key'+str(i)][channel]*init[channel]) for i in range(g['m_NumColorKeys'])])
  a=[(g['atime'+str(i)]/65535,g['key'+str(i)]['a']*init['a']) for i in range(g['m_NumAlphaKeys'])]
  return rgb,a
 return [[(0,init[c]),(1,init[c])] for c in 'rgb'],[(0,init['a']),(1,0)]
