#include "limitflow.h"
// 参考https://www.thinksaas.cn/group/topic/85037

LimitFlow::LimitFlow(boost::asio::io_service& io, int speed, int time_unit)
: token_counter_(0),
speed_(speed),
head_(0),
tail_(0),
bucket_(0),
bucket_tail_(0),
timer_(io),
time_unit_(time_unit*1000000), // 单位用的是纳秒，外面可以设置time_unit的值来设置单位，现在默认设置为秒
next_speed_(0),
status_(kStopped){
}

void LimitFlow::Start() {
    begin_ = std::chrono::steady_clock::now();
    tail_ = speed_;
    bucket_tail_ = tail_;
    status_ = kRunning;
    timer_.expires_from_now(std::chrono::nanoseconds(time_unit_));
    timer_.async_wait(boost::bind(&LimitFlow::GenToken, shared_from_this(), _1));
}

void LimitFlow::Stop() {
    std::lock_guard<std::mutex> stop_change_lock(stop_mutex_);
    status_ = kStopped;
    timer_.cancel();
}

bool LimitFlow::Wait(int msg_num) {
    if (speed_ > 0 && msg_num > speed_) {
        return false;
    }
    std::lock_guard<std::mutex> lock(mutex_); // 这个锁，主要是防止外面的多线程调用此方法
    while(bucket_tail_ - head_ < msg_num) {
       /* std::cout << "wait msg_num=" << msg_num
            << " bucket_tail-head=" << bucket_tail_ - head_
            << " bucket_tail=" << bucket_tail_
            << " head=" << head_ << std::endl;
        */
        std::this_thread::sleep_for(std::chrono::microseconds(1));
   }
    if (msg_num > 0) {
        head_ += msg_num;
        //std::cout << "wait head=" << head_ << std::endl;
    }
    return true;
}

bool LimitFlow::ChangeSpeed(int speed, int bucket) {
    std::lock_guard<std::mutex> stop_change_lock(stop_mutex_);
    if (speed <=0 || bucket <0) {
        return false;
    }
    if (status_ == kPending) {
        return false;
    }

    if (kStopped == status_) {
        speed_ = speed;
        return true;
    }

    next_speed_ = speed;
    status_ = kPending;
    return true;
}

void LimitFlow::DoChangeSpeed() {
    speed_ = next_speed_;
    status_ = kRunning;
}

void LimitFlow::GenToken(const error_code& ec)
{
    std::lock_guard<std::mutex> stop_change_lock(stop_mutex_);
    if (!ec) {
        if ( status_ == kRunning || status_ == kPending) {
            if (kPending == status_) {
                DoChangeSpeed();
            }
            int local_head = head_;
            token_counter_ = tail_ - local_head;
            if (token_counter_ < 0) {
                token_counter_ = 0;
            }
            bucket_ = std::min(token_counter_, speed_);
            tail_ = local_head + speed_* (timer_.expires_at() - begin_).count()  / time_unit_;
            bucket_tail_ = tail_ + bucket_;
            std::chrono::duration<double> span
                = std::chrono::duration_cast<std::chrono::duration<double> >(timer_.expires_at() - begin_);

            begin_ = timer_.expires_at();

            /*std::cout << " gentoken spend time=" << span.count()
                << " bucket=" << bucket_
                << " tail=" << tail_
                << " head=" << head_
                << " bucket_tail=" << bucket_tail_ << std::endl;*/
            timer_.expires_from_now(std::chrono::nanoseconds(time_unit_));
            timer_.async_wait(boost::bind(&LimitFlow::GenToken, shared_from_this(), _1));
        }
    }

}
