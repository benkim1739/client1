# Project Summary

## 1. Project Overview
This project is a Qt-based client application for connecting to a server, sending control commands, and displaying RTSP/H.264 video streams on Ubuntu or WSL2.

## 2. Directory Structure
The repository follows this structure:

```text
project-root
├── agents
├── backlogs
├── src
├── include
├── tests
├── docker
├── docs
└── README.md
```

### Directory Responsibilities
- agents: agent definitions and handoff rules
- backlogs: product backlog and sprint backlog documents
- src: application source code
- include: public headers
- tests: unit and integration tests
- docker: container configuration
- docs: project documentation
- README.md: project overview and setup information

## 3. Agent Structure
The project uses the following agents:

- Scrum Master
  - defines product backlog items
  - selects sprint backlog items
  - coordinates the agent workflow
  - prepares sprint end reports
- Developer
  - implements sprint backlog items
  - writes or updates source code
  - runs unit tests and reports results
- Reviewer
  - reviews implementation quality
  - runs integration or acceptance tests
  - reports review findings
- Setup Manager
  - prepares the development environment for Ubuntu/WSL2

## 4. Scrum Development Process
The project follows an agent-only Scrum workflow:

1. The Scrum Master derives product backlog items from the requirements.
2. Backlog items are prioritized.
3. Sprint backlog items are selected for the current sprint.
4. The Developer agent implements the selected work.
5. Unit tests are executed and results are reported.
6. The Reviewer agent validates the implementation and runs integration or acceptance tests.
7. The Scrum Master prepares a sprint end report and requests confirmation to continue.
8. If problems occur, the product backlog is revised and the process restarts.

## 5. Current Implementation Status
Sprint 1 has been implemented and verified.

### Completed in Sprint 1
- basic Qt-based UI
- connect/disconnect state handling
- automated unit test coverage for state changes
- build verification through CMake and CTest

## 6. Development Environment
The project is intended to run on:
- Ubuntu / WSL2
- C++ with CMake
- Qt
- OpenCV
- vcpkg and Docker
- Google Test
- g++ and gdb
