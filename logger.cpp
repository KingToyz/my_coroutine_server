#include "logger.h"

#include <cstdarg>
#include <string>
#include <chrono>
#include <filesystem>
#include <vector>
#include <algorithm>
#include <iostream>

#include <cerrno>
#include <cstring>


Logger::Logger(std::string Path,std::string File,int MaxSize):pathname(Path),filename(File),MaxFileSize(MaxSize) {
    std::filesystem::path directory(pathname);
    std::filesystem::create_directories(directory);

    file = fopen((pathname+filename).c_str(),"a");

    if (file == nullptr)
    {
        int errNum = errno;
        
        std::cout << "[saveYuvFile] open fail errno = +"+ std::to_string(errNum) + ", reason = " + strerror(errNum) << std::endl;
        return;
        
    }

    FileSize = getFileSize(file);
}

Logger::~Logger() {

}


void Logger::RollLogs() {
    
    std::vector<std::string> log_files;

    // 遍历目录并查找以filename开头的文件
    for (const auto& entry : std::filesystem::directory_iterator(pathname)) {
        if (entry.is_regular_file() && entry.path().filename().string().rfind(filename, 0) == 0) {
            log_files.push_back(entry.path().filename().string());
        }
    }

    // 按文件名降序排序
    std::sort(log_files.begin(), log_files.end(), std::greater<>());

    // 重命名文件，将日志文件向后滚动
    for (const auto& log_file : log_files) {
        // 跳过没有后缀的文件
        if (log_file == filename) continue;

        std::string old_path = pathname + "/" + log_file;
        std::string new_path = pathname + "/" + filename + "." + std::to_string(std::stoi(log_file.substr(log_file.find_last_of('.') + 1)) + 1);
        std::filesystem::rename(old_path, new_path);
    }

    // 将没有后缀的文件重命名为".1"
    if (std::filesystem::exists(pathname + "/" + filename)) {
        std::filesystem::rename(pathname + "/" + filename, pathname + "/" + filename + ".1");
    }

    fclose(file);

    file = fopen((pathname+filename).c_str(),"a");

    if (file == nullptr)
    {
        int errNum = errno;
        
        std::cout << "open fail errno = +"+ std::to_string(errNum) + ", reason = " + strerror(errNum) << std::endl;
        return;
        
    }

    FileSize = getFileSize(file);

}

std::string Logger::GetNowTimeString() {
    auto curr = std::chrono::system_clock::now();
    time_t tm = std::chrono::system_clock::to_time_t(curr);
    auto lt = std::localtime(&tm);
    
    char buf[1024];
    int ret = std::snprintf(buf,sizeof(buf),"%04d-%02d-%02d %02d:%02d:%02d",lt->tm_year + 1900,lt->tm_mon + 1,lt->tm_mday ,lt->tm_hour,lt->tm_min,lt->tm_sec);
    return std::string(buf,ret);
}

int Logger::getFileSize(std::FILE* file) {
    // 保存当前文件位置
    int current_position = std::ftell(file);

    // 将文件指针移动到文件末尾
    std::fseek(file, 0, SEEK_END);

    // 获取当前文件位置（即文件大小）
    int file_size = std::ftell(file);

    // 恢复文件指针到原始位置
    std::fseek(file, current_position, SEEK_SET);

    return file_size;
}


void Logger::LogWithLevel(std::string level,...) {

    va_list args;
    va_start(args, level);

    LogWithFormatAndLevel(level,args);

    va_end(args);
}

void Logger::LogWithFormatAndLevel(std::string level,va_list args) {

    std::string str =  "[" + GetNowTimeString() +"][" + level+ "]["+  __FILE__ + "][" + __FUNCTION__ + ":" + std::to_string(__LINE__) + "]:";
    
    FileSize += fprintf(file, "%s", str.c_str());
    const char* strFmt = va_arg(args, const char*);
    
    FileSize += vfprintf(file, strFmt, args);

    if(FileSize > MaxFileSize) {
        RollLogs();
    }
}