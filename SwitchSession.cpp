#include "SwitchSession.h"
#include "Dispatcher.h"
#include "openflow.h"
void switch_session::start()
{
    read();
}

void switch_session::read()
{
    boost::asio::async_read(socket_,
                            boost::asio::buffer(data_,sizeof(ofp_header)),
                            boost::bind(&switch_session::handle_read_header,
                                        this,
                                        boost::asio::placeholders::error,
                                        boost::asio::placeholders::bytes_transferred)
                           );
}


void switch_session::set_controller_session(controller_session* c_session)
{
    c_session_ = c_session;
}

void switch_session::set_dispatcher(const Dispatcher& dispatcher)
{
    dispatcher_ = dispatcher;
}

void switch_session::handle_read_header(const boost::system::error_code& error,
                                        size_t bytes_transferred)
{
    ofp_header* header = (ofp_header *)data_;
    boost::asio::async_read(socket_,
                            boost::asio::buffer(data_, header->length - sizeof(ofp_header)),
                            boost::bind(&switch_session::handle_read,this,
                                        boost::asio::placeholders::error,
                                        boost::asio::placeholders::bytes_transferred,
                                        header));
}


void switch_session::handle_read(const boost::system::error_code& error,
                                 size_t bytes_transferred,
                                 ofp_header * header)
{
    if (!error)
    {
        dispatcher_.onMessage(data_);
    }
    else
    {
        std::cerr << "SwitchSession : handle_read error " << error.message() << std::endl;
    }
}

void switch_session::write(void * message_ptr, size_t length)
{
    io_service_.post(boost::bind(&switch_session::write_in_io_thread,this,message_ptr, length));
}

void switch_session::write_in_io_thread(void * message_ptr, size_t length)
{
    boost::asio::async_write(c_session_->socket(),
                             boost::asio::buffer(message_ptr, length),
                             boost::bind(&switch_session::handle_write,
                                         this,
                                         boost::asio::placeholders::error,
                                         boost::asio::placeholders::bytes_transferred
                                        )
                            );
}

void switch_session::handle_write(const boost::system::error_code& error, size_t bytes_transferred)
{
    if (!error)
    {
        read();
    }
    else
    {
        std::cerr << "SwitchSession : handle_write error " << error.message() << std::endl;
    }
}
