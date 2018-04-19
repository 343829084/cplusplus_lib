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
#include "limitflow.h"


int main(int argc, char* argv[])
{
    int speed_opt = 1000;
    int msg_num = 1000;
    if (argc == 2) {
        speed_opt = atoi((argv[1]));
    } else if (argc == 3) {
        speed_opt = atoi((argv[1]));
        msg_num = atoi((argv[2]));
    } else {
        std::cout << "./xxx speed test_msg_count" << std::endl;
        return false;
    }
    boost::asio::io_service io;
    boost::asio::io_service::work worker(io);
    boost::thread io_thread(boost::bind(&boost::asio::io_service::run, &io));
    boost::shared_ptr<LimitFlow> instance(new LimitFlow(io, speed_opt, 100));
    instance->Start();
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    for (int i = 0; i < msg_num; ++i) {
       if(!instance->Wait()) {
           std::cout << "error! spend token too big." << std::endl;
           break;
       }
    }
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::chrono::steady_clock::duration time_span = end - begin;
    double nsec = double(time_span.count()) * std::chrono::steady_clock::period::num / std::chrono::steady_clock::period::den;
    std::cout << "msg_num = " << msg_num << " spend time=" << nsec << std::endl;
    io.stop();
    io_thread.join();
    return true;
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
