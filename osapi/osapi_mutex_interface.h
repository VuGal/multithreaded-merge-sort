#ifndef OSAPI_MUTEX_INTERFACE_H
#define OSAPI_MUTEX_INTERFACE_H

/** Base interface for all mutexes. */
class MutexInterface
{
public:

    /** Virtual destructor required to properly destroy derived class objects. */
    virtual ~MutexInterface() { }

    /** Locks the mutex. In case the mutex is already locked, it may cause the calling thread to block,
     *  waiting for the mutex to become unlocked, for the maximum given timeout.
     *  @param[in] timeout maximum number of milliseconds allowed to block the calling thread while waiting for mutex to become unlocked
     *  @retval true if the mutex was successfully locked (calling thread owns now this lock)
     *  @retval false if the mutex was not locked within the given time
     */
    virtual bool lock(unsigned int timeout) = 0;

    /** Unlocks the mutex */
    virtual void unlock() = 0;

};


#endif // OSAPI_MUTEX_INTERFACE_H
