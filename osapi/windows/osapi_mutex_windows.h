#ifndef OSAPI_MUTEX_WINDOWS_H
#define OSAPI_MUTEX_WINDOWS_H


class Mutex : public MutexInterface
{
	private:
		HANDLE mutexHandle;

	public:
		Mutex() {
			mutexHandle = CreateSemaphoreW(nullptr, 1, 1, NULL);
		}

		virtual ~Mutex() {
			CloseHandle(mutexHandle);
		}

		virtual bool lock(unsigned int timeout) {
			return (WaitForSingleObject(mutexHandle, timeout) == WAIT_OBJECT_0);
		}

		virtual void unlock() {
			ReleaseSemaphore(mutexHandle, 1, NULL);
		}
};



#endif // OSAPI_MUTEX_WINDOWS_H
