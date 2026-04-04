# API Endpoints

This document describes the available API endpoints for the `http-server`.
All endpoints are wrapped by the project's middleware.

---

## `GET /health`

Returns the current health status of the path-tracer and udp-server processes.

### Inputs

- **Method:** `GET`

- **Headers:** None required.

- **Body:** Empty.

### Outputs

- **Success (200 OK):**

  - **Content-Type:** `application/json`

  - **Body:** JSON object (mirrored from `state/status.json`).

- **Failure (500 Internal Server Error):**

  - **Content-Type:** `text/plain`

  - **Body:** JSON error message (e.g., "Failed to read status.json").

---

## `POST /submit-render`

Submits a scene description in JSON format to be queued for rendering.

### Inputs

- **Method:** `POST`

- **Headers:**
  - `Content-Type: application/json`

- **Body:** A valid JSON scene description, request ID (for bookeeping), and receiver port + IP.

### Outputs

- **Success (202 Accepted):** (Planned)

  - **Content-Type:** `application/json`

  - **Body:** Job ID and status.

- **Failure (400 Bad Request):**

  - **Content-Type:** `application/json`

  - **Body:** `{ "status": 400, "message": "Invalid JSON" }` (`check_json` middleware).

---
