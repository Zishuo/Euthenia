#pragma once
#include <map>
#include <functional>
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

        //can not find registed call back for message type
        if(funs.find((ofp_type)header->type) == funs.end())
        {
            if(!defaultCallback)
            {
                defaultCallback(message_ptr, (ofp_type)header->type);
            }
            else
            {
                onError();
            }
        }

        //invoke message callback respectively
        funs[(ofp_type)(header->type)](message_ptr);
    };

    void onError()
    {

    };

private:
    std::map<ofp_type,std::function<void(void * Message)>> funs;
    std::function<void(void * Message_ptr, size_t length)> defaultCallback;
};
