#pragma once
#include <functional>

struct iRun {
	virtual void run(const std::function<void()>&& func) = 0;
	virtual void run(const std::function<void()>& func) = 0;
};
