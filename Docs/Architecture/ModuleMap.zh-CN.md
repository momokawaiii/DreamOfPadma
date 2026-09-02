# 模块图

- 英文原文（Agent 阅读）：`Docs/Architecture/ModuleMap.md`

## 目标依赖方向

```text
PadmaCore
   ↑
PadmaGameplay    PadmaWorld    PadmaUI
       \            |            /
              PadmaGame
```

`PadmaGame` 是组合根。`PadmaCore` 包含规则和数据契约，不得依赖 UI、地图、Niagara 或具体玩法 Actor。

## 目标职责

### PadmaCore

日历、资源、卡牌定义、合成、带种子的随机流、命令、事件、结果类型和存档模式。

### PadmaGameplay

单位、战斗、能力、GAS、Buff、目标选择、AI 策略和战斗编排。

### PadmaWorld

六边形地图、节点状态、遭遇、世界变化、World Partition、PCG、天气和环境系统。

### PadmaUI

Widget、ViewModel、HUD、卡牌界面、地图界面和输入到命令的适配器。

### PadmaGame

GameMode、GameState、PlayerController、会话子系统、模式路由和跨模块组合。

### PadmaEditor 和 PadmaTests

仅编辑器资源/数据工具和自动化测试。它们不能成为运行时依赖。

## 分阶段采用

当前空白项目只有一个生成模块。不要为了看起来完整而创建空模块。先在目录和文档中建立边界，等某个边界拥有稳定负责人或独立测试/构建需求后，再拆成 UE 模块。

## 程序文档地图

- 项目级所有权和依赖规则：ProgramArchitecture.zh-CN.md
- 数据定义、资产类型、加载、校验和版本：DataDrivenArchitecture.zh-CN.md
- 架构 Agent 规则：AGENTS.zh-CN.md
- 模块 Agent 规则：Modules/AGENTS.zh-CN.md

每个逻辑模块都有一份技术 README：

- Modules/PadmaCore/README.zh-CN.md
- Modules/PadmaGame/README.zh-CN.md
- Modules/PadmaGameplay/README.zh-CN.md
- Modules/PadmaWorld/README.zh-CN.md
- Modules/PadmaUI/README.zh-CN.md
- Modules/PadmaEditor/README.zh-CN.md
- Modules/PadmaTests/README.zh-CN.md

README 是实现契约和所有权记录。没有代码时也可以先描述计划中的边界，但不能把 README 当成已经存在独立 UE 模块的证明。
