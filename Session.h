#pragma once
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include "openflow.h"
using boost::asio::ip::tcp;

class session
{
public:
    session(boost::asio::io_service& io_service)
        : io_service_(io_service),socket_(io_service)
    {
    }

    tcp::socket& socket()
    {
        return socket_;
    }
    virtual void start() {};
    virtual void read() {};
    virtual void write(void * message_ptr, size_t length) {};
    virtual void handle_read(const boost::system::error_code& error, size_t bytes_transferred, ofp_header * header) {};
    virtual void handle_write(const boost::system::error_code& error, size_t bytes_transferred) {};
    virtual ~session() {};
protected:
    boost::asio::io_service & io_service_;
    tcp::socket socket_;
    enum { max_length = 1024 };
    char data_[max_length];
};

