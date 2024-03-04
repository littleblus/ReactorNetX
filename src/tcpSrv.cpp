#include "server.hpp"

void HandleRead(int fd) {

}

void HandleWrite(int fd) {

}

void HandleClose(int fd) {

}

void HandleError(int fd) {

}

void HandleEvent(int fd) {

}

void Accepter(Poller* poller, Socket* lst_sock) {
    int fd = lst_sock->GetFd();
    int newfd = accept(fd, NULL, NULL);
    if (newfd < 0) return;
    Channel* channel = new Channel(newfd, poller);
    channel->SetReadCallback();
    channel->SetWriteCallback();
    channel->SetCloseCallback();
    channel->SetErrorCallback();
    channel->SetEventCallback();
    channel->EnableRead();
}

int main() {
    Poller poller;
    Socket lst_sock;
    lst_sock.CreateServer(8888);
    Channel channel(lst_sock.GetFd(), &poller);
    channel.SetReadCallback(std::bind(&Socket::Accept, &lst_sock));

    return 0;
}