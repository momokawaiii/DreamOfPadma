# Runtime Flow

- Chinese companion for user reading: `Docs/Architecture/RuntimeFlow.zh-CN.md`

The project separates input, rules, state, and presentation. The strategic sandbox, turn-based Encounter, and RealTimeAction battle routes all enter the same typed command path:

```text
Sandbox strategic input
          \
Encounter action-timeline input  -----> PlayerCommand
          /                         ↓
ACT RealTimeAction input      Mode and legality gate
                                      ↓
                               Rule or Application Service
                                      ↓
                                   Result / Event
                                      ↓
                              WorldState / GameplayState
                                      ↓
                       UI / animation / VFX / audio / camera
```

Examples of commands:

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

The same command path should be usable by normal player input, GM commands, automated tests, and future network requests.

The mode and legality gate is authoritative. It rejects a basic non-A card in the sandbox, outside an eligible player action turn in Encounter, after the one-card-per-player-action-turn limit, or in ACT RealTimeAction without the required bullet-time and numeric-slot input context. In the ACT MVP, clicking `Tab` opens the card repository, blurs the background, reveals five cards, and sets world time to 1/10. During that state, movement and attack input are rejected and only basic non-A card-related input is accepted, while the battle scene continues with slowed time and inertia. Whether reaction, interrupt, or extra-action windows provide a separate card allowance remains open with the exact Encounter rules. It accepts the outside-battle Sandbox activation path for ABC-card active abilities by default when their data-defined conditions, restrictions, and costs are valid.

Encounter and ACT use the same card repository: `Tab` opens it, the current page exposes slots 1-5, the mouse wheel requests the next page, and the Encounter hand maps one-to-one to the RealTimeAction slots. In the ACT MVP, opening it also performs the background blur and bullet-time transition. The total page count and page-boundary behavior are intentionally deferred. These are input/view rules; the authoritative services still validate the card operation.

When an ABC-card-controlled character or unit moves onto a point where an enemy is staying, the route resolver evaluates both sides' route constraints. The resolved route can select or trigger an Encounter, ACT, a future RealTimeAction presentation, and related story events; this is a gameplay transition, not merely a camera change.

Entering any local battle creates a complete versioned pre-battle snapshot of all battle-mutated run state and pauses sandbox time. A successful battle commits its result. Defeat or player exit restores that snapshot exactly before the typed result returns control to the sandbox.

## Runtime ownership

| State | Suggested owner |
|---|---|
| Card definitions | Asset Manager / card catalog subsystem |
| Current run and cross-level state | GameInstance subsystem |
| Current world state | WorldState subsystem |
| Current local battle, action timeline, and mode state | Battle Director / GameState |
| Local player input | PlayerController |
| Local UI | UI subsystem / view models |
| Disk persistence | Save subsystem / USaveGame |
| Network authority later | Server GameState and server-side services |
