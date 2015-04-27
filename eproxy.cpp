#include <cstdlib>

#include <iostream>

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>

#include "Session.h"
#include "SwitchSession.h"
#include "ControllerSession.h"
#include "Dispatcher.h"
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

        switch_session* s_session = new switch_session(io_service);
        controller_session* c_session = new controller_session(io_service);

        //set Default Callbcak for Dispatcher
        dispatcher_from_switch.setDefaultCallback(
            std::bind(
                &controller_session::write,
                c_session,
                std::placeholders::_1,
                std::placeholders::_2));

        dispatcher_from_controller.setDefaultCallback(
            std::bind(
                &switch_session::write,
                s_session,
                std::placeholders::_1,
                std::placeholders::_2));

        s_session->set_dispatcher(dispatcher_from_switch);
        c_session->set_dispatcher(dispatcher_from_controller);

        //connect to controller
        boost::asio::ip::address c_ip =  boost::asio::ip::address::from_string("128.238.147.221");
        boost::asio::ip::tcp::endpoint c_edp(c_ip,6633);
        c_session->socket().connect(c_edp);
        BOOST_LOG_TRIVIAL(info) << "controller session started";

        //listen from switch
        using namespace std; // For atoi.
        boost::asio::ip::tcp::acceptor s_acpt(io_service,boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(),9000));
        s_acpt.accept(s_session->socket());
        BOOST_LOG_TRIVIAL(info) << "switch session started";

        s_session->set_controller_session(c_session);
        c_session->set_switch_session(s_session);
        s_session->start();
        c_session->start();

        BOOST_LOG_TRIVIAL(info) << "start io service";
        io_service.run();
    }
    catch (std::exception& e)
    {
        BOOST_LOG_TRIVIAL(fatal) << "Exception: " << e.what();
    }

    return 0;
}
