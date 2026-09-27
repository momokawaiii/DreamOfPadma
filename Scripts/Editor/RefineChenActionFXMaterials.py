"""Re-save the R lit-surface/backdrop adaptation without rebuilding Niagara systems."""
import unreal as u,json
from pathlib import Path
P=Path(u.Paths.project_dir()).resolve();H=P/'Scripts/Editor/ChenFX'
exec((H/'support.py').read_text(),globals());dest='/Game/Sandbox/ACT/Character/ChenQianyu/Art/Niagara';S=R/'SourceAssetsV2'
def ff(x):return f'{float(x):.9f}'
def fv(a):return 'float'+str(len(a))+'('+','.join(ff(x) for x in a)+')'
exec((H/'material.py').read_text(),globals())
data=json.loads((R/'prepared_v2.json').read_text());changed=[]
for effect in data:
 for p in effect['particles']:
  if 'exhibit_start_01_beijing' in p['ue_name'] or p['ue_name']=='chen_exhibit_03_start_01_3':
   material_v2(p);changed.append(p['ue_name'])
usages=[]
for path in lib.list_assets(dest+'/Materials',recursive=False):
 mat=u.load_asset(path)
 if not isinstance(mat,u.Material):continue
 for usage in [u.MaterialUsage.MATUSAGE_NIAGARA_SPRITES,u.MaterialUsage.MATUSAGE_NIAGARA_RIBBONS,u.MaterialUsage.MATUSAGE_NIAGARA_MESH_PARTICLES]:ml.set_material_usage(mat,usage)
 ml.recompile_material(mat);assert lib.save_loaded_asset(mat,False);usages.append(path)
(R/'material-refinement.json').write_text(json.dumps({'saved':changed,'usage_saved':usages,'lit_surface':'HGRP/LitEffect uses original base tint and MRO; no emissive fallback','backdrop':'UE camera edge feather adaptation'},indent=2))
