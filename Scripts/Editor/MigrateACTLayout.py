"""One-shot UE migration, requires -PadmaApplyACTLayout and a reviewed plan.json.
Backs up and verifies all source packages before using Unreal AssetTools to rename.
Never overwrite a destination or run against a partially migrated tree.
"""
import hashlib
import json
import zipfile
from pathlib import Path
import unreal as u

assert u.SystemLibrary.parse_param(u.SystemLibrary.get_command_line(), 'PadmaApplyACTLayout'), 'Explicit -PadmaApplyACTLayout required'
project = Path(u.Paths.project_dir()).resolve()
out = project / 'Artifacts/ACTFinalLayout'
plan = json.loads((out / 'plan.json').read_text(encoding='utf8'))
lib = u.EditorAssetLibrary
reg = u.AssetRegistryHelpers.get_asset_registry()
reg.search_all_assets(True)
options = u.AssetRegistryDependencyOptions(True, True, True, True, True)
for f in plan['files']:
    assert hashlib.sha256((project / f['file']).read_bytes()).hexdigest() == f['sha256'], 'Changed source: ' + f['file']
for a in plan['assets']:
    if a['operation'] == 'move':
        assert not lib.does_asset_exist(a['target']), 'Destination exists: ' + a['target']
external = sorted({r for a in plan['assets'] for r in a['referencers'] if r.startswith('/Game/') and not r.startswith('/Game/Sandbox/ACT/')})
assert not external, 'Unexpected external referencers must be backed up/reviewed: ' + str(external)
backup = out / 'before-migration.zip'
if not backup.exists():
    with zipfile.ZipFile(backup, 'w', zipfile.ZIP_STORED) as z:
        for f in plan['files']:
            z.write(project / f['file'], f['file'])
with zipfile.ZipFile(backup) as z:
    for f in plan['files']:
        assert hashlib.sha256(z.read(f['file'])).hexdigest() == f['sha256']
(out / 'backup-verified.json').write_text(json.dumps({'files': len(plan['files']), 'archive': str(backup)}), encoding='utf8')
renames = []
# Viewport-history keys are native CDO soft references, not authored level dependencies.
# Clear the process-local copy only; never rewrite the user's Saved configuration.
u.SystemLibrary.execute_console_command(None, 'Padma.ACT.PrepareLayoutRename')
for a in plan['assets']:
    if a['operation'] == 'move':
        obj = lib.load_asset(a['path'])
        assert obj, a['path']
        folder, name = a['target'].rsplit('/', 1)
        renames.append(u.AssetRenameData(obj, folder, name))
assert u.AssetToolsHelpers.get_asset_tools().rename_assets(renames), 'Rename failed: inspect partial state; backup retained'
assert lib.save_directory('/Game/Sandbox/ACT', only_if_is_dirty=False, recursive=True)
# Retire only the reviewed isolated fixture chain. Its bytes remain in the verified zip.
retired = {a['path'] for a in plan['assets'] if a['operation'] == 'archive'}
for path in retired:
    refs = {str(r) for r in reg.get_referencers(path, options)}
    assert refs <= retired, 'Unexpected retained reference before retirement: ' + path + ': ' + str(refs)
# Delete consumers before dependencies; never force-delete the whole folder.
while retired:
    leaves = [p for p in retired if not ({str(r) for r in reg.get_referencers(p, options)} & retired)]
    assert leaves, 'Retired fixture cycle requires explicit review'
    for path in leaves:
        assert lib.delete_asset(path), 'Could not retire: ' + path
        retired.remove(path)
report = {'renamed': len(renames), 'archived': [a['path'] for a in plan['assets'] if a['operation'] == 'archive'],
          'redirectors': [str(a.package_name) for a in reg.get_assets_by_path('/Game/Sandbox/ACT', recursive=True) if str(a.asset_class_path.asset_name) == 'ObjectRedirector']}
(out / 'migration.json').write_text(json.dumps(report, indent=2), encoding='utf8')
u.log('ACT_LAYOUT_MIGRATED ' + str(len(renames)))
