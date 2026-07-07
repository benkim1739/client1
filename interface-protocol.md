# Interface Protocol Specification

## Overview
This document defines the detailed communication protocols and interfaces for the Remote Control and Video Streaming Server System. It covers both video streaming (RTSP) and control channels (TCP JSON/REST API).

---

## 1. RTSP Video Streaming Interface

### 1.1 Connection Details
- **Protocol**: RTSP (Real Time Streaming Protocol)
- **Port**: 8554
- **Base Path**: /camera
- **URL Format**: `rtsp://{server-ip}:8554/camera`
- **Example**: `rtsp://192.168.1.100:8554/camera`

### 1.2 Video Specifications
- **Default Configuration**
  - Resolution: 1920x1080 (Full HD)
  - Frame Rate: 30 FPS
  - Codec: H.264

- **Supported Resolutions**
  - 1280x720 (HD)
  - 1920x1080 (Full HD)
  - 3840x2160 (4K)

- **Supported Codecs**
  - H.264 (Primary)
  - MJPEG (Motion JPEG)

- **Supported Frame Rates**
  - 24 FPS
  - 30 FPS (Default)
  - 60 FPS

### 1.3 Latency Requirements
- **Maximum Latency**: 500ms or less
- **Recommended Latency**: 200ms or less

### 1.4 RTSP Connection Lifecycle
```
Client                          Server (MediaMTX)
  |                                  |
  |-------- RTSP SETUP ------>        |
  |<------- Response (200) -----------|
  |                                  |
  |-------- RTSP PLAY ------>        |
  |<------- Response (200) -----------|
  |                                  |
  |<====== Video Stream RTP ===========|
  |        (H.264/MJPEG)             |
  |                                  |
  |-------- RTSP PAUSE ------>        |
  |<------- Response (200) -----------|
  |                                  |
  |-------- RTSP TEARDOWN ----->      |
  |<------- Response (200) -----------|
```

---

## 2. Control Channel Interface (TCP JSON/REST API)

### 2.1 Communication Details
- **Protocol**: TCP with JSON payload
- **Alternative**: REST API over HTTP
- **Default Port**: 9090 (for control channel)
- **Content-Type**: application/json
- **Encoding**: UTF-8

### 2.2 Command Structure

#### Command Format
```json
{
  "command_id": "unique-command-id",
  "command_type": "COMMAND_TYPE",
  "timestamp": "2026-07-07T17:17:09Z",
  "parameters": {
    "key1": "value1",
    "key2": "value2"
  }
}
```

#### Response Format
```json
{
  "command_id": "unique-command-id",
  "status": "SUCCESS|FAILURE|PENDING",
  "timestamp": "2026-07-07T17:17:09Z",
  "result": {
    "message": "Operation completed successfully",
    "data": {}
  },
  "error": null
}
```

#### Error Response Format
```json
{
  "command_id": "unique-command-id",
  "status": "FAILURE",
  "timestamp": "2026-07-07T17:17:09Z",
  "result": null,
  "error": {
    "code": "ERROR_CODE",
    "message": "Error description",
    "details": {}
  }
}
```

### 2.3 Available Commands

#### 2.3.1 Connection Management

##### CONNECT Command
**Purpose**: Establish client connection and create session

**Request**:
```json
{
  "command_id": "conn-001",
  "command_type": "CONNECT",
  "timestamp": "2026-07-07T17:17:09Z",
  "parameters": {
    "client_id": "client-001",
    "client_name": "Remote Control Client"
  }
}
```

**Response**:
```json
{
  "command_id": "conn-001",
  "status": "SUCCESS",
  "timestamp": "2026-07-07T17:17:09Z",
  "result": {
    "message": "Client connected successfully",
    "data": {
      "session_id": "sess-abc123",
      "client_id": "client-001",
      "rtsp_url": "rtsp://192.168.1.100:8554/camera",
      "connected_at": "2026-07-07T17:17:09Z"
    }
  },
  "error": null
}
```

##### DISCONNECT Command
**Purpose**: Disconnect client and cleanup resources

**Request**:
```json
{
  "command_id": "disc-001",
  "command_type": "DISCONNECT",
  "timestamp": "2026-07-07T17:17:09Z",
  "parameters": {
    "session_id": "sess-abc123",
    "client_id": "client-001"
  }
}
```

**Response**:
```json
{
  "command_id": "disc-001",
  "status": "SUCCESS",
  "timestamp": "2026-07-07T17:17:09Z",
  "result": {
    "message": "Client disconnected successfully",
    "data": {
      "session_id": "sess-abc123",
      "disconnected_at": "2026-07-07T17:17:10Z"
    }
  },
  "error": null
}
```

#### 2.3.2 Display Control Commands

##### SET_FRAME_RATE Command
**Purpose**: Adjust video frame rate

**Request**:
```json
{
  "command_id": "fps-001",
  "command_type": "SET_FRAME_RATE",
  "timestamp": "2026-07-07T17:17:09Z",
  "parameters": {
    "session_id": "sess-abc123",
    "fps": 30
  }
}
```

**Supported Values**: 24, 30, 60

**Response**:
```json
{
  "command_id": "fps-001",
  "status": "SUCCESS",
  "timestamp": "2026-07-07T17:17:09Z",
  "result": {
    "message": "Frame rate updated successfully",
    "data": {
      "session_id": "sess-abc123",
      "frame_rate": 30,
      "applied_at": "2026-07-07T17:17:10Z"
    }
  },
  "error": null
}
```

##### SET_RESOLUTION Command
**Purpose**: Adjust video resolution

**Request**:
```json
{
  "command_id": "res-001",
  "command_type": "SET_RESOLUTION",
  "timestamp": "2026-07-07T17:17:09Z",
  "parameters": {
    "session_id": "sess-abc123",
    "width": 1920,
    "height": 1080
  }
}
```

**Supported Resolutions**:
- 1280x720 (HD)
- 1920x1080 (Full HD)
- 3840x2160 (4K)

**Response**:
```json
{
  "command_id": "res-001",
  "status": "SUCCESS",
  "timestamp": "2026-07-07T17:17:09Z",
  "result": {
    "message": "Resolution updated successfully",
    "data": {
      "session_id": "sess-abc123",
      "resolution": {
        "width": 1920,
        "height": 1080
      },
      "applied_at": "2026-07-07T17:17:10Z"
    }
  },
  "error": null
}
```

#### 2.3.3 Status and Monitoring Commands

##### GET_STATUS Command
**Purpose**: Retrieve current session and stream status

**Request**:
```json
{
  "command_id": "stat-001",
  "command_type": "GET_STATUS",
  "timestamp": "2026-07-07T17:17:09Z",
  "parameters": {
    "session_id": "sess-abc123"
  }
}
```

**Response**:
```json
{
  "command_id": "stat-001",
  "status": "SUCCESS",
  "timestamp": "2026-07-07T17:17:09Z",
  "result": {
    "message": "Status retrieved successfully",
    "data": {
      "session_id": "sess-abc123",
      "client_id": "client-001",
      "connection_status": "CONNECTED",
      "stream_status": "STREAMING",
      "current_resolution": {
        "width": 1920,
        "height": 1080
      },
      "current_fps": 30,
      "current_codec": "H.264",
      "uptime_seconds": 3600,
      "frame_count": 108000
    }
  },
  "error": null
}
```

---

## 3. Error Handling Protocol

### 3.1 Error Codes

| Error Code | Description | HTTP Status | Recovery Action |
|-----------|-------------|------------|-----------------|
| INVALID_COMMAND | Unknown or malformed command | 400 | Retry with correct command |
| SESSION_NOT_FOUND | Session ID does not exist | 404 | Reconnect with CONNECT |
| INVALID_PARAMETER | Parameter validation failed | 400 | Check parameter values |
| UNSUPPORTED_RESOLUTION | Requested resolution not supported | 400 | Use supported resolution |
| UNSUPPORTED_FPS | Requested FPS not supported | 400 | Use supported FPS (24, 30, 60) |
| RTSP_CONNECTION_FAILED | Cannot establish RTSP connection | 503 | Retry with exponential backoff |
| STREAM_INTERRUPTION | Video stream interrupted | 503 | Auto-reconnect or manual retry |
| FRAME_TIMEOUT | Frame reception timeout | 504 | Check network, auto-reconnect |
| CODEC_ERROR | Video codec error | 500 | Fallback codec selection |
| INTERNAL_SERVER_ERROR | Unexpected server error | 500 | Retry operation |
| SERVER_BUSY | Server at capacity | 429 | Retry after delay |

### 3.2 RTSP Client Detection Requirements

Clients must handle and detect:
- RTSP connection failure
- Stream interruption
- Frame timeout
- Codec error
- Auto-reconnection

### 3.3 Reconnection Policy

**Reconnection Strategy**: Automatic retry with exponential backoff
- **Initial Interval**: 3 seconds
- **Backoff Multiplier**: 1.5x
- **Maximum Interval**: 60 seconds
- **Maximum Retries**: Unlimited (or configurable)

**Reconnection Sequence**:
```
Attempt 1: Wait 3s   → Retry
Attempt 2: Wait 4.5s → Retry
Attempt 3: Wait 6.75s → Retry
Attempt 4: Wait 10.1s → Retry
...
(up to 60s maximum)
```

### 3.4 Error Notification

#### Server-to-Client Error Notification
```json
{
  "notification_type": "ERROR",
  "timestamp": "2026-07-07T17:17:09Z",
  "severity": "ERROR|WARNING|INFO",
  "error": {
    "code": "ERROR_CODE",
    "message": "Error description",
    "affected_session": "sess-abc123",
    "recommended_action": "RECONNECT|RETRY|FALLBACK|CONTACT_ADMIN"
  }
}
```

#### Error Display Destinations
- **Admin UI**: Display all errors with detailed information
- **Client**: Send error notifications with recovery suggestions
- **Logs**: Log all errors with timestamp, context, and stack trace

---

## 4. REST API Endpoints (Alternative to TCP JSON)

### 4.1 Base Configuration
- **Base URL**: `http://{server-ip}:9091/api/v1`
- **Authentication**: Bearer token (future implementation)
- **Rate Limiting**: 1000 requests/hour per client

### 4.2 Endpoints

#### POST /sessions/connect
Create new client session

**Request Body**:
```json
{
  "client_id": "client-001",
  "client_name": "Remote Control Client"
}
```

**Response**: 200 OK
```json
{
  "session_id": "sess-abc123",
  "client_id": "client-001",
  "rtsp_url": "rtsp://192.168.1.100:8554/camera",
  "connected_at": "2026-07-07T17:17:09Z"
}
```

#### POST /sessions/{session_id}/disconnect
Terminate client session

**Response**: 200 OK
```json
{
  "session_id": "sess-abc123",
  "disconnected_at": "2026-07-07T17:17:10Z"
}
```

#### POST /sessions/{session_id}/frame-rate
Update frame rate

**Request Body**:
```json
{
  "fps": 30
}
```

**Response**: 200 OK
```json
{
  "session_id": "sess-abc123",
  "frame_rate": 30,
  "applied_at": "2026-07-07T17:17:10Z"
}
```

#### POST /sessions/{session_id}/resolution
Update resolution

**Request Body**:
```json
{
  "width": 1920,
  "height": 1080
}
```

**Response**: 200 OK
```json
{
  "session_id": "sess-abc123",
  "resolution": {
    "width": 1920,
    "height": 1080
  },
  "applied_at": "2026-07-07T17:17:10Z"
}
```

#### GET /sessions/{session_id}/status
Retrieve session and stream status

**Response**: 200 OK
```json
{
  "session_id": "sess-abc123",
  "client_id": "client-001",
  "connection_status": "CONNECTED",
  "stream_status": "STREAMING",
  "current_resolution": {
    "width": 1920,
    "height": 1080
  },
  "current_fps": 30,
  "current_codec": "H.264",
  "uptime_seconds": 3600,
  "frame_count": 108000
}
```

#### GET /health
Server health check

**Response**: 200 OK
```json
{
  "status": "HEALTHY",
  "timestamp": "2026-07-07T17:17:09Z",
  "services": {
    "rtsp_server": "UP",
    "control_channel": "UP",
    "database": "UP"
  }
}
```

---

## 5. WebSocket Interface (Optional Real-time Streaming)

### 5.1 Connection
- **Protocol**: WebSocket (ws:// or wss://)
- **URL**: `ws://{server-ip}:9092/stream`
- **Upgrade**: HTTP to WebSocket

### 5.2 Message Types

#### Subscribe to Stream Events
```json
{
  "type": "SUBSCRIBE",
  "session_id": "sess-abc123",
  "events": ["STATUS_CHANGED", "ERROR", "STREAM_METRICS"]
}
```

#### Receive Stream Events
```json
{
  "type": "STATUS_CHANGED",
  "session_id": "sess-abc123",
  "data": {
    "connection_status": "CONNECTED",
    "stream_status": "STREAMING"
  }
}
```

#### Stream Metrics
```json
{
  "type": "STREAM_METRICS",
  "session_id": "sess-abc123",
  "data": {
    "current_bitrate": 5000000,
    "frames_received": 108000,
    "frames_dropped": 5,
    "latency_ms": 150,
    "packet_loss_percentage": 0.002
  }
}
```

---

## 6. Communication Flow Examples

### 6.1 Complete Session Flow

```
CLIENT                              SERVER
   |                                  |
   |------ CONNECT ---->              |
   |<----- SESSION_ID ----------------| (TCP JSON)
   |                                  |
   |--- SUBSCRIBE to WebSocket --->   | (optional)
   |<--- WebSocket Connected ---------|
   |                                  |
   |------ RTSP SETUP ------>         | (MediaMTX)
   |<----- RTSP OK ----------|         |
   |                        |         |
   |------ RTSP PLAY ------>          |
   |<----- RTSP OK ----------|         |
   |                        |         |
   |<==== Video RTP Stream ====|      |
   |     (H.264/MJPEG)       |       |
   |                        |         |
   |---- SET_FRAME_RATE ---> |        | (TCP JSON)
   |<--- SUCCESS ------------|        |
   |                        |         |
   |<==== Updated Stream ====|        |
   |                        |         |
   |---- SET_RESOLUTION --->  |       | (TCP JSON)
   |<--- SUCCESS ------------|        |
   |                        |         |
   |<==== Updated Stream ====|        |
   |                        |         |
   |---- GET_STATUS ------>  |        | (TCP JSON)
   |<--- STATUS ------------|        |
   |                        |         |
   |------ DISCONNECT ----->  |       | (TCP JSON)
   |<----- SUCCESS ----------|        |
   |                        |         |
   |------ RTSP TEARDOWN --->         | (MediaMTX)
   |<----- RTSP OK ----------|         |
```

### 6.2 Error Recovery Flow

```
CLIENT                              SERVER
   |                                  |
   |---- RTSP SETUP ------>           |
   |<!!!! Connection Error !!!!        |
   |                                  |
   | [Wait 3 seconds]                 |
   |                                  |
   |---- RTSP SETUP ------>           |
   |<!!!! Connection Error !!!!        |
   |                                  |
   | [Wait 4.5 seconds]               |
   |                                  |
   |---- RTSP SETUP ------>           |
   |<----- RTSP OK --------|           |
   |                       |           |
   |--- RTSP PLAY -------> |           |
   |<----- RTSP OK --------|           |
   |                       |           |
   |<==== Video Stream ====|           |
```

---

## 7. Performance and Timeout Parameters

### 7.1 Timeout Values

| Parameter | Value | Description |
|-----------|-------|-------------|
| RTSP Setup Timeout | 5s | Time to establish RTSP connection |
| RTSP Play Timeout | 5s | Time to start receiving stream |
| Command Response Timeout | 10s | TCP JSON command response wait |
| Frame Timeout | 3s | Maximum time between consecutive frames |
| Session Idle Timeout | 300s | Session inactivity timeout |
| Reconnection Timeout | 60s | Maximum backoff before reset |

### 7.2 Performance Targets

| Metric | Target |
|--------|--------|
| Connection Establishment | < 1s |
| Command Processing | < 100ms |
| Stream Latency | 200ms (recommended), 500ms (max) |
| Resolution Change Latency | < 2s |
| Frame Rate Change Latency | < 1s |
| Throughput (Full HD @ 30fps) | ~5 Mbps (H.264) |

---

## 8. Security Considerations (Future Implementation)

- **Authentication**: Bearer token validation
- **Authorization**: Session-based access control
- **Encryption**: TLS/SSL for control channels
- **RTSP Security**: RTSP authentication (optional)
- **Rate Limiting**: Per-client request throttling
- **Input Validation**: Strict command parameter validation
- **Logging**: Audit trail for all operations

---

## 9. Documentation Version

- **Document Version**: 1.0
- **Last Updated**: 2026-07-07
- **Status**: Draft
- **Based on**: requirements.md v2.0

