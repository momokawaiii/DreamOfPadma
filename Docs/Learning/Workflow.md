# Development Learning Workflow

- Chinese companion: Workflow.zh-CN.md

Development and learning are separate outcomes. The default is delivery with a brief explanation. A user request for reasons needs an explanation, not an exam or a mandatory exercise.

## Select the depth

- Explanation: explain the relevant decision and tradeoff using the actual changed artifact.
- Guided practice: when requested, choose at most two concepts, give one prediction and one safe user-performed exercise, then compare observations and ask a focused teach-back.
- Deeper lesson: add prerequisites or a transfer exercise only when useful to the user's selected topic.

Do not activate a lesson simply because a legacy TASK has learning fields. Use known user experience; do not repeat completed basics or require new proof before continuing development. Ask one learning question at a time, and do not perform the learner's exercise for them.

## Evidence

Record user-reported learning as user-reported. Agent explanations can establish Introduced; user practice supports Practiced; successful teach-back plus observable transfer supports Demonstrated. Missing mastery evidence does not block feature delivery unless the user explicitly makes it an acceptance criterion.

Keep the artifact/check result in the TASK. Add learning evidence there only for an actual lesson; do not duplicate progress across every document or update the curriculum for routine fixes.

Examples: change one Data Asset reference and observe the model; explain why a SaveGame stores IDs; compare one bounded child prompt with a full-history prompt. Use existing project artifacts; do not expand feature scope to create a lesson.
