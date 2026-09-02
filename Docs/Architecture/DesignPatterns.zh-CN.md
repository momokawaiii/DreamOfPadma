# 设计模式指导

- 英文原文（Agent 阅读）：`Docs/Architecture/DesignPatterns.md`

GoF 模式是描述局部变化的词汇，不是整个项目的固定模板。应先确定所有权、依赖方向、数据契约和测试，再选择模式。

## 推荐用途

| 模式 | 在 Padma 中的用途 |
|---|---|
| Command | 玩家行动、GM 命令、重放和未来网络请求。 |
| Strategy | 伤害规则、合成评分、AI 策略和目标选择。 |
| State | 日历阶段、遭遇战行动条、战略沙盘和 RealTimeAction 战斗模式。 |
| Observer | 状态变化通知 UI、剧情、音频和任务。 |
| Factory / Registry | 卡牌和能力表现创建。 |
| Adapter | 把沙盘、遭遇战和 RealTimeAction 输入/仓库交互转换为共享命令。 |
| Facade | `PadmaGame` 组合子系统，避免成为规则上帝对象。 |
| Composite | 数据定义的卡牌效果序列。 |

除非明确记录生命周期和所有权，避免新建 Singleton、Service Locator 或万能 Event Bus。UE Subsystem 是生命周期工具，不代表所有系统都可以无条件访问全部状态。
