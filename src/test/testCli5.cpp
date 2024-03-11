// 一次性发送多条数据, 每一条都应该被正常处理
#include "../server.hpp"
#include <cassert>

int main() {
    Socket cli_sock;
    std::cout << cli_sock.CreateClient(8888, "127.0.0.1") << std::endl;
    std::string req = "GET /hello HTTP/1.1\r\n";
    req += "Connection: keep-alive\r\n";
    req += "Content-Length: 0\r\n";
    req += "\r\n";

    std::string muti_req;
    for (int i = 0; i < 3; ++i) {
        muti_req += req;
    }
    for (;;) {
        assert(cli_sock.Send(muti_req.c_str(), muti_req.size()) != -1);
        char buf[1024]{};
        assert(cli_sock.Recv(buf, sizeof(buf) - 1) != -1);
        std::cout << buf << std::endl;
        sleep(3);
    }

    return 0;
}