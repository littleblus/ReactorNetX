// 给服务器发送1024字节数据, 但是实际发送的数据不足1024, 查看处理结果
// 如果只发送一次, 服务器得不到完成的请求, 不会进行业务处理, 直到超时
// 如果发送了多次小请求, 服务器会将后边的请求当作是上一个请求的后续部分, 可能因为错误而导致关闭连接
#include "../server.hpp"
#include <cassert>

int main() {
    Socket cli_sock;
    std::cout << cli_sock.CreateClient(8888, "127.0.0.1") << std::endl;
    std::string req = "GET /hello HTTP/1.1\r\n";
    req += "Connection: keep-alive\r\n";
    req += "Content-Length: 100\r\n";
    req += "\r\n";
    req += "hello world"; // 11字节
    for (;;) {
        assert(cli_sock.Send(req.c_str(), req.size()) != -1);
        char buf[1024]{};
        assert(cli_sock.Recv(buf, sizeof(buf) - 1) != -1);
        std::cout << buf << std::endl;
        sleep(3);
    }

    return 0;
}