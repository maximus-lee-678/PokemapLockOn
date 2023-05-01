#include "../headers/thread_operations.h"

// Used in conjunction with handle_sleep_thread to create a thread that waits indefinitely until any character is selected by the user.
DWORD WINAPI sleep_thread(void* param)
{
	thread_params* params = (thread_params*)param;

	_getch();
}

// Makes terminal wait for a bit. 
// 
// Return values:
// 1: user entered a character | 2: sleep time fully run down
// -1: thread creation failed | -2: should never happen, no mutex | -3: general failure | -4: thread closing failed
int handle_sleep_thread(int sleep_time_millis) {
	// https://learn.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-waitforsingleobject
	// https://www.installsetupconfig.com/win32programming/threadprocesssynchronizationapis11_13.html
	// https://stackoverflow.com/questions/5654015/windows-c-thread-parameter-passing

	static HANDLE sleep_thread_handle = NULL;		// Singleton style
	DWORD thread_id, dword_return;
	thread_params sleep_thread_params = { sleep_time_millis };

	int return_code = 0;

	// Creates a thread to execute within the virtual address space of the calling process
	// if its null make a thread, else use existing one
	if (sleep_thread_handle == NULL)
	{
		sleep_thread_handle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)sleep_thread, (LPVOID)&sleep_thread_params, 0, &thread_id);

		if (sleep_thread_handle == NULL)
		{
			// thread failed to create, error code retrievable using
			// printf("%u", GetLastError());
			return_code = -1;
			return return_code;
		}
		// else thread created, thread id retrievable using
		// printf("%u", thread_id);
	}

	// Blocks/wait till thread terminates.
	// Waits until the specified object is in the signaled state(completed execution) or the time-out interval elapses.
	// The INFINITE parameter makes the function return only when the object is signaled
	dword_return = WaitForSingleObject(sleep_thread_handle, sleep_time_millis);

	switch (dword_return)
	{
	case WAIT_ABANDONED:	// Object is a mutex object that was not released by the thread that owned the mutex object before the owning thread terminated
		return_code = -2;
		break;
	case WAIT_OBJECT_0:		// The state of the specified object is signaled.
		return_code = 1;
		break;
	case WAIT_TIMEOUT:		// The time-out interval elapsed, and the object's state is nonsignaled.
		return_code = 2;
		break;
	case WAIT_FAILED:		// The function has failed. To get extended error information, call GetLastError.
		return_code = -3;
		break;
	}

	// Timed-out, leave handle up
	if (return_code == 2) {
		return return_code;
	}

	// We only close the handle if the thread doesn't time out, else handle is leaked
	// Also nullify static thread_handle so next time function called, can make new one
	// (Thread still exists but handle invalidated, thread is lost out there somewhere)
	// (The orphaned thread still works as well in this case, many getchs mean many buttons required)
	if (return_code == 1 && CloseHandle(sleep_thread_handle) != 0) {
		sleep_thread_handle = NULL;
		return return_code;
	}
	else {
		// thread failed to close, error code retrievable using
		// printf("%u", GetLastError());
		return_code = -4;
		return return_code;
	}
}
