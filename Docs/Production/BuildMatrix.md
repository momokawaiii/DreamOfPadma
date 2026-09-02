# Build Matrix

- Chinese companion for user reading: `Docs/Production/BuildMatrix.zh-CN.md`

## Configurations

| Configuration | Purpose | GM commands | Debug symbols |
|---|---|---:|---:|
| DebugGame | Source-level learning and debugging | Yes | Yes |
| Development | Daily playable build | Yes, restricted | Optional |
| Test | Automation and CI | Yes, scripted | Yes |
| Shipping | Release candidate | No or minimal | No |

The reproducible entry points are under `Scripts/`. Build profiles are stored under `Build/Profiles/`; generated output belongs in ignored `Artifacts/`.
