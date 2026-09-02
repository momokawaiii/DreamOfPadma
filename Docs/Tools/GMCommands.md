# GM and Development Commands

- Chinese companion for user reading: `Docs/Tools/GMCommands.zh-CN.md`

GM commands are development and test interfaces. They must use the same services and command path as normal gameplay whenever possible.

## Naming

Use the `padma.` prefix. Commands are disabled or restricted in Shipping builds.

## Initial command set

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
padma.battle.bullet_time <on|off>       # debug override; player entry is Tab in ACT MVP
padma.battle.play_slot <1-5> [page]
padma.card.repository <open|close>      # ACT open mirrors the Tab panel transition
padma.card.page <n|next|previous>       # page count and boundary policy are not fixed yet
padma.card.ability <card_id> <ability_id> [target]
padma.battle.win
padma.battle.lose
padma.mode.set <Sandbox|EncounterTurnBased|RealTimeAction>
padma.save.save <slot>
padma.save.load <slot>
padma.save.list
padma.save.reset <slot>
```

## Safety requirements

- Log the command, parameters, seed, and result in Development/Test builds.
- Reject invalid IDs and arguments with usage help.
- Reject basic non-A card play outside battle, outside the player's eligible Encounter action turn, after the one-card-per-turn limit, or without the ACT RealTimeAction bullet-time/slot/page context. Do not infer an extra allowance for reaction, interrupt, or extra-action windows before the exact Encounter rules are confirmed.
- In the ACT MVP, `Tab` must open the repository, blur the background, reveal five cards, set world time to 1/10, and reject movement/attack input; only basic non-A card input is accepted while the state is active. The debug bullet-time command may force the state for testing.
- Allow ABC-card active-ability commands from the sandbox by default only when the ability's data-defined conditions, restrictions, and cost are valid; do not infer battle legality.
- A failed synthesis must not grant a reward. If Sigma dice remain, log only their explicitly configured non-reward consequence.
- Make commands deterministic when a seed is supplied.
- Protect destructive commands with a development-only guard.
- Never let a client bypass server authority when networking is enabled.

Useful Unreal built-ins include `stat fps`, `stat unit`, `stat gpu`, `stat niagara`, `profilegpu`, and `memreport`.
