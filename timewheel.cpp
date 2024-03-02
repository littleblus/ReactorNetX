#include <functional>
#include <cstdint>
#include <vector>
#include <memory>
#include <unordered_map>
#include <cstdio>
#include <unistd.h>

class TimerTask {
public:
	using TaskFunc = std::function<void()>;
	using ReleaseFunc = std::function<void()>;

	TimerTask(uint64_t id, uint64_t timeout, const TaskFunc& task) : _id(id), _timeout(timeout), _task(task) {}
	void setRelease(const ReleaseFunc& release) { _release = release; }
	void cancel() { _canceled = true; }
	uint64_t timeout() const { return _timeout; }
	~TimerTask() { if (!_canceled) _task(); _release(); }
private:
	uint64_t _id; // 任务对象的唯一标识
	uint64_t _timeout; // 任务的超时时间
	TaskFunc _task; // 任务的回调函数
	ReleaseFunc _release; // 任务的释放函数(释放TimerWheel中的任务对象)
	bool _canceled = false; // 任务是否被取消
};

class TimerWheel {
	using TaskPtr = std::shared_ptr<TimerTask>;
	using TaskWeakPtr = std::weak_ptr<TimerTask>;
public:
	TimerWheel() : _tick(0), _wheelSize(60) {
		_wheel.resize(_wheelSize);
	}

	// 添加任务
	void addTask(uint64_t id, uint64_t timeout, const TimerTask::TaskFunc& task) {
		TaskPtr pt(new TimerTask(id, timeout, task));
		pt->setRelease([this, id]() {
			_taskMap.erase(id);
			});
		_taskMap[id] = TaskWeakPtr(pt);
		_wheel[(_tick + timeout) % _wheelSize].push_back(pt);
	}

	// 刷新任务
	void refreshTask(uint64_t id) {
		// 通过id找到任务对象，构造一个新的任务对象，添加到时间轮中
		auto it = _taskMap.find(id);
		if (it != _taskMap.end()) {
			TaskPtr pt = it->second.lock();
			if (pt) _wheel[(_tick + pt->timeout()) % _wheelSize].push_back(pt);
		}
	}

	void tick() {
		// 时间轮的指针向前移动
		_tick = (_tick + 1) % _wheelSize;
		// 执行当前时间轮上的任务
		_wheel[_tick].clear();
	}

	// 删除任务
	void removeTask(uint64_t id) {
		auto it = _taskMap.find(id);
		if (it != _taskMap.end()) {
			TaskPtr pt = it->second.lock();
			if (pt) pt->cancel();
			_taskMap.erase(id);
		}
	}
private:
	std::vector<std::vector<TaskPtr>> _wheel; // 时间轮
	std::unordered_map<uint64_t, TaskWeakPtr> _taskMap; // 任务对象的映射
	size_t _tick; // 当前时间轮的索引
	size_t _wheelSize; // 时间轮的大小(最大超时时间)
};

int main() {
	TimerWheel tw;
	tw.addTask(888, 5, []() { printf("task 1\n"); });
	for (int i = 0; i < 5; i++) {
		tw.refreshTask(888);
		printf("refresh task\n");
		sleep(1);
	}
	tw.removeTask(888);
	for (;;) {
		printf("---------------------------\n");
		tw.tick();
		sleep(1);
	}
	return 0;
}