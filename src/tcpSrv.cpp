#include "server.hpp"

void ConnectionDestroy(const PtrConnection& conn) {
    std::cout << "Disconnected: " << conn->GetId() << std::endl;
}

void OnConnected(const PtrConnection& conn) {
    std::cout << "Connected: " << conn->GetId() << std::endl;
}

void OnMessage(const PtrConnection& conn, Buffer* buffer) {
    std::string msg = buffer->ReadAsString(buffer->ReadableSize(), true);
    std::cout << "Received: " << msg << std::endl;
    std::string rsp = "Hello, " + msg;
    conn->Send(rsp.data(), rsp.size());
}

int main() {
    TcpServer srv(8888, 2);
    srv.EnableInactivityRelease(10);
    srv.SetCloseCallback(ConnectionDestroy);
    srv.SetConnectedCallback(OnConnected);
    srv.SetMessageCallback(OnMessage);
    srv.Start();

    return 0;
}