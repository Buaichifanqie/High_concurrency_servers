#pragma once

extern struct Dispatcher EpollDispatcher;
#include "Dispatcher.h"
struct EventLoop
{
	Dispatcher* dispatcher;
	void* dispatcherData;
};

