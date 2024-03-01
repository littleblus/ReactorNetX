using TaskFunc = std::function<void()>;

class TimerTask {
public:
	TimerTask(uint64_t id, uint64_t timeout, const TaskFunc& task) : _id(id), _timeout(timeout), _task(task) {}

private:
	uint64_t _id; // 任务对象的唯一标识
	uint64_t _timeout; // 任务的超时时间
	TaskFunc _task; // 任务的回调函数
};