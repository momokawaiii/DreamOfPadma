# TASK-001 Foundation Verification

- Chinese companion for user reading: `Docs/Production/Tasks/TASK-001-Foundation.zh-CN.md`

## Goal

Verify that the new repository can be validated, compiled, and launched in PIE on the development machine before adding game rules. Packaging is deferred until after MVP completion by product-owner decision.

## Read first

- `AGENTS.md`
- `Docs/ProjectState.md`
- `Docs/Architecture/ModuleMap.md`
- `Docs/Production/BuildMatrix.md`

## Allowed paths

- `Docs/ProjectState.md`
- `Build/Profiles/`
- `Scripts/`

## Out of scope

- Gameplay implementation
- Old prototype migration
- Bulk asset import
- Engine version upgrade
- Windows Development packaging

## Acceptance criteria

- [x] `Scripts/ValidateProject.ps1` passes.
- [x] Git LFS is installed and active for the repository.
- [x] The blank project compiles in the recorded UE version.
- [ ] PIE launches the default map.
- [x] Windows Development packaging is explicitly deferred until after MVP completion and is not a TASK-001 gate.
- [x] No generated folders are staged.

## Learning targets

- Git and Git LFS
- UE project and target files
- Build.cs and editor build entry points
- Reproducible development workflows

Unreal Automation, cooking, staging, packaging, and standalone-build verification are post-MVP learning targets.

## Completion report

Record commands, engine path, build result, PIE result, and any environment-specific blocker in `Docs/ProjectState.md`.
