#pragma once

#include <queue>
#include <memory>
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
    void poll();

public:
    HttpRequest(httpcContext context);
    ~HttpRequest();

    uint32_t httpStatusCode = 0;
    State state = CREATED;
    uint8_t *result = nullptr;
};

class HttpRequestWorker
{
    std::shared_ptr<HttpRequest> current = nullptr;
    std::queue<std::shared_ptr<HttpRequest>> queue;
    void processCurrent();

public:
    void poll();
    std::shared_ptr<HttpRequest> add(httpcContext context);
};