"""Source particle motion. Execute after emission.py in the authoring context.

Linear life velocities retain per-particle random samples. Clamp uses the explicit
60 Hz approximation in motion.py. Shape circle direction is radial, not cone-axis.
"""
def link(name,typ=Typ.VEC3):return f.create_script_input_linked_parameter(name,typ)
def addv(a,b):return dynamic('Add/Add_Vector',Typ.VEC3,A=a,B=b)
def mulv(a,b):return dynamic('Multiply/Multiply_VectorByFloat',Typ.VEC3,Vector=a,Float=b)

def life_curve(ce,value,key):
 import copy
 if value.get('minMaxState')==0:return F(value['scalar'])
 if value.get('minMaxState')==3 and value.get('minScalar')==value['scalar']:return F(value['scalar'])
 high=copy.deepcopy(value);high['minMaxState']=1 if value.get('minMaxState') in [1,2] else 0
 hp=[(i/32,source_curve(high,i/32)) for i in range(33)]
 if value.get('minMaxState') not in [2,3]:return curve_input(hp)
 low=copy.deepcopy(high);low['scalar']=value['scalar'] if value.get('minMaxState')==2 else value.get('minScalar',value['scalar']);low['maxCurve']=value['minCurve']
 lp=[(i/32,source_curve(low,i/32)) for i in range(33)]
 rnd='Particles.SourceRandom'+key
 ce.set_parameter_directly(rnd,dynamic('UniformRange/V2/RandomRangeFloat',Typ.FLOAT,Minimum=F(0),Maximum=F(1)),Cat.PARTICLE_SPAWN)
 delta=curve_input([(a[0],a[1]-b[1]) for a,b in zip(hp,lp)])
 return dynamic('Add/Add_Float',Typ.FLOAT,A=curve_input(lp),B=dynamic('Multiply/Multiply_Float',Typ.FLOAT,A=delta,B=link(rnd,Typ.FLOAT)))

def nonzero(value):
 import copy
 if value.get('minMaxState')==3:return max(abs(value['scalar']),abs(value.get('minScalar',value['scalar'])))>1e-6
 if any(abs(source_curve(value,i/16))>1e-6 for i in range(17)):return True
 if value.get('minMaxState')==2:
  v=copy.deepcopy(value);v['maxCurve']=value['minCurve']
  return any(abs(source_curve(v,i/16))>1e-6 for i in range(17))
 return False

def source_motion(ce,p):
 q=p['particle'];initial=q['InitialModule'];vm=q['VelocityModule'];speed=scalar(initial['startSpeed'])*100
 active=nonzero(initial['startSpeed']) or vm['enabled'] or q['ForceModule']['enabled']
 if not active:return
 if nonzero(initial['startSpeed']):
  vel=module(ce,'SourceVelocity',Paths.script_add_velocity,Cat.PARTICLE_SPAWN)
  shape=q['ShapeModule']
  if shape['enabled'] and shape['type'] in [0,10]:
   position=dynamic('Transforms/ConvertPositionToVector',Typ.VEC3,**{'Input Position':link('Particles.Position',Typ.POSITION)})
   direction=dynamic('Helpers/SafeNormalizeVector',Typ.VEC3,**{'Vector To Normalize':dynamic('Subtract/Subtract_Vector',Typ.VEC3,A=position,B=V(shape_frame(p)[0]))})
   setp(vel,'Velocity Mode',f.create_script_input_enum(Paths.enum_niagara_velocity_mode,'Linear'))
   setp(vel,'Velocity',mulv(direction,random_float(initial['startSpeed'],100)))
  else:
   setp(vel,'Velocity Mode',f.create_script_input_enum(Paths.enum_niagara_velocity_mode,'In Cone'));setp(vel,'Velocity Speed',random_float(initial['startSpeed'],100))
   axis=shape_frame(p)[1][2] if shape['enabled'] else p['v2_rotation_axis'];n=max(1e-8,math.sqrt(sum(v*v for v in axis)))
   setp(vel,'Cone Axis',V([v/n for v in axis]));setp(vel,'Cone Angle',F(shape['angle']/180 if shape['enabled'] and shape['type'] in [4,8] else 0));setp(vel,'Cone Angle Mode',f.create_script_input_enum(Paths.enum_niagara_angle_input,'Normalized Angle (0-1)'))
 ce.set_parameter_directly('Particles.SourceInitialVelocity',link('Particles.Velocity'),Cat.PARTICLE_SPAWN)
 velocity=link('Particles.SourceInitialVelocity')
 if q['ClampVelocityModule']['enabled']:velocity=mulv(velocity,curve_input(motion_envelope(p)))
 if vm['enabled']:
  # C = (-X,Z,Y), local velocity additionally follows the source emitter rotation.
  matrix=p['unity_transform_matrix']
  axes=[[-matrix[0][i],matrix[2][i],matrix[1][i]] for i in range(3)]
  if vm['inWorldSpace']:axes=[[-1,0,0],[0,0,1],[0,1,0]]
  linear=V([0,0,0])
  for i,k in enumerate('xyz'):
   if not nonzero(vm[k]):continue
   n=max(1e-8,math.sqrt(sum(x*x for x in axes[i])))
   linear=addv(linear,mulv(V([x*100/n for x in axes[i]]),life_curve(ce,vm[k],k)))
  if vm['inWorldSpace']:
   linear=dynamic('Transforms/TransformVector',Typ.VEC3,Vector=linear,**{'Source Space':f.create_script_input_enum(Paths.enum_niagara_coordinate_space,'World'),'Destination Space':f.create_script_input_enum(Paths.enum_niagara_coordinate_space,'Simulation')})
  position=dynamic('Transforms/ConvertPositionToVector',Typ.VEC3,**{'Input Position':link('Particles.Position',Typ.POSITION)})
  offset=V(p['v2_position']);omega=V([0,0,0])
  for i,k in enumerate('XYZ'):
   n=max(1e-8,math.sqrt(sum(x*x for x in axes[i])))
   axis=V([x/n for x in axes[i]])
   if nonzero(vm['orbitalOffset'+k]):offset=addv(offset,mulv(axis,dynamic('Multiply/Multiply_Float',Typ.FLOAT,A=life_curve(ce,vm['orbitalOffset'+k],'Offset'+k),B=F(100))))
   if nonzero(vm['orbital'+k]):omega=addv(omega,mulv(axis,life_curve(ce,vm['orbital'+k],'Orbit'+k)))
  relative=dynamic('Subtract/Subtract_Vector',Typ.VEC3,A=position,B=offset)
  direction=dynamic('Helpers/SafeNormalizeVector',Typ.VEC3,**{'Vector To Normalize':relative})
  radial=mulv(direction,dynamic('Multiply/Multiply_Float',Typ.FLOAT,A=life_curve(ce,vm['radial'],'Radial'),B=F(100))) if nonzero(vm['radial']) else V([0,0,0])
  orbit=dynamic('Vectors/CrossProduct',Typ.VEC3,**{'Vector A':omega,'Vector B':relative,'Normalize Incoming Vectors':f.create_script_input_bool(False)}) if any(nonzero(vm['orbital'+k]) for k in 'XYZ') else V([0,0,0])
  # Angular integration is a UE Euler approximation; preserve exported axis curves.
  velocity=mulv(addv(addv(addv(velocity,linear),radial),orbit),life_curve(ce,vm['speedModifier'],'Speed'))
 force=q['ForceModule']
 if force['enabled']:
  import copy
  for i,k in enumerate('xyz'):
   if not nonzero(force[k]):continue
   value=copy.deepcopy(force[k]);mode=value['minMaxState']
   for curve_name in ['maxCurve','minCurve']:
    old=copy.deepcopy(force[k]);old['maxCurve']=value[curve_name];old['minMaxState']=1 if mode in [1,2] else 0
    if mode==3 and curve_name=='minCurve':old['scalar']=old.get('minScalar',old['scalar'])
    integral=0.;keys=[];previous=source_curve(old,0)
    for j in range(33):
     t=j/32;current=source_curve(old,t)
     if j:integral+=(previous+current)*.5/32
     keys.append({'time':t,'value':integral,'inSlope':current,'outSlope':current});previous=current
    value[curve_name]['m_Curve']=keys
   value['minMaxState']=2 if mode in [2,3] else 1
   value['scalar']=1.
   matrix=p['unity_transform_matrix'];axis=[-matrix[0][i],matrix[2][i],matrix[1][i]]
   if force.get('inWorldSpace'):axis=[[-1,0,0],[0,0,1],[0,1,0]][i]
   n=max(1e-8,math.sqrt(sum(x*x for x in axis)))
   impulse=mulv(V([x*100/n for x in axis]),dynamic('Multiply/Multiply_Float',Typ.FLOAT,A=life_curve(ce,value,'Force'+k),B=link('Particles.Lifetime',Typ.FLOAT)))
   if force.get('inWorldSpace'):impulse=dynamic('Transforms/TransformVector',Typ.VEC3,Vector=impulse,**{'Source Space':f.create_script_input_enum(Paths.enum_niagara_coordinate_space,'World'),'Destination Space':f.create_script_input_enum(Paths.enum_niagara_coordinate_space,'Simulation')})
   velocity=addv(velocity,impulse)
 ce.set_parameter_directly('Particles.Velocity',velocity,Cat.PARTICLE_UPDATE)
 module(ce,'Motion','/Niagara/Modules/Solvers/SolveForcesAndVelocity.SolveForcesAndVelocity',Cat.PARTICLE_UPDATE)
