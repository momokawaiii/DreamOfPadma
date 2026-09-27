"""UE Python: bind source-derived FX to the 12 non-Attack01 action Montages.
Preserves sections, root-motion segments, gameplay notifies, BP/ABP and the original library.
Requires existing Content FX, afterimage material and authored dragon notifies; no raw import.
Pass -PadmaValidateOnly on the UE command line to check prerequisites without authoring or quitting.
"""
import unreal as u,json,hashlib,traceback
from pathlib import Path
P=Path(u.Paths.project_dir()).resolve();O=Path(u.Paths.project_saved_dir()).resolve()/'ChenAuthoring/FX'
ROOT='/Game/Sandbox/ACT/Character/ChenQianyu';LIB=u.EditorAssetLibrary;AN=u.AnimationLibrary
VALIDATE_ONLY=u.SystemLibrary.parse_param(u.SystemLibrary.get_command_line(),'PadmaValidateOnly')
def required_asset(path,expected_type=None):
 obj=u.load_asset(path)
 if not obj:raise RuntimeError('Missing required Content asset: '+path+'; restore/import it explicitly before binding.')
 if expected_type and not isinstance(obj,expected_type):raise RuntimeError('Unexpected asset type: '+path)
 return obj

def protected_hashes():
 # Snapshot current Content, independent of historical extraction reports/absolute paths.
 root=P/'Content/Sandbox/ACT/Character/ChenQianyu'
 files=list((root/'Blueprints').rglob('*.uasset'))+list((root/'Animation/AnimBlueprints').rglob('*.uasset'))+list((P/'Content/Sandbox/ACT/Training/Maps').rglob('*.umap'))
 files.append(root/'Animation/Montages/AM_Chen_Attack01.uasset')
 for role in ['Systems','Materials','Textures','Meshes']:
  files.extend((root/'Art/Niagara'/role/'BladeContact').rglob('*.uasset'))
 return {str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in files}

def main():
 O.mkdir(parents=True,exist_ok=True)
 manifest=json.loads((P/'Scripts/Editor/ChenActionFXManifest.json').read_text())
 protected=protected_hashes()
 ghost=required_asset(ROOT+'/Art/Niagara/Materials/M_Chen_SourceAfterimage',u.MaterialInterface);report=[]
 # Load every required asset before changing any notify tracks.
 montages={row['name']:required_asset(ROOT+'/Animation/Montages/AM_Chen_'+row['name'],u.AnimMontage) for row in manifest['actions']}
 if 'Attack01' in montages:raise RuntimeError('Attack01 is outside binding scope')
 required_asset(ROOT+'/AbilitySystem/Abilities/DA_Chen_PerfectDodge')
 for name,m in montages.items():
  if not m.get_editor_property('skeleton'):raise RuntimeError('Missing Montage skeleton: '+name)
  if AN.get_sequence_length(m)<=.002:raise RuntimeError('Empty Montage: '+name)
 dragon=[]
 for event in AN.get_animation_notify_events(montages['LieFengShuang']):
  n=event.get_editor_property('notify')
  if isinstance(n,u.AnimNotify_PadmaACTFX):
   fx=n.get_editor_property('effect')
   if fx.get_editor_property('animated_mesh'):
    if not fx.get_editor_property('mesh_animation'):raise RuntimeError('Existing dragon FX is missing mesh_animation on AM_Chen_LieFengShuang')
    dragon.append((AN.get_anim_notify_event_trigger_time(event),fx,n.get_editor_property('stop')))
 if not dragon:raise RuntimeError('Missing authored dragon FX references on AM_Chen_LieFengShuang; restore/author them explicitly before binding.')
 if not any(not stop for _,_,stop in dragon):raise RuntimeError('Missing dragon start notify on AM_Chen_LieFengShuang')
 systems=set()
 def system(name):
  path=ROOT+'/Art/Niagara/Systems/NS_'+name[2:]
  obj=required_asset(path,u.NiagaraSystem);systems.add(path);return obj
 for row in manifest['actions']:
  for e in row['events']:
   if e['rotation']!=[0.,0.,0.]:raise RuntimeError('Unsupported FX rotation: '+row['name']+' / '+e['effect'])
   system(e['effect'])
  hit=next((h['effect'] for h in row['hits']),None) or next(iter(row.get('nested_hit_names',[])),None)
  if hit:system(hit)
 system('P_common_teleport_01_start_ready')
 u.log('Padma preflight PASS BindChenActionFX: '+str(len(montages))+' Montages, '+str(len(systems))+' Niagara systems, afterimage material, dragon references and recovery skill validated.')
 if VALIDATE_ONLY:
  (O/'validate.json').write_text(json.dumps({'status':'validated','validate_only':True,
   'montages':[m.get_path_name() for m in montages.values()],'systems':sorted(systems),
   'afterimage_material':ghost.get_path_name(),'dragon_events':len(dragon),
   'protected_assets':len(protected),'authoring_skipped':True},indent=2),encoding='utf-8')
  u.log('PadmaValidateOnly: BindChenActionFX skipped notify/material edits, recovery authoring and asset saves.')
  return
 for row in manifest['actions']:
  name=row['name'];assert name!='Attack01'
  m=montages[name]
  before_gameplay=[str(e.get_editor_property('notify_state_class')) for e in AN.get_animation_notify_events(m) if e.get_editor_property('notify_state_class')]
  # Remove the owned notify name so track moves cannot retain duplicate points.
  # These twelve Montages had no FX before this delivery.
  AN.remove_animation_notify_events_by_name(m,'PadmaACTFX')
  if 'SourceFX' in [str(x) for x in AN.get_animation_notify_track_names(m)]:AN.remove_animation_notify_track(m,'SourceFX')
  AN.add_animation_notify_track(m,'SourceFX');length=AN.get_sequence_length(m);added=[]
  def notify(time,fx,stop=False,label=''):
   time=max(.001,min(time,length-.001));n=AN.add_animation_notify_event(m,'SourceFX',time,u.AnimNotify_PadmaACTFX)
   n.set_editor_property('effect',fx);n.set_editor_property('stop',stop);added.append({'time':time,'stop':stop,'effect':label})
  for e in row['events']:
   pos=e['offset'];rot=e['rotation'];assert rot==[0.,0.,0.],(name,rot)
   transform=u.Transform(location=u.Vector(-pos[0]*100,pos[2]*100,pos[1]*100),scale=u.Vector(*e['scale']))
   # Mount 52/63 source locator resolution is retained in the manifest; current adapter uses root.
   fx=u.PadmaACTMeleeFX(system=system(e['effect']),weapon_index=-1,follow_mount=e['follow'],relative_transform=transform)
   time=.001 if name=='Plunge' and e['effect']=='P_chen_air_atk_start_01' else e['time']
   if name=='Plunge' and e['effect']=='P_chen_air_atk_start_atk':time+=.001
   notify(time,fx,label=e['effect'])
   end=(1+2/3+.001) if name=='Plunge' and e['effect']=='P_chen_air_atk_start_01' else e['end']
   if end<length:notify(end,fx,True,e['effect'])
  if name in ['Dodge','PerfectDodge']:
   # Original renderer-afterimage action 2-8 at 30 Hz; first burst .1 seconds into it.
   fx=u.PadmaACTMeleeFX(afterimage_material=ghost,afterimage_lifetime=.42)
   notify(2/30+.1,fx,label='P_char_canying_01_start_asset')
  if name=='LieFengShuang':
   for time,fx,stop in dragon:notify(time,fx,stop,'P_chen_exhibit_03_lonng')
  if name=='Jump':
   # Shared source air streak, scaled for the existing UE jump. No original Chen jump FX claim.
   fx=u.PadmaACTMeleeFX(system=system('P_common_teleport_01_start_ready'),weapon_index=-1,follow_mount=False,relative_transform=u.Transform(scale=u.Vector(.22,.22,.22)))
   notify(.06,fx,label='UE jump adaptation / source teleport streak')
   notify(2.9+.01,fx,label='UE landing adaptation / source teleport streak')
  hit_name=next((h['effect'] for h in row['hits']),None)
  if not hit_name:hit_name=next(iter(row.get('nested_hit_names',[])),None)
  hit_count=0
  if hit_name or name=='Execution':
   for event in AN.get_animation_notify_events(m):
    state=event.get_editor_property('notify_state_class')
    if isinstance(state,u.AnimNotifyState_PadmaACTHitWindow):
     window=state.get_editor_property('window')
     # Both source F damage actions have empty effectName and playDefaultHitEffect=false.
     if hit_name:window.set_editor_property('hit_effect',system(hit_name))
     else:
      # Assigning None to an existing Python soft reference may retain its asset path.
      # Copy gameplay fields into a fresh struct whose soft path is actually empty.
      source_window=window;window=u.PadmaACTMeleeWindow()
      for key in ['weapon_index','damage_multiplier','hit_socket','face_target_planar','area_radius','area_offset','shapes','primary_target_only']:
       window.set_editor_property(key,source_window.get_editor_property(key))
      assert window.get_editor_property('hit_effect') is None
     window.set_editor_property('suppress_fallback_hit_effect',name=='Execution')
     window.set_editor_property('face_target_planar',True);state.set_editor_property('window',window);hit_count+=1
  after_gameplay=[str(e.get_editor_property('notify_state_class')) for e in AN.get_animation_notify_events(m) if e.get_editor_property('notify_state_class')]
  assert before_gameplay==after_gameplay,(name,'Gameplay notify identity changed')
  assert sum(isinstance(e.get_editor_property('notify'),u.AnimNotify_PadmaACTFX) for e in AN.get_animation_notify_events(m))==len(added),(name,'Duplicate FX notify')
  assert LIB.save_loaded_asset(m,False)
  report.append({'name':name,'events':added,'hit_effect':hit_name,'hit_windows':hit_count,'unconverted_layers':[e for e in row['non_particle'] if e['effect'] not in ['P_chen_exhibit_03_lonng','P_char_canying_01_start_asset']]})
 assert all(hashlib.sha256(Path(p).read_bytes()).hexdigest()==h for p,h in protected.items())
 # Recompute the locomotion gate if the authored ghost timing/lifetime changes.
 from ConfigureChenMovementTransitions import apply_perfect_dodge
 apply_perfect_dodge()
 (O/'bindings.json').write_text(json.dumps({'status':'saved','actions':report,'protected_unchanged':True},indent=2))
if __name__=='__main__':
 try:
  O.mkdir(parents=True,exist_ok=True)
  main()
 except Exception:
  u.log_error('BindChenActionFX FAILED: '+traceback.format_exc())
  if VALIDATE_ONLY:
   (O/'validate.json').write_text(json.dumps({'status':'error','validate_only':True,'error':traceback.format_exc()},indent=2),encoding='utf-8')
  (O/'bind-error.txt').write_text(traceback.format_exc());raise
 finally:
  if not VALIDATE_ONLY:u.SystemLibrary.quit_editor()
