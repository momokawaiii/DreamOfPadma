# Git 工作流与 GitHub 备份

- 英文原文（Agent 阅读）：`Docs/Agent/GitWorkflow.md`

## 核心模型

Git 不是直接把某一个工作文件上传，而是遵循：

```text
工作文件 -> 暂存快照 -> 本地提交 -> GitHub 远端分支
```

新文件和修改过的文件使用相同流程。VS Code 中绿色 `U` 表示未跟踪文件：文件只存在于本地，还没有加入提交。运行 `git push` 时，GitHub 接收的是提交。

## `main` 上的安全上传闭环

在 PowerShell 中运行以下命令。每组命令后先检查输出，不要把互不相关的命令粘在同一行。

```powershell
Set-Location -LiteralPath 'E:\2026ue\Demo\DreamOfPadma'
git status --short --branch
```

暂存前查看已有跟踪文件的变化：

```powershell
git diff
```

只暂存本次需要的新文件和修改文件。Markdown 中英文配对必须同时添加：

```powershell
git add -- 'Docs/Agent/Workflow.md' 'Docs/Agent/Workflow.zh-CN.md'
```

对于已经检查过、范围严格的一次文档任务，可以显式暂存这些目录：

```powershell
git add -- '.codex' '.agents' 'Docs' 'AGENTS.md' 'AGENTS.zh-CN.md'
```

没有先检查仓库状态时，不要使用 `git add .`，否则可能混入无关修改或生成文件。永远不要添加 `Binaries/`、`DerivedDataCache/`、`Intermediate/`、`Saved/`、`.vs/`、`.vscode/`、生成的解决方案、日志或打包产物。

检查即将提交的确切快照：

```powershell
git diff --cached --stat
git diff --cached
```

运行任务要求的检查。Markdown 或仓库结构发生变化时运行：

```powershell
& '.\Scripts\AuditDocs.ps1'
& '.\Scripts\ValidateProject.ps1' -Strict
git diff --check
```

为这次完整、单一范围的任务创建本地提交：

```powershell
git commit -m 'docs: describe the change'
```

确认分支，然后把该分支尚未上传的所有本地提交推送出去：

```powershell
git status --short --branch
git push
```

当 `main` 已经跟踪 `origin/main` 时，普通 `git push` 就够了。它会发送 `main` 上所有尚未推送的提交，并非只上传最新文件。完全同步后，状态显示 `main...origin/main`，不再有 `ahead N`。

## 新任务分支或 worktree

Codex 管理的 worktree 默认从 detached `HEAD` 开始，**不会**自动拥有分支。创建任务时应从需要的已集成提交开始。当工作需要以普通分支长期保存或推送时，先在任务标题栏使用 **Create branch here**。也可以使用 Handoff，把任务及修改安全移动到 Local checkout。

在 worktree 中创建分支后，Primary Agent 才在该分支提交，默认不负责推送或合并。确认下面第一条命令能输出分支名，然后执行第一次获得授权的推送：

```powershell
git branch --show-current
git push -u origin <branch-name>
```

如果 `git branch --show-current` 没有输出，worktree 仍处于 detached 状态。不要从该状态猜测推送命令，应先在 Codex 中创建分支。Git 只允许一个分支同时被一个 worktree checkout，因此不要同时在 Local 中 checkout 该分支。

Review 通过后，集成协调者在集成 checkout 中按依赖顺序合并批准的提交，运行集成验证，需要时创建集成提交，然后在获得用户授权后运行 `git push`。

不要再次运行 `git init`。不要把强制推送、`git reset --hard` 或大范围 checkout/clean 当作常规冲突解决方式。

新 worktree 会包含已跟踪项目文件，但被忽略的 UE 输出和本地配置不会自动携带。应预期 worktree 需要自己的生成/构建状态或文档化初始化步骤，不能通过提交生成目录解决。同一时间仍只使用一条 UE 构建与 Editor 验证通道。

## 有意删除或重命名

删除一个已跟踪文件：

```powershell
git rm -- 'path\to\file'
```

重命名一个已跟踪文件：

```powershell
git mv -- 'old\path' 'new\path'
```

Markdown 的英文和中文镜像必须一同删除或重命名，然后运行文档审计。UE 资源需要专门迁移任务和重定向器验证；不要在 Editor 不知情时用文件系统命令移动。

## 修正暂存集合

如果误暂存一个文件，在不删除工作文件的情况下把它移出暂存快照：

```powershell
git restore --staged -- 'path\to\file'
```

然后重新检查 `git status`。如果修改属于另一个 Goal，就保持不动并报告，不能塞进当前提交。

## 标签与里程碑基线

普通任务提交不需要标签。只有验收后的里程碑或发布基线才创建标签，并显式推送：

```powershell
git tag -a 'mvp-slice-v0.2.0' -m 'Accepted MVP slice baseline'
git push origin 'mvp-slice-v0.2.0'
```

已经发布的基线标签不得移动或覆盖，应创建新版本。

## SSH 密钥缓存

仓库远端使用 GitHub SSH。Windows 重新登录或 ssh-agent 重启后，为该会话添加一次项目密钥：

```powershell
Start-Service ssh-agent
ssh-add "$env:USERPROFILE\.ssh\id_ed25519_dreamofpadma"
ssh-add -l
```

身份进入缓存后，`git fetch`、`git pull` 和 `git push` 不应反复要求同一个口令。项目和密钥可以位于不同盘符；Git 会把密钥路径独立交给 SSH，与仓库路径无关。

## 宣布备份完成前

```powershell
git status --short --branch
git log -1 --oneline
git ls-remote --heads origin main
```

本地 `main` 提交和远端 `main` 提交应一致。本地提交只能防止工作区误操作；只有成功推送才提供所需的异盘 GitHub 备份。
