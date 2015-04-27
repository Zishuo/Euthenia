#pragma once
#include <arpa/inet.h>
#include <map>
#include <functional>
#include <string>
#include <memory>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include "openflow.h"
class Dispatcher
{
public:

    void setCallBackOnMessage(ofp_type type, std::function<void(std::shared_ptr<std::string>)> f)
    {
        funs[type] = f;
    };

    void setCallBackOnError()
    {

    };

    void setDefaultCallback(std::function<void(std::shared_ptr<std::string> message)> callback)
    {
        defaultCallback = callback;
    };

    void onMessage(void * message_ptr )
    {
        ofp_header * header = (ofp_header * )message_ptr;
        size_t length = ntohs(header->length);

        BOOST_LOG_TRIVIAL(trace) << "Dispatcher | onMessage";
        std::shared_ptr<std::string> new_message = std::make_shared<std::string>((char *)message_ptr, length);
            
        //can not find registed call back for message type
        if(funs.find((ofp_type)header->type) == funs.end())
        {

            if(defaultCallback != NULL)
            {
                BOOST_LOG_TRIVIAL(trace) << "Dispatcher | defaultCallback";
                defaultCallback(new_message);
            }
            else
            {
                onError();
            }
        }
        else
        {
            //invoke message callback respectively
            funs[(ofp_type)(header->type)](new_message);

        }
    };

    void onError()
    {
        BOOST_LOG_TRIVIAL(warning) << "Dispatcher | onError";
    };

private:
    std::map<ofp_type,std::function<void(std::shared_ptr<std::string> message)>> funs;
    std::function<void(std::shared_ptr<std::string> message)> defaultCallback;
};
