# 存档模式

- 英文原文（Agent 阅读）：`Docs/Architecture/SaveSchema.md`

## 必须遵守的原则

- 保存稳定 ID，不保存 Actor 或 UObject 指针。
- 包含模式版本和迁移路径。
- 不把临时表现状态写入存档。
- 如果需要复现结果，保存随机种子或随机流状态。
- 在支持战斗中途存档前，先保证战斗快照格式稳定；最初可只在安全边界存档。

## 存档部分

```text
Metadata
  SchemaVersion, BuildVersion, SaveTime, RunId
Random
  MasterSeed, stream/event position
Calendar
  Chapter, Era, Day, Phase
Global
  SoulFlow, Entropy, Faith, BattleScale, flags
World
  Node states, mutations, ownership, discoveries, garrisons, event state
Player
  Position, cards, deck, warehouse, upgrades, unlocks
Encounter
  Encounter ID、交战路线、战斗模式、单位、稳定值、Buff、冷却，
  行动条、先攻、反应/打断窗口、额外行动状态，
  Encounter 当前行动，或明确支持的 ACT RealTimeAction 快照
BattleTransaction
  战前快照 ID、进入时世界哈希、提交/回滚状态，
  所有被战斗修改的本局存档部分的完整版本化副本
Story
  Choices, quests, character relations, unlocked events
EventHistory
  Ordered world-changing events or a compact history reference
```

## MVP 存档范围

第一版存档应支持日历状态、全局资源、当前节点、卡牌收藏、已完成局部战斗状态、剧情标记和随机种子。进入局部战斗时，创建一个完整的版本化战前快照，覆盖战斗可能修改的全部本局状态，包括世界变化、归属、资源、卡牌位置/生命周期、熵、信仰、威慑力、战争天平、剧情标记、事件历史和确定性随机流位置。胜利提交战斗结果；失败或玩家退出时精确恢复完整快照，不留下部分战斗消耗或状态。战斗中途存档可以等行动条和 ACT 快照格式稳定后再加入。

除非批准独立且确定性的 ACT 快照契约，存档不能序列化临时按键状态、子弹时间输入状态、镜头状态或动画进度。安全边界 MVP 应恢复到战斗前，或恢复到战斗完成后。

## 迁移

新字段应使用默认值追加。破坏性变化需要迁移函数，以及至少加载一个旧版本夹具的测试。
