#pragma once

#include "http_parser.h"
#include <vector>
#include "message.h"
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
    public:
        Parser();
        std::pair<std::shared_ptr<Message>,int> Execute(const char* data,int size);
        void Destory(){};
        void SetGetCallBackID();
        void SetCallBackID(int CallBackID);
        int GetCallBackID();
        bool CheckCallBackID();
        void SetBody(const char* Data,int len);
};