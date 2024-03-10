#pragma once
#include "../server.hpp"
#include <fstream>
#include <sys/stat.h>

namespace Util {
    // 分割字符串
    std::vector<std::string> Split(const std::string& str, const std::string& delim) {
        if (delim.empty()) { return {str}; }
        std::vector<std::string> tokens;
        size_t offset = 0;
        for (size_t pos = str.find(delim, offset); pos != std::string::npos; pos = str.find(delim, offset)) {
            if (pos == offset) { // 连续分隔符
                offset += delim.size();
                continue;
            }
            std::string token = str.substr(offset, pos - offset);
            offset = pos + delim.size();
            tokens.push_back(token);
        }
        tokens.push_back(str.substr(offset));
        // 去除末尾的空字符串
        if (!tokens.empty() && tokens.back().empty()) {
            tokens.pop_back();
        }
        return tokens;
    }
    // 读取文件内容
    bool ReadFile(const std::string& file, std::string& buf) {
        std::ifstream ifs(file, std::ios::binary);
        if (!ifs) {
            lg(Error, "open file failed: %s", file.c_str());
            return false;
        }
        ifs.seekg(0, std::ios::end);
        auto size = ifs.tellg();
        ifs.seekg(0, std::ios::beg);
        buf.resize(size, 0);
        ifs.read(&buf[0], size);
        if (ifs.bad()) {
            lg(Error, "read file failed: %s", file.c_str());
            return false;
        }
        return true;
    }
    // 写入文件内容(覆盖原有内容)
    bool WriteFile(const std::string& file, const std::string& buf) {
        std::ofstream ofs(file, std::ios::binary | std::ios::trunc);
        if (!ofs) {
            lg(Error, "open file failed: %s", file.c_str());
            return false;
        }
        ofs.write(buf.c_str(), buf.size());
        if (ofs.bad()) {
            lg(Error, "write file failed: %s", file.c_str());
            return false;
        }
        return true;
    }
    // URL解码
    std::string UrlDecode(const std::string& str, bool plus_as_space = false) {
        std::string result;
        for (size_t i = 0; i < str.size(); i++) {
            if (str[i] == '%') {
                std::string hex = str.substr(i + 1, 2);
                int c = std::stoi(hex, nullptr, 16);
                result += static_cast<char>(c);
                i += 2;
            }
            else if (str[i] == '+' && plus_as_space) {
                result += ' ';
            }
            else {
                result += str[i];
            }
        }
        return result;
    }
    // URL编码
    std::string UrlEncode(const std::string& str, bool space_as_plus = false) {
        std::string result;
        for (auto c : str) {
            if (c == '.' || c == '-' || c == '_' || c == '~' || isalnum(c)) {
                result += c;
                continue;
            }
            if (c == ' ' && space_as_plus) {
                result += '+';
                continue;
            }
            char buf[4]{};
            snprintf(buf, sizeof(buf), "%%%02X", c);
            result += buf;
        }
        return result;
    }
    // 响应状态码的描述
    std::string StatusCodeDescription(int code) {
        std::unordered_map<int, std::string> status_code_description = {
            {100, "Continue"},
            {101, "Switching Protocols"},
            {200, "OK"},
            {201, "Created"},
            {202, "Accepted"},
            {203, "Non-Authoritative Information"},
            {204, "No Content"},
            {205, "Reset Content"},
            {206, "Partial Content"},
            {300, "Multiple Choices"},
            {301, "Moved Permanently"},
            {302, "Found"},
            {303, "See Other"},
            {304, "Not Modified"},
            {305, "Use Proxy"},
            {307, "Temporary Redirect"},
            {400, "Bad Request"},
            {401, "Unauthorized"},
            {402, "Payment Required"},
            {403, "Forbidden"},
            {404, "Not Found"},
            {405, "Method Not Allowed"},
            {406, "Not Acceptable"},
            {407, "Proxy Authentication Required"},
            {408, "Request Timeout"},
            {409, "Conflict"},
            {410, "Gone"},
            {411, "Length Required"},
            {412, "Precondition Failed"},
            {413, "Payload Too Large"},
            {414, "URI Too Long"},
            {415, "Unsupported Media Type"},
            {416, "Range Not Satisfiable"},
            {417, "Expectation Failed"},
            {426, "Upgrade Required"},
            {500, "Internal Server Error"},
            {501, "Not Implemented"},
            {502, "Bad Gateway"},
            {503, "Service Unavailable"},
            {504, "Gateway Timeout"},
            {505, "HTTP Version Not Supported"}
        };
        auto it = status_code_description.find(code);
        if (it != status_code_description.end()) {
            return it->second;
        }
        return "Unknown";
    }
    // 获取文件的MIME类型
    std::string GetMimeType(const std::string& file) {
        std::unordered_map<std::string, std::string> mime_types = {
                {".html", "text/html"},
                {".css", "text/css"},
                {".js", "application/javascript"},
                {".json", "application/json"},
                {".xml", "application/xml"},
                {".jpg", "image/jpeg"},
                {".jpeg", "image/jpeg"},
                {".png", "image/png"},
                {".gif", "image/gif"},
                {".bmp", "image/bmp"},
                {".webp", "image/webp"},
                {".svg", "image/svg+xml"},
                {".ico", "image/x-icon"},
                {".pdf", "application/pdf"},
                {".zip", "application/zip"},
                {".7z", "application/x-7z-compressed"},
                {".rar", "application/x-rar-compressed"},
                {".tar", "application/x-tar"},
                {".gz", "application/gzip"},
                {".mp3", "audio/mpeg"},
                {".mp4", "video/mp4"},
                {".wav", "audio/wav"},
                {".flac", "audio/flac"},
                {".ogg", "audio/ogg"},
                {".webm", "video/webm"},
                {".mkv", "video/x-matroska"},
                {".avi", "video/x-msvideo"},
                {".doc", "application/msword"},
                {".docx", "application/vnd.openxmlformats-officedocument.wordprocessingml.document"},
                {".xls", "application/vnd.ms-excel"},
                {".xlsx", "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"},
                {".ppt", "application/vnd.ms-powerpoint"},
                {".pptx", "application/vnd.openxmlformats-officedocument.presentationml.presentation"},
                {".csv", "text/csv"},
                {".txt", "text/plain"},
                {".log", "text/plain"}
        };
        auto pos = file.rfind('.');
        if (pos == std::string::npos) {
            return "application/octet-stream";
        }
        auto it = mime_types.find(file.substr(pos));
        if (it != mime_types.end()) {
            return it->second;
        }
        return "application/octet-stream";
    }
    // 判断文件是否是一个目录
    bool IsDirectory(const std::string& file) {
        struct stat st{};
        if (stat(file.c_str(), &st) == 0) {
            return S_ISDIR(st.st_mode);
        }
        else {
            return false;
        }
    }
    // 判断文件是否是一个普通文件
    bool IsRegularFile(const std::string& file) {
        struct stat st{};
        if (stat(file.c_str(), &st) == 0) {
            return S_ISREG(st.st_mode);
        }
        else {
            return false;
        }
    }
    // 资源路径是否有效
    bool ResourcePathValid(const std::string& path) {
        int level = 0;
        auto tokens = Split(path, "/");
        for (const auto& token : tokens) {
            if (token == "..") {
                level--;
            }
            else if (token == ".") {
                continue;
            }
            else {
                level++;
            }
            if (level < 0) {
                return false;
            }
        }
        return true;
    }
}