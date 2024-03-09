#include "echo.hpp"

int main() {
    EchoServer srv(8888, 2);
    srv.Start();
    return 0;
}