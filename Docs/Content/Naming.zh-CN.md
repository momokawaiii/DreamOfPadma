# 资源命名

- 英文原文（Agent 阅读）：`Docs/Content/Naming.md`

项目自有资源放在 `Content/Padma/` 下，并使用类型前缀：

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

稳定玩法 ID 与资源路径分离。重命名资源不能改变卡牌、节点、遭遇、任务或存档 ID。

第三方资源保留来源目录，不能原地编辑。项目自己的包装材质、数据资产和蓝图放在 `Content/Padma/`。
