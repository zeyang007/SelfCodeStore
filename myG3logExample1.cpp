#include <g3log/g3log.hpp>
#include <g3log/logworker.hpp>
#include <g3log/std2_make_unique.hpp>

int main(int argc, char**argv) {

    auto worker = g3::LogWorker::createLogWorker();

    auto handle= worker->addDefaultLogger("tryG3log", "./log/");

    g3::initializeLogging(worker.get());

    for(int i = 0;i < 100000;i++)
        LOG(INFO)<<i<<" hello world!";

    return 0;
}
