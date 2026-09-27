"""Restore source red-channel dissolve weighting for R's final blade materials only.

Run with full UE editor, PythonScriptPlugin and CascadeToNiagaraConverter enabled.
No particle timing, Montage, gameplay or source files are changed.
"""
import unreal as u,json,shutil
from pathlib import Path
P=Path(u.Paths.project_dir()).resolve();H=P/'Scripts/Editor/ChenFX'
exec((H/'support.py').read_text(),globals())
dest='/Game/Sandbox/ACT/Character/ChenQianyu/Art/Niagara';S=R/'SourceAssetsV2'
def ff(x):return f'{float(x):.9f}'
def fv(a):return 'float'+str(len(a))+'('+','.join(ff(x) for x in a)+')'
exec((H/'material.py').read_text(),globals())
out=P/'Artifacts/TrainingDummy/UltimateMaterialRepair';out.mkdir(exist_ok=True)
row=next(r for r in json.loads((R/'prepared_v2.json').read_text()) if r['effect']=='P_chen_exhibit_03_cut_08')
changed=[]
for p in row['particles']:
 floats=p['materials'][0]['data']['m_SavedProperties']['m_Floats']
 if not floats.get('_UseWeightTex',0) or not floats.get('_UseDissolve',0):continue
 name='M_'+p['ue_name'];file=P/'Content/Sandbox/ACT/Character/ChenQianyu/Art/Niagara/Materials'/(name+'.uasset')
 backup=out/(name+'.before.backup')
 if not backup.exists():shutil.copy2(file,backup)
 mat=material_v2(p)
 lib.set_metadata_tag(mat,'SourceWeightRevision','1');assert lib.save_loaded_asset(mat,False)
 changed.append(name)
assert 'M_chen_exhibit_03_cut_08_2' in changed and 'M_chen_exhibit_03_cut_08_13' in changed
(out/'report.json').write_text(json.dumps({'materials':changed,'timing_unchanged':True,'weight':'1-UseWeightTex + sum(sample.r * UseWeight2)','source':'variant_1294_0.asm'},indent=2))
u.SystemLibrary.quit_editor()
