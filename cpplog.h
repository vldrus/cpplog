/* SPDX-License-Identifier: MIT */

#ifndef CPPLOG_H
#define CPPLOG_H

#include <ctime>
#include <chrono>
#include <string>
#include <vector>
#include <memory>
#include <sstream>
#include <iomanip>
#include <iostream>

#if !defined(_WIN32)
#include <unistd.h>
#include <sys/syscall.h>
#endif

#define LOG \
    cpplog::Logger(cpplog::LogData{"", __FILE__, std::to_string(__LINE__)})

#define LOG_DEBUG                                       \
    if (cpplog::config().level <= cpplog::Level::DEBUG) \
    cpplog::Logger(cpplog::LogData{"D", __FILE__, std::to_string(__LINE__)})

#define LOG_INFO                                       \
    if (cpplog::config().level <= cpplog::Level::INFO) \
    cpplog::Logger(cpplog::LogData{"I", __FILE__, std::to_string(__LINE__)})

#define LOG_WARN                                       \
    if (cpplog::config().level <= cpplog::Level::WARN) \
    cpplog::Logger(cpplog::LogData{"W", __FILE__, std::to_string(__LINE__)})

#define LOG_ERROR                                       \
    if (cpplog::config().level <= cpplog::Level::ERROR) \
    cpplog::Logger(cpplog::LogData{"E", __FILE__, std::to_string(__LINE__)})

namespace cpplog
{
    enum class Level : int
    {
        DEBUG = 1,
        INFO,
        WARN,
        ERROR
    };

    struct Config
    {
        cpplog::Level level;
        bool label;
        bool date;
        bool time;
        bool thread;
        bool source;
    };

    struct LogData
    {
        std::string label;
        std::string file;
        std::string line;
    };

    class Appender
    {
    public:
        virtual void append(const std::string &);
    };

    class ConsoleAppender : public cpplog::Appender
    {
    public:
        void append(const std::string &s) override;
    };

    class Logger
    {
    public:
        Logger(const cpplog::LogData &data);
        ~Logger();

        template <typename T>
        cpplog::Logger &operator<<(const T &arg);

    private:
        cpplog::LogData m_data;
        std::ostringstream m_oss;
    };

    cpplog::Config &config();

    std::vector<std::shared_ptr<cpplog::Appender>> &appenders();

    void localtime_i(const std::time_t *timer, std::tm *tp);

    std::string thread_id();

#if defined(_WIN32)
    extern "C" __declspec(dllimport) unsigned long __stdcall GetCurrentThreadId();
#endif
}

void cpplog::Appender::append(const std::string &)
{
}

void cpplog::ConsoleAppender::append(const std::string &s)
{
    std::cerr << s + '\n';
}

cpplog::Logger::Logger(const cpplog::LogData &data)
    : m_data(data)
{
}

cpplog::Logger::~Logger()
{
    auto now = std::chrono::system_clock::now();
    auto now_time = std::chrono::system_clock::to_time_t(now);
    auto now_epoch = now.time_since_epoch();
    auto now_millis =
        std::chrono::duration_cast<std::chrono::milliseconds>(now_epoch)
            .count();

    std::tm now_tm;

    cpplog::localtime_i(&now_time, &now_tm);

    std::ostringstream fmt;

    if (cpplog::config().label)
    {
        fmt << (this->m_data.label.empty() ? "" : this->m_data.label + ' ');
    }
    if (cpplog::config().date)
    {
        fmt << std::put_time(&now_tm, "%Y-%m-%d")
            << ' ';
    }
    if (cpplog::config().time)
    {
        fmt << std::put_time(&now_tm, "%H:%M:%S")
            << '.'
            << std::setw(3) << std::setfill('0') << (now_millis % 1000)
            << ' ';
    }
    if (cpplog::config().thread)
    {
        fmt << cpplog::thread_id()
            << ' ';
    }
    if (cpplog::config().source)
    {
        fmt << '('
            << this->m_data.file.substr(this->m_data.file.find_last_of("/\\") + 1)
            << ':'
            << this->m_data.line
            << ')'
            << ' ';
    }
    fmt << this->m_oss.str();

    auto res = fmt.str();

    for (auto appender : cpplog::appenders())
    {
        appender->append(res);
    }
}

template <typename T>
cpplog::Logger &cpplog::Logger::operator<<(const T &arg)
{
    this->m_oss << arg;
    return *this;
}

cpplog::Config &cpplog::config()
{
    static cpplog::Config config{
        /* .level  = */ cpplog::Level::INFO,
        /* .label  = */ true,
        /* .date   = */ true,
        /* .time   = */ true,
        /* .thread = */ true,
        /* .source = */ false};
    return config;
}

std::vector<std::shared_ptr<cpplog::Appender>> &cpplog::appenders()
{
    static std::vector<std::shared_ptr<cpplog::Appender>> appenders{
        std::make_shared<cpplog::ConsoleAppender>()};
    return appenders;
}

void cpplog::localtime_i(const std::time_t *timer, std::tm *tp)
{
#if defined(_WIN32) && defined(__BORLANDC__)
    ::localtime_s(timer, tp);
#elif defined(_WIN32)
    ::localtime_s(tp, timer);
#else
    ::localtime_r(timer, tp);
#endif
}

std::string cpplog::thread_id()
{
    std::ostringstream fmt;

    fmt << "[ ";
#if defined(_WIN32)
    fmt << cpplog::GetCurrentThreadId();
#elif defined(SYS_gettid)
    fmt << ::syscall(SYS_gettid);
#elif defined(__NR_gettid)
    fmt << ::syscall(__NR_gettid);
#elif defined(SYS_thr_self)
    long tid;
    ::syscall(SYS_thr_self, &tid);
    fmt << tid;
#else
    fmt << "---";
#endif
    fmt << " ]";

    return fmt.str();
}

#endif /* CPPLOG_H */
