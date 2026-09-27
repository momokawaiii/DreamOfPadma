"""One-shot mechanical text-path migration; never touches Content binaries.
Run after PlanACTLayout.py. Excludes historical evidence and migration recipes.
"""
import json
import re
from pathlib import Path
P = Path(__file__).resolve().parents[2]
assert not (P/'Artifacts/ACTFinalLayout/verification.json').exists(), 'One-shot migration already applied; do not rewrite current authoring code again'
C = '/Game/Sandbox/ACT/Character/ChenQianyu'
plan = json.loads((P/'Artifacts/ACTFinalLayout/plan.json').read_text(encoding='utf8'))
pairs = []
for a in plan['assets']:
    if a['target'] and a['target'] != a['path']:
        old, new = a['path'], a['target']
        pairs.extend([(old, new), (old[6:], new[6:])])
        if old.startswith(C+'/') and new.startswith(C+'/'):
            pairs.append((old[len(C):], new[len(C):]))
pairs.sort(key=lambda p: len(p[0]), reverse=True)
exclude = {'AuditACTAssets.py','PlanACTLayout.py','MigrateACTLayout.py','RewriteACTLayoutReferences.py',
           'PadmaACTMeleeTest.cpp','PadmaACTMaterialTest.cpp','PadmaACTWeaponStowTest.cpp','PadmaACTLegacyFixture.h'}
changed = []
for folder in ['Source','Scripts','Config']:
    for f in (P/folder).rglob('*'):
        if f.suffix not in {'.cpp','.h','.py','.json','.ini','.ps1'} or f.name in exclude: continue
        before = f.read_text(encoding='utf-8-sig')
        text = before
        for old,new in pairs: text = text.replace(old,new)
        # Handle authored path expressions whose asset names are assembled at runtime.
        for old,new in [('/Actions/Data/Skills/','/AbilitySystem/Abilities/'),
                        ('/Actions/Data/','/AbilitySystem/'),
                        ('/Actions/Montages','/Animation/Montages'),
                        ('/Actions/Animations','/Animation/Sequences/Imported'),
                        ('/Centimeter/AS_','/Animation/Sequences/Imported/AS_'),
                        ('/Attack01/V2/','/Art/Niagara/Systems/BladeContact/'),
                        ('/Actions/FX/','/Art/Niagara/Systems/')]:
            # Full Artifacts paths are output/evidence and deliberately untouched.
            text = '\n'.join(line if 'Artifacts/' in line and '/Game/' not in line else line.replace(old,new) for line in text.split('\n'))
        # Scripts with ROOT=<character>/Actions use short suffixes relative to that root.
        if C+"/Actions'" in text or C+'/Actions"' in text:
            text = text.replace(C+'/Actions',C)
            for old,new in [('/Data/Skills/','/AbilitySystem/Abilities/'),('/Data/','/AbilitySystem/'),
                            ('/Montages/','/Animation/Montages/'),('/Animations/','/Animation/Sequences/Imported/'),
                            ('/FX/NS_','/Art/Niagara/Systems/NS_'),('/FX/M_','/Art/Niagara/Materials/M_'),
                            ('/FX/Dragon/','/Art/Niagara/')]:
                text = text.replace(old,new)
        text = re.sub(r'Animation/Sequences/Imported/(AS_[^\s\"\']+_(?:Runtime|IP))',r'Animation/Sequences/Runtime/\1',text)
        if text != before:
            f.write_text(text,encoding='utf8'); changed.append(f.relative_to(P).as_posix())
(P/'Artifacts/ACTFinalLayout/text-rewrites.json').write_text(json.dumps(changed,indent=2),encoding='utf8')
print('Updated',len(changed),'text files; inspect assembled paths before authoring.')
