#include <g3log/g3log.hpp>
#include <g3log/logworker.hpp>
#include <g3log/std2_make_unique.hpp>

int main() {

    std::unique_ptr<g3::LogWorker> logWorker = g3::LogWorker::createLogWorker();

    /**
    A convenience function to add the default g3::FileSink to the log worker
     @param log_prefix that you want
     @param log_directory where the log is to be stored.
     @return a handle for API access to the sink. See the README for example usage
     **/
    logWorker->addDefaultLogger("tryG3log", "./log/");

    g3::initializeLogging(logWorker.get());

    LOGF(INFO, "Hi log %d", 123);
    LOG(INFO) << "Test SLOG INFO";
    LOG(G3LOG_DEBUG) << "Test SLOG DEBUG";
    LOG(INFO) << "one: " << 1;
    LOG(INFO) << "two: " << 2;
    LOG(INFO) << "one and two: " << 1 << " and " << 2;
    LOG(G3LOG_DEBUG) << "float 2.14: " << 1000 / 2.14f;
    LOG(G3LOG_DEBUG) << "pi double: " << 3.1415926;
    LOG(G3LOG_DEBUG) << "pi float: " << 3.14;
    LOGF(INFO, "pi float printf:%.3f", 3.14);

    return 0;
}
