// This file is a "Hello, world!" in C++ language by GCC for wandbox.
#include <iostream>
#include <cstdlib>
#include <map>
#include <tuple>
#include <string>
#include <functional>
#include <utility>

#include <chrono>
#include <ctime>
#include<ratio>
#include "flowctrl.h"

typedef std::tuple<int, int, std::string> ReportKey;

struct ClassCompare {
    bool operator() (const ReportKey& x, const ReportKey& y) const {
        if (std::get<0>(x) != std::get<0>(y)) {
            return std::get<0>(x) < std::get<0>(y);
        } else if (std::get<1>(x) != std::get<1>(y)) {
            return std::get<1>(x) < std::get<1>(y);
        } else if (std::get<2>(x) != std::get<2>(y)) {
            return std::get<2>(x) < std::get<2>(y);
        } else {
            return false;
        }
        return false;
    }
};

void testChrono() {
    //typedef std::chrono::duration<int, std::milli> hour_type;
    std::chrono::hours one_day (24);
    std::cout << one_day.count() << std::endl;
    std::cout << "change to minutes: ";
    std::cout << std::chrono::minutes(one_day).count() << std::endl;

    // time_point
    std::chrono::system_clock::time_point now  = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::cout << "now time is " << std::ctime(&now_c);

    std::chrono::steady_clock::time_point cur_now = std::chrono::steady_clock::now();
    for (int i = 0; i < 10000; ++i) {
        std::cout << i ;
    }
    std::cout << std::endl;
    std::chrono::steady_clock::time_point after_time = std::chrono::steady_clock::now();
    std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double> >(after_time - cur_now);
    std::cout << "steady_clock during time=" << time_span.count();
}

int main()
{
    // 多个item做为map的key
    std::map<ReportKey, std::string, ClassCompare> reportMap;
    ReportKey ele1(1, 11, "aa");
    ReportKey ele2(2, 22, "bb");
    reportMap.insert(std::pair<ReportKey, std::string>(ele1, "aaaa"));
    reportMap.insert(std::pair<ReportKey, std::string>(ele2, "bbbbb"));
    std::map<ReportKey, std::string, ClassCompare>::const_iterator iter;
    if ((iter=reportMap.find(ele1)) != reportMap.end()) {
        std::cout << "find it key=" << std::get<0>(iter->first) << " " << std::get<1>(iter->first) << " " << std::get<2>(iter->first) << " value=" << iter->second << std::endl;
    } else {
        std::cout << "not find it" << std::endl;
        std::cout << "Hello, Wandbox!" << std::endl;
    }
    return true;

    //测试std::chrono下的三个类
    testChrono();
}

// GCC reference:
//   https://gcc.gnu.org/

// C++ language references:
//   https://msdn.microsoft.com/library/3bstk3k5.aspx
//   http://www.cplusplus.com/
//   https://isocpp.org/
//   http://www.open-std.org/jtc1/sc22/wg21/

// Boost libraries references:
//   http://www.boost.org/doc/
