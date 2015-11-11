#include "Dispatcher.h"
#include "PacketInTester.h"
#include "FlowGen.h"

int main(int argc, char * argv[])
{
	if( argc < 2)
	{
		printf("tester device_name\n");
		return 1;
	}
	FlowGen generator(argv[1]);
	Dispatcher dispatcher_from_switch;
	boost::asio::io_service io_service;
	Session s_session(io_service, "switch_session");
	PacketInTester tester(generator, s_session);
	return 0;
}
