#pragma once

#include <iostream>
#include <filesystem>
#include <string>
#include <cstdarg>
#include <ctime>

enum printMethod { Screen, Onefile, Classfile };
enum level { Info, Debug, Warning, Error, Fatal };

const int SIZE = 1024;
printMethod DEFAULT_METHOD = Screen;
std::string DEFAULT_PATH = "./log/";
std::string DEFAULT_NAME = "log.txt";

class Log {
public:
    Log(printMethod method = DEFAULT_METHOD) : _ptype(method), _path(DEFAULT_PATH) {}

    void changeMode(printMethod method) {
        _ptype = method;
    }

    std::string levelToString(int level) {
        switch (level) {
        case Info:
            return "Info";
        case Debug:
            return "Debug";
        case Warning:
            return "Warning";
        case Error:
            return "Error";
        case Fatal:
            return "Fatal";
        default:
            return "None";
        }
    }

    void printLog(int level, const std::string& logtxt) {
        switch (_ptype) {
        case Screen:
            std::cout << logtxt << std::endl;
            break;
        case Onefile:
            printOneFile(DEFAULT_NAME, logtxt);
            break;
        case Classfile:
            printClassFile(DEFAULT_NAME, level, logtxt);
            break;
        default:
            break;
        }
    }

    void printOneFile(const std::string& logname, const std::string& logtxt) {
        std::string _logname = _path + logname;

        // 判断文件是否存在
        std::filesystem::path dir(_path);
        if (!std::filesystem::exists(dir)) {
            std::filesystem::create_directories(dir);
        }

        FILE* fp = fopen(_logname.c_str(), "a");
        if (fp == nullptr) return;
        fwrite(logtxt.c_str(), logtxt.size(), 1, fp);
        fclose(fp);
    }

    void printClassFile(const std::string& logname, int level, const std::string& logtxt) {
        std::string filename = logname;
        filename += ".";
        filename += levelToString(level);
        printOneFile(filename, logtxt);
    }

    void operator()(int level, const char* format, ...) {
        time_t t = time(nullptr);
        struct tm* ctime = localtime(&t);
        char leftbuffer[SIZE];
        snprintf(leftbuffer, sizeof(leftbuffer), "[%s][%d-%d-%d %d:%d:%d]", levelToString(level).c_str(),
            ctime->tm_year + 1900, ctime->tm_mon + 1, ctime->tm_mday,
            ctime->tm_hour, ctime->tm_min, ctime->tm_sec);

        va_list s;
        va_start(s, format);
        char rightbuffer[SIZE];
        vsnprintf(rightbuffer, sizeof(rightbuffer), format, s);
        va_end(s);

        // 格式：默认部分+自定义部分
        char logtxt[SIZE * 2 + 10];
        snprintf(logtxt, sizeof(logtxt), "%s %s\n", leftbuffer, rightbuffer);
        printLog(level, logtxt);
    }
private:
    printMethod _ptype;
    std::string _path;
};