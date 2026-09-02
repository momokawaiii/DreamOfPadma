# Production Roadmap

- Chinese companion for user reading: `Docs/Production/Roadmap.zh-CN.md`

## M0 Foundation

Git/LFS, documentation, Agent contract, validation scripts, clean compile, and PIE launch. The packaging entry point remains scaffolded, but executing and learning the Development packaging pipeline is deferred until after MVP completion.

## M1 Core rules

Calendar, resources, deterministic random streams, card definitions, synthesis service, preview parity, and tests.

## M2 Gameplay vertical slice

One condition-driven Encounter action timeline, one controllable unit, one basic non-A battle skill, one status effect, animation timing, Niagara feedback, and a basic HUD.

## M3 World vertical slice

Small hex map, node state, one world mutation, ABC-card-character-to-enemy-point battle entry, local-battle transaction entry/complete-snapshot commit/rollback, return to map, and SaveGame round trip.

## M4 Mode and presentation slice

Strategic/RTS-like sandbox plus one complete playable ACT RealTimeAction loop: clicking Tab opens the blurred five-card repository, sets world time to 1/10, rejects movement/attack input, and dispatches card slots. The slice also includes shared Tab mapping, five-visible-slot paging, camera transition, audio, post-processing, and performance capture. FPS and other modes remain extension targets; total page count is deferred.

## M5 Content and release

Asset Manager, content packs, validation, cook rules, platform profiles, the first Windows Development package verification, packaging study, and release notes.

Each milestone must be playable and leave behind tests, documentation, and a reproducible build.
