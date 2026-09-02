# TASK-001 Foundation Verification

- Chinese companion for user reading: `Docs/Production/Tasks/TASK-001-Foundation.zh-CN.md`

## Goal

Verify that the new repository is reproducible on the development machine before adding game rules.

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

## Acceptance criteria

- [ ] `Scripts/ValidateProject.ps1` passes.
- [ ] Git LFS is installed and active for the repository.
- [ ] The blank project compiles in the recorded UE version.
- [ ] PIE launches the default map.
- [ ] A Windows Development package completes or the exact blocker is recorded.
- [ ] No generated folders are staged.

## Learning targets

- Git and Git LFS
- UE project and target files
- Build.cs and Unreal Automation entry points
- Reproducible development workflows

## Completion report

Record commands, engine path, build result, package location, and any environment-specific blocker in `Docs/ProjectState.md`.
