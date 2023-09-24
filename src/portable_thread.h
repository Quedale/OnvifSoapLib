#ifndef PORTABLE_THREAD_H
#define PORTABLE_THREAD_H

#define DO_PRAGMA(x) _Pragma (#x) //DO_PRAGMA_(x)

//cross-platform macros to ignore compilier warnings
#if defined(_MSC_VER)
#   define WARNING_CMD(name) DO_PRAGMA(warning name)
#   define PUSH_WARNING_IGNORE(x,y) \
        WARNING_CMD(( push )) \
        WARNING_CMD(( disable : x ))
#   define POP_WARNING_IGNORE(x) _Pragma("warning ( pop )")
#elif defined(__GNUC__) || defined(__GNUG__)
#   define WARNING_CMD(name) DO_PRAGMA(GCC diagnostic name)
#   define PUSH_WARNING_IGNORE(x,y) \
        WARNING_CMD( push ) \
        WARNING_CMD( ignored #y )
#   define POP_WARNING_IGNORE(x) _Pragma("GCC diagnostic pop")
#else
#   define WARNING_CMD(name)
#   define PUSH_WARNING_IGNORE(x,y)
#   define POP_WARNING_IGNORE(x)
#endif

//cross-platform macros to handle threading
#if defined(WIN32) || defined(_WIN32)
#   include <winsock2.h>
#   ifdef UNDER_CE
#       pragma comment(lib, "ws2.lib")
#   else
PUSH_WARNING_IGNORE(4068,-Wunknown-pragmas)
#       pragma comment(lib, "Ws2_32.lib")
POP_WARNING_IGNORE(NULL)
#   endif
#   include <io.h>
#   include <sys/types.h>
#   include <process.h>
#   include <windows.h>
#else
#   include <netdb.h>
#   include <unistd.h>	
#endif

#if defined(_POSIX_THREADS) || defined(_SC_THREADS)
#   include <pthread.h>
#endif

#if defined(WIN32) || defined(_WIN32)
#   define P_THREAD_TYPE	HANDLE
#   define P_THREAD_ID          GetCurrentThreadId()
#   define P_THREAD_CREATE(x,y,z)	((*(x) = (HANDLE)_beginthread((void(__cdecl*)(void*))(y), 0, (z))) == (HANDLE)-1L)
#   define P_THREAD_CREATEX(x,y,z)      ((*(x) = (HANDLE)_beginthreadex(NULL, 0, (void(__cdecl*)(void*))(y), (z), 0, NULL)) == (HANDLE)0L)
#   define P_THREAD_CLOSE(x)	CloseHandle(x)
#   define P_THREAD_DETACH(x)	
#   define P_THREAD_JOIN(x)	(WaitForSingleObject((x), INFINITE) == (DWORD)0xFFFFFFFF)
#   define P_THREAD_EXIT	_endthread()
#   define P_THREAD_CANCEL(x)   (TerminateThread(x, 0) == 0)
#   define P_MUTEX_TYPE		HANDLE
#   define P_MUTEX_INITIALIZER	NULL
#   define P_MUTEX_SETUP(x)	(x) = CreateMutex(NULL, FALSE, NULL)
#   define P_MUTEX_CLEANUP(x)	(CloseHandle(x))
#   define P_MUTEX_LOCK(x)	(WaitForSingleObject(x, INFINITE))
#   define P_MUTEX_UNLOCK(x)	(ReleaseMutex(x))
#   define P_COND_SETUP(x)	({x->waiters_count_ = 0; x->signal_event_ = CreateEvent(NULL, FALSE, FALSE, NULL); InitializeCriticalSection(&x->waiters_count_lock_);})
#   define P_COND_CLEANUP(x)	({CloseHandle(x->signal_event_); DeleteCriticalSection(&x->waiters_count_lock_);})
#   define P_COND_SIGNAL(x)	({ int ret; EnterCriticalSection(&x->waiters_count_lock_); ret = x->waiters_count_; LeaveCriticalSection(&x->waiters_count_lock_); if (ret) ret = SetEvent(x->signal_event_); else ret=0; ret;})
#   define P_COND_WAIT(x,y)	({ int ret; EnterCriticalSection(&x->waiters_count_lock_); x->waiters_count_++; LeaveCriticalSection(&x->waiters_count_lock_); ReleaseMutex(*x); ret = (WaitForSingleObject(x->signal_event_, INFINITE) == WAIT_FAILED); if (!ret) { EnterCriticalSection(&x->waiters_count_lock_); x->waiters_count_--; LeaveCriticalSection(&x->waiters_count_lock_); ret = (WaitForSingleObject(*x, INFINITE) == WAIT_FAILED); }; ret;})
#elif defined(_POSIX_THREADS) || defined(_SC_THREADS)
#   define P_THREAD_TYPE	pthread_t
#   define P_THREAD_ID		pthread_self()
#   define P_THREAD_CREATE(x,y,z)       pthread_create((x), NULL, (y), (z))
#   define P_THREAD_CREATEX(x,y,z)	pthread_create((x), NULL, (y), (z))
#   define P_THREAD_CLOSE(x)
#   define P_THREAD_DETACH(x)	pthread_detach((x))
#   define P_THREAD_JOIN(x)	pthread_join((x), NULL)
#   define P_THREAD_EXIT	pthread_exit(NULL)
#   define P_THREAD_CANCEL(x)	pthread_cancel(x)
#   define P_MUTEX_TYPE		pthread_mutex_t
#   define P_MUTEX_INITIALIZER	PTHREAD_MUTEX_INITIALIZER
#   define P_MUTEX_SETUP(x)	pthread_mutex_init(&(x), NULL)
#   define P_MUTEX_CLEANUP(x)	pthread_mutex_destroy(&(x))
#   define P_MUTEX_LOCK(x)	pthread_mutex_lock(&(x))
#   define P_MUTEX_UNLOCK(x)	pthread_mutex_unlock(&(x))
#   define P_COND_TYPE		pthread_cond_t
#   define P_COND_SETUP(x)	pthread_cond_init(&(x), NULL)
#   define P_COND_CLEANUP(x)	pthread_cond_destroy(&(x))
#   define P_COND_SIGNAL(x)	pthread_cond_signal(&(x))
#   define P_COND_WAIT(x,y)	pthread_cond_wait(&(x), &(y))
#else
#   error "CRITICAL!! - No supported thread library found."
#endif

#endif