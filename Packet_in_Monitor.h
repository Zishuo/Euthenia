#include <atomic>
#include <thread>
#include <chrono>
class PKTINMonitor{
    public:
        PKTINMonitor(unsigned int sample_rate, unsigned int threshold)
            :sample_rate_(sample_rate),thresh_hold_(threshold),is_running(false){
        }
        
        void start(){
            //potential bug here...if is_running is asigned true value after the
            // clear() running.
            is_running = true;
            worker = new std::thread(std::bind(&PKTINMonitor::clear, this));
        }

        void add_counter(){
            ++counter;
        }

        int get_counter(){
          return counter;  
        }

        void set_smaple_rate(unsigned int sample_rate){
           sample_rate_ = sample_rate;
        };
       
        void set_trigger_action(std::function<void(void)> action){
           action_ = action; 
        };

        void stop();
    private:
        void clear(){
            while(is_running){
                counter = 0;
                std::this_thread::sleep_for(std::chrono::milliseconds(sample_rate_));
                if(counter >= thresh_hold_){
                    action_;
                }
            }
        };

    private:
        bool is_running;
        std::thread *worker;
        std::atomic_uint counter;
        unsigned int sample_rate_;
        unsigned int thresh_hold_;
        std::function<void(void)> action_;
};
