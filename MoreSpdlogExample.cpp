#include "spdlog/spdlog.h"
#include <iostream>
 
// 多线程的基于控制台（stdout）的日志记录器，支持高亮。类似的stdout_color_st是单线程版本
auto console = spdlog::stdout_color_mt( "console" );
// 基于文件的简单日志
auto logger = spdlog::basic_logger_mt("basic_logger", "logs/basic.txt");
// 基于滚动文件的日志，每个文件5MB，三个文件
auto logger = spdlog::rotating_logger_mt("file_logger", "myfilename", 1024 * 1024 * 5, 3);
 
// 定制输出格式
spdlog::set_pattern("*** [%H:%M:%S %z] [thread %t] %v ***");
 
// 多个日志器共享SINK
auto daily_sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>("logfile", 23, 59);
// 下面几个同步日志器共享的输出到目标文件
auto net_logger = std::make_shared<spdlog::logger>("net", daily_sink);
auto hw_logger = std::make_shared<spdlog::logger>("hw", daily_sink);
auto db_logger = std::make_shared<spdlog::logger>("db", daily_sink); 
 
// 一个日志器使用多个SINK
std::vector<spdlog::sink_ptr> sinks;
sinks.push_back( std::make_shared<spdlog::sinks::stdout_sink_st>());
sinks.push_back( std::make_shared<spdlog::sinks::daily_file_sink_st>( "logfile", 23, 59 ));
auto combined_logger = std::make_shared<spdlog::logger>( "name", begin( sinks ), end( sinks ));
spdlog::register_logger( combined_logger );
 
// 异步
// 每个日志器分配8192长度的队列，队列长度必须2的幂
spdlog::set_async_mode(8192); 
// 程序退出前清理
spdlog::drop_all();
 
// 注册日志器
spdlog::register_logger(net_logger);
// 注册后，其它代码可以根据名称获得日志器
auto logger = spdlog::get(net_logger);
 
// 记录日志
// 设置最低级别
console->set_level(spdlog::level::debug);
console->debug("Hello World") ;
// 使用占位符
console->info("Hello {}" ,"World"); 
// 带格式化的占位符：d整数，x十六进制，o八进制，b二进制                
console->warn("Support for int: {0:d}; hex: {0:x}; oct: {0:o}; bin: {0:b}", 42);
// 带格式化的占位符：f浮点数
console->info("Support for floats {:03.2f}", 1.23456);
// 左对齐，保证30字符宽度
console->error("{:<30}", "left aligned");
// 指定占位符位置序号
console->info("Positional args are {1} {0}..", "too", "supported");
 
// 记录自定义类型，需要重载<<操作符
#include <spdlog/fmt/ostr.h> 
class Duck{}
std::ostream& operator<<(std::ostream& os, const Duck& duck){ 
    return os << duck.getName(); 
}
Duck duck;
console->info("custom class with operator<<: {}..", duck);
 
// 输出格式，默认的输出格式为，
// [2014-31-10 23:46:59.678] [info] [my_loggername] Some message
// 要定制输出格式，可以调用
spdlog::set_pattern(pattern_string);
// 示例
spdlog::set_pattern("*** [%H:%M:%S %z] [thread %t] %v ***");
// 或者实现自己的格式化器
spdlog::set_formatter(std::make_shared<my_custom_formatter>());
