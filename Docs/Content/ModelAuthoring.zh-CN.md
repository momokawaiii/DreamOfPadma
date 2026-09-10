# 模型与地块场景配置

- 英文原文：ModelAuthoring.md
- 文档 ID：CONTENT-MODEL-AUTHORING-001
- 版本：0.2
- 状态：通用／ACT 模型预览；旧 Demo 来源已由 TASK-048 退役
- Owner：TASK-040

## UE 中已经可以使用的内容

在内容浏览器打开 `/Game/Padma/MVP/Presentation/ModelPreview/L_ModelAuthoringPreview`。选中 **Home - Model Authoring Preview**，在 Details 点击 **Rebuild Preview**，或者进入 PIE。专用预览 GameMode 使用固定斜俯视镜头，不生成玩家 Pawn。地图包含坐镇角色占位、建筑方块和地表平面。

运行时源码路径相对于 `Source/DreamOfPadma/{Public,Private}`：`Presentation/Models/PadmaModelPresentation.h/.cpp`、`Game/Presentation/PadmaPresentationCatalog.h/.cpp`、`Game/Presentation/PadmaNodePreview.h/.cpp`。TASK-047 移除源码的 `MVP/` 上层目录，同时保留既有预览内容资产包路径与夹具名称。见[运行时命名规范](../Architecture/RuntimeNaming.zh-CN.md)。

此预览专门用于模型／槽位配置。当前地图、移动及 Encounter／ACT 运行时见 [NativePlayableDemo](NativePlayableDemo.zh-CN.md)；地图编辑与 PCG 主题见 [WorldMapAuthoring](WorldMapAuthoring.zh-CN.md)。

| 同一 ModelPreview 文件夹中的资产 | 用途 |
|---|---|
| DA_Model_Character_Template | 可复制填写的空通用角色模型草稿 |
| DA_Model_Garrison_Placeholder | 通用驻军球体，不含技能来源 |
| DA_Model_Building_Placeholder | 使用引擎方块的通用静态模型定义 |
| DA_Model_Ground_Placeholder | 使用引擎平面的通用静态模型定义 |
| DA_ModelPreview_Catalog | 带领域的定义键与强类型来源软引用 |
| DA_NodeScene_Preview | 静态场景关卡引用、具名槽位与变换 |

坐镇占位使用 `/Game/Padma/MVP/Presentation/ModelPreview/DA_Model_Garrison_Placeholder`，键为 `Model / Visual.Preview.Garrison`，没有技能行。TASK-048 已退役旧 Demo 角色及三技能适配器；TASK-040 历史日志描述的是原样例。

## 填写模型与来源

为通用人物、建筑、地表或道具创建／复制 **PadmaModelDefinition** 数据资产。填写稳定的 VisualDefinitionId、DisplayName，且 StaticModel 与 SkeletalModel 必须只填一项。AnimationClass 可选，但必须与骨骼模型兼容；ModelTransform 只调整表现。两种模型字段都是软引用。

ACT 角色填写既有 **PadmaACTCharacterDefinition** 的 Model、可选 AnimationClass 和其自身 ACT SkillTable。武器使用 **PadmaACTWeaponDefinition**。强类型表行与技能资产按 [ACTAuthoring](ACTAuthoring.zh-CN.md) 填写。这些仍是 ACT 模型／技能的唯一来源，不在通用模型资产重复维护。

通用模型资产不保存技能。Game 将 Model、ACTCharacter 或 ACTWeapon 解析为中立表现描述，可复用模型 Actor 保持独立于战斗模式类型；DemoCharacter 已退役。

## 将选中身份绑定到场景槽位

你提出的“归处／坐镇位”在正式玩法中的链路是：

```text
世界坐镇状态：NodeId + EntityInstanceId + 领域／DefinitionId
    -> 目录条目：指向对应定义 Data Asset 的强类型软引用
    -> 选中定义自己的模型与技能来源
    -> 在 Garrison 具名槽位生成临时场景 Actor
```

世界状态和 SaveGame 保存稳定身份，目录保存资产软引用，场景 Actor 指针仅在显示期间存在，不作为玩法存档。切换坐镇角色将通过校验服务命令并产生事件；TASK-040 只实现这条链路末端的只读解析和预览。

在 **DA_ModelPreview_Catalog** 新增条目，Domain 与 DefinitionId 必须匹配引用来源。仅填写对应的 Model、ACTCharacter 或 ACTWeapon 字段。未知／重复键和多余来源均校验失败，解析不会自动选第一条兜底。

在 **DA_NodeScene_Preview** 中配置 Slots，每个槽位有唯一 SlotId 与变换。这里仅保存静态布局，不保存当前归属或坐镇者。

预览宿主的 PreviewBindings 将 SlotId 关联到唯一示例 EntityInstanceId 和带领域的定义键。SetPreviewSelection 仅供预览，切换定义时会清空此前的 SelectedSkillRow。修改 Details 后点击 Rebuild Preview。新选择无效时，旧模型／技能会清除，错误显示在 Diagnostics。布局无效时不会构建一半的候选场景。生成的表现 Actor 为临时对象，并在 PIE 中重建。

## 就绪状态与诊断

解析结果分别提供 VisualReady 和 SkillDetailsReady。模型可在技能表缺失时显示。ACT 表行结构错误、所选行不存在、技能身份不匹配、重复绑定或动画不兼容时会给出对应来源诊断。ACT 描述为空就保持为空，不回退读取 Encounter 文本或效果。

两项就绪标记都不代表 GAS 执行器已注册或战斗可玩。预览仅在主动重建／选择或 BeginPlay 时同步加载软引用；正式运行时流式加载和场景切换由后续集成任务负责。

## 移动性元数据

当前原生卡牌行及 ACT 角色定义包含 Mobility。确认策略后勾选 Configured，选择 Movable（可移动）、Immovable（不可移动）或 Conditional（条件移动）。条件移动必须填写 ConditionId，其他策略留空；未配置数据会校验失败。

原生卡牌／本局规则及独立 ACT 配置消费同一类型；该字段不让 ACT 角色变成沙盘部署卡。当前移动行为与数据限制见 [NativePlayableDemo](NativePlayableDemo.zh-CN.md)。

移动行为已确定：点击只查看；右侧“移动到此处”请求移动选中的卡牌实例。A 与全部附着 B 作为一组，逐张校验；任一卡失败都在移动／扣费前拒绝整组。当前值来自已接受的原生样例，不是最终平衡。首版敌方地块信息全部公开，FPS 留待未来。

## 复现与查看

编译 DreamOfPadmaEditor 后运行 `Scripts/Editor/AuthorModelPreview.py`。脚本创建缺失的通用模型预览资产、保留既有内容，并在重跑时检查已保存预览；不需要 TASK-008 fixture 或已退役的 Demo 配置脚本。日志沿用历史前缀 `[TASK-040]`。

自动化用例为 `DreamOfPadma.MVP.TASK040.ModelAndSkillResolution` 与 `DreamOfPadma.MVP.TASK040.NodeBindingReplacement`。构建、重跑、失败用例与真实 UE 渲染证据见任务完成报告。

## 一个小练习

先预测只替换模型软引用会发生什么：画面应改变，定义身份和其技能来源应保持不变。复制模板，指定导入模型，给新定义设置稳定身份，绑定到预览槽位并重建。再选择无效身份，检查旧内容是否清除。解释实例身份、定义身份、资产软引用和实时 Actor 指针的区别，最后把同样操作迁移到建筑槽位。

目前尚未收到用户的预测、实操或复述证据；自动化实现检查不能替代这些学习证据。
