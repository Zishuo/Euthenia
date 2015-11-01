#include "Dispatcher.h"
#include "PacketInTester.h"
#include "FlowGen.h"
int main(int argc, char * argv[])
{
	if( argc < 2) {
		printf("tester device_name\n");
		return 1;
	}
	FlowGen generator(argv[1]);
	PacketInTester tester(generator);
	return 0;
}
