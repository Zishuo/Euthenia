#include "PacketInTester.h"
#include "openflow.h"

void PacketInTester::group_test(Group & group, uint32_t last_sec, uint32_t packet_count)
{
    flow_gen_.a_burst(&group.sent_flow,  packet_count, last_sec);
    group.time = last_sec;
    group.sent_flow_per_sec = group.sent_flow/last_sec;
    sleep(group_interval);
    group_index++;
}

void PacketInTester::send_hello()
{
    char hello[sizeof(ofp_header)];
    ofp_header * hello_ptr = (ofp_header *) hello;
    hello_ptr->type = OFPT_HELLO;
    hello_ptr->version = OFP_VERSION;
    hello_ptr->length = htons(sizeof(ofp_header));
    hello_ptr->xid = htonl(get_XID());

    boost::shared_ptr<std::string> hello_str =
        boost::make_shared<std::string>(hello);
    session_.write(hello_str);
}

void PacketInTester::send_feature_request()
{
    char feature_request[sizeof(ofp_header)];
    ofp_header *feature_request_ptr = (ofp_header *) feature_request;

    feature_request_ptr->version = OFP_VERSION;
    feature_request_ptr->type = OFPT_FEATURES_REQUEST;
    feature_request_ptr->length = htons(sizeof(ofp_header));
    feature_request_ptr->xid = htonl(get_XID());

    boost::shared_ptr<std::string> feature_request_str =
        boost::make_shared<std::string>(feature_request);
    session_.write(feature_request_str);
}


void PacketInTester::onMessage(Message msg)
{
    timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    if(data[group_index].first_packet_time_stamp == 0)
    {
        data[group_index].first_packet_time_stamp = now.tv_sec;
    }
    data[group_index].last_packet_time_stamp = now.tv_sec;
    data[group_index].successed_PKTIN_++;
}
