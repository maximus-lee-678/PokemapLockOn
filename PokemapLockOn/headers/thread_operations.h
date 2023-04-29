#pragma once

#include "main.h"

typedef struct
{
	int time;
}thread_params;

DWORD WINAPI sleep_thread(void* param);
int handle_sleep_thread(int sleep_time_millis);