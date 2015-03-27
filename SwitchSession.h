#pragma once
#include "Session.h"
#include "ControllerSession.h"
class controller_session;
class switch_session:public session
{
public:
    switch_session(boost::asio::io_service& io_service)
        : session(io_service){}

    void start();
    void set_controller_session(controller_session* c_session);
    void handle_read(const boost::system::error_code& error, size_t bytes_transferred);
    void handle_write(const boost::system::error_code& error);
    ~switch_session(){};
private:
    controller_session* c_session_;
};
