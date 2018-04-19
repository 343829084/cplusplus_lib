#ifndef CPP_FLOWCTRL
#define CPP_FLOWCTRL
#include<string>
#include<time.h>
#include<mutex>
#include<thread>
#include<boost/asio.hpp>
#include<boost/bind/bind.hpp>
#include<boost/function.hpp>
#include<boost/thread/thread.hpp>
#include<boost/smart_ptr.hpp>
#include<boost/enable_shared_from_this.hpp>
#include<boost/system/error_code.hpp>
#include<chrono>
#include<ctime>
/* 根据单色三桶原理设计 ，参考代码
 *http://blog.chinaunix.net/xmlrpc.php?r=blog/article&uid=26874207&id=4661792
 *
 *
 */

class LimitFlow:public boost::enable_shared_from_this<LimitFlow>
{
    public:
        typedef boost::system::error_code error_code;


        LimitFlow(boost::asio::io_service& io, int speed, int time_unit);
        bool Wait(int msg_num = 1);
        bool ChangeSpeed(int speed, int bucket);
        void Start();
        void Stop();

    private:
        void GenToken(const error_code& ec);
        void DoChangeSpeed();
    private:
        typedef boost::asio::basic_waitable_timer<std::chrono::steady_clock> steady_timer;

        enum RunStatus {
            kStopped,
            kRunning,
            kPending,
        };

        int token_counter_;   //计数器,用户用掉的桶元素，剩下的数量
        int speed_;
        volatile int head_;   // 第一个桶的
        volatile int tail_;     // 第一个桶
        volatile int bucket_;  // 第二个桶的大小
        volatile int bucket_tail_; // 第二个桶+第一个桶的大小
        steady_timer timer_;
        std::mutex mutex_;
        std::chrono::steady_clock::time_point   begin_;
        int time_unit_;
        int next_speed_;
        RunStatus status_;
        std::mutex stop_mutex_; // 主要是针对外部多线程同时调用stop, changeSpeed,及内部定时器GenToken

};

#endif
