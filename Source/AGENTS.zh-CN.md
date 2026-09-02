# Source 代码约定

- 英文原文（Agent 阅读）：`Source/AGENTS.md`

- 保持公共接口和反射数据类型小而稳定。
- 使用 `Docs/Architecture/ModuleMap.md` 中定义的依赖方向。
- 核心规则必须能够在不加载地图、不创建 Widget 的情况下测试。
- 不要把跨系统全局状态添加到生成的 `DreamOfPadma` 模块中。
- 新依赖需要更新模块图；改变架构边界时还需要 ADR。
- 优先使用组件、子系统、服务和策略，而不是万能 Manager。
- Agent 读取英文规则；修改英文规则时必须同步修改本文件。
