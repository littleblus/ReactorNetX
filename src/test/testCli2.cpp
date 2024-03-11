// 长连接测试, 向服务器发送1次数据直到超时
#include "../server.hpp"
#include <cassert>

int main() {
    Socket cli_sock;
    std::cout << cli_sock.CreateClient(8888, "127.0.0.1") << std::endl;
    std::string req = "GET /hello HTTP/1.1\r\n";
    req += "Connection: keep-alive\r\n";
    req += "Content-Length: 0\r\n";
    req += "\r\n";
    for (;;) {
        assert(cli_sock.Send(req.c_str(), req.size()) != -1);
        char buf[1024]{};
        assert(cli_sock.Recv(buf, sizeof(buf) - 1) != -1);
        std::cout << buf;
        sleep(20);
    }
    
    return 0;
}