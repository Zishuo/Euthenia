#include <cstdlib>

#include <iostream>

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>

#include "Session.h"
#include "Dispatcher.h"
#include "PacketInMonitor.h"
#include "PacketInMigrator.h"
//#define OVS_ASSIST
using boost::asio::ip::tcp;
using namespace std;

void boost_log_init(int severity)
{
	namespace logging = boost::log;
	namespace keywords = boost::log::keywords;
	namespace sinks = boost::log::sinks;
	logging::core::get()->set_filter
	(
	    logging::trivial::severity >= severity
	);
}


int main(int argc, char* argv[])
{
	//initialize boost log
	int severity = 2;
	if(argc > 1)
	{

		try
		{
			severity = std::stoi(argv[1]);
		}
		catch(const std::exception& e)
		{
			BOOST_LOG_TRIVIAL(warning) << "log serverity : " << e.what();
		}

	}
	boost_log_init(severity);

	try
	{

		boost::asio::io_service io_service;

		Dispatcher dispatcher_from_switch;
		Dispatcher dispatcher_from_controller;
		Dispatcher dispatcher_from_ovs;

		Session * s_session = new Session(io_service,"switch_session");
		Session * c_session = new Session(io_service,"controller_session");
#ifdef OVS_ASSIST
		Session * ovs_session = new Session(io_service, "ovs_session");
#endif

		//set Default Callbcak for Dispatcher
		dispatcher_from_switch.setDefaultCallback(
		    std::bind(
		        &Session::write,
		        c_session,
		        std::placeholders::_1));

		dispatcher_from_controller.setDefaultCallback(
		    std::bind(
		        &Session::write,
		        s_session,
		        std::placeholders::_1));
#ifdef OVS_ASSIST
		dispatcher_from_ovs.setDefaultCallback(std::bind(
		        &Session::write,
		        c_session,
		        std::placeholders::_1));
#endif

		//initialize packet in Migrator
		PKTMigrator migrator(s_session,(unsigned int)3);
		//set packet_in callback
		PKTINMonitor monitor(500,10);
		monitor.set_trigger_action(std::bind(&PKTMigrator::switch_path, &migrator));
		monitor.set_follow_action(
		    std::bind(
		        &Session::write,
		        c_session,
		        std::placeholders::_1));

		//setup s_session dispatcher
		dispatcher_from_switch.setCallBackOnMessage(OFPT_PACKET_IN,std::bind(
		            &PKTINMonitor::onMessage,
		            &monitor,
		            std::placeholders::_1
		        ) );

		//setup dispatcher
		s_session->set_dispatcher(dispatcher_from_switch);
		c_session->set_dispatcher(dispatcher_from_controller);
#ifdef OVS_ASSIST
		ovs_session->set_dispatcher(dispatcher_from_ovs);
#endif

		//connect to controller
		c_session->connect(6633,"128.238.147.221");
		BOOST_LOG_TRIVIAL(info) << "controller session started";

		//listen from switch
		s_session->listen(9000);
		BOOST_LOG_TRIVIAL(info) << "switch session started";

#ifdef OVS_ASSIST
		//listen from ovs
		ovs_session->listen(9001);
		BOOST_LOG_TRIVIAL(info) << "ovs session started";
#endif

		monitor.start();
		s_session->start();
		c_session->start();
#ifdef OVS_ASSIST
		ovs_session->start();
#endif

		BOOST_LOG_TRIVIAL(info) << "start io service";
		io_service.run();
	}
	catch (std::exception& e)
	{
		BOOST_LOG_TRIVIAL(fatal) << "Exception: " << e.what();
	}

	return 0;
}
