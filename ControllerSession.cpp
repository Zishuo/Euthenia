#include <arpa/inet.h>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include "ControllerSession.h"
#include "Dispatcher.h"
#include "openflow.h"
void controller_session::start()
{
    BOOST_LOG_TRIVIAL(trace) << "controller_session | start";
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
    BOOST_LOG_TRIVIAL(trace) << "controller_session | read";
    boost::asio::async_read(socket_,
                            boost::asio::buffer(data_,sizeof(ofp_header)),
                            boost::asio::transfer_exactly(sizeof(ofp_header)),
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
    BOOST_LOG_TRIVIAL(debug) << "controller_session | handle_read_header : "
                             << " xid " << ntohl(header->xid)
                             << " length " << htons(header->length)
                             << " type "<< ofp_type(header->type);

    size_t length = htons(header->length) - sizeof(ofp_header);
    boost::asio::async_read(socket_,
                            boost::asio::buffer(data_ + sizeof(ofp_header), length),
                            boost::asio::transfer_exactly(length),
                            boost::bind(&controller_session::handle_read,this,
                                        boost::asio::placeholders::error,
                                        boost::asio::placeholders::bytes_transferred, header));
}

void controller_session::handle_read(const boost::system::error_code& error,
                                     size_t bytes_transferred,
                                     ofp_header * header)

{
    size_t length = ntohs(header->length);
    if (!error)
    {
        BOOST_LOG_TRIVIAL(debug) << "controller_session | handle_read " << data_to_string(length);
        dispatcher_.onMessage(data_);
        read();
    }
    else
    {
        BOOST_LOG_TRIVIAL(fatal)<< "controller_session | handle_read error " << error.message() << std::endl;
    }
}

void controller_session::write(void *message_ptr, size_t length)
{
    BOOST_LOG_TRIVIAL(trace) << "controller_session | write";
    io_service_.post(boost::bind(&controller_session::write_in_io_thread,this,message_ptr, length));
}

void controller_session::write_in_io_thread(void * message_ptr, size_t length)
{
    ofp_header * header = (ofp_header*)message_ptr;
    BOOST_LOG_TRIVIAL(debug) << "controller_session | write_in_io_thread : "
                             << " xid " << ntohl(header->xid)
                             << " length " << htons(header->length)
                             << " type " << ofp_type(header->type);

    boost::asio::async_write(s_session_->socket(),
                             boost::asio::buffer(message_ptr, length),
                             boost::bind(&controller_session::handle_write,
                                         this,
                                         boost::asio::placeholders::error,
                                         boost::asio::placeholders::bytes_transferred
                                        )
                            );
}



void controller_session::handle_write(const boost::system::error_code& error, size_t bytes_transferred)
{
    if (!error)
    {
    }
    else
    {
        std::cerr << "controller_session | handle_write error " << error.message() << std::endl;
    }
}
