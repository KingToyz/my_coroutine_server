#pragma once
#include "singleton.h"
#include <string>

#define LOG_INFO(...) logger->LogWithLevel("INFO",__VA_ARGS__)


class Logger {
    public:
        Logger(std::string Path,std::string File,int MaxSize);
        ~Logger();
        void LogWithLevel(std::string level,...);
        void LogWithFormatAndLevel(std::string level,va_list args);
        
    private:
        void RollLogs();
        std::string filename;
        std::string pathname;
        std::FILE* file;
        std::string GetNowTimeString();
        int FileSize;
        int MaxFileSize;
        int getFileSize(std::FILE* file);
};

