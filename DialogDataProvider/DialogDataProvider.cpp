// DialogDataProvider.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <string>
#include <algorithm>
#include <functional>
#include <atomic>
#include "queuedLogger.h"


using namespace std::chrono_literals;

typedef queuedLogger busSender_t;


namespace {

	static std::atomic<unsigned int> counter(0);

	std::wstring receiveDataFromBus() {
		counter++;
		return std::wstring(L"received data from the bus: ") + std::to_wstring(counter);
	}
}


//------------------------------------------------------------------------------
//DataHandler
//------------------------------------------------------------------------------

struct IDataReceiver {
	virtual void update(const std::wstring& data) = 0;
};

struct IDataSender
{
	virtual void send(const std::wstring& data) = 0;
};


class DataHandler: public IDataSender {
public:
	DataHandler(busSender_t& busSender)
		: m_IDataReceiver(nullptr)
		, m_BusSender(busSender)
	{
	};

	void registerUpdateCallback(IDataReceiver* receiver) {
		//could be a map of pointers to support several - should possibly then be thread save
		m_IDataReceiver = receiver;
	}

	void unregisterUpdateCallback(IDataReceiver* receiver) {
		//could be a map of pointers to support several - should possibly then be thread save
		m_IDataReceiver = nullptr;
	}

	void receiveData(const std::wstring& data) {
		//inform subscriber
		if (m_IDataReceiver)
			m_IDataReceiver->update(data);
	}

	//IDataSender
	void send(const std::wstring& data) override {
		//"send" to the Bus
		m_BusSender.logg(data);
	}

private:
	IDataReceiver* m_IDataReceiver;
	busSender_t& m_BusSender;
};


//------------------------------------------------------------------------------
//Dialog
//------------------------------------------------------------------------------

class Dialog : public IDataReceiver {
public:
	Dialog(DataHandler& dataHandler)
		: m_DataHandler(dataHandler)
		, m_ButtonPressThread([this]() {  std::this_thread::sleep_for(2s);  pressButton(); std::this_thread::sleep_for(1s);  }) //to simulate a button press i spin up a thread and call pressButton
	{
		m_DataHandler.registerUpdateCallback(this);
	}

	~Dialog() {
		m_DataHandler.unregisterUpdateCallback(this);
		m_ButtonPressThread.join();
	}

private:
	//IDataReceiver
	void update(const std::wstring& data) override {
		display(data);
	}

	void display(const std::wstring& status) {
		//somehow i need to update the value which is displayed (status value)
		//for example just output to console
		std::wcout << status << std::endl;
	};

	void pressButton() {
		m_DataHandler.send(std::wstring(L"Button pressed"));
	}

	DataHandler& m_DataHandler;
	//to simulate a button press i spin up a thread and call pressButton
	std::thread m_ButtonPressThread;
};





int main()
{
	busSender_t busSender;

	//1 Dialog
	DataHandler dataHandler(busSender);
	Dialog newDialog(dataHandler);

	dataHandler.receiveData(receiveDataFromBus());


	//spin up x threads to instance x dialogs
	{
		std::vector<std::thread> threads;
		for (auto i = 0u; i < 10u; i++) {
			threads.emplace_back([i, &busSender]() {
				DataHandler dataHandler(busSender);
				Dialog newDialog(dataHandler);
				dataHandler.receiveData(receiveDataFromBus() + L" from instance: " + std::to_wstring(i));
			});
		}

		//join all threads to close down correctly
		std::for_each(threads.begin(), threads.end(), [](auto& thread) { thread.join(); });
	}

}
