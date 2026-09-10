# 程序架构

- 文档 ID：ARCH-PROGRAM-001
- 版本：0.4
- 状态：边界已接受；区分当前实现与离线 Demo 目标
- 英文原文：[ProgramArchitecture.md](ProgramArchitecture.md)
- 负责人：主程 / 架构
- 决策来源：[ADR-0010](../Decisions/ADR-0010-Offline-Demo-Content-and-Map.zh-CN.md)

## 权威与阅读入口

下一交付是 [ChapterZero](../Rules/ChapterZero.zh-CN.md) 定义的离线第零章 Demo，不要求完成全部完整 MVP 待办。规则文档拥有游戏语义，本文拥有依赖和运行时职责。旧的已接受规则继续有效，除非用户的新决定明确修改。

| 问题 | 唯一技术主文档 |
|---|---|
| 代码位置与当前实现 | [ModuleMap](ModuleMap.zh-CN.md)、[ProjectState](../ProjectState.zh-CN.md) |
| 定义字段、表、资产与校验 | [DataDrivenArchitecture](DataDrivenArchitecture.zh-CN.md) |
| 启动、剧情、教程与战斗转换 | [RuntimeFlow](RuntimeFlow.zh-CN.md) |
| 冻结地图、局内状态、恢复与兼容 | [SaveSchema](SaveSchema.zh-CN.md) |
| 地图制作与后续烘焙管线 | [WorldMapAuthoring](../Content/WorldMapAuthoring.zh-CN.md) |
| Demo 顺序与发行证据 | [Roadmap](../Production/Roadmap.zh-CN.md)、[BuildMatrix](../Production/BuildMatrix.zh-CN.md) |
| 范围变化与历史决定 | [ADR-0010](../Decisions/ADR-0010-Offline-Demo-Content-and-Map.zh-CN.md)、各 TASK |

文档区分 **当前**（已有代码）、**目标**（已接受、待实现）和 **暂缓/待定**。目标字段名在结构实现及迁移评审前均为概念名称。

## 依赖与职责

UE5.8 项目只有一个运行时模块 `DreamOfPadma`。Core、Game、Gameplay、World、UI 和 Presentation 是已有目录边界，并非独立编译模块。不要为了文档对称创建空模块。

`Game` 组装应用。Core 拥有值契约和规则结算；Gameplay、World、UI 通过适配器消费这些契约。当前单模块构建并不能强制隔离全部逻辑依赖。

| 归属 | 职责 | 边界 |
|---|---|---|
| Core | 稳定 ID、不可变值定义、可变规则状态、日历/资源/卡牌/合成、图合法性与确定性计算 | 不依赖 UMG、Niagara、PCG 执行、具体 Actor、地图加载或 UGameplayStatics |
| Game | 定义加载、局/会话生命周期、模式切图、事务/存档协调；后续章节协调 | 组装服务，不做万能规则 Manager |
| Gameplay | 独立的 Encounter 与 ACT GAS 配置、验证、执行、时钟和局部战斗状态 | 不强制共用角色集合、技能行或活跃 GAS 状态 |
| World | 地形、道路、节点/模型投影、视觉主题与装饰 PCG | 不决定通行、占领、剧情分支或奖励 |
| UI / Presentation | 输入转命令、只读视图、CommonUI 激活、材质/动效/镜头/音频播放 | Widget 和 LevelSequence 不拥有结算 |
| Editor / Tests | 制作、校验、预览；通过正式契约测试 | 编辑器图和工具对象不能成为 Shipping 依赖 |

模块职责与路径见 [ModuleMap](ModuleMap.zh-CN.md)。结构性依赖、公开结构、持久化或导入方式变化需要 ADR，并按 [Workflow](../Agent/Workflow.zh-CN.md) 做聚焦评审。

## 命令与状态契约

`输入 → 类型化命令 → 权威校验/结算 → 结果/事件 → 表现`

- 定义 ID、玩家持有的卡牌实例 ID、战斗实体 ID 是不同身份。显示名称、坐标和资产路径不是存档身份。
- 命令声明输入、失败原因与受影响状态。被拒绝的命令不能部分扣费或修改状态。
- 静态定义、冻结地图生成结果、可变局内状态具有不同生命周期。不能通过修改 DataAsset 表示当前 HP、占领或教程进度。
- 随机结果声明 Seed/随机流/位置。预览不消耗真实随机流，装饰生成不影响玩法结果。
- 存档恢复在安全边界操作值数据。战斗快照覆盖所有可能被战斗修改的局内字段；成功只提交一次，失败/退出完整恢复。
- 未来网络可复用命令和值边界；联网、服务端实现和回放播放不属于 Demo 交付。

## 模式与表现约束

[ADR-0004](../Decisions/ADR-0004-Separate-Encounter-and-ACT-GAS.zh-CN.md) 拥有 Encounter/ACT 隔离约定。回合制地图单位提供地点/上下文，ACT 使用独立选择的角色/武器阵容。基础非 A 技能可共享身份/槽位，效果、消耗和时钟仍由各模式拥有。限制应作用于指明的特性/武器/技能，不得隐式排除整个角色。

[ADR-0008](../Decisions/ADR-0008-Activatable-Presentation-Layers.zh-CN.md) 拥有页面/浮层激活与输入规则。当前原生 UI 是 CommonUI 加大量 Slate 组合，并非完成的设计师 UMG 皮肤。单击固定地图详情，悬浮仅提供视觉反馈。信息检视只读；表现播放结束不能证明规则动作成功。

当前绘景教程和独立连续地形/PCG 路径都是原型。目标使用固定锚点/显式边，以及编辑器烘焙的算法地形与装饰 PCG；渲染替换和获批美术资产仍待完成。[ADR-0010](../Decisions/ADR-0010-Offline-Demo-Content-and-Map.zh-CN.md) 记录优先级，不重标现有存档，也不宣称已经支持烘焙。

## 完成边界

PIE 和定向自动化只能证明其实际覆盖的范围。可发行质量的离线 Demo 还需要真实黄金路径、获批生产数值、表现验收、稳定恢复，以及 [BuildMatrix](../Production/BuildMatrix.zh-CN.md) 中的两种打包配置。现有临时数值与美术仅是开发证据，不代表最终内容获批。Slate 剧情编辑器及图编译器暂缓，期间可用普通类型化资产/表提供必要运行时数据。
