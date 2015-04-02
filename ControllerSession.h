#pragma once
#include "Session.h"
#include "SwitchSession.h"
#include "Dispatcher.h"
class switch_session;
class controller_session:public session
{
public:
    controller_session(boost::asio::io_service& io_service)
        :session(io_service){};
    void start();
    void read();
    void write_in_io_thread(void * message_ptr, size_t length);
    void write(void * message_ptr, size_t length);
    void set_dispatcher(const Dispatcher & dispatcher);
    void set_switch_session(switch_session* s_session);
    ~controller_session() {};
private:
    void handle_read_header(const boost::system::error_code& error, size_t bytes_transferred);
    void handle_read(const boost::system::error_code& error, size_t bytes_transferred, ofp_header * header);
    void handle_write(const boost::system::error_code& error, size_t bytes_transferred);
    switch_session* s_session_;
    Dispatcher dispatcher_;
};
