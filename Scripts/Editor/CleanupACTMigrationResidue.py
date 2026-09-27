"""Audit only old source packages left on disk after an interrupted AssetTools rename.
Requires -PadmaApplyACTLayout; rejects retained/external inbound references and
emits a hash-bound quarantine manifest. Does not move/delete Content files.
"""
import json, hashlib
from pathlib import Path
import unreal as u
assert u.SystemLibrary.parse_param(u.SystemLibrary.get_command_line(),'PadmaApplyACTLayout')
P=Path(u.Paths.project_dir()).resolve();O=P/'Artifacts/ACTFinalLayout'
plan=json.loads((O/'plan.json').read_text(encoding='utf8'))
assert (O/'backup-verified.json').exists()
reg=u.AssetRegistryHelpers.get_asset_registry();reg.search_all_assets(True)
opt=u.AssetRegistryDependencyOptions(True,True,True,True,True)
old={a['path'] for a in plan['assets'] if a['operation']!='keep'}
lib=u.EditorAssetLibrary
files=[]
for a in plan['assets']:
    if a['target']:
        data=reg.get_asset_by_object_path(a['target']+'.'+a['target'].rsplit('/',1)[1])
        assert str(data.asset_class_path.asset_name)==a['class'],a['target']
for path in old:
    if not lib.does_asset_exist(path):continue
    refs={str(p) for p in reg.get_referencers(path,opt)}
    assert refs<=old,'Still live reference: '+path+str(refs-old)
    for extension in ['.uasset','.umap']:
        f=P/'Content'/(path[6:]+extension)
        if f.exists():files.append({'path':str(f),'relative':f.relative_to(P).as_posix(),'sha256':hashlib.sha256(f.read_bytes()).hexdigest()})
# AssetTools already authored the canonical destinations. This is only a manifest
# of interrupted old physical copies, for guarded recoverable quarantine outside Content.
(O/'residue-approved.json').write_text(json.dumps({'files':files,'no_retained_inbound_references':True},indent=2),encoding='utf8')
u.log('ACT_RESIDUE_QUARANTINE_APPROVED '+str(len(files)))
