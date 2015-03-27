#include "SwitchSession.h"

void switch_session::start()
{
    socket_.async_read_some(boost::asio::buffer(data_, max_length),
                            boost::bind(&switch_session::handle_read, this,
                                        boost::asio::placeholders::error,
                                        boost::asio::placeholders::bytes_transferred));
}


void switch_session::set_controller_session(controller_session* c_session)
{
    c_session_ = c_session;
}

void switch_session::handle_read(const boost::system::error_code& error,
                                 size_t bytes_transferred)
{
    if (!error)
    {
        boost::asio::async_write(c_session_->socket(),
                                 boost::asio::buffer(data_, bytes_transferred),
                                 boost::bind(&switch_session::handle_write, this,
                                             boost::asio::placeholders::error));
    }
    else
    {
        std::cerr << "SwitchSession : handle_read error " << error.message() << std::endl;
    }
}

void switch_session::handle_write(const boost::system::error_code& error)
{
    if (!error)
    {
        socket_.async_read_some(boost::asio::buffer(data_, max_length),
                                boost::bind(&switch_session::handle_read, this,
                                            boost::asio::placeholders::error,
                                            boost::asio::placeholders::bytes_transferred));
    }
    else
    {
        std::cerr << "SwitchSession : handle_write error " << error.message() << std::endl;
    }
}
