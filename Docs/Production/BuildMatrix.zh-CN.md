# 构建矩阵

- 英文原文（Agent 阅读）：`Docs/Production/BuildMatrix.md`

## 配置

| 配置 | 用途 | GM 命令 | 调试符号 |
|---|---|---:|---:|
| DebugGame | 源码级学习和调试 | 是 | 是 |
| Development | 日常可玩构建 | 是，受限 | 可选 |
| Test | 自动化和 CI | 是，脚本化 | 是 |
| Shipping | 发布候选版本 | 否或最少 | 否 |

可复现的入口脚本位于 `Scripts/`。构建配置档位于 `Build/Profiles/`；生成输出放入被忽略的 `Artifacts/`。
