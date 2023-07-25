#pragma once
#include <json/json.h>
#include <vector>
#include "const.h"
class Message
{
    private:    
        char Data[MAX_BUF];
        int HasWrite;
        int Len;
        bool Last;
    public:
        // int ProcessData();
        Message(const char* s,int len,bool LastMessage = false);
        Message(std::string,bool LastMessage = false);
        bool GetLast();
        const char* GetData();
        int GetWritePos();
        int SetHasWrite(int size);
        int GetLen();
};