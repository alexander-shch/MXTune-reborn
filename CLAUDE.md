# MXTune — Claude Code Instructions

## GitHub Project

All work must stay in sync with the GitHub Project at:
**https://github.com/users/alexander-shch/projects/7**

### Rules

1. **Before starting a plan:** Set the corresponding project item to `In Progress`.

2. **When creating a branch:** Create a PR as soon as the branch is pushed (even if not ready to merge), update its description to reference the plan number and plan file (e.g. `plans/05_DEV_TOOLING.md`), and add the PR to the project.

3. **When a PR is open:** Remove the draft project item for that plan (if one exists) — the PR item replaces it.

4. **When a PR is merged:** Set the project item to `Done`. Update `plans/README.md` to mark the plan as Done with the merge date. Update the plan's own markdown file with delivery notes.

5. **Never push directly to `master`** — all work goes through a feature branch and PR, regardless of what the plan doc says.

6. **Project item statuses:**
   - `Todo` — not started
   - `In Progress` — branch exists and/or PR is open
   - `Done` — PR merged to master

### Workflow summary

```
checkout -b feature/<plan-name>
→ do the work
→ push branch
→ gh pr create (title = "Plan NN — Name", body references plan file)
→ gh project item-add (add PR to project #7)
→ set project item status to In Progress
→ (CI passes, PR reviewed)
→ merge PR
→ set project item to Done
→ update plans/README.md + plan markdown file
```

---

## Commits

- No co-author tag in commit messages.
- Always run a local macOS build before committing.
- Never use `--no-verify`.

## General

- Never suggest workarounds or hacks — do it the right way even if harder.
- Keep responses concise — no trailing summaries after completing a task.
