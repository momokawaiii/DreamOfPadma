# GM 与开发命令

- 英文原文（Agent 阅读）：`Docs/Tools/GMCommands.md`

GM 命令是开发和测试接口。只要可能，就应使用与正常玩法相同的服务和命令路径。

## 命名

使用 `padma.` 前缀。Shipping 构建中禁用或限制这些命令。

## 初始命令集

```text
padma.help
padma.status
padma.dump.world
padma.dump.battle
padma.time.set_day <n>
padma.time.set_phase <Dawn|Morning|Dusk>
padma.time.advance_day
padma.resource.set <name> <value>
padma.resource.add <name> <value>
padma.card.add <card_id> [count]
padma.card.preview <card_a> <card_b>
padma.card.synthesize <card_a> <card_b> [seed]
padma.world.reveal_all
padma.world.goto <node_id>
padma.encounter.start <encounter_id>
padma.battle.spawn <card_id> <player|enemy> <slot>
padma.battle.mode <EncounterTurnBased|RealTimeAction>
padma.battle.dump_timeline
padma.battle.resolve_window <reaction|interrupt|extra_action>
padma.battle.end_turn
padma.battle.play_card <card_id> [target]
padma.battle.bullet_time <on|off>       # 调试强制；ACT MVP 玩家通过 Tab 进入
padma.battle.play_slot <1-5> [page]
padma.card.repository <open|close>      # ACT 打开时模拟 Tab 面板切换
padma.card.page <n|next|previous>       # 总页数和边界策略尚未固定
padma.card.ability <card_id> <ability_id> [target]
padma.battle.win
padma.battle.lose
padma.mode.set <Sandbox|EncounterTurnBased|RealTimeAction>
padma.save.save <slot>
padma.save.load <slot>
padma.save.list
padma.save.reset <slot>
```

## 安全要求

- 在 Development/Test 构建中记录命令、参数、种子和结果。
- 参数或 ID 无效时拒绝执行并显示用法。
- 在战斗外、玩家不符合条件的遭遇战行动回合、超过每回合一张限制，或缺少 ACT RealTimeAction 子弹时间/卡槽/分页场景时，拒绝基础非 A 卡打出。在精确遭遇战规则确认前，不得自行推断反应、打断或额外行动窗口拥有额外额度。
- ACT MVP 中，`Tab` 必须打开仓库、使背景虚化、划出五张卡、将世界时间降至 1/10，并禁止移动/攻击输入；状态激活期间只接受基础非 A 卡输入。调试版子弹时间命令可以强制进入该状态。
- 默认从 Sandbox 允许 ABC 卡主动技能命令，但只有能力的数据条件、特殊限制和消耗有效时才执行；不能自行推断战斗内合法性。
- 合成失败不得发放奖励。如果保留西格玛骰子，只记录其显式配置的非奖励失败后果。
- 提供种子时保证命令确定性。
- 删除/重置类命令必须受到仅开发环境可用的保护。
- 启用联网后，客户端不能绕过服务器权威。

有用的 Unreal 内置命令包括 `stat fps`、`stat unit`、`stat gpu`、`stat niagara`、`profilegpu` 和 `memreport`。
