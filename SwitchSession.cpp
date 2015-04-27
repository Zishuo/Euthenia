#include <arpa/inet.h>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include "SwitchSession.h"
#include "Dispatcher.h"
#include "openflow.h"
void switch_session::start()
{
    BOOST_LOG_TRIVIAL(trace) << "switch_session | start";
    read();
}

void switch_session::set_controller_session(controller_session* c_session)
{
    c_session_ = c_session;
}

void switch_session::set_dispatcher(const Dispatcher& dispatcher)
{
    dispatcher_ = dispatcher;
}

void switch_session::read()
{
    BOOST_LOG_TRIVIAL(trace) << "switch_session | read";
    boost::asio::async_read(socket_,
                            boost::asio::buffer(data_,sizeof(ofp_header)),
                            boost::asio::transfer_exactly(sizeof(ofp_header)),
                            boost::bind(&switch_session::handle_read_header,
                                        this,
                                        boost::asio::placeholders::error,
                                        boost::asio::placeholders::bytes_transferred)
                           );
}

void switch_session::handle_read_header(const boost::system::error_code& error,
                                        size_t bytes_transferred)
{
    ofp_header* header = (ofp_header *)data_;
    BOOST_LOG_TRIVIAL(debug) << "switch_session | handle_read_header : "
                             << " xid " << ntohl(header->xid)
                             << " length " << htons(header->length)
                             << " type " << ofp_type(header->type);
    size_t length = htons(header->length) - sizeof(ofp_header);
    if(length == 0)
    {
       BOOST_LOG_TRIVIAL(trace) << "switch_session | handle_read_header OFP Message Body length 0";
       dispatcher_.onMessage(data_); 
    }
    else
    {
        boost::asio::async_read(socket_,
                                boost::asio::buffer(data_ + sizeof(ofp_header), length),
                                boost::asio::transfer_exactly(length),
                                boost::bind(&switch_session::handle_read,this,
                                            boost::asio::placeholders::error,
                                            boost::asio::placeholders::bytes_transferred,
                                            header));
    }
}

void switch_session::handle_read(const boost::system::error_code& error,
                                 size_t bytes_transferred,
                                 ofp_header * header)
{
    size_t length = ntohs(header->length);
    if (!error)
    {
        BOOST_LOG_TRIVIAL(debug) << "switch_session | handle_read " << data_to_string(length);
        dispatcher_.onMessage(data_);
        read();
    }
    else
    {
        BOOST_LOG_TRIVIAL(fatal) << "switch_session | handle_read error " << error.message();
    }
}

void switch_session::write(void * message_ptr, size_t length)
{
    BOOST_LOG_TRIVIAL(trace) << "switch_session | write";
    io_service_.post(boost::bind(&switch_session::write_in_io_thread,this,message_ptr, length));
}

void switch_session::write_in_io_thread(void * message_ptr, size_t length)
{
    ofp_header * header = (ofp_header*)message_ptr;
    BOOST_LOG_TRIVIAL(debug) << "switch_session | write_in_io_thread : "
                             << " xid " << ntohl(header->xid)
                             << " length " << htons(header->length)
                             << " type " << ofp_type(header->type);

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
    }
    else
    {
        BOOST_LOG_TRIVIAL(fatal) << "switch_session | handle_write error " << error.message() << std::endl;
    }
}
