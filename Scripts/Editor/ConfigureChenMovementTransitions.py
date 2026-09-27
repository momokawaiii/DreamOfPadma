"""Patch locomotion transition notifies; preserve hit/FX windows and user tuning."""
import unreal as u
import json, math
from pathlib import Path
ROOT='/Game/Sandbox/ACT/Character/ChenQianyu/Animation/Montages/AM_Chen_'
TRACK='LocomotionTransitions'

def apply_perfect_dodge():
 m=u.load_asset(ROOT+'PerfectDodge');lib=u.AnimationLibrary
 assert m
 ends=[]
 for event in lib.get_animation_notify_events(m):
  notify=event.get_editor_property('notify')
  if isinstance(notify,u.AnimNotify_PadmaACTFX) and not notify.get_editor_property('stop'):
   fx=notify.get_editor_property('effect')
   if fx.get_editor_property('afterimage_material'):
    ends.append(lib.get_anim_notify_event_trigger_time(event)+fx.get_editor_property('afterimage_lifetime'))
 assert ends,'PerfectDodge has no authored afterimage; bind its source FX before configuring recovery'
 # Align to the first 60 Hz frame after the last current authored ghost can expire.
 gate=math.ceil((max(ends)-1e-5)*60)/60
 assert gate<lib.get_sequence_length(m)-.001
 track='AfterimageRecovery'
 if track in [str(x) for x in lib.get_animation_notify_track_names(m)]:lib.remove_animation_notify_events_by_track(m,track)
 else:lib.add_animation_notify_track(m,track)
 for kind,ids in [('BLOCK',['Locomotion.Move','Chen.Jump']),('BLOCK_INPUT',['Input.All'])]:
  ans=lib.add_animation_notify_state_event(m,track,0,gate,u.AnimNotifyState_PadmaACTTransitionWindow)
  ans.set_editor_property('kind',getattr(u.PadmaACTTransitionWindow,kind));ans.set_editor_property('actions',ids)
 skill=u.load_asset('/Game/Sandbox/ACT/Character/ChenQianyu/AbilitySystem/Abilities/DA_Chen_PerfectDodge')
 skill.set_editor_property('wait_for_afterimages_before_locomotion',True)
 skill.set_editor_property('block_all_input_until_afterimages_retire',True)
 assert u.EditorAssetLibrary.save_loaded_asset(skill,False) and u.EditorAssetLibrary.save_loaded_asset(m,False)
 return {'afterimage_end_seconds':max(ends),'locomotion_open_seconds':gate,'frame_60hz_zero_based':round(gate*60)}

def apply():
 report=[]
 for name in ['Attack01','Attack02','Attack03','Attack04','Attack05','Dodge','PerfectDodge','Jump']:
  m=u.load_asset(ROOT+name)
  assert m
  lib=u.AnimationLibrary
  tracks=[str(t) for t in lib.get_animation_notify_track_names(m)]
  if TRACK in tracks:lib.remove_animation_notify_events_by_track(m,TRACK)
  else:lib.add_animation_notify_track(m,TRACK)
  if name=='PerfectDodge':
   report.append({'name':name,'policy':'movement/jump wait until afterimage retirement',**apply_perfect_dodge()})
   continue
  length=lib.get_sequence_length(m)
  # AttackToFree/DashToFree accept jump separately from generic skill priority.
  # Landing control is a UE adaptation of returning to the movement pipeline.
  start=0
  ids=['Chen.Jump']
  if name=='Jump':
   start=sum(lib.get_sequence_length(u.load_asset('/Game/Sandbox/ACT/Character/ChenQianyu/Animation/Sequences/Imported/AS_Chen_'+n+'_CM')) for n in ['JumpStartL','FallL'])
   ids=['Locomotion.Move','Chen.Jump']
  ans=lib.add_animation_notify_state_event(m,TRACK,start,length-start-.001,u.AnimNotifyState_PadmaACTTransitionWindow)
  ans.set_editor_property('kind',u.PadmaACTTransitionWindow.ALLOW)
  ans.set_editor_property('actions',ids)
  assert u.EditorAssetLibrary.save_loaded_asset(m,False)
  report.append({'name':name,'allow':ids,'start':start,'end':length-.001})
 Path(u.Paths.project_dir(),'Artifacts/ChenQianyu/Actions/MovementRecovery/author.json').write_text(json.dumps(report,indent=2),encoding='utf-8')

if __name__=='__main__':
 try:apply()
 finally:u.SystemLibrary.quit_editor()
