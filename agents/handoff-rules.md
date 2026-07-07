# Agent Handoff Rules

## Purpose
This document defines the handoff protocol between the Scrum Master, Developer, and Reviewer agents during the Scrum workflow.

## 1. Handoff Principles
- Every work item must be passed with clear context, expected outcome, and acceptance criteria.
- Handoffs must be explicit, complete, and traceable.
- No handoff should rely on implicit assumptions.
- Each agent must report status, results, and blockers clearly.

## 2. Scrum Master -> Developer Handoff
When the Scrum Master selects a sprint backlog item, the handoff must include:
- Sprint backlog item title
- Requirement summary
- Acceptance criteria
- Relevant constraints from REQUIREMENTS.md
- Expected deliverables
- Expected test scope

The Developer agent must respond with:
- Implementation plan
- Files to be changed
- Unit test plan
- Estimated completion status

## 3. Developer -> Scrum Master Handoff
After implementation and unit testing, the Developer agent must report:
- What was implemented
- Which tests were executed
- Test pass/fail results
- Any open issues or blockers
- Whether the sprint item is ready for review

## 4. Scrum Master -> Reviewer Handoff
When the Scrum Master sends the implementation for review, the handoff must include:
- Implementation summary
- Source files and test results
- Acceptance criteria
- Expected review scope

The Reviewer agent must respond with:
- Review findings
- Integration or acceptance test results
- Remaining issues
- Final recommendation

## 5. Reviewer -> Scrum Master Handoff
After review, the Reviewer agent must report:
- Whether the item passed review
- Any defects or concerns
- Whether further implementation is needed
- Whether the sprint item is ready to close

## 6. Failure Handling
If a handoff reveals a blocker, the Scrum Master must:
- Update the product backlog if the requirement has changed
- Reassign or re-plan the sprint backlog item
- Restart the cycle from backlog refinement if necessary
