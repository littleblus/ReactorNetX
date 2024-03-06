#include "server.hpp"

int main() {
    Socket cli_sock;
    cli_sock.CreateClient(8888, "127.0.0.1");
    for (int i = 0; i < 5; i++) {
        std::string str = "hello, i am client";
        cli_sock.Send(str.data(), str.size());
        char buf[1024]{};
        cli_sock.Recv(buf, sizeof(buf) - 1);
        std::cout << "Recv: " << buf << std::endl;
        sleep(1);
    }
    int cnt = 0;
    for (int i = 0; i < 15; i++) {
        std::cout << "sleep cnt: " << ++cnt << std::endl;
        sleep(1);
    }
    for (int i = 0; i < 5; i++) {
        std::string str = "hello, i am client";
        cli_sock.Send(str.data(), str.size());
        char buf[1024]{};
        cli_sock.Recv(buf, sizeof(buf) - 1);
        std::cout << "Recv: " << buf << std::endl;
        sleep(1);
    }
    cli_sock.Close();

    return 0;
}