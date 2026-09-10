# ACT 角色与技能配置指南

- 英文原文：ACTAuthoring.md
- 文档 ID：CONTENT-ACT-AUTHORING-001
- 版本：0.1
- 状态：已实现配置层；ACT 玩法待实现
- Owner：TASK-036

## 可填写的资产

在 Content Browser 打开 Content/Padma/MVP/Definitions/ACTCharacterCards。先复制模板再填写。新建资产时选择 Miscellaneous → Data Asset，再选择对应原生类。

| 资产／原生类型 | 填写内容 |
|---|---|
| DA_ACTCharacter_Template／PadmaACTCharacterDefinition | 稳定 DefinitionId、ContentVersion、DisplayName、骨骼模型 Model 软引用、可选 AnimationClass 软引用、ACT SkillTable 软引用，以及可选 TraitIds 和显式 TerrainRestrictions |
| DT_ACTSkills_Template／DataTable，行类型 PadmaACTSkillRow | 每行填写 SkillId、ActivationBindingId、指向 ACT 技能资产的 Definition 软引用 |
| DA_ACTSkill_Template／PadmaACTSkillDefinition | 与表行一致的稳定 DefinitionId、DisplayName、ACT AbilityImplementationId，以及可选 Montage 软引用 |
| DA_ACTWeapon_Template／PadmaACTWeaponDefinition | 稳定 DefinitionId、DisplayName、StaticModel 或 SkeletalModel 二选一，可选 AttachmentSocket |
| DA_ACTAuthoring_Template／PadmaACTAuthoringCatalog | 角色／武器软引用，以及地形、激活绑定和能力执行器的只读配置词汇 |

ID 独立于显示名称、模型路径和资产文件名。同一个 ID 不要重用于另一份定义。ContentVersion 记录内容修订，不是技能等级或费用。这些 ACT 角色资产不属于 A／B／C 卡。

本切片要求角色模型与动画 Blueprint／Montage 使用相同骨架。不兼容素材先显式重定向，再填写引用。省略 AnimationClass 可以保留静态配置预览，动画和动作是否就绪由后续运行检查决定。武器挂点在阵容／运行组合时针对选定角色检查，不假定默认角色。

## 校验方式

选中目录资产，在 Details 中点击 **Validate Authoring**。在 Output Log 查看 **[ACT Authoring]** 开头的结果。Blueprint 和自动化也可调用 ValidateCatalog，读取 bValid／Errors。

校验器加载强类型引用，检查必需 ID／显示字段、非空 ACT 技能表、技能身份一致性、重复身份／绑定、已声明执行器／输入 ID、骨架兼容及特性／地形引用。空模板在填完之前有意返回失败。重跑配置脚本会保留已有资产。

TerrainRestrictions 指向已声明 TraitId 和显式 DisallowedTerrainIds，不禁止整个角色，也不决定战斗规则何时生效。这些语义属于已批准的 ACT 运行／阵容政策。

## 填写后能使用到哪一步

定义可以被配置目录以及后续 ACT 阵容／加载器读取。表行描述技能的位置与逻辑触发绑定，不会自行实现一种新动作。TASK-030 需要注册对应原生 GAS 能力执行器、加载所配内容并连接输入／动画／效果；TASK-035 提供战斗总设置和阵容校验。

引用目录中的 ID 是已声明配置词汇，不证明执行器已经安装，因此校验通过不表示 ACT 战斗已经可玩。伤害、费用、冷却、窗口、武器政策及所有未批准值仍留给用户确认。Encounter 配表不会作为 ACT 的后备表使用。

## 可复现的资产创建

仓库 Scripts/Editor/AuthorACTTemplates.py 通过 UE Editor Python 创建五份项目自有模板，保留已有资产，并检查空模板仍然未完成。若项目未启用 PythonScriptPlugin，只在该命令进程临时启用。不要把第三方内容复制进模板，填写用户已导入素材或项目包装资产的引用即可。

学习检查：只替换 Model，先预测 DefinitionId 与技能绑定是否变化，再检查资产。目前尚未提供用户学习证据。

## 场景预览接入

TASK-040 读取同一份 ACT 角色／武器模型字段和选定角色 SkillTable，不另存副本。角色 Mobility 是新增元数据，不代表允许沙盘部署。原生模型／场景预览见 [ModelAuthoring](ModelAuthoring.zh-CN.md)。ACT 尚无来源明确的说明／数值行结构，可读表行不等于已注册 GAS 动作。
