#include "easylogging++.h"

INITIALIZE_EASYLOGGINGPP

static unsigned int idx = 0;

void rolloutHandler(const char* filename, std::size_t size)
{
    std::stringstream stream;
    stream << filename << "." << ++idx;
    filename = (char *)(&idx);
}

int main() {

    // Load configuration from file
    el::Configurations conf("log.conf");
    // Actually reconfigure all loggers instead
    el::Loggers::reconfigureAllLoggers(conf);
    // Now all the loggers will use configuration from file

    for(int i = 0;i < 1000;i++)
    {
        LOG(TRACE)   << "***** trace log  *****";
        LOG(DEBUG)   << "***** debug log  *****";
        LOG(ERROR)   << "***** error log  *****";
        LOG(WARNING) << "***** warn  log  *****";
        LOG(INFO)    << "***** info  log  *****";
    }

    return 0;
}
