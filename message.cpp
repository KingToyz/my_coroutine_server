#include "message.h"

Message::Message(const char* s,int len,bool LastMessage):HasWrite(0),Last(LastMessage),Len(len)
{
    strncpy(Data,s,len);
}

Message::Message(std::string s,bool LastMessage):HasWrite(0),Last(LastMessage),Len(s.size())
{
    strncpy(Data,s.c_str(),s.size());
}

const char* Message::GetData()
{
    return Data;
}

bool Message::GetLast()
{
    return Last;
}


int Message::GetWritePos()
{
    return HasWrite;

}
int Message::SetHasWrite(int size)
{
    HasWrite += size;
    return 0;
}   


int Message::GetLen()
{
    return Len;
}