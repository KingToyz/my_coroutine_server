#include "parse.h"

#include <iostream>
#include "message.h"
#include <memory>

int on_message_begin(http_parser* parser)
{
    // std::cout << "start to parse" << std::endl;

    return 0;
}
int on_url(http_parser* parser, const char* at, size_t len)
{
    std::string str;
    str.assign(at,len);
    // std::cout << "get url "  << str << std::endl;
    return 0;
}
int on_header_field(http_parser* parser, const char* at, size_t len)
{
    std::string str;
    str.assign(at,len);
    if(str == "CallBackID")
    {
        Parser* Parse = static_cast<Parser*>(parser->data);
        Parse->SetGetCallBackID();
    }
    // std::cout << "get header field "  << str << std::endl;
    return 0;
}
int on_header_value(http_parser* parser, const char* at, size_t len)
{
    std::string str;
    str.assign(at,len);
    Parser* Parse = static_cast<Parser*>(parser->data);
    if(Parse->CheckCallBackID())
    {
        Parse->SetCallBackID(std::strtol(str.data(),nullptr,10));
    }
    // std::cout << "get header value "  << str << std::endl;
    return 0;
}
int on_headers_complete(http_parser* parser)
{
    // std::cout << "on header complete "  << std::endl;
    return 0;
}
int on_body(http_parser* parser, const char* at, size_t len)
{
    std::string str;
    str.assign(at,len);
    // std::cout << "get body "  << str << std::endl;
    Parser* Parse = static_cast<Parser*>(parser->data);
    Parse->SetBody(at,len);
    
    // auto f1 = [=](){
    //     EventHandler::GetInstance().ProcessData(message,Parse->GetContext()->GetConnection()->GetAgent(),
    // Parse->GetContext()->GetFD(),Parse->GetCallBackID());
    // };
   


    return 0;
}
int on_message_complete(http_parser* parser)
{
    // std::cout << "complete" << std::endl;
    return 0;
}
Parser::Parser(){
    parser.data = this;
    http_parser_settings_init(&settings);
    settings.on_message_begin = on_message_begin;
    settings.on_url = on_url;
    settings.on_header_field = on_header_field;
    settings.on_header_value = on_header_value;
    settings.on_headers_complete = on_headers_complete;
    settings.on_body = on_body;
    settings.on_message_complete = on_message_complete;
    http_parser_init(&parser, HTTP_REQUEST);
    gotCallBackID = false;
}
std::pair<std::shared_ptr<Message>,int> Parser::Execute(const char* data,int size)
{
    // std::string str(data.begin(),data.end());
    // // std::cout << str;
    // std::string request("GET / HTTP/1.1\r\n"
    //                     "Host: 127.0.0.1\r\n"
    //                     "User-Agent: curl/7.61.1\r\n"
    //                     "Accept: */*\r\n"
    //                     "Connection: keep-alive\r\n\r\n");
    // // std::cout << request;
    size_t nparsed = http_parser_execute(&parser, &settings, data, size);
    if (nparsed != size) {
        // std::cout << "Error: " << http_errno_description(HTTP_PARSER_ERRNO(&parser)) << std::endl;
    }
    if(bodyData != nullptr)
    {
        std::shared_ptr<Message>message= std::make_shared<Message>(bodyData,bodyLen);
        int callBackID = GetCallBackID();
        // std::cout << "parse get CallBackID:" << callBackID << std::endl;
        return {message,callbackID};
    }
    return {nullptr,-1};
}



void Parser::SetGetCallBackID()
{
    gotCallBackID = true;
}

void Parser::SetCallBackID(int CallBackID)
{
    callbackID = CallBackID;
}

bool Parser::CheckCallBackID()
{
    return gotCallBackID;
}

int Parser::GetCallBackID()
{
    gotCallBackID = false;
    return callbackID;
}

void Parser::SetBody(const char* Data,int len)
{
    bodyData = Data;
    bodyLen = len;
}