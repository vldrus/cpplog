/* SPDX-License-Identifier: MIT */

#include <cpplog.h>
#include <thread>

int main()
{
    CppLog::config().level = CppLog::DEBUG;

    LOG << "This is just log";

    CppLog::config().source = true;

    std::thread th([] {
        LOG_WARN << "Hello from thread!";
    });

    LOG_DEBUG << "This is debug";
    LOG_INFO  << "This is info";
    LOG_WARN  << "This is warn";
    LOG_ERROR << "This is error";

    th.join();
}
