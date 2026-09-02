# Design Pattern Guidance

- Chinese companion for user reading: `Docs/Architecture/DesignPatterns.zh-CN.md`

GoF patterns are a vocabulary for local variation, not a template for the whole project. First establish ownership, dependency direction, data contracts, and tests.

## Preferred uses

| Pattern | Padma use |
|---|---|
| Command | Player actions, GM commands, replay, future network requests |
| Strategy | Damage rules, synthesis scoring, AI policy, target selection |
| State | Calendar phase, Encounter action timeline, strategic sandbox, and RealTimeAction battle mode |
| Observer | State changes notifying UI, story, audio, and quests |
| Factory / Registry | Card and ability presentation creation |
| Adapter | Convert sandbox, Encounter, and RealTimeAction input/repository interactions into shared commands |
| Facade | `PadmaGame` composes subsystems without becoming a rules god-object |
| Composite | Data-defined sequences of card effects |

Avoid a new Singleton, Service Locator, or universal Event Bus unless its lifetime and ownership are documented. UE Subsystems are lifecycle tools, not permission to access all state from everywhere.
