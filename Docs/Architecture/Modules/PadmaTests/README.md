# PadmaTests Module Program Document

- Document ID: ARCH-MODULE-TESTS-001
- Version: 0.1
- Status: Planned test boundary; no standalone test module yet
- Canonical language: English for Agent consumption
- Chinese companion: README.zh-CN.md
- Owner: Test and Verification Agent
- Current implementation: no project-owned gameplay tests yet
- Parent architecture: ../../ProgramArchitecture.md and ../../DataDrivenArchitecture.md

## 1. Purpose

PadmaTests verifies rules, data contracts, runtime integration, save migration, deterministic reproduction, and project smoke behavior. Tests must use the same commands, rule services, and data definitions as production paths.

## 2. Responsibilities

- Pure calculation unit tests.
- Data/schema and validation fixtures.
- Command and event integration tests.
- Synthesis preview/execution parity tests.
- Combat, AI, world mutation, and outcome tests.
- Save/load and migration fixtures.
- Deterministic seed/replay tests.
- PIE and Development-package smoke tests.
- Performance and regression measurements when a baseline exists.
- Test evidence linked from task files and ProjectState.

## 3. Non-responsibilities

PadmaTests must not:

- Introduce a second fake rule implementation that can drift from runtime.
- Modify production assets during a test.
- Depend on UI clicks when a command-level test is sufficient.
- Treat a passing empty test queue as proof of gameplay correctness.

Visual tests are added only when a visual acceptance criterion exists.

## 4. Public contracts

- Test fixture profiles and stable seeds.
- Given/when/then command helpers.
- State snapshot and event assertion helpers.
- Data validation report assertions.
- Save fixture version and migration assertions.
- Reproduction bundle containing seed, definition IDs, commands, and expected result.

## 5. Data and runtime ownership

PadmaTests owns fixtures and test-only profiles. It consumes production definitions and services. Test assets must be clearly marked and must not silently enter a shipping content pool.

## 6. Dependencies and integration

Tests may depend on the public contracts of all runtime modules, but production modules must not depend on PadmaTests. Tests should prefer Core services and command paths over private implementation access.

## 7. Required initial test matrix

- Project starts and the editor target compiles.
- Calendar boundaries and resource accounting.
- Card candidate selection and success/failure.
- Synthesis failure has no reward; any retained Sigma path produces only an explicit non-reward consequence.
- World node movement, capture, mutation, and conditional route.
- Combat damage, status, ruler response, and war balance.
- Basic non-A card legality outside battle, on the player's eligible Encounter action turn, the one-card-per-turn limit, and ineligible windows.
- ABC-card active-ability activation from the sandbox without entering a battle.
- ACT RealTimeAction `Tab` repository transition, background blur, 1/10 world-time slowdown, movement/attack input rejection, card-only bullet-time input, inertia, five-visible-slot paging, and complete battle-loop command mapping.
- Encounter high-level action-bar conditions, priority, reactions, interrupts, extra actions, and non-turn windows; exact precedence tests wait for the deferred Encounter rules.
- The two confirmed MVP victory conditions and at least one defeat condition, plus generic resolution of a future outcome definition.
- Save/load round trip, complete pre-battle snapshot restore on defeat/exit, and commit on success.
- Same seed produces the same result.

## 8. Implementation stages

1. Add Core calculation fixtures before content production grows.
2. Add command-path automation for the first sandbox and synthesis.
3. Add Encounter action-timeline, card-activation, AI, outcome, and save tests.
4. Add the complete ACT RealTimeAction input/slot/battle-loop tests.
5. Add PIE/package smoke tests.
6. Add performance baselines and regression gates as the project becomes larger.

## 9. Learning targets and risks

Learning targets: Unreal Automation Framework, test design, deterministic simulation, serialization migration, profiling evidence, and CI thinking.

Main risk: writing tests that assert implementation details instead of player-visible rules and stable contracts.
