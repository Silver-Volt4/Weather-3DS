#include "http.h"

#include <3ds.h>
#include <stdio.h>
#include <array>

HttpRequest::HttpRequest(httpcContext context) : context(context)
{
}

HttpRequest::~HttpRequest()
{
    delete result;
}

void HttpRequestWorker::processCurrent()
{
    if (current->state == HttpRequest::State::CREATED)
    {
        httpcBeginRequest(&current->context);
        current->state = HttpRequest::State::STARTED;
        return;
    }

    if (current->state == HttpRequest::State::STARTED)
    {
        HTTPC_RequestStatus reqStatus;
        httpcGetRequestState(&current->context, &reqStatus);

        if (reqStatus == HTTPC_STATUS_DOWNLOAD_READY)
        {
            httpcGetResponseStatusCode(&current->context, &current->httpStatusCode);
            uint32_t contentsize;
            httpcGetDownloadSizeState(&current->context, nullptr, &contentsize);
            current->result = new uint8_t[contentsize + 1]; // +1 for \0
            printf("state: downloading");
            current->state = HttpRequest::State::DOWNLOADING;
        }

        return;
    }

    if (current->state == HttpRequest::State::DOWNLOADING)
    {
        uint32_t downloadedSize = 0;
        httpcDownloadData(&current->context, current->result + current->downloadedBytes, 0x10, &downloadedSize);

        current->downloadedBytes += downloadedSize;

        if (downloadedSize == 0)
        {
            current->state = HttpRequest::State::FINISHED;
            httpcCloseContext(&current->context);
            current->result[current->downloadedBytes] = '\0';
            current = nullptr;
        }

        return;
    }
}

void HttpRequestWorker::poll()
{
    if (current)
    {
        processCurrent();
        return;
    }
    else if (!queue.empty())
    {
        current = queue.front();
        queue.pop();
        processCurrent();
    }
}

HttpRequest *HttpRequestWorker::add(httpcContext context)
{
    auto c = new HttpRequest(context);
    queue.push(c);
    return c;
}