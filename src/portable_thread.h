#ifndef PORTABLE_THREAD_H
#define PORTABLE_THREAD_H

#ifndef WIN32
#   include <unistd.h>	
#else
#   include <io.h>
#   include <sys/types.h>
#   include <process.h>
#   include <windows.h>
#endif

#if defined(_POSIX_THREADS) || defined(_SC_THREADS)
#   include <pthread.h>
#endif

#if defined(WIN32)
#   define P_THREAD_TYPE		HANDLE
#   define P_THREAD_ID		GetCurrentThreadId()
#   define P_THREAD_CREATE(x,y,z)	((*(x) = (HANDLE)_beginthread((void(__cdecl*)(void*))(y), 0, (z))) == (HANDLE)-1L)
#   define P_THREAD_CREATEX(x,y,z)  ((*(x) = (HANDLE)_beginthreadex(NULL, 0, (void(__cdecl*)(void*))(y), (z), 0, NULL)) == (HANDLE)0L)
#   define P_THREAD_CLOSE(x)	CloseHandle(x)
#   define P_THREAD_DETACH(x)	
#   define P_THREAD_JOIN(x)		(WaitForSingleObject((x), INFINITE) == (DWORD)0xFFFFFFFF)
#   define P_THREAD_EXIT		_endthread()
#   define P_THREAD_CANCEL(x)       (TerminateThread(x, 0) == 0)
#   define P_MUTEX_TYPE		HANDLE
#   define P_MUTEX_INITIALIZER	NULL
#   define P_MUTEX_SETUP(x)		(x) = CreateMutex(NULL, FALSE, NULL)
#   define P_MUTEX_CLEANUP(x)	(CloseHandle(x) == 0)
#   define P_MUTEX_LOCK(x)		emulate_pthread_mutex_lock(&(x))
#   define P_MUTEX_UNLOCK(x)	(ReleaseMutex(x) == 0)
#   define P_COND_SETUP(x)		emulate_pthread_cond_init(&(x))
#   define P_COND_CLEANUP(x)	emulate_pthread_cond_destroy(&(x))
#   define P_COND_SIGNAL(x)		emulate_pthread_cond_signal(&(x))
#   define P_COND_WAIT(x,y)		emulate_pthread_cond_wait(&(x), &(y))
#elif defined(_POSIX_THREADS) || defined(_SC_THREADS)
#   define P_THREAD_TYPE		pthread_t
#   define P_THREAD_ID		pthread_self()
#   define P_THREAD_CREATE(x,y,z)	pthread_create((x), NULL, (y), (z))
#   define P_THREAD_CREATEX(x,y,z)	pthread_create((x), NULL, (y), (z))
#   define P_THREAD_CLOSE(x)
#   define P_THREAD_DETACH(x)	pthread_detach((x))
#   define P_THREAD_JOIN(x)		pthread_join((x), NULL)
#   define P_THREAD_EXIT		pthread_exit(NULL)
#   define P_THREAD_CANCEL(x)	pthread_cancel(x)
#   define P_MUTEX_TYPE		pthread_mutex_t
#   define P_MUTEX_INITIALIZER	PP_THREAD_P_MUTEX_INITIALIZER
#   define P_MUTEX_SETUP(x)		pthread_mutex_init(&(x), NULL)
#   define P_MUTEX_CLEANUP(x)	pthread_mutex_destroy(&(x))
#   define P_MUTEX_LOCK(x)		pthread_mutex_lock(&(x))
#   define P_MUTEX_UNLOCK(x)	pthread_mutex_unlock(&(x))
#   define P_COND_TYPE		pthread_cond_t
#   define P_COND_SETUP(x)		pthread_cond_init(&(x), NULL)
#   define P_COND_CLEANUP(x)	pthread_cond_destroy(&(x))
#   define P_COND_SIGNAL(x)		pthread_cond_signal(&(x))
#   define P_COND_WAIT(x,y)		pthread_cond_wait(&(x), &(y))
#else
#   error "CRITICAL!! - No supported thread library found."
#endif

#endif