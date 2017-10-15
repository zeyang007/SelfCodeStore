#include <g3log/g3log.hpp>
#include <g3log/logworker.hpp>
#include <g3log/std2_make_unique.hpp>

int main() {

    auto worker = g3::LogWorker::createLogWorker();

    worker->addDefaultLogger("tryG3log", "./log/");

    g3::initializeLogging(worker.get());

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
