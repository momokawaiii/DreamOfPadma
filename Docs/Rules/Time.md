# Time and Calendar Rules

- Chinese companion for user reading: `Docs/Rules/Time.zh-CN.md`

This document is normative for the calendar.

## Hierarchy

```text
One day   = Dawn → Morning → Dusk
Seven days = one Era
Seven Eras = one Chapter
```

The calendar must store explicit `ChapterIndex`, `EraIndex`, `DayIndex`, and `Phase`. Do not derive all of them from a single modulo expression in gameplay code.

## State transition requirements

- Phase transitions are explicit events.
- Day-end effects are separate from phase effects.
- Era changes emit an `EraChanged` event.
- Chapter changes emit a `ChapterChanged` event.
- Tests cover the first day, the seventh day, the first day of the next Era, and the first day of the next Chapter.
