#pragma once
#include "Session.h"
#include "SwitchSession.h"
class switch_session;
class controller_session:public session
{
public:
    controller_session(boost::asio::io_service& io_service)
        :session(io_service) {};
    void start();
    void set_switch_session(switch_session* s_session);
    void handle_read(const boost::system::error_code& error, size_t bytes_transferred);
    void handle_write(const boost::system::error_code& error);
    ~controller_session() {};
private:
    switch_session* s_session_;
};
