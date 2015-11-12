#include "PacketInTester.h"
#include "openflow.h"

void PacketInTester::send_hello()
{
	char hello[sizeof(ofp_header)];
	ofp_header * hello_ptr = (ofp_header *) hello;
	hello_ptr->type = OFPT_HELLO;
	hello_ptr->version = OFP_VERSION;
	hello_ptr->length = htons(sizeof(ofp_header));
	hello_ptr->xid = htonl(get_XID());

	std::shared_ptr<std::string> hello_str =
	    std::make_shared<std::string>(hello);
	session_.write(hello_str);
}

void PacketInTester::send_feature_request(Message msg)
{
	char feature_request[sizeof(ofp_header)];
	ofp_header *feature_request_ptr = (ofp_header *) feature_request;

	feature_request_ptr->version = OFP_VERSION;
	feature_request_ptr->type = OFPT_FEATURES_REQUEST;
	feature_request_ptr->length = htons(sizeof(ofp_header));
	feature_request_ptr->xid = htonl(get_XID());

	std::shared_ptr<std::string> feature_request_str =
	    std::make_shared<std::string>(feature_request);
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

void PacketInTester::group_test(Group & group, uint32_t last_sec, uint32_t packet_count)
{
	flow_gen_.a_burst(&group.sent_flow,  packet_count, last_sec);
	group.time = last_sec;
	group.sent_flow_per_sec = group.sent_flow/last_sec;
	sleep(group_interval);
	group_index++;
}
void PacketInTester::calculate_buffered_packet(Group & group)
{
	using namespace boost::asio;
	//every 500ms count thes
	group.buffered_pkt.push_back(group.sent_flow - group.successed_PKTIN_);
	if(group.running)
	{
		shared_ptr<deadline_timer> timer = make_shared<deadline_timer>(ios_);
		timer->expires_from_now(boost::posix_time::millisec(500));
		timer->async_wait(boost::bind(&PacketInTester::calculate_buffered_packet,this,boost::ref(group)));
	}
}

void PacketInTester::do_test()
{

	uint32_t start = 10;
	uint32_t end = 2000;
	uint32_t step = 100;

	for(uint32_t i = start; i <= end; i+=step)
	{
		Group group;
		calculate_buffered_packet(group);
		group_test(group, 5, i);
		data.push_back(group);
		group.running = false;
	}
}

void PacketInTester::test(Message msg)
{
	//new thread to do
	tester_t = make_shared<thread>(bind(&PacketInTester::do_test, this));
}
