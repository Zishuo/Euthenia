#pragma once
#include <arpa/inet.h>
#include <map>
#include <functional>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include "openflow.h"
class Dispatcher
{
public:

    void setCallBackOnMessage(ofp_type type, std::function<void(void * message)> f)
    {
        funs[type] = f;
    };

    void setCallBackOnError()
    {

    };

    void setDefaultCallback(std::function<void(void * Message_ptr, size_t length)> callback)
    {
        defaultCallback = callback;
    };

    void onMessage(void * message_ptr )
    {
        ofp_header * header = (ofp_header * )message_ptr;
        size_t length = ntohs(header->length);
        BOOST_LOG_TRIVIAL(trace) << "Dispatcher | onMessage";

        //can not find registed call back for message type
        if(funs.find((ofp_type)header->type) == funs.end())
        {

            if(defaultCallback != NULL)
            {
                char * new_message = new char[length];
                std::copy_n((char *)message_ptr, length, new_message);
                BOOST_LOG_TRIVIAL(trace) << "Dispatcher | defaultCallback";
                defaultCallback(new_message, length);
            }
            else
            {
                onError();
            }
        }
        else
        {
            //invoke message callback respectively
            funs[(ofp_type)(header->type)](message_ptr);

        }
    };

    void onError()
    {
        BOOST_LOG_TRIVIAL(warning) << "Dispatcher | onError";
    };

private:
    std::map<ofp_type,std::function<void(void * Message)>> funs;
    std::function<void(void * Message_ptr, size_t length)> defaultCallback;
};
