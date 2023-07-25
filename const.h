#pragma once
#include <stdint.h>
#include <string>
const int ReadEvent = 0x12345;
const int WriteEvent = 0x12346;
const int MAX = 1024;
const int MAX_BUF = 10 * 1024;


using EVENTTYPE  = const uint32_t;
EVENTTYPE ACCPET = 0;
EVENTTYPE READ = 1;
EVENTTYPE WRITE = 2;
EVENTTYPE DEL = 3;
EVENTTYPE STARTWRITE = 4;
EVENTTYPE EXIT = 5;
EVENTTYPE ERROR = 6;


using RESPONSETYPE = const uint32_t;
RESPONSETYPE TIMEOUT = 1;
RESPONSETYPE ERRORMETHOD = 2;
RESPONSETYPE SUCCESS = 0; 


const std::string HTTPHEADER = "HTTP/1.1 200 OK\r\n"
                           "Content-Type: text/html\r\n";
                        //    "Content-Length: 29\r\n"
                        //    "\r\n";
