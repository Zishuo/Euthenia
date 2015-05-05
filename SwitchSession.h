#pragma once
#include "Session.h"
#include "ControllerSession.h"
#include "Dispatcher.h"
#include "openflow.h"
class controller_session;
class switch_session:public Session
{
public:
    switch_session(boost::asio::io_service& io_service)
        : Session(io_service) {};

    void start();
    void read();
    void write(std::shared_ptr<std::string> message);
    void write_in_io_thread(std::shared_ptr<std::string> message);
    void set_controller_session(controller_session* c_session);
    void set_dispatcher(const Dispatcher & dispatcher);
    ~switch_session() {};
private:
    void handle_read(const boost::system::error_code& error, size_t bytes_transferred, ofp_header * header);
    void handle_read_header(const boost::system::error_code& error, size_t bytes_transferred);
    void handle_write(const boost::system::error_code& error, size_t bytes_transferred, std::shared_ptr<std::string> message);
    controller_session* c_session_;
    Dispatcher dispatcher_;
};
