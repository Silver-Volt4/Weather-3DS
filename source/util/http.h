#pragma once

#include <queue>
#include <3ds.h>

class HttpRequestWorker;

class HttpRequest
{
    friend HttpRequestWorker;

public:
    enum State
    {
        CREATED,
        STARTED,
        DOWNLOADING,
        FINISHED,
    };

private:
    uint32_t downloadedBytes = 0;
    httpcContext context;
    HTTPC_RequestStatus contextStatus = HTTPC_RequestStatus::HTTPC_STATUS_REQUEST_IN_PROGRESS;
    HttpRequest(httpcContext context);
    ~HttpRequest();
    void poll();

public:
    uint32_t httpStatusCode = 0;
    State state = CREATED;
    uint8_t *result = nullptr;
};

class HttpRequestWorker
{
    HttpRequest *current = nullptr;
    std::queue<HttpRequest *> queue;
    void processCurrent();

public:
    void poll();
    HttpRequest *add(httpcContext context);
};