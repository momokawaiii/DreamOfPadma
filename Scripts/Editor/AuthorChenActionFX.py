import unreal as u
from pathlib import Path
# Reuse tested UE import/context helpers, without running the v1 authoring body.
HELPERS=Path(u.Paths.project_dir()).resolve()/'Scripts/Editor/ChenFX'
exec((HELPERS/'support.py').read_text())
dest='/Game/Sandbox/ACT/Character/ChenQianyu/Art/Niagara';S=R/'SourceAssetsV2';data=json.loads((R/'prepared_v2.json').read_text());report={'status':'started','systems':[],'changes':['Source RGBA over lifetime with active color-space conversion','Source UV scale, offset, rotation, alpha masks and custom-data animation','Angular velocity integrated over lifetime; prefab pivot retained','Size over lifetime, stretched sparks and source lifetime/burst timing'],'approximations':['Shared HG shader custom-data interpretation is reconstructed: xy UV offsets, z dissolve, w disturbance.','Noise, exact random distributions and source refraction are approximations.']}
def scalar(v):
 return (v['scalar']+v.get('minScalar',v['scalar']))*.5 if v['minMaxState']==3 else v['scalar']
def ff(x):return f'{float(x):.9f}'
def fv(a):return 'float'+str(len(a))+'('+','.join(ff(x) for x in a)+')'
def vec_curve(points):
 sc=f.create_script_context(args('/Niagara/DynamicInputs/TypeConversions/MakeVector.MakeVector'))
 for j,k in enumerate(['X','Y','Z']):setp(sc,k,curve_input([(p[0],p[j+1]) for p in points]))
 return f.create_script_input_dynamic(sc,Typ.VEC3)
exec((HELPERS/'material.py').read_text())
if os.environ.get('PADMA_FX_REUSE_MATERIALS')=='1':
 def material_v2(p):
  m=lib.load_asset(dest+'/Materials/M_'+p['ue_name']);assert m,p['ue_name'];return m
import sys
sys.path.insert(0,'E:/Epic Games/UE_5.8/Engine/Plugins/FX/CascadeToNiagaraConverter/Content/Python')
import Paths
exec((HELPERS/'motion.py').read_text())
exec((HELPERS/'emission.py').read_text())
exec((HELPERS/'velocity.py').read_text())
exec((HELPERS/'mesh.py').read_text())
mesh_spawn=json.loads((R/'mesh-spawn.json').read_text()) if (R/'mesh-spawn.json').exists() else {}
lod_config=json.loads((HELPERS.parent/'ChenFXSourceLod.json').read_text())
excluded={r['ue_name']:r for r in lod_config['excluded_emitters']}
def source_enabled(p):
 row=excluded.get(p['ue_name'])
 if row:
  assert (row['cab'],row['path_id'])==(p['source']['sourceFile'],p['source']['pathId']),p['ue_name']
 return row is None
report['excluded_emitters']=lod_config['excluded_emitters']
report['motion_envelopes'] = []
report['approximations'] = ['Custom data x/y drive weighted main/sample UV paths, z dissolve, w disturbance, reconstructed from selected DXBC variants.', 'ClampVelocity uses exported limit/dampen with a 60 Hz mean-lifetime/mean-speed envelope, not recovered native Unity integration.', 'Source noise and exact shape/random distributions remain approximations; refraction/radial blur use separate UE adaptations.']
def random_float(v,scale=1):
 if v['minMaxState']==3:
  sc=f.create_script_context(args('/Niagara/DynamicInputs/UniformRange/V2/RandomRangeFloat.RandomRangeFloat'))
  setp(sc,'Minimum',f.create_script_input_float(min(v['minScalar'],v['scalar'])*scale));setp(sc,'Maximum',f.create_script_input_float(max(v['minScalar'],v['scalar'])*scale))
  return f.create_script_input_dynamic(sc,Typ.FLOAT)
 return f.create_script_input_float(v['scalar']*scale)
def source_trail(ctx,p,j):
 import copy
 trail=p['particle']['TrailModule'];em=ctx.add_empty_emitter('SourceTrailP'+str(j).zfill(2));em.set_local_space(not trail['worldSpace'])
 q=p['particle'];init=basic(em,scalar(q['startDelay'])+emission_duration(q)+max(q['InitialModule']['startLifetime']['scalar'],q['InitialModule']['startLifetime'].get('minScalar',0))+scalar(trail['lifetime']),0);setp(init,'Lifetime',random_float(trail['lifetime']))
 prop=u.NiagaraEventHandlerAddAction();prop.mode=u.NiagaraEventHandlerAddMode.ADD_EVENT_AND_EVENT_GENERATOR;prop.execution_mode=u.ScriptExecutionMode.SPAWNED_PARTICLES;prop.spawn_number=1;prop.source_event_name='LocationEvent';prop.max_events_per_frame=0
 gen=u.NiagaraAddEventGeneratorOptions();gen.source_emitter_name='SourceP'+str(j).zfill(2);gen.event_generator_script_asset_data=f.create_asset_data(Paths.script_generate_location_event);prop.add_event_generator_options=gen;em.add_event_handler(prop)
 recv=em.find_or_add_module_event_script('ReceiveSourceParticle',args(Paths.script_receive_location_event),prop)
 # Converter's standard ReceiveLocationEvent writes the source position and persistent ribbon ID.
 pp=copy.deepcopy(p);pp['materials']=[p['materials'][1]];pp['ue_name']=p['ue_name']+'_trail';mat=material_v2(pp)
 colors=p['v2_trail_color'];comp=f.create_script_context(args('/Niagara/DynamicInputs/LinearColor/MakeLinearColorFromVectorAndFloat.MakeLinearColorFromVectorAndFloat'));setp(comp,'Vector (RGB)',vec_curve([x[:4] for x in colors]));setp(comp,'Float (Alpha)',curve_input([(x[0],x[4]*(1-x[0])) for x in colors]));col=module(em,'TrailColor','/Niagara/Modules/Update/Color/Color.Color',Cat.PARTICLE_UPDATE);setp(col,'Color',f.create_script_input_dynamic(comp,Typ.LINEAR_COLOR))
 width=scalar(p['particle']['InitialModule']['startSize'])*100 if trail['sizeAffectsWidth'] else 100
 em.set_parameter_directly('Particles.RibbonWidth',curve_input([(t,w*width) for t,w in p['v2_trail_width']]),Cat.PARTICLE_UPDATE)
 ren=u.NiagaraRibbonRendererProperties();ren.set_editor_property('material',mat);em.add_renderer('SourceTrailMaterial',ren)
try:
 world=u.EditorLoadingAndSavingUtils.new_blank_map(False)
 for e in data:
  if os.environ.get('PADMA_FX_FILTER') and e['effect'] not in os.environ['PADMA_FX_FILTER'].split(','):continue
  contact_origin_revision=e['effect']=='P_fxbat_chen_common_hit_02'
  if contact_origin_revision:
   # CreateEffectByCfgAtPos supplies the root position at runtime. The exported
   # prefab's scene placement (Unity 0, 2.105, 4.99 m) is not a particle offset.
   for p in e['particles']:
    root=p['transforms'][-1]
    assert root['m_Father']['IsNull'] and root['m_GameObject']['Name']==e['effect']
    assert root['m_LocalRotation']=={'X':0.,'Y':0.,'Z':0.,'W':1.}
    assert all(root['m_LocalScale'][k]==1. for k in 'XYZ')
    position=root['m_LocalPosition'];offset=[-position['X']*100,position['Z']*100,position['Y']*100]
    p['v2_position']=[v-o for v,o in zip(p['v2_position'],offset)]
  mesh_revision=e['effect']=='P_chen_combo_skill'
  if mesh_revision:assert all('chen_combo_skill_'+str(j) in mesh_spawn for j in [2,6,10]),'Run PrepareChenRandomMeshFX.py first'
  name='NS_'+e['effect'][2:];path=dest+'/Systems/'+name
  if lib.does_asset_exist(path):
   existing=lib.load_asset(path)
   assert lib.get_metadata_tag(existing,'PadmaAuthor')=='ChenActionFX20260920',path
   rebuild=set(json.loads((R/'refinement-systems.json').read_text())) if os.environ.get('PADMA_FX_REFINE')=='1' else set()
   if os.environ.get('PADMA_FX_REBUILD')!='1' and (not contact_origin_revision or lib.get_metadata_tag(existing,'ContactOriginRevision')=='1') and (not mesh_revision or lib.get_metadata_tag(existing,'SourceRevision')=='6') and (not any(not source_enabled(p) for p in e['particles']) or lib.get_metadata_tag(existing,'SourceRevision')=='5') and lib.get_metadata_tag(existing,'SourceComplete')=='1' and (e['effect'] not in rebuild or lib.get_metadata_tag(existing,'SourceRevision')=='2'):
    report['systems'].append(path);continue
   u.SystemLibrary.execute_console_command(world,'Padma.ChenActions.FX Reset '+name)
  system=asset(name,u.NiagaraSystem,u.NiagaraSystemFactoryNew());lib.set_metadata_tag(system,'PadmaAuthor','ChenActionFX20260920');ctx=f.create_system_conversion_context(system)
  if contact_origin_revision:lib.set_metadata_tag(system,'ContactOriginRevision','1')
  for j,p in enumerate(e['particles']):
   if not source_enabled(p):continue
   q=p['particle'];initial=q['InitialModule'];ce=ctx.add_empty_emitter('SourceP'+str(j).zfill(2));ce.set_local_space(True)
   bursts=q['EmissionModule'].get('m_Bursts',[]);count=max(0,int(scalar(bursts[0]['countCurve']))) if bursts else 0;delay=scalar(q['startDelay'])+(bursts[0]['time'] if bursts else 0);life=max(.01,scalar(initial['startLifetime']))
   init=setup_emission(ce,p);setp(init,'Lifetime',random_float(initial['startLifetime']))
   mat=material_v2(p)
   setp(init,'Position Mode',f.create_script_input_enum(Paths.enum_niagara_position_initialization_mode,'Simulation Position'));setp(init,'UsePositionOffset',f.create_script_input_bool(True));assert init.set_parameter('Position Offset',source_shape(ce,p),True,True);setp(init,'Position Offset Coordinate Space',f.create_script_input_enum(Paths.enum_niagara_coordinate_space,'Local'))
   colors=p['v2_color'];compose=f.create_script_context(args('/Niagara/DynamicInputs/LinearColor/MakeLinearColorFromVectorAndFloat.MakeLinearColorFromVectorAndFloat'))
   setp(compose,'Vector (RGB)',vec_curve([x[:4] for x in colors]));setp(compose,'Float (Alpha)',curve_input([(x[0],x[4]) for x in colors]))
   color_script=module(ce,'SourceColorOverLifetime','/Niagara/Modules/Update/Color/Color.Color',Cat.PARTICLE_UPDATE);setp(color_script,'Color',f.create_script_input_dynamic(compose,Typ.LINEAR_COLOR))
   setp(module(ce,'InitialColor','/Niagara/Modules/Update/Color/Color.Color',Cat.PARTICLE_SPAWN),'Color',f.create_script_input_linear_color(u.LinearColor(*colors[0][1:])))
   if 'source_obj' in p:
    record=mesh_spawn.get(p['ue_name'])
    if record:assert record['source']['sourceFile']==p['source']['sourceFile'] and record['source']['pathId']==p['source']['pathId']
    mesh=imp(record['mesh']+'.obj',record['mesh'],True) if record else imp(p['source_obj'],'SM_'+p['ue_name'],True)
    mesh.set_material(0,mat);lib.save_loaded_asset(mesh);ren=f.create_mesh_renderer_properties();mp=u.NiagaraMeshRendererMeshProperties();mp.set_editor_property('mesh',mesh);ren.set_editor_property('meshes',[mp]);ce.add_renderer('SourceMesh',ren)
    if record:source_mesh_transform(ce,p,record)
    else:
     ce.set_parameter_directly('Particles.Scale',vec_curve([[x[0],x[1],x[1],x[1]] for x in p['v2_size']]),Cat.PARTICLE_UPDATE)
     rot=f.create_script_context(args('/Niagara/DynamicInputs/TypeConversions/MakeQuatFromAxisAngle.MakeQuatFromAxisAngle'));setp(rot,'Axis',f.create_script_input_vector(u.Vector(*p['v2_rotation_axis'])));setp(rot,'AngleInDegrees',curve_input([(t,a*180/math.pi) for t,a in p['v2_angle']]))
     ce.set_parameter_directly('Particles.MeshOrientation',f.create_script_input_dynamic(rot,Typ.QUATERNION),Cat.PARTICLE_UPDATE)
   else:
    ren=u.NiagaraSpriteRendererProperties();ren.set_editor_property('material',mat);ce.add_renderer('SourceSprite',ren)
    size=max(.2,scalar(initial['startSize'])*100);stretch=p['renderer']['m_RenderMode']==1;sy=scalar(initial['startSizeY'])*100 if initial['size3D'] else size
    length=sy*p['renderer'].get('m_LengthScale',1)+scalar(initial['startSpeed'])*100*p['renderer'].get('m_VelocityScale',0) if stretch else sy
    ce.set_parameter_directly('Particles.SpriteSize',f.create_script_input_vec2(u.Vector2D(size,length)),Cat.PARTICLE_SPAWN)
    ce.set_parameter_directly('Particles.SpriteRotation',random_float(initial['startRotation'],180/math.pi),Cat.PARTICLE_SPAWN)
    if stretch:ren.set_editor_property('alignment',u.NiagaraSpriteAlignment.VELOCITY_ALIGNED)
    uvmod=q['UVModule']
    if uvmod['enabled']:
     ren.set_editor_property('sub_image_size',u.Vector2D(uvmod['tilesX'],uvmod['tilesY']))
     ce.set_parameter_directly('Particles.SubImageIndex',random_float(uvmod['frameOverTime'],uvmod['tilesX']*uvmod['tilesY']),Cat.PARTICLE_SPAWN)
    sz=f.create_script_context(args('/Niagara/DynamicInputs/TypeConversions/MakeVector2D.MakeVector2D'))
    envelope=motion_envelope(p)
    setp(sz,'X',curve_input([(s[0],s[1]*size) for s in p['v2_size_xy']]))
    setp(sz,'Y',curve_input([(s[0],s[2]*(sy*p['renderer'].get('m_LengthScale',1)+scalar(initial['startSpeed'])*100*p['renderer'].get('m_VelocityScale',0)*envelope_value(envelope,s[0])) if stretch else s[2]*sy) for s in p['v2_size_xy']]))
    ce.set_parameter_directly('Particles.SpriteSize',f.create_script_input_dynamic(sz,Typ.VEC2),Cat.PARTICLE_UPDATE)
   source_motion(ce,p)
   if q['TrailModule']['enabled'] and len(p['materials'])>1:source_trail(ctx,p,j)
  ctx.finalize()
  u.SystemLibrary.execute_console_command(world,'Padma.ChenActions.FX Order '+name)
  u.SystemLibrary.execute_console_command(world,'Padma.ChenActions.FX Finish '+name)
  result=(R/'compile.tsv').read_text(encoding='utf-8-sig').strip().split('\t')
  expected=sum(1+int(p['particle']['TrailModule']['enabled'] and len(p['materials'])>1) for p in e['particles'] if source_enabled(p))
  assert result==[path,str(expected),'1'],result
  lib.set_metadata_tag(system,'SourceComplete','1');lib.set_metadata_tag(system,'SourceRevision','6' if mesh_revision else '5');lib.save_loaded_asset(system,False)
  ctx.cleanup();report['systems'].append(path)
  (R/'author.json').write_text(json.dumps(report,indent=2))
 report['status']='saved'
except Exception:
 report.update(status='error',error=traceback.format_exc());u.log_error(report['error'])
 if 'ctx' in globals():ctx.cleanup()
(R/'author.json').write_text(json.dumps(report,indent=2));u.SystemLibrary.quit_editor()
