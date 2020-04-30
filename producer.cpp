#include "producer.h"

using namespace std;

Producer::Producer(int argc, char* argv[],
                   shared_ptr<Buffer> outputBuf):
    _argc(argc),
    _argv(argv),
    _buf(outputBuf)
{

}
