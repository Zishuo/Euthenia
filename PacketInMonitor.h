#pragma once
#include <atomic>
#include <thread>
#include <chrono>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include "Session.h"
class PKTINMonitor
{
public:
    PKTINMonitor(unsigned int sample_rate, unsigned int threshold)
        :worker(NULL), counter(0), sample_rate_(sample_rate),thresh_hold_(threshold),is_running(true)
    {
    }

    void start()
    {
        //potential bug here...if is_running is asigned true value after the
        // clear() running.
        if(worker == NULL)
        {
            worker = new std::thread(std::bind(&PKTINMonitor::clear, this));
        }
    }

    void onMessage(Message message){
        BOOST_LOG_TRIVIAL(trace) << "PKTINMonitor | onMessage";
        add_counter();
        follow_(message);
    }

    void add_counter()
    {
        BOOST_LOG_TRIVIAL(debug) << "PKTINMonitor | add_counter : " << counter;    
        ++counter;
    }

    int get_counter()
    {
        return counter;
    }

    void set_smaple_rate(unsigned int sample_rate)
    {
        sample_rate_ = sample_rate;
    };

    void set_trigger_action(std::function<void(void)> action)
    {
        action_ = action;
    };

    void set_follow_action(std::function<void(Message)> action){
        follow_ = action; 
    }

    void stop()
    {
        is_running = false;
    };

private:
    void clear()
    {
        while(is_running)
        {
            counter = 0;
            std::this_thread::sleep_for(std::chrono::milliseconds(sample_rate_));
            BOOST_LOG_TRIVIAL(debug) << "PKTINMonitor | clear : " << counter;
            if(counter >= thresh_hold_)
            {
                BOOST_LOG_TRIVIAL(debug) << "PKTINMonitor | exceed threshhold, trigger action";
                action_();
            }
        }
    };

private:
    std::thread *worker;
    std::atomic_uint counter;
    unsigned int sample_rate_;
    unsigned int thresh_hold_;
    bool is_running;
    std::function<void(void)> action_;
    std::function<void(Message)> follow_;
};
