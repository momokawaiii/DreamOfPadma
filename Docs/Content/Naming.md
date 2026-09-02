# Asset Naming

- Chinese companion for user reading: `Docs/Content/Naming.zh-CN.md`

Project-owned assets belong under `Content/Padma/` and use a type prefix.

```text
DA_Card_<Id>
DT_<Domain>_<Name>
BP_Artifact_<Name>
BP_Encounter_<Name>
WBP_<ScreenOrWidget>
IA_<Action>
IMC_<Context>
ABP_<Character>
AM_<Action>
NS_<Effect>
M_<Material>
MI_<MaterialVariant>
MF_<MaterialFunction>
PCG_<Graph>
L_<Map>
LS_<LevelSequence>
SM_<Mesh>
SK_<SkeletalMesh>
SFX_<Name>
MUS_<Name>
```

Stable gameplay IDs are separate from asset paths. Renaming an asset must not change a card, node, encounter, quest, or save identifier.

Third-party assets keep their source folder and are not edited in place. Project-owned wrapper materials, data assets, and blueprints belong under `Content/Padma/`.
