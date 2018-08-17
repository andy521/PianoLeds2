/**
 * @file
 *
 * MIT License
 * 
 * @copyright (c) 2017 Daniel Schenk <danielschenk@users.noreply.github.com>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <vector>
#include <string>
#include <mutex>
#include <cstdarg>
#include <algorithm>
#include <stdio.h>

#include "LoggingEntryPoint.h"
#include "Interfaces/ILoggingTarget.h"

std::vector<ILoggingTarget*> LoggingEntryPoint::s_subscribers;
std::mutex LoggingEntryPoint::s_mutex;

void LoggingEntryPoint::subscribe(ILoggingTarget& rSubscriber)
{
    std::lock_guard<std::mutex> lock(s_mutex);
    bool found(false);
    for(auto pLoggingTarget : s_subscribers)
    {
        if(pLoggingTarget == &rSubscriber)
        {
            found = true;
            break;
        }
    }
    if(!found)
    {
        s_subscribers.push_back(&rSubscriber);
    }
}

void LoggingEntryPoint::unsubscribe(ILoggingTarget& rSubscriber)
{
    std::lock_guard<std::mutex> lock(s_mutex);
    s_subscribers.erase(std::remove(s_subscribers.begin(), s_subscribers.end(), &rSubscriber), s_subscribers.end());
}

void LoggingEntryPoint::logMessage(uint64_t time, Logging::TLogLevel level, const char *component, const char *fmt, ...)
{
    std::lock_guard<std::mutex> lock(s_mutex);
    if(s_subscribers.size() > 0)
    {
        std::vector<char> buffer(c_maxMessageSize);
        va_list args;
        va_start(args, fmt);
        vsnprintf(buffer.data(), buffer.size(), fmt, args);
        va_end(args);

        std::string message(buffer.data());
        std::string componentStr(component);
        for(auto pLoggingTarget : s_subscribers)
        {
            if (pLoggingTarget != nullptr)
            {
                pLoggingTarget->logMessage(time, level, componentStr, message);
            }
        }
    }
}