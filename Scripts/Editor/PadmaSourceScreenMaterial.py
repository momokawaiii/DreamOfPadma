"""UE adaptation of source radial-blur particle materials; shared by correction and rebuild recipes."""
import unreal as u

def build_radial_blur(mat,source,root):
 ml=u.MaterialEditingLibrary
 props=source['data']['m_SavedProperties'];fl=props['m_Floats']
 assert '_RadialBlurIntensity' in fl and '_MaskTex' in source['textures']
 entry=source['textures']['_MaskTex']
 name=entry['Name']+'_p'+format(entry['PathID']&((1<<64)-1),'016X')
 texture=u.load_asset(root.rstrip('/')+'/'+name)
 if not texture:raise RuntimeError('Missing source radial blur mask '+name)
 def node(cls,**kw):
  n=ml.create_material_expression(mat,cls)
  for k,v in kw.items():n.set_editor_property(k,v)
  return n
 def wire(a,pin,b,input=''):assert ml.connect_material_expressions(a,pin,b,input)
 ml.delete_all_material_expressions(mat)
 mat.set_editor_property('blend_mode',u.BlendMode.BLEND_TRANSLUCENT)
 mat.set_editor_property('shading_model',u.MaterialShadingModel.MSM_UNLIT)
 mat.set_editor_property('two_sided',True)
 mat.set_editor_property('refraction_method',u.RefractionMode.RM_NONE)
 for usage in [u.MaterialUsage.MATUSAGE_NIAGARA_SPRITES,u.MaterialUsage.MATUSAGE_NIAGARA_MESH_PARTICLES,u.MaterialUsage.MATUSAGE_NIAGARA_RIBBONS]:ml.set_material_usage(mat,usage)
 uv=node(u.MaterialExpressionTextureCoordinate)
 env=props['m_TexEnvs']['_MaskTex'];scale=env['m_Scale'];offset=env['m_Offset']
 st=node(u.MaterialExpressionConstant2Vector,r=scale['X'],g=scale['Y']);scaled=node(u.MaterialExpressionMultiply);wire(uv,'',scaled,'A');wire(st,'',scaled,'B')
 off=node(u.MaterialExpressionConstant2Vector,r=offset['X'],g=offset['Y']);coords=node(u.MaterialExpressionAdd);wire(scaled,'',coords,'A');wire(off,'',coords,'B')
 mask=node(u.MaterialExpressionTextureSample,texture=texture);wire(coords,'',mask,'UVs')
 pc=node(u.MaterialExpressionParticleColor);alpha=node(u.MaterialExpressionMultiply);wire(mask,'R',alpha,'A');wire(pc,'A',alpha,'B')
 strength=node(u.MaterialExpressionMultiply,const_b=fl['_RadialBlurIntensity']);wire(alpha,'',strength,'A')
 assert ml.connect_material_property(strength,'',u.MaterialProperty.MP_OPACITY)
 # Preserve a masked scene effect, not an emissive white billboard.
 # Five taps remain an explicit approximation of the original native screen pass.
 samples=[]
 for amount in [-.004,-.002,0,.002,.004]:
  center=node(u.MaterialExpressionAdd,const_b=-.5);wire(uv,'',center,'A')
  delta=node(u.MaterialExpressionMultiply,const_b=amount);wire(center,'',delta,'A')
  scene=node(u.MaterialExpressionSceneColor,input_mode=u.MaterialSceneAttributeInputMode.OFFSET_FRACTION);wire(delta,'',scene);samples.append(scene)
 result=samples[0]
 for sample in samples[1:]:
  add=node(u.MaterialExpressionAdd);wire(result,'',add,'A');wire(sample,'',add,'B');result=add
 avg=node(u.MaterialExpressionMultiply,const_b=.2);wire(result,'',avg,'A')
 assert ml.connect_material_property(avg,'',u.MaterialProperty.MP_EMISSIVE_COLOR)
 ml.recompile_material(mat)
 return mat
