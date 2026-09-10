# Build Matrix

- Chinese companion: [BuildMatrix.zh-CN.md](BuildMatrix.zh-CN.md)
- Status: Offline Demo acceptance target; packaged acceptance is still pending
- Scope: [ChapterZero](../Rules/ChapterZero.md)

| Configuration | Use | Required for this Demo |
|---|---|---|
| Editor Development / PIE | Native compilation, diagnostics and targeted automation | Yes; cannot substitute for packaged checks |
| Win64 Development | Internal offline installation and full-path debugging | Yes |
| Win64 Shipping | Offline release-quality installation and full-path acceptance | Yes |
| DebugGame / Test | Optional debugging or automation profiles where supported | No separate delivery requirement |

## Evidence required in both packages

- Build/cook/package logs identify build revision, content versions and configuration.
- Install or extract to a clean test location; start without Unreal Editor, project source paths, uncooked assets or Steam services. Installer technology is not yet selected.
- Start/menu/opening/Chapter Zero through Encounter, the single ACT battle, royal-court capture and ending are actually playable. This is the normal path, separate from testing the allowed tutorial skip.
- Reopen/continue, battle failure/exit, tutorial skip and deliberate replay behave consistently; same-run retry does not duplicate rewards.
- Baked map metadata matches MapKey and content loads without runtime PCG regeneration. This is target acceptance, not current implementation.
- UI/input/focus, asset references, readable text and clean shutdown work outside PIE. The intended target hardware/resolution and performance budgets need definition before numerical performance signoff.
- Final golden-path values are approved production values. Public release assets replace reverse-engineered/prototype content according to the user's asset boundary.
- Record actual failures and remaining user visual/playtest acceptance; never report “packaged verified” from only a build or automation exit code.

Use existing Scripts entry points and Build/Profiles where applicable; inspect their current capability before execution. Output/evidence belongs in Artifacts, not generated source folders. No packaging ran in TASK-053.

Steam integration/submission, online tests, cloud saves and a hand-controller certification requirement are outside the currently accepted scope. Future shipping diagnostics/symbol policy must be explicitly configured rather than inferred from a configuration name.
