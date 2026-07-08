# Sprint 2 Backlog

## Goal
Implement the server control REST API client module and RTSP/H.264 video stream receiver with OpenCV-based UI rendering thread, integrated with a premium dark-themed interface.

## Selected Backlog Items
1. 서버 제어 REST API 클라이언트 모듈 구현 (Server Control REST API Client Module)
   - Priority: High
   - Type: Feature
   - Acceptance Criteria:
     - Establish connection/disconnect session, update frame rate and resolution, and query status via REST HTTP endpoints.
     - Parse server JSON responses correctly.
2. RTSP/H.264 비디오 수신 및 OpenCV 기반 UI 렌더링 스레드 구현 (RTSP/H.264 Video Streaming & OpenCV Rendering Thread)
   - Priority: High
   - Type: Feature
   - Acceptance Criteria:
     - OpenCV-based frame reception in a separate background thread.
     - Automatic reconnection with exponential backoff on stream interruption or 3s frame timeout.
     - Safe conversion and rendering of video frames to a Qt UI viewport.

## Acceptance Criteria
- REST API Client correctly implements requests to:
  - `/api/v1/sessions/connect`
  - `/api/v1/sessions/{session_id}/disconnect`
  - `/api/v1/sessions/{session_id}/frame-rate`
  - `/api/v1/sessions/{session_id}/resolution`
  - `/api/v1/sessions/{session_id}/status`
- Video receiver thread parses frames asynchronously without blocking the main UI thread.
- Reconnection policy (initial 3s, 1.5x multiplier, max 60s) works as specified in [interface-protocol.md](file:///E:/prj/client1/interface-protocol.md).
- Qt UI contains input fields for Server IP, controls for settings, a live streaming viewport, and log console with a modern dark theme.
- All core functions are fully covered by unit tests.
