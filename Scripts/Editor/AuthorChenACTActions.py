"""UE Editor Python: explicitly reseed Chen's editable action catalog from existing assets.
Run with -ExecutePythonScript=<this file>. Evidence: Artifacts/ChenQianyu/Actions/GAS.
"""
import unreal as u
import json, traceback, hashlib
from pathlib import Path

ROOT='/Game/Sandbox/ACT/Character/ChenQianyu'
OUT=Path(u.Paths.project_dir()).resolve()/'Artifacts/ChenQianyu/Actions/GAS'
OUT.mkdir(parents=True,exist_ok=True)
lib=u.EditorAssetLibrary
tools=u.AssetToolsHelpers.get_asset_tools()
def create(path, cls, factory):
    existing=u.load_asset(path) if lib.does_asset_exist(path) else None
    if existing:
        assert lib.get_metadata_tag(existing,'PadmaAuthor')=='ChenActions20260920', 'Refusing to overwrite unowned asset: '+path
        return existing
    obj=tools.create_asset(path.rsplit('/',1)[1],path.rsplit('/',1)[0],cls,factory)
    assert obj, path
    lib.set_metadata_tag(obj,'PadmaAuthor','ChenActions20260920')
    return obj
def data(path,cls):
    f=u.DataAssetFactory(); f.set_editor_property('data_asset_class',cls)
    return create(path,cls,f)
def setp(obj,**kw):
    for k,v in kw.items(): obj.set_editor_property(k,v)
def save(obj): assert lib.save_loaded_asset(obj,False),obj.get_path_name()

try:
    assert u.SystemLibrary.parse_param(u.SystemLibrary.get_command_line(),'PadmaReseedChenActions'), 'Reseeding replaces skill tuning; use -PadmaReseedChenActions explicitly. Daily authoring uses the existing DA/DT.'
    mesh=u.load_asset(ROOT+'/Art/Meshes/SK_Chen_FullCharacter_CM')
    skeleton=mesh.get_editor_property('skeleton')
    bp=u.load_asset(ROOT+'/Blueprints/BP_ACT_CHEN'); abp=u.load_asset(ROOT+'/Animation/AnimBlueprints/ABP_ACT_CHEN')
    assert bp and abp
    before={}
    for asset in [bp,abp]:
        name=asset.get_name()
        file=Path(u.Paths.project_content_dir()).resolve()/(asset.get_path_name().split('.')[0][6:]+'.uasset')
        before[name]=hashlib.sha256(file.read_bytes()).hexdigest()
    graph=[]
    for cls in [u.AnimGraphNode_SequencePlayer,u.AnimGraphNode_Slot,u.AnimGraphNode_StateMachine]:
        for node in u.AnimationLibrary.get_nodes_of_class(abp,cls):
            row={'class':cls.__name__,'title':str(u.BlueprintEditorLibrary.get_node_title(node))}
            try: row['node']=str(node.get_editor_property('node'))
            except Exception: pass
            graph.append(row)
    (OUT/'abp-graph.json').write_text(json.dumps(graph,indent=2),encoding='utf-8')
    # Each row uses the existing ACT row schema. Numbers below are explicitly editable test tuning.
    items=[
      ('Attack01','ATTACK','Primary','Attack01','Attack02',None),
      ('Attack02','ATTACK','Combo2','Attack02','Attack03',(.30,.65)),
      ('Attack03','ATTACK','Combo3','Attack03','Attack04',(.35,.75)),
      ('Attack04','ATTACK','Combo4','Attack04','Attack05',(.35,.75)),
      ('Attack05','ATTACK','Combo5','Attack05','',(.40,.85)),
      ('GuiQiongYu','SKILL','SkillE','GuiQiongYu','',(.45,.80)),
      ('JianTianHe','SKILL','SkillQ','JianTianHe','',(.60,1.00)),
      ('LieFengShuang','SKILL','SkillR','LieFengShuang','',(1.00,1.50)),
      ('Execution','EXECUTION','Execution','Execution','',(.60,.90)),
      ('Dodge','DODGE','Dodge','NormalDodgeL','',None),
      ('PerfectDodge','PERFECT_DODGE','PerfectInternal','PerfectDodgeBackL','',None),
      ('Plunge','PLUNGE','Plunge','PlungeStart','',None),
      ('Jump','JUMP','Jump','JumpStartL','',None),
    ]
    montages={}
    for name,kind,binding,clip,next_id,hit in items:
        if not clip: continue
        path=ROOT+'/Animation/Montages/AM_Chen_'+name
        if name=='Attack01':
            if lib.does_asset_exist(path):
                m=u.load_asset(path); assert lib.get_metadata_tag(m,'PadmaAuthor')=='ChenActions20260920'
            else:
                raise RuntimeError('Missing canonical Attack01 Montage; restore it before reseeding: '+path)
        else:
            seq=u.load_asset(ROOT+'/Animation/Sequences/Imported/AS_Chen_'+clip+'_CM')
            factory=u.AnimMontageFactory();factory.source_animation=seq;factory.target_skeleton=skeleton
            m=create(path,u.AnimMontage,factory)
        montages[name]=m
    u.SystemLibrary.execute_console_command(u.EditorLevelLibrary.get_editor_world(),'Padma.ChenActions.ConfigureMontages')
    rows=[]; manifest=[]
    for name,kind,binding,clip,next_id,hit in items:
        m=montages.get(name)
        if m and name!='Attack01':
            u.AnimationLibrary.remove_all_animation_notify_tracks(m)
            for track in ['Damage','Combo','Dodge']:u.AnimationLibrary.add_animation_notify_track(m,track)
            if name=='Plunge':hit=(1.666667,1.966667)
            if hit:
                ans=u.AnimationLibrary.add_animation_notify_state_event(m,'Damage',hit[0],hit[1]-hit[0],u.AnimNotifyState_PadmaACTHitWindow)
                area=kind in ['SKILL','EXECUTION','PLUNGE']
                radius=320.0 if name=='LieFengShuang' else 220.0 if area else 0.0
                win=u.PadmaACTMeleeWindow(weapon_index=0,damage_multiplier=0.2,area_radius=radius,
                    area_offset=u.Vector(80,0,-35) if area else u.Vector(),
                    hit_effect=u.load_asset(ROOT+'/Art/Niagara/Systems/BladeContact/NS_fxbat_chen_common_hit_01'))
                ans.set_editor_property('window',win)
            if next_id:
                u.AnimationLibrary.add_animation_notify_state_event(m,'Combo',.85,1.35,u.AnimNotifyState_PadmaACTComboWindow)
            if kind in ['DODGE','PERFECT_DODGE']:
                ans=u.AnimationLibrary.add_animation_notify_state_event(m,'Dodge',.03,.32,u.AnimNotifyState_PadmaACTDodgeWindow)
                ans.set_editor_property('perfect',kind=='DODGE')
            save(m)
        elif m:save(m)
        d=data(ROOT+'/AbilitySystem/Abilities/DA_Chen_'+name,u.PadmaACTSkillDefinition)
        setp(d,definition_id='Chen.'+name,content_version=2,display_name=name,ability_implementation_id='Native.ACT.CharacterAction',
            action_kind=getattr(u.PadmaACTActionKind,kind),montage=m,start_section='Attack01' if name=='Attack01' else 'Start' if name in ['Plunge','Jump'] else 'Action' if m else '',
            next_combo_id='Chen.'+next_id if next_id else '',perfect_dodge_id='Chen.PerfectDodge' if kind=='DODGE' else '',
            play_rate=1.0,damage_scale=1.0,cooldown_seconds=0.0,flow_cost=0.0,calculation_cost=0.0,
            can_interrupt=kind in ['DODGE','PLUNGE'],dodge_speed=600.0,plunge_speed=1200.0,target_range=250.0)
        save(d)
        rows.append({'Name':name,'SkillId':'Chen.'+name,'ActivationBindingId':binding,'Definition':d.get_path_name()})
        manifest.append({'name':name,'binding':binding,'definition':d.get_path_name(),'montage':m.get_path_name() if m else None,'test_hit_window':hit})
    f=u.DataTableFactory();f.struct=u.PadmaACTSkillRow.static_struct()
    table=create(ROOT+'/AbilitySystem/DT_ACTSkills_CHEN',u.DataTable,f)
    assert u.DataTableFunctionLibrary.fill_data_table_from_json_string(table,json.dumps(rows))
    save(table)
    profile=u.load_asset(ROOT+'/AbilitySystem/DA_Chen_Equipment')
    assert profile, 'Missing canonical equipment profile; restore before reseeding'
    setp(profile,attack_montage=montages['Attack01'],attack_section='Attack01',mesh_relative_rotation=u.Rotator(0,0,-90))
    save(profile)
    character=data(ROOT+'/AbilitySystem/DA_ACTCharacter_CHEN',u.PadmaACTCharacterDefinition)
    setp(character,definition_id='chen-act-research',content_version=2,display_name='Chen ACT',model=mesh,
        animation_class=abp.generated_class(),character_class=bp.generated_class(),melee_profile=profile,
        skill_table=table,use_character_actions=True,walk_speed=160.0,jump_speed=600.0)
    save(character)
    catalog=data(ROOT+'/AbilitySystem/DA_ACTCatalog_CHEN',u.PadmaACTAuthoringCatalog)
    setp(catalog,characters=[character],references=u.PadmaACTAuthoringReferences(
        activation_binding_ids=[r['ActivationBindingId'] for r in rows],ability_implementation_ids=['Native.ACT.CharacterAction']))
    validation=catalog.validate_catalog()
    assert validation.valid,str(validation.errors)
    save(catalog)
    # Update the retained ACT map in place; do not restore the retired Attack01 lab.
    le=u.get_editor_subsystem(u.LevelEditorSubsystem)
    map_path='/Game/Sandbox/ACT/Training/Maps/L_ChenACT'
    assert le.load_level(map_path)
    for actor in u.get_editor_subsystem(u.EditorActorSubsystem).get_all_level_actors():
        if isinstance(actor,u.PadmaACTMeleeLab): actor.set_editor_property('character_definition',character)
    assert u.EditorLoadingAndSavingUtils.save_map(u.EditorLevelLibrary.get_editor_world(),map_path)
    for name,h in before.items():
        asset=bp if name=='BP_ACT_CHEN' else abp
        file=Path(u.Paths.project_content_dir()).resolve()/(asset.get_path_name().split('.')[0][6:]+'.uasset')
        assert hashlib.sha256(file.read_bytes()).hexdigest()==h
    (OUT/'author.json').write_text(json.dumps({'status':'saved','items':manifest,'blueprints_unchanged':before,'map':map_path},indent=2),encoding='utf-8')
except Exception:
    (OUT/'author-error.txt').write_text(traceback.format_exc(),encoding='utf-8')
    raise
finally:u.SystemLibrary.quit_editor()
