#include <iostream>
#include "seriealizeFunctions.h"
#include "queuedLogger.h"


class queuedLoggerImpl  {

public:
	queuedLoggerImpl() :
		m_SerializeFunctions() {
	}

	~queuedLoggerImpl() {
	}

	//implementation of ilogger
	void logg(const std::wstring& str)  {
		m_SerializeFunctions.run([str]() {
			auto stringToPring = str + std::wstring(L"\r\n");
			std::wcout << stringToPring;
		});
	};

private:
	seriealizeFunctions	m_SerializeFunctions;
};



queuedLogger::queuedLogger() :
	m_ImplQueuedLogger(std::make_unique<queuedLoggerImpl>())
{
}

queuedLogger::~queuedLogger()
{
}

void queuedLogger::logg(const std::wstring& str) {
	m_ImplQueuedLogger->logg(str);
}
