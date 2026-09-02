---
name: padma-learning-loop
description: Turn a Dream of Padma development task into a novice-friendly learning cycle with scoped concepts, prediction, hands-on practice, observable evidence, teach-back, and transfer. Use whenever a task includes learning targets or the user asks to learn the module being built.
---

# Padma Learning Loop

Teach through the project's current artifact. Agent completion is not evidence that the user learned the topic.

## Build the learning contract

1. Read the task's Goal, acceptance criteria, learning targets, and relevant English module documentation.
2. Select at most two primary concepts for one task. Put prerequisites first and defer unrelated topics.
3. Define evidence for each target before implementation. Evidence must be observable: a prediction, diagram, inspected debugger state, user-executed command, controlled data edit, test interpretation, or teach-back.

## Run the loop

1. **Orient:** explain why the concept exists in this project and where it sits in the runtime flow.
2. **Predict:** ask the learner what should happen before showing the result.
3. **Observe:** point to the smallest relevant file, UE panel, log, or test.
4. **Practice:** give one safe action the learner performs personally.
5. **Verify:** compare actual evidence with the prediction and explain discrepancies.
6. **Teach back:** ask the learner to explain the concept and one failure mode in their own words.
7. **Transfer:** ask how the same concept would apply to another Padma module or later feature.

## Record evidence

In the task completion report, separate:

- artifact evidence produced by the Agent;
- hands-on evidence produced by the user;
- concepts demonstrated;
- misconceptions or unanswered questions;
- the next prerequisite or transfer exercise.

Do not expand feature scope to create a lesson. Do not silently perform the learner's exercise. Mark a target `Introduced`, `Practiced`, or `Demonstrated`; reserve `Demonstrated` for successful teach-back plus observable evidence.
