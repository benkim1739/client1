# Client 요구사항 명세서

## 1. 개요
본 문서는 Client 애플리케이션의 요구사항을 정의한다.
Client는 서버에 제어 명령을 전송하고, 서버로부터 전송되는 비디오 스트리밍을 수신하여 사용자에게 표시하는 역할을 수행한다.

## 2. 기능 요구사항

### 2.1 서버 제어 명령 전송
Client는 서버에 다음과 같은 제어 명령을 전송할 수 있어야 한다.
- 접속
- 접속 해제
- 화면 제어
- Frame Rate 조절
- Resolution 조절

### 2.2 서버 접속 및 스트리밍 수신
Client는 서버에 접속한 후 다음 기능을 제공해야 한다.
- 서버로부터 비디오 스트리밍을 수신한다.
- 수신된 스트림을 화면에 표시한다.
- RTSP/H.264 기반의 영상 스트림을 처리한다.

### 2.3 오류 처리
Client는 동작 중 문제가 발생할 경우 다음을 수행해야 한다.
- 사용자에게 에러 메시지를 화면에 표시한다.
- 연결 실패, 스트림 오류, 제어 명령 실패 등 예외 상황을 적절히 처리한다.

## 3. 실행 환경 요구사항
- OS: Ubuntu
- Package/Deployment Tool: vcpkg, Docker
- Server Interface: RTSP, H.264
- UI Framework: QT

## 4. 개발 환경 요구사항
- OS: Ubuntu / WSL2
- IDE: VS Code, Copilot, Codex, Claude
- Programming Language: C++
- Build Tool: CMake
- Package/Deployment Tool: vcpkg, Docker
- Compiler: g++
- Debugging Tool: gdb
- Vision Library: OpenCV
- UI: QT
- Test Framework: Google Test

## 5. 비기능 요구사항
- 안정적인 서버 연결 및 연결 해제 처리가 가능해야 한다.
- 영상 스트리밍 재생 시 지연이 최소화되어야 한다.
- 사용자 인터페이스는 직관적이고 사용하기 쉬워야 한다.
- 오류 발생 시 명확한 메시지를 제공해야 한다.
