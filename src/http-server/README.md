# HTTP-SERVER

## About

A simple HTTP server set up via cpp-httplib to provide basic health
checks + an endpoint to send scene data to be rendered by the path-tracer process.

## Endpoints

- `GET /health`: a basic health check of the http-server process

- `POST /submit-render`: parse + validate scene data, and send to path-tracer
