#include "consumer.h"

using namespace std;

Consumer::Consumer(shared_ptr<Buffer> inputBuf):
    _buf(inputBuf)
{

}
