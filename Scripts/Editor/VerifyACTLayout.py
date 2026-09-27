"""Fresh UE process verification of every migrated asset and ACT reference edge. No Content writes."""
import json
import runpy
from pathlib import Path
import unreal as u

P=Path(u.Paths.project_dir()).resolve()
O=P/'Artifacts/ACTFinalLayout'
plan=json.loads((O/'plan.json').read_text(encoding='utf8'))
mapping={a['path']:a['target'] for a in plan['assets']}
reg=u.AssetRegistryHelpers.get_asset_registry();reg.search_all_assets(True)
options=u.AssetRegistryDependencyOptions(True,True,True,True,True)
assets=reg.get_assets_by_path('/Game/Sandbox/ACT',recursive=True)
actual={str(a.package_name):a for a in assets}
expected={a['target'] for a in plan['assets'] if a['target']}
errors=[];verified=[]
if set(actual)!=expected:errors.append({'package_set_difference':sorted(set(actual)^expected)})
for a in plan['assets']:
    target=a['target']
    if not target:continue
    data=actual.get(target);obj=data.get_asset() if data else None
    if not obj:errors.append({'load_failed':target});continue
    if str(data.asset_class_path.asset_name)!=a['class']:errors.append({'class_changed':target})
    before={mapping.get(p,p) for p in a['dependencies'] if p.startswith('/Game/Sandbox/ACT/')}
    after={str(p) for p in reg.get_dependencies(data.package_name,options) if str(p).startswith('/Game/Sandbox/ACT/')}
    if before!=after:errors.append({'references_changed':target,'missing':sorted(before-after),'added':sorted(after-before)})
    if 'segments' in a:
        current=[s.get_editor_property('anim_reference').get_path_name()
                 for t in obj.get_editor_property('slot_anim_tracks')
                 for s in t.get_editor_property('anim_track').get_editor_property('anim_segments')
                 if s.get_editor_property('anim_reference')]
        expected_segments=[]
        for original in a['segments']:
            package,dot,name=original.partition('.')
            expected_segments.append(mapping.get(package,package)+dot+name)
        if current!=expected_segments:errors.append({'montage_segments_changed':target})
    if 'table_json' in a:
        translated_table=a['table_json']
        for source,dest in sorted(mapping.items(),key=lambda p:len(p[0]),reverse=True):
            if dest:translated_table=translated_table.replace(source,dest)
        if json.loads(u.DataTableFunctionLibrary.export_data_table_to_json_string(obj))!=json.loads(translated_table):
            errors.append({'table_rows_changed':target})
    for key,old in a['properties'].items():
        value=obj.get_editor_property(key)
        now=value.get_path_name() if isinstance(value,u.Object) else str(value)
        translated=old
        package, dot, name = old.partition('.')
        if package in mapping and mapping[package]:translated=mapping[package]+dot+name
        if now!=translated:errors.append({'property_changed':target,'property':key,'before':translated,'after':now})
    verified.append(target)
report={'loaded':len(verified),'expected':len(expected),'errors':errors}
(O/'verification.json').write_text(json.dumps(report,ensure_ascii=False,indent=2),encoding='utf8')
assert not errors, 'Layout verification failed: inspect verification.json'
catalog=u.load_asset('/Game/Sandbox/ACT/Character/ChenQianyu/AbilitySystem/DA_ACTCatalog_CHEN')
assert catalog.validate_catalog().valid
runpy.run_path(str(P/'Scripts/Editor/VerifyChenSceneParticipants.py'),run_name='__main__')
for script in ['ConfigureChenCombatIdle.py','ConfigureChenWeaponLifecycle.py','BindChenActionFX.py']:
    assert u.SystemLibrary.parse_param(u.SystemLibrary.get_command_line(),'PadmaValidateOnly')
    runpy.run_path(str(P/'Scripts/Editor'/script),run_name='__main__')
u.log('ACT_LAYOUT_VERIFIED '+str(len(verified)))
