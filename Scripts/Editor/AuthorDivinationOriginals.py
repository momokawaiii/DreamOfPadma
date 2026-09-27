"""TASK-054: create original divination UI materials and editable style.
Usage: UnrealEditor-Cmd.exe <project> -run=pythonscript
       -script=<project>/Scripts/Editor/AuthorDivinationOriginals.py -unattended -nop4 -AllowCommandletRendering
First-time font import requires full UnrealEditor.exe -ExecutePythonScript (Slate initialized).
Use a real RHI for shader validation. Existing assets and edits are preserved.
"""
import json
import os
import unreal

ROOT=unreal.Paths.convert_relative_path_to_full(unreal.Paths.project_dir())
FOLDER='/Game/Padma/UI/Divination'
LIB=unreal.MaterialEditingLibrary
ASSETS=unreal.EditorAssetLibrary
TOOLS=unreal.AssetToolsHelpers.get_asset_tools()
REPORT={'created': [], 'preserved': [], 'materials': {}}

def save(asset):
    if not ASSETS.save_loaded_asset(asset):
        raise RuntimeError('Cannot save '+asset.get_path_name())

def node(mat, cls, x=0, y=0):
    result=LIB.create_material_expression(mat,cls,x,y)
    if not result: raise RuntimeError('Cannot create material expression')
    return result

def connect(a,b,pin):
    if not LIB.connect_material_expressions(a,'',b,pin):
        raise RuntimeError('Cannot connect '+pin)

def material(name, scalars, atlas=None, textures=None, vectors=None, blend=None, code_name=None):
    vectors=vectors or {}
    textures=textures or {}
    path=FOLDER+'/Materials/M_'+name
    code=open(os.path.join(ROOT,'Scripts','Editor','Shaders',(code_name or name)+'.ush'),encoding='utf-8').read()
    if ASSETS.does_asset_exist(path):
        mat=unreal.load_asset(path)
        if not isinstance(mat,unreal.Material): raise RuntimeError('Wrong existing class: '+path)
        if mat.get_editor_property('material_domain')!=unreal.MaterialDomain.MD_UI: raise RuntimeError('Not a UI material: '+path)
        REPORT['preserved'].append(path)
    else:
        mat=TOOLS.create_asset('M_'+name,FOLDER+'/Materials',unreal.Material,unreal.MaterialFactoryNew())
        mat.set_editor_property('material_domain',unreal.MaterialDomain.MD_UI)
        mat.set_editor_property('blend_mode',blend or unreal.BlendMode.BLEND_TRANSLUCENT)
        custom=node(mat,unreal.MaterialExpressionCustom)
        custom.set_editor_property('description','TASK-054 original '+name)
        custom.set_editor_property('code',code)
        custom.set_editor_property('output_type',unreal.CustomMaterialOutputType.CMOT_FLOAT4)
        names=['UV','Accent']+list(scalars)+(['CardAtlas'] if atlas else [])+list(textures)+list(vectors)
        inputs=[]
        for key in names:
            item=unreal.CustomInput();item.set_editor_property('input_name',key);inputs.append(item)
        custom.set_editor_property('inputs',inputs)
        connect(node(mat,unreal.MaterialExpressionTextureCoordinate,-450,-180),custom,'UV')
        accent=node(mat,unreal.MaterialExpressionVectorParameter,-450,-50)
        accent.set_editor_property('parameter_name','Accent')
        accent.set_editor_property('default_value',unreal.LinearColor(.015,.32,1,1))
        connect(accent,custom,'Accent')
        for i,(key,value) in enumerate(scalars.items()):
            n=node(mat,unreal.MaterialExpressionScalarParameter,-450,100+i*130)
            n.set_editor_property('parameter_name',key);n.set_editor_property('default_value',value)
            connect(n,custom,key)
        for key,value in vectors.items():
            n=node(mat,unreal.MaterialExpressionVectorParameter,-700,-600)
            n.set_editor_property('parameter_name',key);n.set_editor_property('default_value',unreal.LinearColor(*value))
            if not LIB.connect_material_expressions(n,'RGBA',custom,key):raise RuntimeError('Cannot connect vector '+key)
        if atlas:
            tex=node(mat,unreal.MaterialExpressionTextureObjectParameter,-450,-330)
            tex.set_editor_property('parameter_name','CardAtlas');tex.set_editor_property('texture',atlas)
            tex.set_editor_property('sampler_type',unreal.MaterialSamplerType.SAMPLERTYPE_COLOR)
            connect(tex,custom,'CardAtlas')
        for key,texture in textures.items():
            tex=node(mat,unreal.MaterialExpressionTextureObjectParameter,-700,-500)
            tex.set_editor_property('parameter_name',key);tex.set_editor_property('texture',texture)
            tex.set_editor_property('sampler_type',unreal.MaterialSamplerType.SAMPLERTYPE_COLOR if texture.get_editor_property('srgb') else unreal.MaterialSamplerType.SAMPLERTYPE_LINEAR_COLOR)
            connect(tex,custom,key)
        for opacity,prop in [(False,unreal.MaterialProperty.MP_EMISSIVE_COLOR),(True,unreal.MaterialProperty.MP_OPACITY)]:
            mask=node(mat,unreal.MaterialExpressionComponentMask,350,140 if opacity else 0)
            for c in ('r','g','b','a'):mask.set_editor_property(c,(c=='a')==opacity)
            connect(custom,mask,'')
            if not LIB.connect_material_property(mask,'',prop):raise RuntimeError('Cannot connect UI output')
        errors=LIB.recompile_material(mat)
        if errors: raise RuntimeError('Shader compilation failed: '+str(errors))
        REPORT['created'].append(path)
        save(mat)
    errors=LIB.recompile_material(mat)
    if errors: raise RuntimeError('Shader compilation failed: '+str(errors))
    instance_path=FOLDER+'/Materials/MI_'+name
    if ASSETS.does_asset_exist(instance_path):
        instance=unreal.load_asset(instance_path)
        if not isinstance(instance,unreal.MaterialInstanceConstant) or instance.get_editor_property('parent')!=mat:
            raise RuntimeError('Wrong existing material instance: '+instance_path)
        REPORT['preserved'].append(instance_path)
    else:
        instance=TOOLS.create_asset('MI_'+name,FOLDER+'/Materials',unreal.MaterialInstanceConstant,unreal.MaterialInstanceConstantFactoryNew())
        LIB.set_material_instance_parent(instance,mat);LIB.update_material_instance(instance);save(instance)
        REPORT['created'].append(instance_path)
    unreal.SystemLibrary.execute_console_command(None,'AssetCompilingManager.FinishAllCompilation')
    stats=LIB.get_statistics(mat)
    if stats.get_editor_property('num_pixel_shader_instructions')<=0:
        raise RuntimeError('No valid compiled pixel shader; use -AllowCommandletRendering and check the shader log: '+path)
    REPORT['materials'][name]={'domain':str(mat.get_editor_property('material_domain')),'stats':str(stats)}
    return instance

def run():
    source=os.path.join(ROOT,'Content','ThirdParty','Arknights','ACT54Side','Source')
    manifest=json.load(open(os.path.join(source,'provenance.json'),encoding='utf-8'))
    runtime=os.path.join(ROOT,'Content','Padma','UI','Divination','Source','runtime_scene_v2.json')
    if not os.path.isfile(runtime):runtime=os.path.join(source,'runtime_scene.json')
    texture_sampling=json.load(open(runtime,encoding='utf-8')).get('textureSampling',{})
    textures={}
    for filename in manifest['files']:
        key=os.path.splitext(filename)[0];path='/Game/ThirdParty/Arknights/ACT54Side/Textures/T_'+key
        if not ASSETS.does_asset_exist(path):
            task=unreal.AssetImportTask();task.set_editor_property('filename',os.path.join(source,filename))
            task.set_editor_property('destination_path','/Game/ThirdParty/Arknights/ACT54Side/Textures');task.set_editor_property('destination_name','T_'+key)
            task.set_editor_property('automated',True);task.set_editor_property('save',True);TOOLS.import_asset_tasks([task])
            tex=unreal.load_asset(path)
            tex.set_editor_property('compression_settings',unreal.TextureCompressionSettings.TC_EDITOR_ICON)
            tex.set_editor_property('lod_group',unreal.TextureGroup.TEXTUREGROUP_UI)
            tex.set_editor_property('mip_gen_settings',unreal.TextureMipGenSettings.TMGS_NO_MIPMAPS)
            save(tex);REPORT['created'].append(path)
        else:REPORT['preserved'].append(path)
        textures[key]=unreal.load_asset(path)
        if key=='act54side_card_home_bg_char':
            unreal.log('Original character sRGB='+str(textures[key].get_editor_property('srgb')))
            if not textures[key].get_editor_property('srgb'):
                textures[key].set_editor_property('srgb',True);save(textures[key])
        if not isinstance(textures[key],unreal.Texture2D):raise RuntimeError('Missing original texture '+key)
        sampling=texture_sampling.get(key,{}).get('m_TextureSettings',{})
        if sampling:
            tex=textures[key]
            tex.set_editor_property('address_x',unreal.TextureAddress.TA_CLAMP if sampling['m_WrapU']==1 else unreal.TextureAddress.TA_WRAP)
            tex.set_editor_property('address_y',unreal.TextureAddress.TA_CLAMP if sampling['m_WrapV']==1 else unreal.TextureAddress.TA_WRAP)
            tex.set_editor_property('filter',unreal.TextureFilter.TF_BILINEAR if sampling['m_FilterMode']==1 else unreal.TextureFilter.TF_NEAREST)
            save(tex)
    textures['engine_black']=unreal.load_asset('/Engine/EngineResources/Black.Black')
    fonts={}
    for key,filename in manifest.get('fonts',{}).items():
        path='/Game/ThirdParty/Arknights/ACT54Side/Fonts/FF_'+key
        if not ASSETS.does_asset_exist(path):
            task=unreal.AssetImportTask();task.set_editor_property('filename',os.path.join(source,'fonts',filename))
            task.set_editor_property('destination_path','/Game/ThirdParty/Arknights/ACT54Side/Fonts');task.set_editor_property('destination_name','FF_'+key)
            task.set_editor_property('factory',unreal.FontFileImportFactory())
            task.set_editor_property('automated',True);task.set_editor_property('save',True);TOOLS.import_asset_tasks([task])
            REPORT['created'].append(path)
        fonts[key]=unreal.load_asset(path)
        if not isinstance(fonts[key],unreal.FontFace):raise RuntimeError('Missing original font '+key)
    water=material('DivinationWaterArk',{'PreviewTime':0,'RevealPhase':0,'WaterStrength':.75},textures={'Flow':textures['flow'],'FlowDetail':textures['flow_02']})
    card=material('DivinationCardArk',{'Back':0,'PreviewTime':0,'Tilt':0},textures={'CardFace':textures['tarot_4'],'CardBack':textures['tarot_card_back'],'Warp':textures['card_water_normal_fix'],'Gradient':textures['card_warp_gradient']})
    scene=json.load(open(runtime,encoding='utf-8'))
    groups={}
    for effect in scene.get('pageEffects',{}).values():
        group=groups.setdefault(effect['shader'],{'floats':{'PreviewTime':0},'vectors':{'VertexColor':[1,1,1,1]},'textures':{},'blend':effect['blend']})
        group['floats'].update(effect['floats']);group['vectors'].update(effect['vectors']);group['textures'].update(effect['textures'])
    page_materials={}
    for name,g in groups.items():
        page_materials[name]=material(name,g['floats'],textures={k:textures[v] for k,v in g['textures'].items()},vectors=g['vectors'],blend=unreal.BlendMode.BLEND_ADDITIVE if g['blend']=='additive' else unreal.BlendMode.BLEND_TRANSLUCENT,code_name=name.removesuffix('Add'))
    path=FOLDER+'/DA_DivinationStyleArk'
    if ASSETS.does_asset_exist(path):
        style=unreal.load_asset(path);REPORT['preserved'].append(path)
        existing=dict(style.get_editor_property('original_art'))
        keys={str(key) for key in existing}
        for key,value in textures.items():
            if key not in keys:existing[key]=value
        style.set_editor_property('original_art',existing)
        style.set_editor_property('original_layout_json',open(runtime,encoding='utf-8').read());save(style)
    else:
        factory=unreal.DataAssetFactory();factory.set_editor_property('data_asset_class',unreal.PadmaDivinationStyle)
        style=TOOLS.create_asset('DA_DivinationStyleArk',FOLDER,unreal.PadmaDivinationStyle,factory)
        style.set_editor_property('original_art',textures)
        style.set_editor_property('original_layout_json',open(runtime,encoding='utf-8').read())
        style.set_editor_property('water_material',water);style.set_editor_property('card_material',card)
        save(style);REPORT['created'].append(path)
    unreal.SystemLibrary.execute_console_command(None,'AssetCompilingManager.FinishAllCompilation')
    style.set_editor_property('original_fonts',fonts);style.set_editor_property('page_materials',page_materials);save(style)
    REPORT['status']='passed'

try:run()
except Exception as error:REPORT['status']='failed';REPORT['error']=str(error);raise
finally:
    target=os.path.join(ROOT,'Artifacts','TASK-054','authoring-originals.json');os.makedirs(os.path.dirname(target),exist_ok=True)
    with open(target,'w',encoding='utf-8') as f:json.dump(REPORT,f,ensure_ascii=False,indent=2)
    unreal.log('[PadmaDivinationOriginals] '+json.dumps(REPORT,ensure_ascii=False))
