#pragma once

#include "buffer.h"

#include <memory>

class Consumer
{
public:
    explicit Consumer(std::shared_ptr<Buffer> inputBuf);

private:
    std::shared_ptr<Buffer> _buf;
};
