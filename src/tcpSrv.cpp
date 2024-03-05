#include "server.hpp"

void HandleClose(Channel* channel) {
    std::cout << "Close: " << channel->GetFd() << std::endl;
    channel->Remove();
    // 注意防止重复释放
    delete channel;
}

void HandleRead(Channel* channel) {
    int fd = channel->GetFd();
    char buf[1024]{};
    int ret = recv(fd, buf, sizeof(buf) - 1, 0);
    if (ret < 0) return;
    std::cout << "Recv: " << buf << std::endl;
    channel->EnableWrite();
}

void HandleWrite(Channel* channel) {
    int fd = channel->GetFd();
    const char* data = "Hello, world!";
    int ret = send(fd, data, strlen(data), 0);
    if (ret < 0) return;
    channel->DisableWrite();
}

void HandleError(Channel* channel) {
    lg(Warning, "Error channel: %d", channel->GetFd());
}

void HandleEvent(Channel* channel) {
    std::cout << "Event: " << channel->GetFd() << std::endl;
}

void Accepter(Poller* poller, Socket* lst_sock) {
    int fd = lst_sock->GetFd();
    int newfd = accept(fd, NULL, NULL);
    if (newfd < 0) return;
    // 为新连接设置回调函数
    Channel* channel = new Channel(newfd, poller);
    channel->SetReadCallback(std::bind(HandleRead, channel));
    channel->SetWriteCallback(std::bind(HandleWrite, channel));
    channel->SetCloseCallback(std::bind(HandleClose, channel));
    channel->SetErrorCallback(std::bind(HandleError, channel));
    channel->SetEventCallback(std::bind(HandleEvent, channel));
    channel->EnableRead();
}

int main() {
    Poller poller;
    Socket lst_sock;
    lst_sock.CreateServer(8888);
    // 为监听套接字设置回调函数
    Channel channel(lst_sock.GetFd(), &poller);
    channel.SetReadCallback(std::bind(Accepter, &poller, &lst_sock));
    channel.EnableRead();
    for (;;) {
        std::vector<Channel*> actives;
        poller.Poll(actives);
        for (auto& channel : actives) {
            channel->HandleEvent();
        }
    }

    return 0;
}