"""Read-only UE asset audit. Run UnrealEditor-Cmd <project> -run=pythonscript -script=<this file> -NullRHI -unattended.
Records every Sandbox/ACT file, loaded asset type, hard/soft dependencies, referencers and authored identity.
Writes evidence only to Artifacts/ACTFinalLayout; never saves Content.
"""
import hashlib
import json
from pathlib import Path
import unreal as u

ROOT = '/Game/Sandbox/ACT'
project = Path(u.Paths.project_dir()).resolve()
out = project / 'Artifacts/ACTFinalLayout'
out.mkdir(parents=True, exist_ok=True)
reg = u.AssetRegistryHelpers.get_asset_registry()
reg.search_all_assets(True)
options = u.AssetRegistryDependencyOptions(True, True, True, True, True)
assets = sorted(reg.get_assets_by_path(ROOT, recursive=True), key=lambda a: str(a.package_name))
rows = []
for index, data in enumerate(assets):
    path = str(data.package_name)
    obj = data.get_asset()
    row = {'path': path, 'class': str(data.asset_class_path.asset_name), 'loaded': bool(obj),
           'dependencies': sorted(str(p) for p in reg.get_dependencies(data.package_name, options)),
           'referencers': sorted(str(p) for p in reg.get_referencers(data.package_name, options)), 'properties': {}}
    if obj:
        for key in ['definition_id', 'model', 'animation_class', 'character_class', 'melee_profile', 'skill_table',
                    'skeleton', 'enable_root_motion', 'force_root_lock', 'root_motion_root_lock', 'sequence_length',
                    'static_model', 'skeletal_model', 'attack_montage', 'montage', 'material_domain', 'blend_mode']:
            try:
                value = obj.get_editor_property(key)
                row['properties'][key] = value.get_path_name() if isinstance(value, u.Object) else str(value)
            except Exception:
                pass
        if isinstance(obj, u.AnimMontage):
            row['segments'] = [str(s.get_editor_property('anim_reference').get_path_name())
                               for track in obj.get_editor_property('slot_anim_tracks')
                               for s in track.get_editor_property('anim_track').get_editor_property('anim_segments')
                               if s.get_editor_property('anim_reference')]
        if isinstance(obj, u.DataTable):
            row['table_json'] = u.DataTableFunctionLibrary.export_data_table_to_json_string(obj)
    rows.append(row)
    if index % 100 == 0:
        u.log('ACT_AUDIT %d/%d' % (index, len(assets)))
files = []
for file in sorted((project / 'Content/Sandbox/ACT').rglob('*')):
    if file.is_file():
        files.append({'file': file.relative_to(project).as_posix(), 'bytes': file.stat().st_size,
                      'sha256': hashlib.sha256(file.read_bytes()).hexdigest()})
inventory_name = 'current-inventory.json' if (out / 'plan.json').exists() else 'inventory.json'
(out / inventory_name).write_text(json.dumps({'assets': rows, 'files': files}, ensure_ascii=False, indent=2), encoding='utf8')
assert all(row['loaded'] for row in rows), 'Some ACT assets failed to load; inspect inventory.json'
u.log('ACT_AUDIT_COMPLETE assets=%d files=%d' % (len(rows), len(files)))
