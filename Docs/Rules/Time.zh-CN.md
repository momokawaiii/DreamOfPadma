# 时间与日历规则

- 英文原文（Agent 阅读）：`Docs/Rules/Time.md`

本文档是日历的规范契约。

## 层级

```text
一天 = 黎明 -> 上午 -> 黄昏
七天 = 一个时代
七个时代 = 一个 Chapter
```

日历必须显式保存 `ChapterIndex`、`EraIndex`、`DayIndex` 和 `Phase`。不要在玩法代码中只用一个取模表达式推导全部层级。

## 状态变化要求

- 阶段变化是明确的事件。
- 日终效果与阶段效果分开。
- 时代变化发送 `EraChanged` 事件。
- Chapter 变化发送 `ChapterChanged` 事件。
- 测试覆盖第一天、第七天、下一个时代的第一天和下一个 Chapter 的第一天。
