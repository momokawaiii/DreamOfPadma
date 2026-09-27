"""Targeted source-rule migration. Preserves user BP/ABP, FX notifies, costs, cooldowns and damage scales."""
import unreal as u
import json, math, hashlib, traceback
from pathlib import Path
ROOT='/Game/Sandbox/ACT/Character/ChenQianyu'
PROJECT=Path(u.Paths.project_dir()).resolve()
OUT=PROJECT/'Artifacts/ChenQianyu/Actions/SourceRules'
OUT.mkdir(parents=True,exist_ok=True)
LIB=u.EditorAssetLibrary
def quat_mul(a,b):
 x,y,z,w=a;X,Y,Z,W=b
 return (w*X+x*W+y*Z-z*Y,w*Y-x*Z+y*W+z*X,w*Z+x*Y-y*X+z*W,w*W-x*X-y*Y-z*Z)
def rotation(e):
 x,y,z=[math.radians(v)*.5 for v in e]
 q=quat_mul(quat_mul((0,math.sin(y),0,math.cos(y)),(math.sin(x),0,0,math.cos(x))),(0,0,math.sin(z),math.cos(z)))
 return u.Quat(q[2],q[0],q[1],q[3]).rotator()
def vec(v): return u.Vector(v[2]*100,v[0]*100,v[1]*100)
def prop(o,**args):
 for k,v in args.items():o.set_editor_property(k,v)
def apply():
 rules=json.loads((PROJECT/'Scripts/Editor/ChenACTSourceRules.json').read_text(encoding='utf-8'))
 protected=[PROJECT/'Content/Sandbox/ACT/Character/ChenQianyu'/p for p in ['Blueprints/BP_ACT_CHEN.uasset','Animation/AnimBlueprints/ABP_ACT_CHEN.uasset']]
 hashes={str(f):hashlib.sha256(f.read_bytes()).hexdigest() for f in protected}
 import ConfigureChenUltimateTiming
 ConfigureChenUltimateTiming.apply()
 u.SystemLibrary.execute_console_command(u.EditorLevelLibrary.get_editor_world(),'Padma.ChenActions.ResetRules')
 report=[]
 for row in rules['actions']:
  name=row['name'];m=u.load_asset(ROOT+'/Animation/Montages/AM_Chen_'+name);d=u.load_asset(ROOT+'/AbilitySystem/Abilities/DA_Chen_'+name)
  assert m and d and LIB.get_metadata_tag(d,'PadmaAuthor')=='ChenActions20260920'
  length=u.AnimationLibrary.get_sequence_length(m)
  tracks=[str(t) for t in u.AnimationLibrary.get_animation_notify_track_names(m)]
  if 'SourceRules' not in tracks:u.AnimationLibrary.add_animation_notify_track(m,'SourceRules')
  offset=0
  if name=='Plunge':
   offset=sum(u.AnimationLibrary.get_sequence_length(u.load_asset(ROOT+'/Animation/Sequences/Imported/AS_Chen_'+n+'_CM')) for n in ['PlungeStart','PlungeLoop'])
  def window(kind,start,end,ids=(),cache=.3):
   start+=offset;end=min(end+offset,length-.001)
   if end<=start:return
   ans=u.AnimationLibrary.add_animation_notify_state_event(m,'SourceRules',start,end-start,u.AnimNotifyState_PadmaACTTransitionWindow)
   prop(ans,kind=getattr(u.PadmaACTTransitionWindow,kind),actions=ids,cache_seconds=cache,carry_across_actions=kind=='BLOCK')
  window('RECOVERY',row['exclusive_frame']/30+.00002,length-offset)
  if name=='Dodge':
   attacks=['Chen.Attack0'+str(i) for i in range(1,6)]
   window('BLOCK_INPUT',0,.1,attacks)
   window('BLOCK',0,.35,attacks)
   window('CACHE',.1,length,attacks,.4)
  for w in row['allow']:
   window('ALLOW',w['frames'][0]/30,w['frames'][1]/30,w['ids'])
   if name.startswith('Attack'):
    u.AnimationLibrary.add_animation_notify_state_event(m,'SourceRules',w['frames'][0]/30,(w['frames'][1]-w['frames'][0])/30,u.AnimNotifyState_PadmaACTComboWindow)
  for w in row['cache']:window('CACHE',w['frames'][0]/30,w['frames'][1]/30,[w['id']],w['seconds'] if w['seconds'] is not None else rules['input_cache_seconds'][w['id'].split('.')[-1]])
  for hit in row['hits']:
   start=offset+hit['frames'][0]/30;end=offset+max(hit['frames'][0]+1,hit['frames'][1])/30
   assert end<length,(name,end,length)
   ans=u.AnimationLibrary.add_animation_notify_state_event(m,'SourceRules',start,end-start,u.AnimNotifyState_PadmaACTHitWindow)
   shapes=[u.PadmaACTContactShape(size=vec(s['size']),center=vec(s['center']),rotation=rotation(s['euler'])) for s in hit.get('shapes',[])]
   prop(ans,window=u.PadmaACTMeleeWindow(weapon_index=0,damage_multiplier=.2,shapes=shapes,primary_target_only=hit.get('primary_target',False),
      hit_effect=u.load_asset('/Game/Sandbox/ACT/Character/ChenQianyu/Art/Niagara/Systems/BladeContact/NS_fxbat_chen_common_hit_01')))
  prop(d,interrupt_priority=row['priority'],can_interrupt=name=='Plunge',input_cache_seconds=rules['input_cache_seconds'][name],use_primary_target=name in ['Execution','LieFengShuang'],content_version=3)
  if name.startswith('Attack'):prop(d,next_combo_id='Chen.Attack0'+str(int(name[-1])%5+1))
  if 'target_range_cm' in row:prop(d,target_range=row['target_range_cm'])
  assert LIB.save_loaded_asset(m,False) and LIB.save_loaded_asset(d,False)
  report.append({'name':name,'length':length,'land_offset':offset,'damage_windows':len(row['hits']),'exclusive_seconds':row['exclusive_frame']/30,'priority':row['priority']})
 # Perfect dodge shares Dodge's source action class; controller-specific invulnerability stays editable.
 for name in ['PerfectDodge','Jump']:
  m=u.load_asset(ROOT+'/Animation/Montages/AM_Chen_'+name);d=u.load_asset(ROOT+'/AbilitySystem/Abilities/DA_Chen_'+name)
  prop(d,interrupt_priority=6 if name=='PerfectDodge' else 0,can_interrupt=False,input_cache_seconds=rules['input_cache_seconds'][name])
  if name=='PerfectDodge':
   if 'SourceRules' not in [str(t) for t in u.AnimationLibrary.get_animation_notify_track_names(m)]:u.AnimationLibrary.add_animation_notify_track(m,'SourceRules')
   ans=u.AnimationLibrary.add_animation_notify_state_event(m,'SourceRules',.2,u.AnimationLibrary.get_sequence_length(m)-.201,u.AnimNotifyState_PadmaACTTransitionWindow)
   prop(ans,kind=u.PadmaACTTransitionWindow.RECOVERY)
   attacks=['Chen.Attack0'+str(i) for i in range(1,6)]
   for kind,start,end,ids,seconds in [('BLOCK_INPUT',0,.1,attacks,0),('BLOCK',0,.25,attacks,0),('BLOCK',0,.5,['Chen.Dodge'],0),('CACHE',.1,u.AnimationLibrary.get_sequence_length(m)-.001,attacks,.3)]:
    ans=u.AnimationLibrary.add_animation_notify_state_event(m,'SourceRules',start,end-start,u.AnimNotifyState_PadmaACTTransitionWindow)
    prop(ans,kind=getattr(u.PadmaACTTransitionWindow,kind),actions=ids,cache_seconds=seconds,carry_across_actions=kind=='BLOCK')
  assert LIB.save_loaded_asset(m,False) and LIB.save_loaded_asset(d,False)
 assert all(hashlib.sha256(Path(f).read_bytes()).hexdigest()==h for f,h in hashes.items())
 (OUT/'author.json').write_text(json.dumps({'status':'saved','rules':report,'protected_blueprints':hashes,'limitations':rules['limitations']},indent=2),encoding='utf-8')
 # ResetRules removes all gameplay transition states, including the locomotion follow-up.
 import ConfigureChenMovementTransitions
 ConfigureChenMovementTransitions.apply()
if __name__=='__main__':
 try:apply()
 except Exception:
  (OUT/'error.txt').write_text(traceback.format_exc(),encoding='utf-8');raise
 finally:u.SystemLibrary.quit_editor()
