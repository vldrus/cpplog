/* SPDX-License-Identifier: MIT */

#include <cpplog.h>

int main()
{
    cpplog::config().level = cpplog::Level::DEBUG;

    LOG << "This is just log";

    cpplog::config().source = true;

    LOG_DEBUG << "This is debug";
    LOG_INFO  << "This is info";
    LOG_WARN  << "This is warn";
    LOG_ERROR << "This is error";
}
