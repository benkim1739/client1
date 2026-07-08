# Sprint 2 End Report

## Sprint Information
- Sprint Name: Sprint 2
- Status: Completed
- Date: 2026-07-08

## Summary
- Completed backlog items:
  - 서버 제어 REST API 클라이언트 모듈 구현 (Server Control REST API Client Module)
  - RTSP/H.264 비디오 수신 및 OpenCV 기반 UI 렌더링 스레드 구현 (RTSP/H.264 Video Streaming & OpenCV UI Rendering Thread)
- Blocked backlog items: None
- Key results:
  - Implemented the `RestClient` module utilizing `QNetworkAccessManager` to handle connections, disconnection, FPS, and resolution settings according to the specification.
  - Implemented the `VideoReceiverThread` running OpenCV `cv::VideoCapture` with a 3s frame timeout and exponential backoff reconnection (3s, 1.5x multiplier, max 60s).
  - Redesigned `MainWindow` with a high-fidelity dark-themed stylesheet (QSS), server controls, a 640x360 video viewport, and a styled logging console.
  - Developed `MockHttpServer` (over `QTcpServer`) and `MockVideoSource` (over `VideoSource` interface) to run isolated unit/integration tests.
  - Rebuilt the workspace under WSL/Ubuntu and verified that 100% of tests pass (3 tests run and passed in 521 ms total).

## Agent Outcomes
- Implementation result: Completed
- Review result: Passed (verified code modularity, thread safety, memory leak prevention, and UI style alignment)
- Test result: Passed (verified connection lifecycle, status updates, settings propagation, and streaming error paths)

## Issues and Risks
- Problems encountered:
  - Linker vtable errors occurred due to Mock headers not being scanned by AUTOMOC; resolved by adding the mock headers to `add_executable(client1_tests)` in `CMakeLists.txt`.
- Recommended actions:
  - Continue integration tests against the Java Spring Boot streaming server in Sprint 3.

## Decision
- Proceed to next sprint: Yes
- Confirmation requested from user: Yes
