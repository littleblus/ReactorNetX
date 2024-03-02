#include <vector>
#include <stdexcept>

class Buffer {
public:
    Buffer(std::size_t size = 1024) : _buffer(size), _read_idx(0), _write_idx(0) {}

    void* ReadPos() { return &_buffer[_read_idx]; }
    void* WritePos() { return &_buffer[_write_idx]; }
    std::size_t FrontSize() { return _read_idx; }
    std::size_t BackSize() { return _buffer.size() - _write_idx; }
    std::size_t ReadableSize() { return _write_idx - _read_idx; }
    std::size_t WritableSize() { return BackSize() + FrontSize(); }

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
                std::copy(ReadPos(), WritePos(), static_cast<void*>(&_buffer[0]));
                _write_idx -= FrontSize();
                _read_idx = 0;
            }
        }
    }

    void* Read(std::size_t len) {
        if (len > ReadableSize()) return nullptr;
        void* data = ReadPos();
        MoveReadIdx(len);
        return data;
    }
    void Write(const void* data, std::size_t len) {
        EnsureWritable(len);
        std::copy(data, data + len, WritePos());
        _write_idx += len;
    }
    void Clear() {
        _buffer.clear();
        _read_idx = 0;
        _write_idx = 0;
    }
private:
    std::vector<char> _buffer;
    std::size_t _read_idx;
    std::size_t _write_idx;
};