"""Resume after asset rename: verify destinations, fix scoped redirectors, retire isolated fixtures.
Requires -PadmaApplyACTLayout. Writes evidence; verified backup must already exist.
"""
import json
from pathlib import Path
import unreal as u

assert u.SystemLibrary.parse_param(u.SystemLibrary.get_command_line(), 'PadmaApplyACTLayout')
project = Path(u.Paths.project_dir()).resolve()
out = project / 'Artifacts/ACTFinalLayout'
plan = json.loads((out/'plan.json').read_text(encoding='utf8'))
assert (out/'backup-verified.json').exists() and (out/'before-migration.zip').exists()
lib = u.EditorAssetLibrary
reg = u.AssetRegistryHelpers.get_asset_registry()
reg.search_all_assets(True)
options = u.AssetRegistryDependencyOptions(True,True,True,True,True)
u.SystemLibrary.execute_console_command(None, 'Padma.ACT.PrepareLayoutRename')
for a in plan['assets']:
    if a['target']:
        data = reg.get_asset_by_object_path(a['target']+'.'+a['target'].rsplit('/',1)[1])
        assert str(data.asset_class_path.asset_name) == a['class'], 'Destination absent/wrong class: '+a['target']
u.SystemLibrary.execute_console_command(None, 'Padma.ACT.FixLayoutRedirectors')
retired = {a['path'] for a in plan['assets'] if a['operation']=='archive' and lib.does_asset_exist(a['path'])}
for path in retired:
    refs = {str(r) for r in reg.get_referencers(path,options)}
    assert refs <= retired, 'Retirement has retained references: '+path+str(refs)
while retired:
    leaves = [p for p in retired if not ({str(r) for r in reg.get_referencers(p,options)} & retired)]
    assert leaves, 'Cycle in retired fixtures'
    for path in leaves:
        assert lib.delete_asset(path), path
        retired.remove(path)
u.SystemLibrary.execute_console_command(None, 'Padma.ACT.FixLayoutRedirectors')
assets = reg.get_assets_by_path('/Game/Sandbox/ACT',recursive=True)
remaining = [str(a.package_name) for a in assets if str(a.asset_class_path.asset_name)=='ObjectRedirector']
report={'targets':sum(bool(a['target']) for a in plan['assets']),'redirectors':remaining,'archived':[a['path'] for a in plan['assets'] if a['operation']=='archive']}
report['old_physical_files']=[str(project/'Content'/(a['path'][6:]+ext))
    for a in plan['assets'] if a['operation']!='keep' for ext in ['.uasset','.umap']
    if (project/'Content'/(a['path'][6:]+ext)).exists()]
(out/'finalize.json').write_text(json.dumps(report,indent=2),encoding='utf8')
assert not remaining, 'Redirectors remain; inspect before cleanup'
assert not report['old_physical_files'], 'Old files remain on disk; use guarded residue audit/quarantine before claiming completion'
u.log('ACT_FINALIZE_COMPLETE')
