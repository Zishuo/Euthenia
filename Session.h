#pragma once
#include <string>
#include <sstream>
#include <memory>
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
    virtual void write(std::shared_ptr<std::string> message) {};
    virtual ~session() {};
protected:
    enum { max_length = 102400 };
    virtual void handle_read(const boost::system::error_code& error, size_t bytes_transferred) {};
    virtual void handle_write(const boost::system::error_code& error, size_t bytes_transferred, std::shared_ptr<std::string> message) {};
    boost::asio::io_service & io_service_;
    tcp::socket socket_;
    unsigned char data_[max_length];

    static std::string to_hex_string(const char * data, size_t length){
        std::stringstream ss;
        for(size_t i = 0; i < length; ++i)
        {
            ss << std::hex << (unsigned short)((unsigned char)data[i]) << " ";
        }
        return ss.str();
    }
};

