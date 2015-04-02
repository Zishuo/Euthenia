#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include "Session.h"
#include "SwitchSession.h"
#include "ControllerSession.h"
#include "Dispatcher.h"
using boost::asio::ip::tcp;
using namespace std;
int main(int argc, char* argv[])
{
    try
    {
        boost::asio::io_service io_service;
        Dispatcher dispatcher_from_switch;
        Dispatcher dispatcher_from_controller;

        switch_session* s_session = new switch_session(io_service);
        controller_session* c_session = new controller_session(io_service);
        dispatcher_from_switch.setDefaultCallback(
            std::bind(
                &switch_session::write_in_io_thread,
                s_session,
                std::placeholders::_1,
                std::placeholders::_2));

        dispatcher_from_controller.setDefaultCallback(
            std::bind(
                &controller_session::write_in_io_thread,
                c_session,
                std::placeholders::_1,
                std::placeholders::_2));

        //connect to controller
        boost::asio::ip::address c_ip =  boost::asio::ip::address::from_string("128.238.147.221");
        boost::asio::ip::tcp::endpoint c_edp(c_ip,6633);
        c_session->socket().connect(c_edp);
        cout << "controller session started" << endl;

        //listen from switch
        using namespace std; // For atoi.
        boost::asio::ip::tcp::acceptor s_acpt(io_service,boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(),9000));
        s_acpt.accept(s_session->socket());
        cout << "switch session started" << endl;

        s_session->set_controller_session(c_session);
        c_session->set_switch_session(s_session);
        s_session->start();
        c_session->start();

        cout << "start io service" <<endl;
        io_service.run();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
