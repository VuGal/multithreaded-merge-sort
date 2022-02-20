#ifndef OSAPI_THREAD_WINDOWS_H
#define OSAPI_THREAD_WINDOWS_H

/** Thread interface implementation for Windows. */
class Thread : public ThreadInterface
{
	private:
		int threadPriority {};
		const unsigned int threadStackSize {};
		const Joinable threadIsJoinable {};
		const char * threadName {};
		HANDLE threadSemaphore {};
		HANDLE threadHandle {};
		bool threadIsRunning {};

    public:

		static DWORD WINAPI threadFunction(void* argument) {
			Thread* osapiThreadObject = reinterpret_cast<Thread*>(argument);
			if (osapiThreadObject) {
				osapiThreadObject->job();
				if (osapiThreadObject->threadIsJoinable) {
					ReleaseSemaphore(osapiThreadObject->threadSemaphore, 1, NULL);
				}
			}
			osapiThreadObject->threadIsRunning = false;
			ExitThread(0);
		}
    
        /** Thread constructor.
         *  @param[in] priority thread priority
         *  @param[in] stackSize thread stack size in bytes
         *  @param[in] isJoinable decides if the thread supports join operation or not
         *  @param[in] name optional thread name
         */
        Thread(int priority, unsigned int stackSize, Joinable isJoinable, const char* name = "unnamed") :
        	threadPriority(priority), threadStackSize(stackSize), threadIsJoinable(isJoinable), threadName(name)
        {
        	if (isJoinable == JOINABLE) {
				threadSemaphore = CreateSemaphoreW(nullptr, 0, 1, NULL);
			}
        }
        
        /** Virtual destructor required to properly destroy derived class objects. */
        virtual ~Thread() { 
            CloseHandle(threadSemaphore);
        }
        
        /** Runs the thread.
         *  @retval true if the thread was started successfully, 
         *  @retval false if the thread was not started successfully, or the thread was already running
         */
        virtual bool run() {
        	if (threadHandle == NULL) {
				threadHandle = CreateThread(nullptr, threadStackSize, threadFunction, this, 0, NULL);
				threadIsRunning = true;
				return setPriority(threadPriority);
        	}
        	else {
        		return false;
        	}
        }
        
        /** Checks if the thread is running.
         *  @retval true if the thread is running
         *  @retval false if the thread is not running
         */
        virtual bool isRunning() { 
        	return threadIsRunning;
        }   

        /** Waits for the thread to finish executing, with a given timeout.
         *  @param timeout[in] number of milliseconds to wait for the thread to finish executing
         *  @retval true if the thread was successfully joined in the given time
         *  @retval false if the thread was not joined within the given time or the thread is not joinable at all
         */
        virtual bool join(unsigned int timeout) {
        	return (WaitForSingleObject(threadSemaphore, timeout) == WAIT_OBJECT_0);
        }

        /** Checks, if the thread is joinable.
         *  @retval true if the thread is joinable
         *  @retval false if the thread is not joinable
         */
        virtual bool isJoinable() {
            return threadIsJoinable;
        }

        /** Suspends thread execution.
         *  @retval true if the thread was suspended successfully
         *  @retval false if the thread was not suspended for some reason
         */
        virtual bool suspend() {
            if (SuspendThread(threadHandle) != (DWORD)(-1)) {
            	threadIsRunning = 0;
            	return true;
            }
            else {
            	return false;
            }
        }

        /** Resumes thread execution.
         *  @retval true if the thread was resumed successfully
         *  @retval false if the thread was not resumed for some reason
         */
        virtual bool resume() {
        	if (ResumeThread(threadHandle) != (DWORD)(-1)) {
        		threadIsRunning = 1;
        		return true;
        	}
        	else {
        		return false;
        	}
        }

        /** Sets thread priority
         *  @param[in] priority new thread priority
         *  @retval true if the priority for the thread was set successfully
         *  @retval false if the priority for the thread was not set successfully for some reason
         */
        virtual bool setPriority(int priority) {
        	if (threadHandle != NULL) {
        		SetThreadPriority(threadHandle, priority);
        		return true;
        	}
        	else {
        		return false;
        	}
        }

        /** Gets the thread priority
         *  @return current thread priority
         */
        virtual int getPriority() {
            return GetThreadPriority(threadHandle);
        }
        
        /** Gets thread name
         *  @return name of the thread
         */
        virtual const char* getName() {
            return threadName;
        }
    
    protected:
        
        /** Delays thread execution for a given time.
         *  @param time[in] number of milliseconds to delay thread execution
         */
        virtual void sleep(unsigned int time) {
            Sleep(time);
        }
};


#endif /* OSAPI_THREAD_WINDOWS_H */
