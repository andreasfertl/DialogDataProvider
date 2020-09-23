#pragma once

#include <thread>
#include <mutex>
#include <queue>
#include <functional>
#include <condition_variable>
#include "iRun.h"

//implements a queue which handles functions in sequence
//can be used to run functions in sequence, and to protect concurrent data access by
//putting the according functions to access the data in sequence
class seriealizeFunctions : public iRun {

public:
	seriealizeFunctions();
	~seriealizeFunctions();

	//implementation of iRun
	void run(const std::function<void()>&& func) override;
	void run(const std::function<void()>& func) override;

private:
	void handler();

	struct Event {
		std::mutex m_Mutex;
		std::condition_variable m_Condition;
	};

	bool m_Stop;
	std::queue<std::function<void()>> m_Queue;
	Event m_Event;
	std::thread m_RunThread;
};




