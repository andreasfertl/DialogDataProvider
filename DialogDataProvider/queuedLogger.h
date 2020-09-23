#pragma once
#include <memory>
#include <string>

class queuedLogger  {

public:
	queuedLogger();
	~queuedLogger();

	void logg(const std::wstring& str);

private:
	std::unique_ptr<class queuedLoggerImpl>	m_ImplQueuedLogger;
};