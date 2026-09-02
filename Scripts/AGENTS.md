# Script Rules

- Chinese companion for user reading: `Scripts/AGENTS.zh-CN.md`

- Scripts must be safe to run repeatedly.
- Scripts should fail with a clear message when required tools or paths are missing.
- Do not delete project or user data from a script without an explicit, guarded command.
- Build and test output belongs under ignored `Saved/` or `Artifacts/` paths.
- Prefer repository-relative paths and an explicit `-EngineRoot` parameter over machine-specific hard-coded paths.
- Any new script must have a short usage description in `README.md` or the relevant `Docs/` file.
- Run `AuditDocs.ps1` after Markdown changes and keep its result in the completion report.
