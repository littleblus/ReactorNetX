#pragma once

#include "log.hpp"
#include <vector>
#include <stdexcept>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

class Buffer {
private:

    const char* ReadPos() const { return &_buffer[_read_idx]; }
    char* WritePos() { return &_buffer[_write_idx]; }
    std::size_t FrontSize() const { return _read_idx; }
    std::size_t BackSize() const { return _buffer.size() - _write_idx; }

    void MoveReadIdx(std::size_t len) {
        if (_read_idx + len > _write_idx) throw std::out_of_range("move read idx out of range");
        _read_idx += len;
    }
    void MoveWriteIdx(std::size_t len) {
        if (len > BackSize()) throw std::out_of_range("move write idx out of range");
        _write_idx += len;
    }

    void EnsureWritable(std::size_t len) {
        if (len > BackSize()) {
            if (len > WritableSize()) {
                // 如果空间不够，那就直接扩容，不进行移动
                // 尽量少地进行移动是为了减少内存拷贝的次数，提高性能
                _buffer.resize(_buffer.size() + len);
            }
            else {
                // 将数据移动到起始位置
                std::copy(ReadPos(), static_cast<const char*>(WritePos()), &_buffer[0]);
                _write_idx -= FrontSize();
                _read_idx = 0;
            }
        }
    }
    char* FindCRLF() {
        for (auto p = ReadPos(); p < WritePos(); p++) {
            if (*p == '\n') return const_cast<char*>(p);
        }
        return nullptr;
    }
public:
    explicit Buffer(std::size_t size = 1024) : _buffer(size), _read_idx(0), _write_idx(0) {}
    Buffer(const Buffer& buf) : Buffer() { Write(buf); } // 拷贝构造函数（委托构造）

    std::size_t ReadableSize() const { return _write_idx - _read_idx; }
    std::size_t WritableSize() const { return BackSize() + FrontSize(); }

    void Read(void* buf, std::size_t len, bool pop = false) {
        if (len > ReadableSize()) return;
        std::copy(ReadPos(), ReadPos() + len, static_cast<char*>(buf));
        if (pop) MoveReadIdx(len);
    }
    std::string ReadAsString(std::size_t len, bool pop = false) {
        if (len > ReadableSize()) return "";
        std::string str(ReadPos(), len);
        if (pop) MoveReadIdx(len);
        return str;
    }
    std::string ReadLine(bool pop = false) {
        auto p = FindCRLF();
        if (p) return ReadAsString(p - ReadPos() + 1, pop);
        else return "";
    }
    void Write(const void* data, std::size_t len, bool push = true) {
        EnsureWritable(len);
        std::copy(static_cast<const char*>(data), static_cast<const char*>(data) + len, WritePos());
        if (push) MoveWriteIdx(len);
    }
    void Write(const std::string& str, bool push = true) { Write(str.data(), str.size(), push); }
    void Write(const Buffer& buf, bool push = true) { Write(buf.ReadPos(), buf.ReadableSize(), push); }
    void Clear() { _read_idx = _write_idx = 0; }
private:
    std::vector<char> _buffer;
    std::size_t _read_idx;
    std::size_t _write_idx;
};

Log lg(Onefile);

class Socket {
public:
    Socket() : _sockfd(-1) {}
    explicit Socket(int fd) : _sockfd(fd) {}
    ~Socket() { Close(); }
    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;

    // 获取套接字描述符
    int GetFd() const { return _sockfd; }
    // 创建套接字
    bool Create() {
        _sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (_sockfd == -1) {
            lg(Error, "create socket failed");
            return false;
        }
        return true;
    }
    // 绑定地址
    bool Bind(const std::string& ip, uint16_t port) {
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = inet_addr(ip.c_str());
        socklen_t len = sizeof(addr);
        if (bind(_sockfd, reinterpret_cast<struct sockaddr*>(&addr), len) == -1) {
            lg(Error, "bind address %s:%d failed", ip.c_str(), port);
            return false;
        }
        return true;
    }
    // 监听
    bool Listen(int backlog = 1024) {
        if (listen(_sockfd, backlog) == -1) {
            lg(Error, "listen failed");
            return false;
        }
        return true;
    }
    // 客户发起连接
    bool Connect(const std::string& ip, uint16_t port) {
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = inet_addr(ip.c_str());
        socklen_t len = sizeof(addr);
        if (connect(_sockfd, reinterpret_cast<struct sockaddr*>(&addr), len) == -1) {
            lg(Error, "connect server %s:%d failed", ip.c_str(), port);
            return false;
        }
        return true;
    }
    // 获取新连接
    int Accept() {
        int newfd = accept(_sockfd, nullptr, nullptr);
        if (newfd == -1) lg(Error, "accept failed");
        return newfd;
    }
    // 接收数据
    ssize_t Recv(void* buf, std::size_t len, int flag = 0) {
        ssize_t ret = recv(_sockfd, buf, len, flag);
        if (ret == -1) {
            // EAGAIN: 没有数据可读
            // EINTR:  被信号中断
            if (errno == EAGAIN || errno == EINTR) return 0;
            else lg(Error, "recv failed");
        }
        return ret;
    }
    // 发送数据
    ssize_t Send(const void* buf, std::size_t len, int flag = 0) {
        ssize_t ret = send(_sockfd, buf, len, flag);
        if (ret == -1) {
            // EAGAIN: 没有数据可写
            // EINTR:  被信号中断
            if (errno == EAGAIN || errno == EINTR) return 0;
            else lg(Error, "send failed");
        }
        return ret;
    }
    // 关闭套接字
    void Close() {
        if (_sockfd != -1) {
            close(_sockfd);
            _sockfd = -1;
        }
    }
    // 创建服务端连接
    bool CreateServer(uint16_t port, bool block = true, const std::string& ip = "0.0.0.0", int backlog = 1024) {
        if (!Create()) return false;
        if (!Bind(ip, port)) return false;
        if (!Listen(backlog)) return false;
        if (block == false) NonBlock();
        ReuseAddr();
        return true;
    }
    // 创建客户端连接
    bool CreateClient(uint16_t port, const std::string& ip) {
        if (!Create()) return false;
        if (!Connect(ip, port)) return false;
        NonBlock();
        return true;
    }
    // 设置地址、端口复用
    void ReuseAddr() {
        int opt = 1; // 1: 开启 0: 关闭
        if (setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
            lg(Error, "set reuse address failed");
        }
        opt = 1;
        if (setsockopt(_sockfd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) == -1) {
            lg(Error, "set reuse port failed");
        }
    }
    // 设置非阻塞
    void NonBlock() {
        // 获取当前标志
        int opt = fcntl(_sockfd, F_GETFL, 0);
        if (opt == -1) {
            lg(Error, "get socket flag failed");
            return;
        }
        // 增加非阻塞标志
        if (fcntl(_sockfd, F_SETFL, opt | O_NONBLOCK) == -1) {
            lg(Error, "set nonblock failed");
        }
    }
private:
    int _sockfd;
};