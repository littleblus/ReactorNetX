#include "server.hpp"

std::unordered_map<uint64_t, PtrConnection> g_conns;
std::vector<LoopThread> threads(2);
EventLoop base_loop;

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
int next_loop = 0;

void NewConnection(int fd) {
    // 为新连接设置回调函数
    next_loop = (next_loop + 1) % threads.size();
    auto loop = threads[next_loop].GetLoop();
    PtrConnection conn(new Connection(loop, conn_id, fd));
    conn->SetMessageCallback(std::bind(OnMessage, std::placeholders::_1, std::placeholders::_2));
    conn->SetCloseCallback(std::bind(ConnectionDestroy, std::placeholders::_1));
    conn->SetConnectedCallback(std::bind(OnConnected, std::placeholders::_1));
    conn->EnableInactivityRelease(10);
    conn->Establish();
    g_conns[conn_id++] = conn;
}

int main() {
    Accepter accepter(&base_loop, 8888, std::bind(NewConnection, std::placeholders::_1));
    accepter.Listen();
    for (;;) {
        base_loop.Start();
    }

    return 0;
}