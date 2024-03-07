#include "server.hpp"

std::unordered_map<uint64_t, PtrConnection> g_conns;

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

void Accepter(EventLoop* loop, Socket* lst_sock) {
    int fd = lst_sock->GetFd();
    int newfd = accept(fd, NULL, NULL);
    if (newfd < 0) return;
    // 为新连接设置回调函数
    Channel* channel = new Channel(newfd, loop);
    PtrConnection conn(new Connection(loop, conn_id, newfd));
    conn->SetMessageCallback(std::bind(OnMessage, std::placeholders::_1, std::placeholders::_2));
    conn->SetCloseCallback(std::bind(ConnectionDestroy, std::placeholders::_1));
    conn->SetConnectedCallback(std::bind(OnConnected, std::placeholders::_1));
    conn->EnableInactivityRelease(10);
    conn->Establish();
    g_conns[conn_id++] = conn;
}

int main() {
    EventLoop loop;
    Socket lst_sock;
    lst_sock.CreateServer(8888);
    // 为监听套接字设置回调函数
    Channel channel(lst_sock.GetFd(), &loop);
    channel.SetReadCallback(std::bind(Accepter, &loop, &lst_sock));
    channel.EnableRead();
    for (;;) {
        loop.Start();
    }

    return 0;
}