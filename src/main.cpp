#include "http/http.hpp"

int main() {
    HttpServer server(8888, 3);
    if (!server.SetRoot("../wwwroot")) {
        std::cerr << "SetRoot failed" << std::endl;
        return 1;
    }
    server.Get("/hello", [](const HttpRequest& req, HttpResponse& resp) {
        std::string s;
        s += "<html><head><title>hello</title></head><body><h1>hello</h1></body></html>";
        resp.SetContent(s, "text/html");
    });
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
    server.Post("/login", echo);
    server.Put("/1234.txt", echo);
    server.Delete("/1234.txt", echo);
    server.Start();

    return 0;
}