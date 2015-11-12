#include <thread>
#include "Dispatcher.h"
#include "FlowGen.h"
#include "PacketInTester.h"
#include "openflow.h"

int main(int argc, char * argv[])
{
	if( argc < 2)
	{
		printf("tester device_name\n");
		return 1;
	}
	boost::asio::io_service io_service;
    //Flow generator will send packet on given device
	FlowGen generator(argv[1]);
    //maintain a session between openflow switch and tester for sending
    //control message
	Session s_session(io_service, "switch_session");
	Dispatcher dispatcher_from_switch;
    //all coming packets will send to dispatcher for dispacting 
	s_session.set_dispatcher(dispatcher_from_switch);

	// send packet through generator, send control message from s_session
	PacketInTester tester(generator, s_session, io_service);

    //when get OFPT_HELLO response send feature request
	dispatcher_from_switch.setCallBackOnMessage(OFPT_HELLO,
	        std::bind(
	            &PacketInTester::send_feature_request,
	            &tester,
	            std::placeholders::_1
	        ));
    //when get OFPT_FEATURES_REPLY we can start to test
	dispatcher_from_switch.setCallBackOnMessage(OFPT_FEATURES_REPLY,
	        std::bind(
	            &PacketInTester::test,
	            &tester,
	            std::placeholders::_1
	        ));
	//when get packet :  s_session -> dipatcher -> tester.onMessage
	dispatcher_from_switch.setCallBackOnMessage(OFPT_PACKET_IN,
	        std::bind(
	            &PacketInTester::onMessage,
	            &tester,
	            std::placeholders::_1
	        ));

    //listing on 9000 for incoming OFP siwtch
    s_session.listen(9000);
    //start test
    tester.send_hello();
	io_service.run();

	return 0;
}
