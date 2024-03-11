#pragma once
#include "../server.hpp"
#include <fstream>
#include <sys/stat.h>
#include <regex>

const int kMaxHttpLineSize = 8192;

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

class HttpRequest {
public:
    void Clear() {
        _method.clear();
        _path.clear();
        _version = "HTTP/1.1";
        _body.clear();
        _headers.clear();
        _params.clear();
        std::smatch match;
        _matches.swap(match);
    }
    // 插入头部字段
    void SetHeader(const std::string& key, const std::string& value) {
        _headers[key] = value;
    }
    // 是否存在指定的头部字段
    bool HasHeader(const std::string& key) const {
        return _headers.find(key) != _headers.end();
    }
    // 获取指定头部字段的值
    std::string GetHeader(const std::string& key) const {
        auto it = _headers.find(key);
        if (it != _headers.end()) {
            return it->second;
        }
        return "";
    }
    // 插入URL参数
    void SetParam(const std::string& key, const std::string& value) {
        _params[key] = value;
    }
    // 是否存在指定的URL参数
    bool HasParam(const std::string& key) const {
        return _params.find(key) != _params.end();
    }
    // 获取指定URL参数的值
    std::string GetParam(const std::string& key) const {
        auto it = _params.find(key);
        if (it != _params.end()) {
            return it->second;
        }
        return "";
    }
    // 获取正文长度
    size_t GetContentLength() const {
        return HasHeader("Content-Length") ? std::stoul(GetHeader("Content-Length")) : 0;
    }
    // 是否是短连接
    bool IsKeepAlive() const {
        return HasHeader("Connection") && GetHeader("Connection") == "keep-alive";
    }

    std::string _method;
    std::string _path;
    std::string _version = "HTTP/1.1";
    std::string _body;
    std::smatch _matches;
    std::unordered_map<std::string, std::string> _headers; // 头部字段
    std::unordered_map<std::string, std::string> _params; // URL参数
};

class HttpResponse {
public:
    explicit HttpResponse(int status_code = 200) : _status_code(status_code), _redirect(false) {}

    void Clear() {
        _status_code = 200;
        _redirect = false;
        _redirect_url.clear();
        _body.clear();
        _headers.clear();
    }
    void SetHeader(const std::string& key, const std::string& value) {
        _headers[key] = value;
    }
    bool HasHeader(const std::string& key) const {
        return _headers.find(key) != _headers.end();
    }
    std::string GetHeader(const std::string& key) const {
        auto it = _headers.find(key);
        if (it != _headers.end()) {
            return it->second;
        }
        return "";
    }
    void SetContent(const std::string& body, const std::string& mime_type = "text/plain") {
        _body = body;
        SetHeader("Content-Type", mime_type);
        SetHeader("Content-Length", std::to_string(_body.size()));
    }
    void SetRedirect(const std::string& url, int status_code = 302) {
        _redirect = true;
        _redirect_url = url;
        _status_code = status_code;
    }
    bool IsKeepAlive() const {
        return HasHeader("Connection") && GetHeader("Connection") == "keep-alive";
    }
    int _status_code;
    bool _redirect;
    std::string _redirect_url;
    std::string _body;
    std::unordered_map<std::string, std::string> _headers; // 头部字段
};

enum class HttpRecvState {
    kRECV_HTTP_LINE,
    kRECV_HTTP_HEAD,
    kRECV_HTTP_BODY,
    kRECV_HTTP_DONE,
    kRECV_HTTP_ERROR
};

class HttpContext {
public:
    HttpContext() : _resp_state(200), _recv_state(HttpRecvState::kRECV_HTTP_LINE) {}
    int GetRespState() const { return _resp_state; }
    HttpRecvState GetRecvState() const { return _recv_state; }
    HttpRequest& GetRequest() { return _request; }
    // 接受并解析HTTP请求
    void RecvHttpRequest(Buffer* buf) {
        switch (_recv_state) {
        case HttpRecvState::kRECV_HTTP_LINE:
            RecvHttpLine(buf);
        case HttpRecvState::kRECV_HTTP_HEAD:
            RecvHttpHead(buf);
        case HttpRecvState::kRECV_HTTP_BODY:
            RecvHttpBody(buf);
        }
    }
    void Clear() {
        _resp_state = 200;
        _recv_state = HttpRecvState::kRECV_HTTP_LINE;
        _request.Clear();
    }
private:
    bool RecvHttpLine(Buffer* buf) {
        if (_recv_state != HttpRecvState::kRECV_HTTP_LINE) {
            return false;
        }
        auto line = buf->ReadLine();
        // 缓冲区中没有完整的一行
        if (line.empty()) {
            if (buf->ReadableSize() > kMaxHttpLineSize) {
                _recv_state = HttpRecvState::kRECV_HTTP_ERROR;
                _resp_state = 414; // URI Too Long
                return false;
            }
            return true;
        }
        // 解析HTTP请求行
        if (line.size() > kMaxHttpLineSize) {
            _recv_state = HttpRecvState::kRECV_HTTP_ERROR;
            _resp_state = 414;
            return false;
        }
        if (parseHttpLine(line)) {
            buf->MoveReadIdx(line.size());
            _recv_state = HttpRecvState::kRECV_HTTP_HEAD;
            return true;
        }
        else {
            return false;
        }
    }
    bool RecvHttpHead(Buffer* buf) {
        if (_recv_state != HttpRecvState::kRECV_HTTP_HEAD) {
            return false;
        }
        // 一行一行读, 直到遇到空行
        for (;;) {
            auto line = buf->ReadLine();
            // 缓冲区中没有完整的一行
            if (line.empty()) {
                if (buf->ReadableSize() > kMaxHttpLineSize) {
                    _recv_state = HttpRecvState::kRECV_HTTP_ERROR;
                    _resp_state = 414; // URI Too Long
                    return false;
                }
                return true;
            }
            // 解析HTTP请求行
            if (line.size() > kMaxHttpLineSize) {
                _recv_state = HttpRecvState::kRECV_HTTP_ERROR;
                _resp_state = 414;
                return false;
            }
            if (line == "\n" || line == "\r\n") {
                buf->MoveReadIdx(line.size());
                break;
            }
            if (parseHttpHead(line)) {
                buf->MoveReadIdx(line.size());
            }
            else {
                return false;
            }
        }
        _recv_state = HttpRecvState::kRECV_HTTP_BODY;
        return true;
    }
    bool RecvHttpBody(Buffer* buf) {
        if (_recv_state != HttpRecvState::kRECV_HTTP_BODY) {
            return false;
        }
        // 读取Content-Length长度的数据
        size_t content_length = _request.GetContentLength();
        if (content_length == 0) {
            _recv_state = HttpRecvState::kRECV_HTTP_DONE;
            return true;
        }
        // 缓冲区中没有足够的数据
        size_t need_size = content_length - _request._body.size();
        if (buf->ReadableSize() < need_size) {
            _request._body += buf->ReadAsString(buf->ReadableSize(), true);
            return true;
        }
        else {
            _request._body += buf->ReadAsString(need_size, true);
            _recv_state = HttpRecvState::kRECV_HTTP_DONE;
            return true;
        }
    }
    bool parseHttpLine(const std::string& line) {
        std::smatch matches;
        std::regex e("(GET|POST|PUT|DELETE|HEAD|OPTIONS|TRACE|CONNECT) ([^?]*)(?:\\?(.*))? (HTTP/1\\.[01])(?:\n|\r\n)", std::regex::icase);
        // GET www.baidu.com/login?username=123&password=456 HTTP/1.1
        // 1 : GET
        // 2 : www.baidu.com/login
        // 3 : username=123&password=456
        // 4 : HTTP/1.1
        if (std::regex_match(line, matches, e)) {
            _request._method = matches[1];
            // 转换成大写
            std::transform(_request._method.begin(), _request._method.end(), _request._method.begin(), ::toupper);
            _request._path = Util::UrlDecode(matches[2], false);
            _request._version = matches[4];
            // key=value&key=value
            auto queries = Util::Split(matches[3], "&");
            for (const auto& query : queries) {
                auto pos = query.find('=');
                if (pos != std::string::npos) {
                    std::string k = Util::UrlDecode(query.substr(0, pos), true);
                    std::string v = Util::UrlDecode(query.substr(pos + 1), true);
                    _request.SetParam(k, v);
                }
                else {
                    _recv_state = HttpRecvState::kRECV_HTTP_ERROR;
                    _resp_state = 400; // Bad Request
                    return false;
                }
            }
            return true;
        }
        else {
            _recv_state = HttpRecvState::kRECV_HTTP_ERROR;
            _resp_state = 400; // Bad Request
            return false;
        }
    }
    bool parseHttpHead(const std::string& head) {
        // key: value\r\n
        auto pos = head.find(": ");
        if (pos != std::string::npos) {
            std::string k = head.substr(0, pos);
            std::string v = head.substr(pos + 2);
            _request.SetHeader(k, v);
            return true;
        }
        else {
            _recv_state = HttpRecvState::kRECV_HTTP_ERROR;
            _resp_state = 400; // Bad Request
            return false;
        }
    }
private:
    int _resp_state;
    HttpRecvState _recv_state;
    HttpRequest _request;
};

class HttpServer {
public:
    using Handler = std::function<void(const HttpRequest&, HttpResponse&)>;
    using Handlers = std::vector<std::pair<std::regex, Handler>>;
    HttpServer(int port, int _thread_num, int timeout = 30) : _server(port, _thread_num) {
        _server.SetConnectedCallback([this](auto && PH1) { OnConnected(std::forward<decltype(PH1)>(PH1)); });
        _server.SetMessageCallback([this](auto && PH1, auto && PH2) { OnMessage(std::forward<decltype(PH1)>(PH1), std::forward<decltype(PH2)>(PH2)); });
        _server.EnableInactivityRelease(timeout);
    }
    // 设置静态资源根目录
    bool SetRoot(std::string root) {
        if (root.back() != '/') root += "/";
        if (!Util::IsDirectory(root)) return false;
        _root = root;
        return true;
    }
    // 添加GET处理函数
    void Get(const std::string& pattern, const Handler& handler) {
        _get_handlers.emplace_back(std::regex(pattern), handler);
    }
    // 添加POST处理函数
    void Post(const std::string& pattern, const Handler& handler) {
        _post_handlers.emplace_back(std::regex(pattern), handler);
    }
    // 添加PUT处理函数
    void Put(const std::string& pattern, const Handler& handler) {
        _put_handlers.emplace_back(std::regex(pattern), handler);
    }
    // 添加DELETE处理函数
    void Delete(const std::string& pattern, const Handler& handler) {
        _delete_handlers.emplace_back(std::regex(pattern), handler);
    }
    void Start() { _server.Start(); }
private:
    void WriteResponse(const PtrConnection& conn, const HttpRequest& req, HttpResponse& resp) {
        if (req.IsKeepAlive()) {
            resp.SetHeader("Connection", "keep-alive");
        }
        else {
            resp.SetHeader("Connection", "close");
        }
        if (!resp._body.empty() && !resp.HasHeader("Content-Length")) {
            resp.SetHeader("Content-Length", std::to_string(resp._body.size()));
        }
        if (!resp._body.empty() && !resp.HasHeader("Content-Type")) {
            resp.SetHeader("Content-Type", "application/octet-stream");
        }
        if (resp._redirect) {
            resp.SetHeader("Location", resp._redirect_url);
        }
        std::string response = req._version + " " + std::to_string(resp._status_code) + " " + Util::StatusCodeDescription(resp._status_code) + "\r\n";
        for (auto& [key, value] : resp._headers) {
            response += key;
            response += ": ";
            response += value;
            response += "\r\n";
        }
        response += "\r\n";
        response += resp._body;
        conn->Send(response.c_str(), response.size());
    }
    bool IsFileRequest(const HttpRequest& req) {
        if (_root.empty()) return false;
        if (req._method != "GET" && req._method != "HEAD") return false;
        if (!Util::ResourcePathValid(req._path)) return false;
        // 如果请求的末尾是一个/, 则默认请求index.html
        std::string path = _root + req._path;
        if (req._path.back() == '/') {
            path += "/index.html";
        }
        if (!Util::IsRegularFile(path)) return false;
        return true;
    }
    bool FileHandler(HttpRequest& req, HttpResponse& resp) {
        req._path = _root + req._path + (req._path.back() == '/' ? "index.html" : "");
        if (Util::ReadFile(req._path, resp._body)){
            auto mime = Util::GetMimeType(req._path);
            resp.SetHeader("Content-Type", mime);
            resp.SetHeader("Content-Length", std::to_string(resp._body.size()));
            return true;
        }
        else {
            resp._status_code = 404; // Not Found
            return false;
        }
    }
    void ErrorHandler(HttpResponse& resp) {
        std::string body = "<html><head><title>Error</title></head><body><h1>";
        body += std::to_string(resp._status_code) + " " + Util::StatusCodeDescription(resp._status_code);
        body += "</h1></body></html>";
        resp.SetContent(body, "text/html");
    }
    void Dispatch(HttpRequest& req, HttpResponse& resp, Handlers& handlers) {
        for (auto& [key, func] : handlers) {
            if (std::regex_match(req._path, req._matches, key)) {
                func(req, resp);
            }
        }
        resp._status_code = 404; // Not Found
    }
    void Route(HttpRequest& req, HttpResponse& resp) {
        if (IsFileRequest(req)) {
            if (!FileHandler(req, resp)) {
                ErrorHandler(resp);
            }
            return;
        }
        if (req._method == "GET" || req._method == "HEAD") {
            Dispatch(req, resp, _get_handlers);
        }
        else if (req._method == "POST") {
            Dispatch(req, resp, _post_handlers);
        }
        else if (req._method == "PUT") {
            Dispatch(req, resp, _put_handlers);
        }
        else if (req._method == "DELETE") {
            Dispatch(req, resp, _delete_handlers);
        }
        else {
            resp._status_code = 405; // Method Not Allowed
            ErrorHandler(resp);
        }
    }
    void OnConnected(const PtrConnection& conn) {
        conn->SetContext(HttpContext());
    }
    void OnMessage(const PtrConnection& conn, Buffer* buf) {
        while (buf->ReadableSize() > 0) {
            auto context = std::any_cast<HttpContext>(conn->GetContext());
            context->RecvHttpRequest(buf);
            HttpRequest &req = context->GetRequest();
            HttpResponse resp;
            if (context->GetRespState() >= 400) {
                ErrorHandler(resp);
                WriteResponse(conn, req, resp);
                conn->Shutdown();
                return;
            }
            if (context->GetRecvState() != HttpRecvState::kRECV_HTTP_DONE) {
                // 当前请求还未接收完整, 继续等待
                return;
            }
            // 路由查找+处理
            Route(req, resp);
            WriteResponse(conn, req, resp);
            context->Clear();
            if (!resp.IsKeepAlive()) {
                conn->Shutdown();
                return;
            }
        }
    }
private:
    Handlers _get_handlers;
    Handlers _post_handlers;
    Handlers _put_handlers;
    Handlers _delete_handlers;
    std::string _root; // 静态资源根目录
    TcpServer _server;
};