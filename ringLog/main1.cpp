#include "rlog.h"
#include <unistd.h>

int main(int argc, char** argv)
{
    LOG_INIT("log", "myLog", INFO);
    for (int i = 0;i < 1000; ++i)
    {
        LOG_INFO("my number is %d", i);
    }
    while(1);
    return 0;
}
