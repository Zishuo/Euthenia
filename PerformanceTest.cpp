#include "PacketInTester.h"
#include "PacketInMonitor.h"
int main(int argc, char * argv[])
{


    boost::asio::io_service io_service;
    Dispatcher dispatcher;
    FlowGen engine(argv[1]);
    PKTINMonitor monitor(500,-1);

    Session session(io_service, "switch_session");
    session.set_dispatcher(dispatcher);
//    PacketInTester tester(session,engine);
//    dispatcher.setCallBackOnMessage(OFPT_PACKET_IN,
//                                    std::bind(
//                                        &PKTINMonitor::onMessage,
//                                        &monitor,
//                                        std::placeholders::_1
//                                    ) );
//    monitor.set_follow_action(
//        std::bind(
//            &PacketInTester::onMessage,
//            &tester,
//            std::placeholders::_1
//        )
//    );
//
//    boost::asio::ip::tcp::acceptor s_acpt(io_service,boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(),9000));
//    s_acpt.accept(session.socket());
//    tester.start();
}
