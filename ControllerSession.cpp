#include <iostream>
#include "ControllerSession.h"
void controller_session::start()
{
    std::cout << "ControllerSession start" <<std::endl;
    socket_.async_read_some(boost::asio::buffer(data_, max_length),
                            boost::bind(&session::handle_read, this,
                                        boost::asio::placeholders::error,
                                        boost::asio::placeholders::bytes_transferred));
}

void controller_session::controller_session::set_switch_session(switch_session* s_session)
{
    s_session_ = s_session;
}

void controller_session::handle_read(const boost::system::error_code& error,
                                     size_t bytes_transferred)
{
    if (!error)
    {
        std::cout << "message from controller" << std::endl;
        boost::asio::async_write(s_session_->socket(),
                                 boost::asio::buffer(data_, bytes_transferred),
                                 boost::bind(&controller_session::handle_write, this,
                                             boost::asio::placeholders::error));
    }
    else
    {
        std::cerr << "ControllerSession : handle_read error " << error.message() << std::endl; 
    }
}
void controller_session::handle_write(const boost::system::error_code& error)
{
    if (!error)
    {
        std::cout << "message from switch" << std::endl;
        socket_.async_read_some(boost::asio::buffer(data_, max_length),
                                boost::bind(&session::handle_read, this,
                                            boost::asio::placeholders::error,
                                            boost::asio::placeholders::bytes_transferred));
    }
    else
    {
        std::cerr << "ControllerSession : handle_write error " << error.message() << std::endl; 
    }
}
