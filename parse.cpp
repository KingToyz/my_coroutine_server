#include "parse.h"

#include <iostream>
#include "message.h"
#include <memory>
#include "socket.h"
#include "epoll_agent.h"

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
    else if(str == "Content-Length")
    {
        Parser* Parse = static_cast<Parser*>(parser->data);
        Parse->SetGetContentLength();
    }
    std::cout << "get header field "  << str << std::endl;
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
    else if(Parse->CheckContentLength())
    {   
        Parse->SetContentLength(std::strtol(str.data(),nullptr,10));
    }
    std::cout << "get header value "  << str << std::endl;
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
Parser::Parser(EpollAgent* agent):agent(agent){
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
task<std::pair<std::shared_ptr<Message>,int>> Parser::Execute(const char* data,int size)
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
        int sockfd;
        if( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) { 
            std::cout << "socket create error" << std::endl ;
            co_return {nullptr,-1};
        }
        // co_return {message,callBackID};
        std::cout << "start to connect" << std::endl;
        
        struct sockaddr_in  servaddr;
        memset(&servaddr, 0, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(10009);
        if (inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr) <= 0){
            perror("pton");
        } 
        int ret = connect(sockfd,(struct sockaddr*)&servaddr, sizeof(servaddr));

        if(ret == -1 && errno != EINPROGRESS) {
            std::cout << "connect error" << std::endl;
            exit(-1);
        } else if(ret == 0) {
            std::cout << "connect success!" << std::endl;
        }
        Socket writeS(sockfd,agent);
        size_t bSend = 0;
        while(message->GetLen()- message->GetWritePos() > 0) {
            ssize_t res = co_await writeS.write((void*)(message->GetData()+message->GetWritePos()),message->GetLen()-message->GetWritePos());
            if(res < 0) {
                std::cout << "parse not get sendmessage error!" << std::endl;
                co_return {nullptr,-1};
            }
            message->SetHasWrite(res);
        }
        std::cout << "write:" << message->GetData() << std::endl;
        ssize_t bRecv = 1;
        
        std::string recvStr;
        while(bRecv > 0 && recvStr.size() < ContentLength) {
            char buffer[1024] = {0};
            std::cout << "start to co_await recv" << std::endl;
            bRecv = co_await writeS.recv(buffer,sizeof buffer);
            std::cout << "recv:" << bRecv << std::endl;
            recvStr += std::string(buffer,bRecv);
        }
        std::shared_ptr<Message>sendmessage = std::make_shared<Message>(recvStr);
        std::cout << "parse get sendmessage:" << sendmessage->GetData() << std::endl;
        co_return {sendmessage,callbackID};
    }
    std::cout << "parse not get sendmessage error2!" << std::endl;
    co_return {nullptr,-1};
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

void Parser::SetGetContentLength()
{
    gotContentLength = true;
}
bool Parser::CheckContentLength()
{
    return gotContentLength;
}
void Parser::SetContentLength(int Length)
{
    gotContentLength = false;
    ContentLength = Length;
}