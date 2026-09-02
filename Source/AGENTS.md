# Source Rules

- Chinese companion for user reading: `Source/AGENTS.zh-CN.md`

- Keep public interfaces and reflected data types small and stable.
- Use the dependency direction documented in `Docs/Architecture/ModuleMap.md`.
- Core rules must be testable without loading a map or creating a widget.
- Do not add cross-system global state to the generated `DreamOfPadma` module.
- New dependencies require an update to the module map and an ADR when they change a boundary.
- Prefer components, subsystems, services, and strategies over a universal Manager.
