# Product Backlog

This file stores the prioritized product backlog for the Client project.

## Product Backlog Items
1. Connect to server
   - Priority: High
   - Acceptance Criteria: User can connect successfully and see connection state.
2. Disconnect from server
   - Priority: High
   - Acceptance Criteria: User can disconnect cleanly and resources are released.
3. Send server control commands
   - Priority: High
   - Acceptance Criteria: Connect, disconnect, screen control, frame rate, and resolution commands are supported.
4. Receive and display video stream
   - Priority: High
   - Acceptance Criteria: RTSP/H.264 stream is displayed in the UI.
5. Handle stream and connection errors
   - Priority: Medium
   - Acceptance Criteria: Error messages are shown and recovery is possible.
6. Build Qt-based user interface
   - Priority: High
   - Acceptance Criteria: Main controls and stream view are available.
7. Configure frame rate and resolution
   - Priority: Medium
   - Acceptance Criteria: User can change stream parameters and commands are applied.
8. Prepare development environment
   - Priority: High
   - Acceptance Criteria: Ubuntu/WSL2 tools, Qt, OpenCV, CMake, vcpkg, Docker, and gdb are available.
9. Add automated tests
   - Priority: Medium
   - Acceptance Criteria: Unit and integration tests are set up and runnable.
10. Document setup and project usage
    - Priority: Medium
    - Acceptance Criteria: Setup instructions and project overview are documented.

## Sprint Selection Rule
At the start of each sprint, the Scrum Master selects the highest-priority backlog items that can be completed within the sprint scope and hands them off to the Developer agent for implementation.
