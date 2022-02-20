#ifndef OSAPI_RECURSIVE_MUTEX_WINDOWS_H
#define OSAPI_RECURSIVE_MUTEX_WINDOWS_H

class RecursiveMutex : public MutexInterface
{
	private:
		HANDLE mutexHandle;
		unsigned int locksCount {};

	public:
		RecursiveMutex() {
			mutexHandle = CreateMutexA(nullptr, TRUE, nullptr);
		}

		virtual ~RecursiveMutex() {
			CloseHandle(mutexHandle);
		}

		virtual bool lock(unsigned int timeout) {
			if (WaitForSingleObject(mutexHandle, timeout) == WAIT_OBJECT_0) {
				++locksCount;
				return true;
			}
			else {
				return false;
			}
		}

		virtual void unlock() {
			if (locksCount) {
				if (ReleaseMutex(mutexHandle) != 0) {
					--locksCount;
				}
			}
		}
};


#endif // OSAPI_RECURSIVE_MUTEX_WINDOWS_H
