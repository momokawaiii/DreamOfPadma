"""Source emission and transformed birth-volume adapters, executed in authoring context."""
def dynamic(path,typ,**inputs):
 sc=f.create_script_context(args('/Niagara/DynamicInputs/'+path+'.'+path.split('/')[-1]))
 for k,v in inputs.items():setp(sc,k,v)
 return f.create_script_input_dynamic(sc,typ)
F=f.create_script_input_float
V=lambda x:f.create_script_input_vector(u.Vector(*x))
def sampled_curve(points,index):
 keys=[]
 for t,v in points:
  k=u.RichCurveKeyBP();k.time=t;k.value=v;k.interp_mode=u.RichCurveInterpMode.RCIM_LINEAR;keys.append(k)
 return dynamic('ValueFromCurve/FloatFromCurve',Typ.FLOAT,FloatCurve=f.create_script_input_di(f.create_float_curve_di(keys)),CurveIndex=index)
def shape_frame(p):
 # Unity shape TRS is additional to the prefab transform; Euler order is ZXY.
 sh=p['particle']['ShapeModule'];m=p['unity_transform_matrix']
 x,y,z=[math.radians(sh['m_Rotation'][k]) for k in 'xyz']
 sx,cx,sy,cy,sz,cz=math.sin(x),math.cos(x),math.sin(y),math.cos(y),math.sin(z),math.cos(z)
 r=[[cy*cz+sy*sx*sz,-cy*sz+sy*sx*cz,sy*cx],[cx*sz,cx*cz,-sx],[-sy*cz+cy*sx*sz,sy*sz+cy*sx*cz,cy*cx]]
 axes=[]
 for j,k in enumerate('xyz'):
  a=[sum(m[i][l]*r[l][j] for l in range(3))*sh['m_Scale'][k] for i in range(3)]
  axes.append([-a[0],a[2],a[1]])
 pos=[sum(m[i][j]*sh['m_Position'][k] for j,k in enumerate('xyz'))*100 for i in range(3)]
 return [a+b for a,b in zip(p['v2_position'],[-pos[0],pos[2],pos[1]])],axes

def source_shape(ce,p):
 sh=p['particle']['ShapeModule']
 if not sh['enabled']:return V(p['v2_position'])
 center,axes=shape_frame(p);kind=sh['type'];radius=sh['radius']['value']*100
 assert kind in [0,4,5,8,10,12],('Unsupported source shape',kind)
 rnd=[]
 for key in 'XYZ':
  name='Particles.SourceShapeRandom'+key
  ce.set_parameter_directly(name,dynamic('UniformRange/V2/RandomRangeFloat',Typ.FLOAT,Minimum=F(0),Maximum=F(1)),Cat.PARTICLE_SPAWN)
  rnd.append(f.create_script_input_linked_parameter(name,Typ.FLOAT))
 def lookup(fn,index):
  points=[(i/64,fn(i/64)) for i in range(65)]
  if max(v for _,v in points)-min(v for _,v in points)<1e-10:return F(points[0][1])
  return sampled_curve(points,index)
 def mul(a,b):return dynamic('Multiply/Multiply_Float',Typ.FLOAT,A=a,B=b)
 def add(a,b):return dynamic('Add/Add_Float',Typ.FLOAT,A=a,B=b)
 inner=max(0,1-sh.get('radiusThickness',1));arc=math.radians(sh['arc']['value'])
 if kind==5:coords=[lookup(lambda t:(t-.5)*100,q) for q in rnd]
 elif kind==12:coords=[lookup(lambda t:(t*2-1)*radius,rnd[0]),F(0),F(0)]
 elif kind==0:
  ring=lookup(lambda t:math.sqrt(max(0,1-(2*t-1)**2)),rnd[1]);r=lookup(lambda t:radius*(inner**3+t*(1-inner**3))**(1/3),rnd[2])
  coords=[mul(mul(lookup(lambda t:math.cos(t*2*math.pi),rnd[0]),ring),r),mul(mul(lookup(lambda t:math.sin(t*2*math.pi),rnd[0]),ring),r),mul(lookup(lambda t:2*t-1,rnd[1]),r)]
 else:
  has_length=kind==8 and sh['length']>1e-6
  radial=lookup(lambda t:math.sqrt(inner*inner+t*(1-inner*inner)),rnd[1]);z=lookup(lambda t:t*sh['length']*100,rnd[2]) if has_length else F(0)
  # Source R contains 90-degree cones with zero length: a flat disk. Avoid
  # evaluating tan(90) in a dynamic expression, even multiplied by zero.
  r=add(F(radius),mul(z,F(math.tan(math.radians(sh['angle']))))) if has_length else F(radius)
  coords=[mul(mul(lookup(lambda t:math.cos(t*arc),rnd[0]),radial),r),mul(mul(lookup(lambda t:math.sin(t*arc),rnd[0]),radial),r),z]
 result=V(center)
 for axis,value in zip(axes,coords):result=dynamic('Add/Add_Vector',Typ.VEC3,A=result,B=dynamic('Multiply/Multiply_VectorByFloat',Typ.VEC3,Vector=V(axis),Float=value))
 return result

def emission_duration(q):
 # Burst-only emitters can stop emitting after their last burst. Complete still
 # waits for live particles; no particle lifetime or rate curve is shortened.
 em=q['EmissionModule'];rate=em['rateOverTime']
 if q['looping'] or any(source_curve(rate,i/16)>0 for i in range(17)):return max(.01,q['lengthInSec'])
 last=max([b['time']+(max(1,b.get('cycleCount',1))-1)*b.get('repeatInterval',0) for b in em.get('m_Bursts',[])]+[0])
 # Leave room for a 15 Hz game tick to cross a delayed burst before completion.
 return min(max(.01,q['lengthInSec']),last+.1)
def setup_emission(ce,p):
 q=p['particle'];em=q['EmissionModule'];delay=max(0,scalar(q['startDelay']))
 rate=em['rateOverTime'];has_rate=em['enabled'] and any(source_curve(rate,i/16)>0 for i in range(17))
 init=basic(ce,emission_duration(q),0,rate=1 if has_rate else None)
 state=module(ce,'EmitterState','/Niagara/Modules/Emitter/EmitterState.EmitterState',Cat.EMITTER_UPDATE)
 setp(state,'Inactive Response',f.create_script_input_enum(Paths.enum_niagara_inactive_mode,'Complete (Let Particles Finish then Kill Emitter)'))
 if delay:assert state.set_parameter('Loop Delay',F(delay),True,True)
 if q['looping']:setp(state,'Loop Behavior',enum('ENiagara_EmitterStateOptions','Infinite'))
 if has_rate:
  rate_module=module(ce,'Rate','/Niagara/Modules/Emitter/SpawnRate.SpawnRate',Cat.EMITTER_UPDATE)
  setp(rate_module,'SpawnRate',sampled_curve([(i/64,max(0,source_curve(rate,i/64))) for i in range(65)],f.create_script_input_linked_parameter('Emitter.NormalizedLoopAge',Typ.FLOAT)))
 for i,b in enumerate(em.get('m_Bursts',[]) if em['enabled'] else []):
  for cycle in range(max(1,b.get('cycleCount',1))):
   mod=module(ce,'SourceBurst%d_%d'%(i,cycle),'/Niagara/Modules/Emitter/SpawnBurst_Instantaneous.SpawnBurst_Instantaneous',Cat.EMITTER_UPDATE)
   setp(mod,'Spawn Count',f.create_script_input_int(max(0,int(scalar(b['countCurve'])))))
   setp(mod,'Spawn Time',F(b['time']+cycle*b.get('repeatInterval',0)))
 return init
