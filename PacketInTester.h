#pragma once
#include <string>
#include "FlowGen.h"
#include "Session.h"
using namespace std;

class Group
{
public:
    uint32_t time;
    uint32_t sent_flow;
    uint32_t sent_flow_per_sec;
    uint32_t successed_flow_;
    uint32_t successed_PKTIN_;
    uint32_t avg_success_flow_per_sec;
    uint32_t first_packet_time_stamp;
    uint32_t last_packet_time_stamp;
    uint32_t PKTIN_buffer_size_;
};

class PacketInTester
{
public:
    PacketInTester(Session & session,FlowGen & flow_gen):session_(session), flow_gen_(flow_gen)
    {
        XID = 0;
    };
    void start()
    {
        send_hello();
        send_feature_request();
        test();
    };
    void onMessage(Message);

protected:
    void test();
    void send_hello();
    void send_feature_request();
    void group_test(Group &group, uint32_t last_sec, uint32_t packet_count);
    uint32_t get_XID(){
        return ++XID;
    }
    Session & session_;
    FlowGen & flow_gen_;

    uint32_t XID;
    const int group_interval = 5; 
    vector<Group> data;
    uint32_t group_index = 0;
};

typedef std::shared_ptr<PacketInTester> PktTst_ptr;
