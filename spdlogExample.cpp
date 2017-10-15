#define SPDLOG_TRACE_ON
#define SPDLOG_DEBUG_ON

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#include <iostream>
#include <memory>

void async_example();
void user_defined_example();
void err_handler_example();

namespace spd = spdlog;

int main(int, char*[])
{
    try
    {
        // Console logger with color
        auto console = spd::stdout_color_mt("console");
        console->info("Welcome to spdlog!");
        console->error("Some error message with arg{}..", 1);

        // Conditional logging example
        console->info_if(true, "Welcome to spdlog conditional logging!");

        // Formatting examples
        console->warn("Easy padding in numbers like {:08d}", 12);
        console->critical("Support for int: {0:d};  hex: {0:x};  oct: {0:o}; bin: {0:b}", 42);
        console->info("Support for floats {:03.2f}", 1.23456);
        console->info("Positional args are {1} {0}..", "too", "supported");
        console->info("{:<30}", "left aligned");

        // The logger can be obtained by name
        spd::get("console")->info("loggers can be retrieved from a global registry using the spdlog::get(logger_name) function");

        // Customize msg format for all messages
        spd::set_pattern("[%Y-%m-%d %H:%M:%S.%f] [thread %t] [level %l] %v ");

        // 1 Create a file rotating logger with 5mb size max and 3 rotated files
        auto rotating_logger = spd::rotating_logger_mt("some_logger_name", "logs/mylogfile", 1048576 * 5, 3);
        // Customize msg format for specific logger's log
        rotating_logger->set_pattern("[%Y-%m-%d %H:%M:%S.%f] [Process %P] [level %l] %v ");
        for (int i = 0; i < 1000; ++i)
            rotating_logger->info("{} * {} equals 0", i, i);

        // 2 Create a daily logger - a new file is created every day on 2:30am
        auto daily_logger = spd::daily_logger_mt("daily_logger", "logs/daily", 2, 30);
        // trigger flush if the log severity is error or higher
        daily_logger->flush_on(spd::level::err);
        daily_logger->info(123.44);

        // Runtime log levels
        spd::set_level(spd::level::info);
        // Set global log level to info
        console->debug("This message shold not be displayed!");
        // Set specific logger's log level
        console->set_level(spd::level::debug);
        console->debug("This message shold be displayed..");

        // Asynchronous logging is very fast..
        // Just call spdlog::set_async_mode(q_size) and all created loggers from now on will be asynchronous..
        async_example();

        // Log user-defined types example
        user_defined_example();

        // Change default log error handler
        err_handler_example();

        // Apply a function on all registered loggers
        spd::apply_all([&](std::shared_ptr<spdlog::logger> l)
        {
            l->info("End of example.");
        });

        // Release and close all loggers
        spdlog::drop_all();
    }
    // Exceptions will only be thrown upon failed logger or sink construction (not during logging)
    catch (const spd::spdlog_ex& ex)
    {
        std::cout << "Log init failed: " << ex.what() << std::endl;
        return 1;
    }
}

void async_example()
{
    size_t q_size = 4096; //queue size must be power of 2
    spdlog::set_async_mode(q_size);
    // 3 Create a async logger
    auto async_file = spd::daily_logger_st("async_file_logger", "logs/async_log");

    for (int i = 0; i < 1000; ++i)
        async_file->info("Async message #{}", i);
}

// user defined types logging by implementing operator<<
struct my_type
{
    int i;
    template<typename OStream>
    friend OStream& operator<<(OStream& os, const my_type &c)
    {
        return os << "[my_type i="<<c.i << "]";
    }
};

#include "spdlog/fmt/ostr.h" // must be included
void user_defined_example()
{
    spd::get("console")->info("user defined type: {}", my_type { 14 });
}

//custom error handler
void err_handler_example()
{
    //can be set globaly or per logger(logger->set_error_handler(..))
    spdlog::set_error_handler([](const std::string& msg)
    {
        std::cerr << "my err handler: " << msg << std::endl;
    });
    spd::get("console")->info("some invalid message to trigger an error {}{}{}{}", 3);
}
