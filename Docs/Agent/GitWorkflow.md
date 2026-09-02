# Git Workflow and GitHub Backup

- Chinese companion for user reading: `Docs/Agent/GitWorkflow.zh-CN.md`

## Core model

Git does not upload individual working files directly. The local sequence is:

```text
working files -> staged snapshot -> local commit -> remote branch on GitHub
```

New files and modified files use the same sequence. A green `U` in VS Code means an untracked file: it exists locally but has not been added to a commit. GitHub receives commits when `git push` runs.

## Safe upload cycle on `main`

Run these commands from PowerShell. Review the output after every block instead of pasting unrelated commands onto one line.

```powershell
Set-Location -LiteralPath 'E:\2026ue\Demo\DreamOfPadma'
git status --short --branch
```

Inspect existing tracked changes before staging:

```powershell
git diff
```

Stage only the intended new and modified paths. Add both Markdown language companions together:

```powershell
git add -- 'Docs/Agent/Workflow.md' 'Docs/Agent/Workflow.zh-CN.md'
```

For one already reviewed, tightly scoped documentation task, directories may be staged explicitly:

```powershell
git add -- '.codex' '.agents' 'Docs' 'AGENTS.md' 'AGENTS.zh-CN.md'
```

Do not use `git add .` without first checking the repository status; it can include unrelated or generated files. Never add `Binaries/`, `DerivedDataCache/`, `Intermediate/`, `Saved/`, `.vs/`, `.vscode/`, generated solution files, logs, or package output.

Review the exact staged snapshot:

```powershell
git diff --cached --stat
git diff --cached
```

Run the checks required by the task. For Markdown or repository-structure changes:

```powershell
& '.\Scripts\AuditDocs.ps1'
& '.\Scripts\ValidateProject.ps1' -Strict
git diff --check
```

Create one local commit for the coherent task:

```powershell
git commit -m 'docs: describe the change'
```

Verify the branch and push all local commits not yet on its upstream:

```powershell
git status --short --branch
git push
```

When `main` already tracks `origin/main`, plain `git push` is enough. It sends every unpushed commit on `main`, not just the newest file. A clean successful result ends with `main...origin/main` and no `ahead N` marker.

## New task branch or worktree

A Codex-managed worktree starts in detached `HEAD` by default; it does **not** automatically own a branch. Start the task from the intended integrated commit. Before work must be preserved as a normal branch or pushed, use **Create branch here** in the task header. Alternatively, use Handoff to move the task and its changes safely to the Local checkout.

After a branch has been created in the worktree, the Primary Agent commits there; it does not push or merge by default. Confirm that the command prints a branch name, then perform the first authorized push:

```powershell
git branch --show-current
git push -u origin <branch-name>
```

If `git branch --show-current` prints nothing, the worktree is still detached. Do not invent a push command from that state; create the branch in Codex first. Git permits one branch to be checked out in only one worktree, so do not try to check out that branch simultaneously in Local.

After review, the Integration Coordinator merges the approved commit in dependency order on the integration checkout, runs consolidated validation, creates the integration commit if needed, and then runs `git push` with user authorization.

Do not run `git init` again. Do not use force push, `git reset --hard`, or broad checkout/clean commands as routine conflict resolution.

Tracked project files are present in a new worktree, but ignored UE outputs and local setup files are not carried automatically. Expect the worktree to need its own generated/build state or a documented setup step; do not solve this by committing generated folders. Keep UE builds and Editor use in the single project validation lane.

## Intentional deletion or rename

For a tracked file that should be removed:

```powershell
git rm -- 'path\to\file'
```

For a tracked file that should be renamed:

```powershell
git mv -- 'old\path' 'new\path'
```

For Markdown, remove or rename its English and Chinese companion together, then run the documentation audit. UE assets need a dedicated migration task and redirector verification; do not move them with filesystem commands while the Editor is unaware.

## Correcting the staged set

If a file was staged accidentally, remove it from the staged snapshot without deleting the working file:

```powershell
git restore --staged -- 'path\to\file'
```

Then inspect `git status` again. If a change belongs to another Goal, leave it untouched and report it; do not absorb it into the current commit.

## Tags and milestone baselines

Normal task commits do not need tags. Create a tag only for an accepted milestone or release baseline, then push it explicitly:

```powershell
git tag -a 'mvp-slice-v0.2.0' -m 'Accepted MVP slice baseline'
git push origin 'mvp-slice-v0.2.0'
```

Never move or overwrite a published baseline tag. Use a new version.

## SSH key cache

The repository remote uses GitHub SSH. After a Windows login or ssh-agent restart, add the project key once for that session:

```powershell
Start-Service ssh-agent
ssh-add "$env:USERPROFILE\.ssh\id_ed25519_dreamofpadma"
ssh-add -l
```

After the identity is cached, `git fetch`, `git pull`, and `git push` should not ask for the same passphrase repeatedly. The project and key may be on different drives; Git passes the key path to SSH independently of the repository path.

## Before declaring backup complete

```powershell
git status --short --branch
git log -1 --oneline
git ls-remote --heads origin main
```

The local `main` commit and remote `main` commit should match. A local commit is recoverable from this disk; only the successful push provides the requested off-disk GitHub backup.
