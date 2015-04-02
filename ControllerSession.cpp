#include <iostream>
#include "ControllerSession.h"
void controller_session::start()
{
    read();
}

void controller_session::controller_session::set_switch_session(switch_session* s_session)
{
    s_session_ = s_session;
}

void controller_session::set_dispatcher(const Dispatcher& dispatcher)
{

    dispatcher_ = dispatcher;
}


void controller_session::read()
{
    boost::asio::async_read(socket_,
                            boost::asio::buffer(data_,sizeof(ofp_header)),
                            boost::bind(&controller_session::handle_read_header,
                                        this,
                                        boost::asio::placeholders::error,
                                        boost::asio::placeholders::bytes_transferred)
                           );


};

void controller_session::handle_read_header(const boost::system::error_code& error,
        size_t bytes_transferred)
{
    ofp_header* header = (ofp_header *)data_;
    boost::asio::async_read(socket_,
                            boost::asio::buffer(data_, header->length - sizeof(ofp_header)),
                            boost::bind(&controller_session::handle_read,this,
                                        boost::asio::placeholders::error,
                                        boost::asio::placeholders::bytes_transferred, header));
}

void controller_session::handle_read(const boost::system::error_code& error,
                                     size_t bytes_transferred,
                                     ofp_header * header)

{
    if (!error)
    {
        dispatcher_.onMessage(data_);
    }
    else
    {
        std::cerr << "ControllerSession : handle_read error " << error.message() << std::endl;
    }
}

void controller_session::write_in_io_thread(void * message_ptr, size_t length)
{
    boost::asio::async_write(s_session_->socket(),
                             boost::asio::buffer(message_ptr, length),
                             boost::bind(&controller_session::handle_write,
                                         this,
                                         boost::asio::placeholders::error,
                                         boost::asio::placeholders::bytes_transferred
                                        )
                            );
}

void controller_session::write(void *message_ptr, size_t length)
{
    boost::asio::async_write(s_session_->socket(),
                             boost::asio::buffer(message_ptr, length),
                             boost::bind(&controller_session::handle_write,
                                         this,
                                         boost::asio::placeholders::error,
                                         boost::asio::placeholders::bytes_transferred
                                        ));
}


void controller_session::handle_write(const boost::system::error_code& error, size_t bytes_transferred)
{
    if (!error)
    {
        read();
    }
    else
    {
        std::cerr << "ControllerSession : handle_write error " << error.message() << std::endl;
    }
}
