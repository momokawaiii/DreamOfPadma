"""UE Python commandlet: create the exposure-stable world training panel material. Safe to rerun."""
import unreal as u
m=u.MaterialEditingLibrary;lib=u.EditorAssetLibrary
path='/Game/Padma/UI/Training/M_ACTTrainingHologram'
a=lib.load_asset(path) if lib.does_asset_exist(path) else u.AssetToolsHelpers.get_asset_tools().create_asset('M_ACTTrainingHologram','/Game/Padma/UI/Training',u.Material,u.MaterialFactoryNew())
m.delete_all_material_expressions(a)
a.set_editor_property('blend_mode',u.BlendMode.BLEND_TRANSLUCENT)
a.set_editor_property('shading_model',u.MaterialShadingModel.MSM_UNLIT)
a.set_editor_property('two_sided',True)
a.set_editor_property('disable_depth_test',False)
def node(cls,**props):
 n=m.create_material_expression(a,cls)
 for k,v in props.items():n.set_editor_property(k,v)
 return n
def link(x,p,y,q):assert m.connect_material_expressions(x,p,y,q)
tex=node(u.MaterialExpressionTextureSampleParameter2D,parameter_name='SlateUI',texture=lib.load_asset('/Engine/EngineResources/WhiteSquareTexture'))
tint=node(u.MaterialExpressionVectorParameter,parameter_name='TintColorAndOpacity',default_value=u.LinearColor(1,1,1,1))
color=node(u.MaterialExpressionMultiply);link(tex,'RGB',color,'A');link(tint,'RGB',color,'B')
inv=node(u.MaterialExpressionEyeAdaptationInverse);link(color,'',inv,str(m.get_material_expression_input_names(inv)[0]));assert m.connect_material_property(inv,'',u.MaterialProperty.MP_EMISSIVE_COLOR)
alpha=node(u.MaterialExpressionMultiply);link(tex,'A',alpha,'A');link(tint,'A',alpha,'B');assert m.connect_material_property(alpha,'',u.MaterialProperty.MP_OPACITY)
m.recompile_material(a);assert lib.save_loaded_asset(a)
u.log('ACT_HOLOGRAM_MATERIAL_SAVED '+path)
