# VFXBaseV2 reconstruction from selected DXBC variants 1284/1287/1290/1294 and soft-depth equivalents.
# UV and color math follow the bytecode. Scene exposure, fog and motion-vector output remain engine-specific.
def material_v2(p):
 source=p['materials'][0];props=source['data']['m_SavedProperties'];fl=props['m_Floats'];colors=props['m_Colors'];tx=source['textures']
 if '_BaseColor' in colors and '_BaseColorBrighterScale' in fl:
  # HGRP/LitEffect is a lit surface, not a missing-white-texture VFXBase material.
  mat=asset('M_'+p['ue_name'],u.Material,u.MaterialFactoryNew());ml.delete_all_material_expressions(mat)
  mat.set_editor_property('blend_mode',u.BlendMode.BLEND_MASKED);mat.set_editor_property('shading_model',u.MaterialShadingModel.MSM_DEFAULT_LIT)
  ml.set_material_usage(mat,u.MaterialUsage.MATUSAGE_NIAGARA_MESH_PARTICLES)
  c=colors['_BaseColor'];base=expression(mat,u.MaterialExpressionConstant3Vector,constant=u.LinearColor(c['r'],c['g'],c['b']))
  pc=expression(mat,u.MaterialExpressionParticleColor);mul=expression(mat,u.MaterialExpressionMultiply);wire(base,'',mul,'A');wire(pc,'RGB',mul,'B');ml.connect_material_property(mul,'',u.MaterialProperty.MP_BASE_COLOR)
  fade=expression(mat,u.MaterialExpressionMaterialFunctionCall)
  assert fade.set_material_function(u.load_asset('/Engine/Functions/Engine_MaterialFunctions02/Utility/DitherTemporalAA'))
  wire(pc,'A',fade,'Alpha Threshold');ml.connect_material_property(fade,'',u.MaterialProperty.MP_OPACITY_MASK)
  if '_MROMap' in tx:
   t=expression(mat,u.MaterialExpressionTextureSample,texture=tex(tx['_MROMap']));ml.connect_material_property(t,'G',u.MaterialProperty.MP_ROUGHNESS);ml.connect_material_property(t,'R',u.MaterialProperty.MP_METALLIC)
  ml.recompile_material(mat);lib.save_loaded_asset(mat,False);return mat
 if '_RadialBlurIntensity' in fl:
  for entry in tx.values():tex(entry)
  # Source screen-pass materials must not fall through to the white VFXBaseV2 fallback.
  import sys
  sys.path.insert(0,str(Path(u.Paths.project_dir())/'Scripts/Editor'))
  from PadmaSourceScreenMaterial import build_radial_blur
  mat=build_radial_blur(asset('M_'+p['ue_name'],u.Material,u.MaterialFactoryNew()),source,dest+'/Textures')
  lib.save_loaded_asset(mat,False)
  return mat
 if '_RefractTex' in tx and '_MainTex' not in tx:
  mat=asset('M_'+p['ue_name'],u.Material,u.MaterialFactoryNew());ml.delete_all_material_expressions(mat)
  mat.set_editor_property('blend_mode',u.BlendMode.BLEND_TRANSLUCENT);mat.set_editor_property('shading_model',u.MaterialShadingModel.MSM_UNLIT);mat.set_editor_property('two_sided',True)
  for usage in [u.MaterialUsage.MATUSAGE_NIAGARA_SPRITES,u.MaterialUsage.MATUSAGE_NIAGARA_RIBBONS,u.MaterialUsage.MATUSAGE_NIAGARA_MESH_PARTICLES]:ml.set_material_usage(mat,usage)
  mat.set_editor_property('refraction_method',u.RefractionMode.RM_2D_OFFSET)
  t=expression(mat,u.MaterialExpressionTextureSample,texture=tex(tx['_RefractTex']));pc=expression(mat,u.MaterialExpressionParticleColor)
  m=expression(mat,u.MaterialExpressionMultiply);wire(t,'R',m,'A');wire(pc,'A',m,'B')
  gain=expression(mat,u.MaterialExpressionMultiply,const_b=.001);wire(m,'',gain,'A');ml.connect_material_property(gain,'',u.MaterialProperty.MP_REFRACTION)
  zero=expression(mat,u.MaterialExpressionConstant,r=0.);ml.connect_material_property(zero,'',u.MaterialProperty.MP_OPACITY);ml.connect_material_property(zero,'',u.MaterialProperty.MP_EMISSIVE_COLOR)
  ml.recompile_material(mat);lib.save_loaded_asset(mat,False);return mat
 mat=asset('M_'+p['ue_name'],u.Material,u.MaterialFactoryNew());ml.delete_all_material_expressions(mat)
 mat.set_editor_property('blend_mode',u.BlendMode.BLEND_ALPHA_COMPOSITE);mat.set_editor_property('two_sided',True);mat.set_editor_property('shading_model',u.MaterialShadingModel.MSM_UNLIT)
 for usage in [u.MaterialUsage.MATUSAGE_NIAGARA_SPRITES,u.MaterialUsage.MATUSAGE_NIAGARA_RIBBONS,u.MaterialUsage.MATUSAGE_NIAGARA_MESH_PARTICLES]:ml.set_material_usage(mat,usage)
 custom=expression(mat,u.MaterialExpressionCustom);custom.set_editor_property('output_type',u.CustomMaterialOutputType.CMOT_FLOAT4);ins=[];connections=[]
 def add(name,node,pin=''):
  ci=u.CustomInput();ci.set_editor_property('input_name',name);ins.append(ci);connections.append((name,node,pin))
 add('UV',expression(mat,u.MaterialExpressionTextureCoordinate));add('Age',expression(mat,u.MaterialExpressionParticleRelativeTime));pc=expression(mat,u.MaterialExpressionParticleColor);add('PC',pc,'RGBA')
 lut=imp(p['v2_custom_lut'],'T_Curve_'+p['ue_name']);lut.set_editor_property('srgb',False);lut.set_editor_property('compression_settings',u.TextureCompressionSettings.TC_VECTOR_DISPLACEMENTMAP);lut.set_editor_property('mip_gen_settings',u.TextureMipGenSettings.TMGS_NO_MIPMAPS);lib.save_loaded_asset(lut,False);add('CurveTex',expression(mat,u.MaterialExpressionTextureObject,texture=lut))
 code='float4 cd=Texture2DSample(CurveTex,CurveTexSampler,float2(saturate(Age),0.5))*'+fv(p['v2_custom_range'][1])+'+'+fv(p['v2_custom_range'][0])+';\n'
 def value(k,default=0):return ff(fl.get(k,default))
 def cv(k,default=(0,0,0,0)):return [colors.get(k,dict(zip('rgba',default)))[x] for x in 'rgba']
 def st(slot):
  env=props['m_TexEnvs'].get(slot,{});s=env.get('m_Scale',{'X':1,'Y':1});o=env.get('m_Offset',{'X':0,'Y':0});return [s['X'],s['Y'],o['X'],o['Y']]
 def uv(slot,transform,channel):
  weights=cv(slot+'UVWeights',(1,0,0,0));speed=cv(slot+'UVSpeed');rot=cv(slot+'UVRotateMat',(1,0,0,1))
  # VFXBaseV2 VS: choose UV0/UV1, add Time.xy + custom scalar * speed.zw, rotate about .5, then ST.
  # Current source meshes have UV0 only; duplicated UV0 is the explicit UV1 fallback.
  raw=f'(float2(UV.x,1-UV.y)*{ff(weights[0]+weights[1])}+Age*{ff(p["particle"]["InitialModule"]["startLifetime"]["scalar"])}*{fv(speed[:2])}+cd.{channel}*{fv(speed[2:])}-.5)'
  return f'((mul(float2x2({ff(rot[0])},{ff(rot[2])},{ff(rot[1])},{ff(rot[3])}),{raw})+.5)*{fv(transform[:2])}+{fv(transform[2:])})'
 def sample(slot,uvexpr):
  entry=tx.get(slot)
  if not entry:return 'float4(1,1,1,1)'
  name=slot.lstrip('_');texture=tex(entry)
  # Preserve each source Texture2D's color-space metadata, rather than treating every mask slot as linear.
  metaPath=list((R/'objects/Texture2D').glob('*_p'+f'{entry["PathID"]&((1<<64)-1):016X}'+'.json'))
  if metaPath:
   td=json.loads(metaPath[0].read_text(encoding='utf-8-sig'))
   if td.get('m_ColorSpace') is not None:texture.set_editor_property('srgb',td['m_ColorSpace']==1)
   wrap=td.get('m_TextureSettings',{}).get('m_WrapMode',0)
   texture.set_editor_property('address_x',u.TextureAddress.TA_CLAMP if wrap==1 else u.TextureAddress.TA_WRAP)
   texture.set_editor_property('address_y',u.TextureAddress.TA_CLAMP if wrap==1 else u.TextureAddress.TA_WRAP)
  lib.save_loaded_asset(texture,False);add(name,expression(mat,u.MaterialExpressionTextureObject,texture=texture))
  # Imported mesh UVs and exported PNG rows both flip V. Run source ST/rotation
  # in Unity UV coordinates, then flip the final sample back to PNG coordinates.
  return f'Texture2DSample({name},{name}Sampler,float2(({uvexpr}).x,1-({uvexpr}).y))'
 # Source pixel variant 1294: baseline 1-UseWeightTex, then weighted RED sums.
 # Opaque alpha here erases spatial weighting and prematurely dissolves blades.
 code+='float4 disturbance=float4(1,1,1,1),mask=float4(1,1,1,1),blend=float4(0,0,0,0);float weight=1-'+value('_UseWeightTex')+',dissolve=0;\n'
 for n in range(6):
  slot='_SampleTex'+str(n)
  if slot not in tx or '_SAMPLE_TEX'+str(n) not in source['data']['m_ValidKeywords']:continue
  targets=['_DisturbTex'+str(min(n+1,2)),'_DisturbTex2','_WeightTex','_MaskTex','_BlendTex','_DissolveTex'];w=[fl.get(slot+'UseWeight'+str(j),0) for j in range(6)]
  transform=[sum(w[j]*st(targets[j])[k] for j in range(6)) for k in range(4)]
  code+=f'float4 s{n}='+sample(slot,uv(slot,transform,'y'))+';\n'
  code+=f's{n}=lerp(s{n},float4(1,1,1,s{n}.r),'+value('_UseSampleTex'+str(n)+'AsAlpha')+');\n'
  if w[0] or w[1]:code+=f'disturbance*=s{n}*{ff(w[0]+w[1])};\n'
  if w[2]:code+=f'weight+=s{n}.r*{ff(w[2])};\n'
  if w[3]:code+=f'mask*=s{n}*{ff(w[3])};\n'
  if w[4]:code+=f'blend+=s{n}*{ff(w[4])};\n'
  if w[5]:code+=f'dissolve+=s{n}.r*{ff(w[5])};\n'
 code+='float2 duv=0;\n'
 if fl.get('_UseDisturb',0) or fl.get('_UseDisturb2',0):
  code+='float noise=disturbance.r*(1+'+value('_Bi_Disturb')+')-'+value('_Bi_Disturb')+';\n'
  code+='duv=noise*'+fv([fl.get('_DisturbUIntensity1',0),fl.get('_DisturbVIntensity1',0)])+';\n'
  if fl.get('_DisturbTex1Normal',0):code+='duv=(disturbance.rg*2-1)*'+value('_DisturbUIntensity1')+';\n'
  if fl.get('_UseParticleDisturb',0):code+='duv*=cd.w;\n'
 mainuv=uv('_MainTex',st('_MainTex'),'x')+'+duv*'+value('_MainTexUseDisturb')
 code+='float4 mainSample='+sample('_MainTex',mainuv)+';\n'
 code+='mainSample=lerp(mainSample,float4(1,1,1,mainSample.r),'+value('_UseMainTexAsAlpha')+');\n'
 if '_MainTex' not in tx and not any(k.startswith('_SampleTex') for k in tx):code+='mainSample.a*=saturate((1-length(UV*2-1))*4);\n'
 code+='float4 col='+('float4(1,1,1,1)' if fl.get('_DisableVertColor',0) else 'PC')+'*'+fv(cv('_TintColor',(1,1,1,1)))+'*'+fv([fl.get('_TintColorIntensity',1)]*3+[fl.get('_TintColorAlpha',1)])+'*mainSample;col*=mask;\n'
 if fl.get('_UseBlend',0):code+='col.rgb+=blend.rgb*saturate((col.a+blend.a)*'+ff(cv('_BlendTint',(1,1,1,1))[3])+')*'+fv(cv('_BlendTint',(1,1,1,1))[:3])+';\n'
 if fl.get('_UseDissolve',0):
  code+='float dw=lerp(1,weight,'+value('_WeightTexIntensity')+'*'+value('_DissolveUseWeight')+');float threshold=dw*((cd.z+'+value('_DissolveScheduleOffset')+')*2.02-.01)-1;float diff=dissolve-threshold;col.a*=saturate(diff*'+value('_DissolveEdgeSharp',1)+');\n'
  edge=cv('_DissolveEmissiveColor')[:3]
  if any(edge):code+='float edge=saturate(('+value('_DissolveEmissiveEdge')+'-diff)*'+value('_DissolveEdgeSharp',1)+');col.rgb=lerp(col.rgb,'+fv(edge)+'*'+value('_TintColorIntensity',1)+'*edge,edge);\n'
 # Preserve source HDR values; apply_source_halo_bridge below adapts the
 # original _IgnorePostExposure contract through UE EyeAdaptationInverse.
 code+='col.rgb=max(0,col.rgb)+max(0,col.rgb-'+value('_ExpThreshold')+')*'+value('_ExpIntensity')+';col.a=saturate(col.a);\n'
 # Source backdrop expects its cinematic camera. Feather its exposed edges for the UE gameplay camera.
 if 'exhibit_start_01_beijing' in p['ue_name']:code+='col.a*=smoothstep(0,.08,min(min(UV.x,1-UV.x),min(UV.y,1-UV.y)));\n'
 mode=fl.get('_BlendMode',0)
 code+='return float4(col.rgb*col.a,'+('0' if mode==1 else 'col.a')+');'
 custom.set_editor_property('code',code);custom.set_editor_property('inputs',ins)
 for name,node,pin in connections:wire(node,pin,custom,name)
 rgb=expression(mat,u.MaterialExpressionComponentMask,r=True,g=True,b=True,a=False);wire(custom,'',rgb);ml.connect_material_property(rgb,'',u.MaterialProperty.MP_EMISSIVE_COLOR)
 alpha=expression(mat,u.MaterialExpressionComponentMask,r=False,g=False,b=False,a=True);wire(custom,'',alpha);ml.connect_material_property(alpha,'',u.MaterialProperty.MP_OPACITY)
 import sys
 sys.path.insert(0,str(Path(u.Paths.project_dir())/'Scripts/Editor'))
 from PadmaSourceGlowMaterial import apply_source_halo_bridge
 apply_source_halo_bridge(mat,p['ue_name'],fl)
 ml.recompile_material(mat);lib.save_loaded_asset(mat,False)
 return mat
