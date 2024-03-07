#include "server.hpp"

std::unordered_map<uint64_t, PtrConnection> g_conns;
EventLoop loop;

void ConnectionDestroy(const PtrConnection& conn) {
    g_conns.erase(conn->GetId());
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
    //conn->Shutdown();
}

uint64_t conn_id = 1;

void NewConnection(int fd) {
    // 为新连接设置回调函数
    PtrConnection conn(new Connection(&loop, conn_id, fd));
    conn->SetMessageCallback(std::bind(OnMessage, std::placeholders::_1, std::placeholders::_2));
    conn->SetCloseCallback(std::bind(ConnectionDestroy, std::placeholders::_1));
    conn->SetConnectedCallback(std::bind(OnConnected, std::placeholders::_1));
    conn->EnableInactivityRelease(10);
    conn->Establish();
    g_conns[conn_id++] = conn;
}

int main() {
    Accepter accepter(&loop, 8888, std::bind(NewConnection, std::placeholders::_1));
    accepter.Listen();
    for (;;) {
        loop.Start();
    }

    return 0;
}