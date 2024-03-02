#include <vector>
#include <stdexcept>

class Buffer {
public:
    Buffer(std::size_t size = 1024) : _buffer(size), _read_idx(0), _write_idx(0) {}

    const char* ReadPos() const { return &_buffer[_read_idx]; }
    char* WritePos() { return &_buffer[_write_idx]; }
    std::size_t FrontSize() const { return _read_idx; }
    std::size_t BackSize() const { return _buffer.size() - _write_idx; }
    std::size_t ReadableSize() const { return _write_idx - _read_idx; }
    std::size_t WritableSize() const { return BackSize() + FrontSize(); }

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
            if (*p == 'n') return const_cast<char*>(p);
        }
        return nullptr;
    }

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