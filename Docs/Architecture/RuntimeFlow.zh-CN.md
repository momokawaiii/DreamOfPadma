# 运行时流程

- 英文原文（Agent 阅读）：`Docs/Architecture/RuntimeFlow.md`

项目把输入、规则、状态和表现分开。战略沙盘、回合制遭遇战和 RealTimeAction 战斗路线，都进入同一条类型化命令路径：

```text
沙盘战略输入
          \
遭遇战行动条输入  -----> PlayerCommand
          /                    ↓
ACT RealTimeAction 输入   模式和合法性门
                                  ↓
                           规则或应用服务
                                  ↓
                               结果 / 事件
                                  ↓
                          世界状态 / 玩法状态
                                  ↓
                   UI / 动画 / VFX / 音频 / 镜头
```

命令示例：

- `RequestMoveToNode`
- `RequestEnterEncounter`
- `RequestEndEncounterTurn`
- `RequestPlayBasicBattleCard`
- `RequestActivateABCAbility`
- `RequestEnterBulletTime`
- `RequestPlayBattleCardSlot`
- `RequestOpenCardRepository`
- `RequestChangeBattleCardPage`
- `RequestResolveReactionOrInterrupt`
- `RequestChangeControlMode`
- `RequestSaveGame`

正常玩家输入、GM 命令、自动化测试和未来网络请求，都应能使用同一条命令路径。

模式和合法性门是权威校验层：基础非 A 卡在沙盘、遭遇战不属于玩家符合条件的行动回合、超过每个玩家行动回合一张的限制，或 ACT RealTimeAction 缺少子弹时间和数字卡槽输入场景时，必须被拒绝。ACT MVP 中点击 `Tab` 会打开卡牌仓库、使背景虚化、划出五张卡，并将世界时间降至 1/10。该状态禁止移动和攻击输入，只接受基础非 A 卡相关输入，战斗场景在减速和保持惯性的状态下继续运行。反应、打断或额外行动窗口是否额外给予卡牌使用额度，随精确遭遇战规则留空。ABC 卡主动技能默认接受战斗外 Sandbox 发动路径，但必须满足数据配置的条件、限制和消耗。

Encounter 和 ACT 使用同一个卡牌仓库：按下 `Tab` 唤起仓库，当前页面提供 1—5 卡槽，鼠标滚轮请求下一页，Encounter 手牌与 RealTimeAction 卡槽一一对应。ACT MVP 中打开仓库还会触发背景虚化和子弹时间切换。总页数和页面边界行为暂时留空。这些是输入/视图规则，权威服务仍需校验具体卡牌操作。

当 ABC 卡控制的角色或单位移动到敌方停留的点位时，路线解析器会同时评估敌我双方的路线约束。解析后的路线可以选择或触发 Encounter、ACT、未来的 RealTimeAction 表现模式以及相关剧情事件；这属于玩法状态切换，而不只是镜头变化。

进入任何局部战斗都会创建包含所有被战斗修改的本局状态的完整版本化战前快照，并暂停沙盘时间。战斗胜利时提交结果；失败或玩家退出时精确恢复该快照，再通过类型化结果把控制权交还沙盘。

## 运行时所有权

| 状态 | 建议所有者 |
|---|---|
| 卡牌定义 | Asset Manager / 卡牌目录子系统 |
| 当前流程和跨关卡状态 | GameInstance 子系统 |
| 当前世界状态 | WorldState 子系统 |
| 当前局部战斗、行动条和模式状态 | Battle Director / GameState |
| 本地玩家输入 | PlayerController |
| 本地 UI | UI 子系统 / ViewModel |
| 磁盘持久化 | Save 子系统 / `USaveGame` |
| 未来网络权威 | Server GameState 和服务器侧服务 |
