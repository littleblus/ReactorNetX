// 大文件传输测试
#include "../server.hpp"
#include <cassert>
#include <fstream>

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

int main() {
    Socket cli_sock;
    std::cout << cli_sock.CreateClient(8888, "127.0.0.1") << std::endl;
    std::string req = "PUT /1234.txt HTTP/1.1\r\nConnection: keep-alive\r\n";

    std::string body;
    ReadFile("../large.txt", body);
    req.append("Content-Length: " + std::to_string(body.size()) + "\r\n\r\n");

    assert(cli_sock.Send(req.c_str(), req.size()) != -1);
    assert(cli_sock.Send(body.c_str(), body.size()) != -1);
    char buf[1024]{};
    assert(cli_sock.Recv(buf, sizeof(buf) - 1) != -1);
    std::cout << buf << std::endl;
    sleep(3);

    return 0;
}