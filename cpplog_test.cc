/* SPDX-License-Identifier: MIT */

#include <cpplog.h>

int main()
{
    CppLog::config().level = CppLog::DEBUG;

    LOG_DEBUG << "This is debug";
    LOG_INFO  << "This is info";
    LOG_WARN  << "This is warn";
    LOG_ERROR << "This is error";
}
