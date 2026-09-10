# Time and Calendar Rules

- Chinese companion for user reading: `Docs/Rules/Time.zh-CN.md`

This document is normative for the calendar.

## Hierarchy

```text
One day   = Dawn → Morning → Dusk
Seven days = one Era
Seven Eras = one Chapter
```

Morning is displayed as 晌午 in the Chinese prototype, matching the user's terminology.

The calendar must store explicit `ChapterIndex`, `EraIndex`, `DayIndex`, and `Phase`. Do not derive all of them from a single modulo expression in gameplay code.

## State transition requirements

- Phase transitions are explicit events.
- On entry into Dusk, an opted-in TASK-051 tutorial run permits one bounded ruler wilderness frontier step. Invalid, battle or dialogue advances do not execute it. See [WorldState](WorldState.md).
- On entry into Morning, apply each deployed A's tendencies to its attached B fields once for that day (user accepted 2026-09-09).
- Day-end effects are separate from phase effects.
- Era changes emit an `EraChanged` event.
- Chapter changes emit a `ChapterChanged` event.
- Tests cover the first day, the seventh day, the first day of the next Era, and the first day of the next Chapter.
