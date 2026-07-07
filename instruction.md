# Project Instruction

## 1. Project Purpose
This project is a Client application that connects to a server, sends control commands, receives RTSP/H.264 video streaming, and displays the stream to the user.

## 2. Mandatory Development Rules
- Target environment is Ubuntu or WSL2.
- Develop in C++.
- Build with CMake and prefer Ninja.
- Use g++ as the compiler and gdb for debugging.
- Use Qt for the UI.
- Use OpenCV for vision-related processing.
- Use vcpkg and Docker for package/deployment management.
- Use Google Test for unit testing.
- Follow the RTSP/H.264 streaming interface requirements.

## 3. Engineering Principles
- Keep the code modular: separate UI, control commands, networking, streaming, and error handling.
- Prefer clear interfaces and small responsibilities over large monolithic classes.
- Handle connection, stream, and command failures gracefully.
- Always show user-friendly error messages when runtime issues occur.
- Use RAII and safe resource management for sockets, streams, and UI resources.
- Prefer asynchronous or thread-safe handling for streaming and control operations.
- Avoid hard-coded secrets and environment-dependent values when possible.

## 4. Coding Standards
- Write readable and maintainable C++ code.
- Use descriptive names for classes, methods, and variables.
- Keep functions focused and avoid unnecessary complexity.
- Add comments only where behavior is non-obvious.
- Ensure code builds cleanly on Ubuntu/WSL2.
- Prefer standard C++ features and avoid platform-specific shortcuts unless necessary.

## 5. UI and User Experience Rules
- The UI should be intuitive and responsive.
- Provide clear controls for connect/disconnect, screen control, frame rate, and resolution.
- Display streaming state and errors clearly on the screen.
- Ensure the UI remains usable even when the server is unavailable.

## 6. Testing and Validation Rules
- Add or update tests whenever behavior changes.
- Validate network and streaming behavior with realistic scenarios.
- Verify that error cases are handled and reported properly.
- Before completion, confirm that the project still builds and relevant tests pass.

## 7. Documentation Template
When implementing a new feature or fixing a bug, include:
- Summary of the change
- Requirement or problem being addressed
- Files involved
- Validation steps
- Known limitations or follow-up items

## 8. Task Execution Template
For each implementation task, follow this structure:
1. Review the requirement.
2. Identify affected modules.
3. Implement the smallest correct change.
4. Add or update tests if needed.
5. Verify build and test results.
6. Document the outcome.

## 9. Project Directory Structure
The repository should follow this structure:

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
- agents: agent definitions and workflow instructions
- backlogs: product backlog and sprint planning documents
- src: application source code
- include: public headers and interface declarations
- tests: unit and integration tests
- docker: Dockerfiles and container-related configuration
- docs: design notes and additional documentation
- README.md: project overview and setup instructions

## 10. Default Response Style for Agents
- Be concise and practical.
- Prefer action-oriented steps.
- Mention any dependency or environment issue clearly.
- Do not claim completion without verification.
