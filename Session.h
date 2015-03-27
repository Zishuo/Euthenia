#pragma once
#include <boost/bind.hpp>
#include <boost/asio.hpp>
using boost::asio::ip::tcp;

class session
{
public:
    session(boost::asio::io_service& io_service)
        : socket_(io_service)
    {
    }

    tcp::socket& socket()
    {
        return socket_;
    }
    virtual void start() {};
    virtual void handle_read(const boost::system::error_code& error, size_t bytes_transferred) {};
    virtual void handle_write(const boost::system::error_code& error) {};
    virtual ~session() {};
protected:
    tcp::socket socket_;
    enum { max_length = 1024 };
    char data_[max_length];
};

