// 业务处理超时测试, 当服务器达到了性能瓶颈（花费太长时间超过了非活跃超时时间）
// 在一次业务处理中耗费太长时间, 导致其他连接也被连累超时

#include "../server.hpp"
#include <cassert>

int main() {
    signal(SIGCHLD, SIG_IGN);
    for (int i = 0; i < 10; i++) {
        auto pid = fork();
        if (pid == 0) {
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
                std::cout << buf << std::endl;
            }
        }
    }

    return 0;
}