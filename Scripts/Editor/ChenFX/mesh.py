"""Per-particle Unity ZXY mesh rotation for the audited Q blade layers.

Source-local mesh coordinates use C=(-X,Z,Y). C is a proper rotation, so mapped
axes preserve handedness. Random values are written once at spawn, not per tick.
"""
def source_mesh_transform(ce, p, record):
 import copy
 def quat(axis, angle):
  return dynamic('TypeConversions/MakeQuatFromAxisAngle',Typ.QUATERNION,
   Axis=V(axis),AngleInDegrees=angle)
 def multiply(a,b):
  return dynamic('Rotation/MultiplyQuaternion',Typ.QUATERNION,
   **{'Quaternion A':a,'Quaternion B':b})
 def add(a,b):return dynamic('Add/Add_Float',Typ.FLOAT,A=a,B=b)
 def mul(a,b):return dynamic('Multiply/Multiply_Float',Typ.FLOAT,A=a,B=b)
 initial=p['particle']['InitialModule'];rotation=p['particle']['RotationModule']
 angles=[]
 for index,(key,channel) in enumerate(zip(['startRotationX','startRotationY','startRotation'],['x','y','curve'])):
  name='Particles.SourceInitialAngle'+str(index)
  ce.set_parameter_directly(name,random_float(initial[key],180/math.pi) if initial['rotation3D'] or index==2 else F(0),Cat.PARTICLE_SPAWN)
  angle=link(name,Typ.FLOAT)
  if rotation['enabled'] and (rotation['separateAxes'] or index==2):
   value=copy.deepcopy(rotation[channel]);mode=value['minMaxState']
   for curve_name in ['maxCurve','minCurve']:
    original=copy.deepcopy(rotation[channel]);original['maxCurve']=value[curve_name]
    original['minMaxState']=1 if mode in [1,2] else 0
    if mode==3 and curve_name=='minCurve':original['scalar']=original.get('minScalar',original['scalar'])
    integral=0.;previous=source_curve(original,0);keys=[]
    for j in range(65):
     t=j/64;current=source_curve(original,t)
     if j:integral+=(previous+current)*.5/64
     keys.append(dict(time=t,value=integral,inSlope=current,outSlope=current));previous=current
    value[curve_name]={'m_Curve':keys}
   value['scalar']=value['minScalar']=1.;value['minMaxState']=2 if mode in [2,3] else 1
   angle=add(angle,mul(life_curve(ce,value,'MeshRotation'+str(index)),mul(link('Particles.Lifetime',Typ.FLOAT),F(180/math.pi))))
  angles.append(angle)
 # Unity applies Z, then X, then Y. Niagara MultiplyQuaternion is A * B.
 q=multiply(quat([0,0,1],angles[1]),multiply(quat([-1,0,0],angles[0]),quat([0,1,0],angles[2])))
 q=multiply(quat(record['prefab_axis'],F(record['prefab_angle'])),q)
 ce.set_parameter_directly('Particles.MeshOrientation',q,Cat.PARTICLE_UPDATE)
 name='Particles.SourceInitialMeshSize'
 ce.set_parameter_directly(name,random_float(initial['startSize'],record['prefab_scale']),Cat.PARTICLE_SPAWN)
 scale=mul(link(name,Typ.FLOAT),curve_input([(t,v) for t,v in p['v2_size']]))
 ce.set_parameter_directly('Particles.Scale',dynamic('TypeConversions/VectorFromFloat',Typ.VEC3,Value=scale),Cat.PARTICLE_UPDATE)
