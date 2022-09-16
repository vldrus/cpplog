/* SPDX-License-Identifier: MIT */

#ifndef CPPLOG_H
#define CPPLOG_H

#include <ctime>
#include <chrono>
#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>

#if defined(_WIN32)
extern "C" __declspec(dllimport) unsigned long __stdcall GetCurrentThreadId();
#else
#include <unistd.h>
#include <sys/syscall.h>
#endif

#define LOG_DEBUG                                \
    if (CppLog::config().level <= CppLog::DEBUG) \
    CppLog(CppLog::Data{"D", __FILE__, std::to_string(__LINE__)})

#define LOG_INFO                                \
    if (CppLog::config().level <= CppLog::INFO) \
    CppLog(CppLog::Data{"I", __FILE__, std::to_string(__LINE__)})

#define LOG_WARN                                \
    if (CppLog::config().level <= CppLog::WARN) \
    CppLog(CppLog::Data{"W", __FILE__, std::to_string(__LINE__)})

#define LOG_ERROR                                \
    if (CppLog::config().level <= CppLog::ERROR) \
    CppLog(CppLog::Data{"E", __FILE__, std::to_string(__LINE__)})

class CppLog
{
public:
    enum Level : int
    {
        DEBUG = 1,
        INFO,
        WARN,
        ERROR
    };

    struct Config
    {
        Level level;
        bool label;
        bool date;
        bool time;
        bool thread;
        bool source;
    };

    struct Data
    {
        std::string label;
        std::string file;
        std::string line;
    };

    static Config &config();

    CppLog(const Data &data);

    ~CppLog();

    template <typename T>
    CppLog &operator<<(const T &arg);

private:
    void localtime_i(const std::time_t *timer, std::tm *tp);

    std::string thread_id();

    Data m_data;

    std::ostringstream m_oss;
};

CppLog::Config &CppLog::config()
{
    static Config config{
        INFO, true, true, true, true, true};
    return config;
}

CppLog::CppLog(const CppLog::Data &data)
    : m_data(data)
{
}

CppLog::~CppLog()
{
    auto now = std::chrono::system_clock::now();
    auto now_time = std::chrono::system_clock::to_time_t(now);
    auto now_epoch = now.time_since_epoch();
    auto now_millis =
        std::chrono::duration_cast<std::chrono::milliseconds>(now_epoch)
            .count();

    std::tm now_tm;

    localtime_i(&now_time, &now_tm);

    std::ostringstream fmt;

    if (config().label)
    {
        fmt << m_data.label
            << ' ';
    }
    if (config().date)
    {
        fmt << std::put_time(&now_tm, "%Y-%m-%d")
            << ' ';
    }
    if (config().time)
    {
        fmt << std::put_time(&now_tm, "%H:%M:%S")
            << '.'
            << std::setw(3) << std::setfill('0') << now_millis % 1000
            << ' ';
    }
    if (config().thread)
    {
        fmt << thread_id()
            << ' ';
    }
    if (config().source)
    {
        fmt << '('
            << m_data.file.substr(m_data.file.find_last_of("/\\") + 1)
            << ':'
            << m_data.line
            << ')'
            << ' ';
    }
    fmt << m_oss.str();

    std::cerr << fmt.str() + '\n';
}

template <typename T>
CppLog &CppLog::operator<<(const T &arg)
{
    m_oss << arg;
    return *this;
}

void CppLog::localtime_i(const std::time_t *timer, std::tm *tp)
{
#if defined(_WIN32) && defined(__BORLANDC__)
    localtime_s(timer, tp);
#elif defined(_WIN32)
    localtime_s(tp, timer);
#else
    localtime_r(timer, tp);
#endif
}

std::string CppLog::thread_id()
{
    std::ostringstream fmt;

    fmt << '['
#if defined(_WIN32)
        << GetCurrentThreadId()
#elif defined(SYS_gettid)
        << syscall(SYS_gettid)
#elif defined(__NR_gettid)
        << syscall(__NR_gettid)
#else
        << "---"
#endif
        << ']';

    return fmt.str();
}

#endif /* CPPLOG_H */
