#pragma once
#include "Session.h"
#include "ControllerSession.h"
#include "Dispatcher.h"
#include "openflow.h"
class controller_session;
class switch_session:public session
{
public:
    switch_session(boost::asio::io_service& io_service)
        : session(io_service) {};

    void start();
    void read();
    void write(void * message_ptr, size_t length);
    void write_in_io_thread(void * message_ptr, size_t length);
    void set_controller_session(controller_session* c_session);
    void set_dispatcher(const Dispatcher & dispatcher);
    ~switch_session() {};
private:
    void handle_read(const boost::system::error_code& error, size_t bytes_transferred, ofp_header * header);
    void handle_read_header(const boost::system::error_code& error, size_t bytes_transferred);
    void handle_write(const boost::system::error_code& error, size_t bytes_transferred);
    controller_session* c_session_;
    Dispatcher dispatcher_;
};
