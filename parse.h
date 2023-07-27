#pragma once

#include "http_parser.h"
#include <vector>
#include "task.h"
#include "message.h"
#include <iostream>

class EpollAgent;
class Parser
{
    private:
        http_parser_settings settings;
        http_parser parser;
        bool keepalive;
        bool gotCallBackID;
        const char* bodyData;
        int bodyLen;
        int callbackID;
        EpollAgent* agent;
        bool gotContentLength;
        int ContentLength;
    public:
        Parser(EpollAgent* agent);
        ~Parser() {
            std::cout <<"parse close" << std::endl;
        }
        task<std::pair<std::shared_ptr<Message>,int>> Execute(const char* data,int size);
        void Destory(){};
        void SetGetCallBackID();
        void SetCallBackID(int CallBackID);
        int GetCallBackID();
        bool CheckCallBackID();
        void SetBody(const char* Data,int len);
        void SetGetContentLength();
        bool CheckContentLength();
        void SetContentLength(int ContentLength);

};