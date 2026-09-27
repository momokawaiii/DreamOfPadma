"""Audit-first lowering of reference material math. Never guesses unsupported nodes."""
from pathlib import Path
import json,re
ROOT=Path(__file__).resolve().parents[3]
DATA=json.loads((ROOT/'Artifacts/ZMDRenderAudit/objects.json').read_text(encoding='utf-8'))
class Graph:
 def __init__(self,label):
  key=next(k for k in DATA if k.endswith('_M_Common_'+label))
  self.nodes={o['name']:{l.split('=',1)[0]:l.split('=',1)[1] for l in o['props'] if '=' in l} for o in DATA[key] if o['props'] and o['name'].startswith('MaterialExpression')}
  self.label=label;self.lines=[];self.cache={};self.params={};self.textures={};self.extras={}
 def name(self,n):return re.sub('[^a-zA-Z0-9_]','_',n)
 def number(self,p,key,default=0):return p.get(key,str(default))
 def ref(self,line,default='0'):
  m=re.search(r'Expression="[^\"]*:([^\']+)\'"',line)
  if not m:return (str(default),1)
  value,width=self.node(m[1]);oi=int(re.search(r'OutputIndex=(\d+)',line)[1]) if 'OutputIndex=' in line else 0
  typ=m[1].split('_')[0]
  if typ in ['MaterialExpressionTextureSampleParameter2D','MaterialExpressionTextureSample','MaterialExpressionVectorParameter','MaterialExpressionCurveAtlasRowParameter','MaterialExpressionVertexColor']:
   sw='rgba'[oi-1] if 1<=oi<=4 else ('rgba' if oi==5 else 'rgb')
   return value+'.'+sw,len(sw)
  if 'Mask=1' in line:
   sw=''.join(c.lower() for c in 'RGBA' if 'Mask'+c+'=1' in line)
   if sw:return value+'.'+sw,len(sw)
  return value,width
 def node(self,name):
  if name in self.cache:return self.cache[name]
  p=self.nodes[name];kind=name.split('_')[0].removeprefix('MaterialExpression')
  def a(k,default=0):return self.ref(p.get(k,''),p.get('Const'+k,str(default)))
  def x(k,default=0):return a(k,default)[0]
  w=1;e=None
  if kind in ['NamedRerouteDeclaration','Reroute']:return self.ref(p['Input'])
  if kind=='NamedRerouteUsage':return self.node(re.search(r':([^\']+)\'',p['Declaration'])[1])
  if kind in ['ScalarParameter','VectorParameter']:
   n=p['ParameterName'].strip('"');ident='P_'+self.name(n);w=4 if kind=='VectorParameter' else 1
   self.params[n]={'kind':kind,'default':p.get('DefaultValue','0'),'identifier':ident};r=(ident,w);self.cache[name]=r;return r
  if kind in ['Add','Subtract','Multiply','Divide','DotProduct','CrossProduct']:
   aa,wa=a('A',0 if kind in ['Add','Subtract'] else 1);bb,wb=a('B',1 if kind!='Divide' else 2);w=max(wa,wb)
   op={'Add':'+','Subtract':'-','Multiply':'*','Divide':'/'}
   if kind in op:e=f'({aa} {op[kind]} {bb})'
   else:e=f'{"dot" if kind=="DotProduct" else "cross"}({aa},{bb})';w=1 if kind=='DotProduct' else 3
  elif kind in ['OneMinus','Saturate','SquareRoot','Normalize','Abs']:
   arg,w=a('VectorInput' if kind=='Normalize' else 'Input');e=f'(1-({arg}))' if kind=='OneMinus' else f'{dict(Saturate="saturate",SquareRoot="sqrt",Normalize="normalize",Abs="abs")[kind]}({arg})'
  elif kind=='LinearInterpolate':aa,wa=a('A');bb,wb=a('B',1);w=max(wa,wb);e=f'lerp({aa},{bb},{x("Alpha",.5)})'
  elif kind=='Power':aa,w=a('Base');e=f'pow(max({aa},0),{x("Exponent",2)})'
  elif kind=='SmoothStep':aa,w=a('Value');e=f'smoothstep({x("Min",0)},{x("Max",1)},{aa})'
  elif kind=='Step':aa,w=a('X');e=f'step({x("Y")},{aa})'
  elif kind=='If':
   gt,w=a('AGreaterThanB');eq,we=a('AEqualsB');lt,wl=a('ALessThanB');w=max(w,we,wl);e=f'(({x("A")}>({x("B")})+{p.get("EqualsThreshold","0.00001")})?{gt}:(({x("A")}<({x("B")})-{p.get("EqualsThreshold","0.00001")})?{lt}:{eq}))'
  elif kind=='ChannelMaskParameter':
   n=p['ParameterName'].strip('"');ident='P_'+self.name(n);self.params[n]={'kind':'VectorParameter','default':p.get('DefaultValue','(R=1,G=0,B=0,A=0)'),'identifier':ident};e=f'dot({x("Input")},{ident})'
  elif kind=='StaticSwitchParameter' and self.label=='Cloth':
   # Chen's audited reference variant. Weather, thin film and auxiliary M/E
   # branches are intentionally outside this fixed comparison specialization.
   enabled=p['ParameterName'].strip('"')=='b_UseRimHighLight'
   return a('A' if enabled else 'B')
  elif kind=='AppendVector':aa,wa=a('A');bb,wb=a('B');w=wa+wb;e=f'float{w}({aa},{bb})'
  elif kind=='Constant3Vector':v=p.get('Constant','(R=0,G=0,B=0)');e='float3('+','.join(re.search(c+r'=([-\d.]+)',v)[1] for c in 'RGB')+')';w=3
  elif kind=='Constant':e=p.get('R','0')
  elif kind=='CameraVectorWS':e='ViewDirection';w=3
  elif kind=='CameraPositionWS':e='CameraPosition';w=3
  elif kind=='WorldPosition':e='WorldPosition';w=3
  elif kind=='VertexNormalWS':e='GeometricNormal';w=3
  elif kind=='PixelNormalWS' and self.label=='Cloth':
   normal=self.ref(self.root_attributes.get('Inputs(6)',self.root_attributes.get('Normal','')))[0];e=f'normalize(mul({normal},TangentToWorld))';w=3
  elif kind=='VertexColor':self.extras['VertexColor']='float4';e='VertexColor';w=4
  elif kind=='SkyAtmosphereLightDirection':e='LightDirection';w=3
  elif kind=='TextureCoordinate':e='UV';w=2
  elif kind=='Transform':arg,w=a('Input');e=f'mul(TangentToWorld,{arg})' if p.get('TransformSourceType')=='TRANSFORMSOURCE_World' else f'mul({arg},TangentToWorld)'
  elif kind=='TextureSampleParameter2D':
   n=p['ParameterName'].strip('"');ident='T_'+self.name(n);self.textures[n]={'path':p['Texture'],'sampler':p.get('SamplerType','SAMPLERTYPE_Color'),'identifier':ident};e=f'Texture2DSample({ident},{ident}Sampler,{x("Coordinates","UV")})';w=4
  elif kind=='TextureSample':
   n='ReferenceFixedTexture';ident='T_'+n;self.textures[n]={'path':p['Texture'],'sampler':p.get('SamplerType','SAMPLERTYPE_Color'),'identifier':ident};e=f'Texture2DSample({ident},{ident}Sampler,{x("Coordinates","UV")})';w=4
  elif kind=='TextureObjectParameter':
   n=p['ParameterName'].strip('"');ident='T_'+self.name(n);self.textures[n]={'path':p['Texture'],'sampler':p.get('SamplerType','SAMPLERTYPE_Color'),'identifier':ident};return ident,0
  elif kind=='EyeAdaptationInverse':
   arg,w=a('LightValueInput');self.extras['Exposure']='float';e=f'({arg}*exp(-({x("AlphaInput",1)})*log(max(Exposure,1e-8))))'
  elif kind=='Custom' and self.label=='Face':
   args={re.search(r'InputName="([^\"]+)"',v)[1]:self.ref(v)[0] for k,v in p.items() if k.startswith('Inputs(')}
   mirrored=1 if '1.0 - TexCoord.x' in p['Code'] else 0
   e=f'PadmaReferenceFaceSDF({args["TexObject"]},{args["TexObject"]}Sampler,{args["TexCoord"]},{mirrored},{args["BlurIntensity"]},{args["Dit"]})'
  elif kind=='CollectionParameter':e='PointLightPosition';w=3
  elif kind=='ComponentMask':arg,wa=a('Input');sw=''.join(c.lower() for c in 'RGBA' if p.get(c)=='True');e=f'({arg}).{sw}';w=len(sw)
  elif kind=='SphereMask':
   hardness=x('Hardness') if 'Expression=' in p.get('Hardness','') else str(float(p.get('HardnessPercent','100'))*.01)
   e=f'saturate((1-distance({x("A")},{x("B")})/max({x("Radius",256)},1e-5))/max(1-({hardness}),1e-5))'
  elif kind=='CurveAtlasRowParameter':e=f'PadmaReferenceHairRamp({x("InputTime")})';w=4
  elif kind=='MaterialFunctionCall':
   f=p['MaterialFunction'].split("'")[1].rsplit('.',1)[-1];args={}
   for k,v in p.items():
    if k.startswith('FunctionInputs('):
     key=re.search(r'InputName="([^\"]+)"',v)[1];args[key]=self.ref(v)
   v=lambda n:args[n][0]
   if f=='MF_Mask_Adjust':e=f'saturate({v("Input")}*{v("Smooth")}-{v("Offset")})';w=args['Input'][1]
   elif f=='MF_ColorBlend':e=f'lerp({v("Input")},lerp({v("Input")}*{v("BlendColor_RGB")},{v("BlendColor_RGB")},{v("BlenderAlpha_A")}),{v("Alpha")})';w=3
   elif f=='MF_UnpackNormal':e=f'PadmaReferenceUnpack(float2({v("R")},{v("G")}))';w=3
   elif f=='FlattenNormal':e=f'lerp({v("Normal")},float3(0,0,1),{v("Flatness")})';w=3
   elif f=='BlendAngleCorrectedNormals':e=f'PadmaReferenceBlendNormals({v("BaseNormal")},{v("AdditionalNormal")})';w=3
   elif f=='MF_DisableFilter':
    self.params['Tonemap']={'kind':'ScalarParameter','default':'0','identifier':'P_Tonemap'};e=f'lerp(FilmToneMapInverse({v("RGB")}),{v("RGB")},saturate(P_Tonemap))';w=3
   elif f=='DitherTemporalAA':self.extras['ReferenceDither']='float';e='ReferenceDither'
   elif f=='GGXSpecular':e=f'PadmaReferenceGGX({v("Normal")},{v("Light Vector")},{v("Roughness")})'
   elif f=='MF_UV_Adjust':e=f'(UV*{v("Scale")}+float2({v("X")},{v("Y")}))';w=2
   else:raise ValueError('Unsupported function '+f)
  else:raise ValueError('Unsupported live node '+name)
  var='n'+str(len(self.lines));self.lines.append(f'    float{w if w>1 else ""} {var} = {e}; // {name}');self.cache[name]=(var,w);return var,w
 def emit(self,full=False):
  attr=next((p for n,p in self.nodes.items() if n.startswith('MaterialExpressionSetMaterialAttributes')),None)
  if attr is None:
   key=next(k for k in DATA if k.endswith('_M_Common_'+self.label));editor=next(o for o in DATA[key] if o['name'].endswith('EditorOnlyData') and o['props']);attr={p.split('=',1)[0]:p.split('=',1)[1] for p in editor['props'] if '=' in p}
  self.root_attributes=attr
  result,w=self.ref(attr.get('Inputs(5)',attr.get('EmissiveColor','')))
  self.outputs={}
  if full:
   for i,n,t in [(1,'BaseColor','float3'),(2,'Metallic','float'),(3,'Specular','float'),(4,'Roughness','float'),(6,'Normal','float3'),(7,'Opacity','float')]:
    line=attr.get('Inputs('+str(i)+')',attr.get(n,''))
    if 'Expression=' in line:self.outputs[n]={'type':t,'value':self.ref(line)[0]}
  # Stable ABI: traversal order changes must never shuffle material arguments.
  self.params=dict(sorted(self.params.items()));self.textures=dict(sorted(self.textures.items()));self.extras=dict(sorted(self.extras.items()))
  args=['float2 UV','float3 GeometricNormal','float3 ViewDirection','float3 LightDirection','float3 WorldPosition','float3 CameraPosition','float3 PointLightPosition','float3x3 TangentToWorld']
  args += [('float4 ' if d['kind']=='VectorParameter' else 'float ')+d['identifier'] for d in self.params.values()]
  args += [t+' '+n for n,t in self.extras.items()]
  for d in self.textures.values():args += ['Texture2D '+d['identifier'],'SamplerState '+d['identifier']+'Sampler']
  args += ['out '+d['type']+' Out'+n for n,d in self.outputs.items()]
  return 'float3 PadmaReference'+self.label+'(\n    '+',\n    '.join(args)+'\n)\n{\n'+'\n'.join(self.lines)+'\n'+''.join('    Out'+n+' = '+d['value']+';\n' for n,d in self.outputs.items())+'    return '+result+';\n}\n'
if __name__=='__main__':
 g=Graph('Hair');text=g.emit();out=ROOT/'Artifacts/PadmaNPRVisual';out.mkdir(exist_ok=True)
 (out/'Hair.generated.ush').write_text(text,encoding='utf-8');(out/'Hair.inputs.json').write_text(json.dumps({'parameters':g.params,'textures':g.textures},indent=2),encoding='utf-8');print('Generated',len(g.lines),'live expressions;',len(g.params),'parameters')
