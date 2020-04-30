#pragma once

#include "buffer.h"

#include <memory>

class Producer
{
public:
    explicit Producer(int argc, char* argv[],
                      std::shared_ptr<Buffer> outputBuf);

private:
    int _argc;
    char** _argv;
    std::shared_ptr<Buffer> _buf;
};
