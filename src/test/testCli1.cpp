// 长连接测试, 持续向服务器发送数据直到超时
#include "../server.hpp"

int main() {
    Socket cli_sock;
    cli_sock.CreateClient(8888, "127.0.0.1");
    std::string req = "GET /hello HTTP/1.1\r\n";
    req += "Connection: keep-alive\r\n";
    req += "Content-Length: 0\r\n";
    req += "\r\n";
    for (;;) {
        cli_sock.Send(req.c_str(), req.size());
        char buf[1024]{};
        int n = cli_sock.Recv(buf, sizeof(buf));
        if (n <= 0) {
            break;
        }
        std::cout << std::string(buf, n) << std::endl;
        sleep(3);
    }
    std::cout << "client exit" << std::endl;

    return 0;
}