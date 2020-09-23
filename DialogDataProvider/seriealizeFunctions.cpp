#include "seriealizeFunctions.h"



seriealizeFunctions::seriealizeFunctions() :
	m_Stop(false),
	m_Queue(),
	m_Event(),
	m_RunThread(&seriealizeFunctions::handler, this)
{
}

seriealizeFunctions::~seriealizeFunctions()
{
	run([this]() { this->m_Stop = true; });
	m_RunThread.join();
}

void seriealizeFunctions::run(const std::function<void()>&& func)
{
	{
		std::lock_guard<std::mutex> lk(m_Event.m_Mutex);
		m_Queue.push(std::move(func));
	}
	m_Event.m_Condition.notify_one();
}

void seriealizeFunctions::run(const std::function<void()>& func)
{
	{
		std::lock_guard<std::mutex> lk(m_Event.m_Mutex);
		m_Queue.push(func);
	}
	m_Event.m_Condition.notify_one();
}


void seriealizeFunctions::handler()
{
	while (!m_Stop)
	{
		std::queue<std::function<void()>> handlerQueue;

		{
			std::unique_lock<std::mutex> lk(m_Event.m_Mutex);
			m_Event.m_Condition.wait(lk, [this]() { return !this->m_Queue.empty(); });
			//just "swap" the workload to another queue to be able to hold the lock as little as possible
			m_Queue.swap(handlerQueue);
		}

		//and finally call all the functions to be handled
		while (!handlerQueue.empty())
		{
			auto& callFunction = handlerQueue.front();
			callFunction();
			handlerQueue.pop();
		}

	}
}



