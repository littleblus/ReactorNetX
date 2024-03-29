#pragma once
#include "server.hpp"

class EchoServer {
private:
    void ConnectionDestroy(const PtrConnection& conn) {
        std::cout << "Disconnected: " << conn->GetId() << std::endl;
    }

    void OnConnected(const PtrConnection& conn) {
        std::cout << "Connected: " << conn->GetId() << std::endl;
    }

    void OnMessage(const PtrConnection& conn, Buffer* buffer) {
        std::string msg = buffer->ReadAsString(buffer->ReadableSize(), true);
        conn->Send(msg.data(), msg.size());
        conn->Shutdown();
    }
public:
    EchoServer(int port, int threadNum = 0)
        : _server(port, threadNum) {
        // _server.SetConnectedCallback(std::bind(&EchoServer::OnConnected, this, std::placeholders::_1));
        _server.SetMessageCallback([this](auto && PH1, auto && PH2) { OnMessage(std::forward<decltype(PH1)>(PH1), std::forward<decltype(PH2)>(PH2)); });
        // _server.SetCloseCallback(std::bind(&EchoServer::ConnectionDestroy, this, std::placeholders::_1));
    }
    void Start() { _server.Start(); }
private:
    TcpServer _server;
};