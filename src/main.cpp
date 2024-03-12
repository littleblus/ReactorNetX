#include "http/http.hpp"

const std::string WWWROOT = "../wwwroot/";

void PutFile(const HttpRequest& req, HttpResponse& resp) {
    std::string pathname = WWWROOT + req._path;
    Util::WriteFile(pathname, req._body);
}

int main() {
    HttpServer server(8888, 20, 30);
    if (!server.SetRoot(WWWROOT)) {
        std::cerr << "SetRoot failed" << std::endl;
        return 1;
    }
    auto echo = [](const HttpRequest& req, HttpResponse& resp) {
        std::string s;
        s += req._method + " " + req._path + " " + req._version + "\r\n";
        for (auto& p : req._params) {
            s += p.first + ": " + p.second + "\r\n";
        }
        for (auto& p : req._headers) {
            s += p.first + ": " + p.second + "\r\n";
        }
        s += "\r\n";
        s += req._body;
        resp.SetContent(s, "text/plain");
        };
    server.Get("/hello", echo);
    server.Start();

    return 0;
}