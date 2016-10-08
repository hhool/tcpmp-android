/*****************************************************************************
 *
 * This program is free software ; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 * $Id: multithread_win32.c 343 2005-11-16 20:11:07Z picard $
 *
 * The Core Pocket Media Player
 * Copyright (c) 2004-2005 Gabor Kovacs
 *
 ****************************************************************************/

#include "../common.h"

#if defined(TARGET_ANDROID)

#if defined(MULTITHREAD)

//time usleep()
#include <unistd.h>
#include <pthread.h>

void* LockCreate()
{
	pthread_mutexattr_t attr;

	pthread_mutex_t *p_mutex = malloc(sizeof(pthread_mutex_t));

	int                 i_result;

	pthread_mutexattr_init( &attr );

	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);

	i_result = pthread_mutex_init( p_mutex, &attr );

	pthread_mutexattr_destroy( &attr );

	DEBUG_MSG(DEBUG_SYS,T("multithread_android::LockCreate"));

	if(i_result)
	{
		free(p_mutex);
		DEBUG_MSG(DEBUG_SYS,T("multithread_android::LockCreate FAILED"));
		return NULL;
	}

	return p_mutex;
}

void LockDelete(void* p)
{
	//DEBUG_MSG(DEBUG_SYS,T("multithread_android::LockDelete"));
	if (p)
	{
		pthread_mutex_destroy(p);
		free(p);
	}
}

void LockEnter(void* p)
{
	//DEBUG_MSG(DEBUG_SYS,T("multithread_android::LockEnter"));
	if (p)
	{
		pthread_mutex_lock(p);
	}
}

void LockLeave(void* p)
{
	//DEBUG_MSG(DEBUG_SYS,T("multithread_android::LockLeave"));
	if (p)
	{
		pthread_mutex_unlock(p);
	}
}

int ThreadPriority(void* Thread,int Priority)
{
	DEBUG_MSG(DEBUG_SYS,T("multithread_android::ThreadPriority"));
	return 0;
}

bool_t EventWait(void* Handle,int Time,void* mutex)
{
	DEBUG_MSG1(DEBUG_SYS,T("multithread_android::EventWait Time %d"),Time);

	if(Time>0)
	{
		struct timespec time;
		struct timeval date;
		int64_t datams;

		bzero(&time, sizeof(struct timespec));
		bzero(&date, sizeof(struct timeval));

		gettimeofday( &date, NULL );

		datams = (((int64_t)date.tv_sec)*1000000 + date.tv_usec)/1000+Time;

		time.tv_sec = datams/1000;
		time.tv_nsec = datams%1000 * 1000000;
		pthread_cond_timedwait((pthread_cond_t*)Handle, (pthread_mutex_t*)mutex,&time);
	}
	else
	{
		pthread_cond_wait((pthread_cond_t*)Handle, (pthread_mutex_t*)mutex );
	}

	DEBUG_MSG(DEBUG_SYS,T("multithread_android::EventWait end"));
	return 1;
}
//Fixme
void* EventCreate(bool_t ManualReset,bool_t InitState)
{
	pthread_condattr_t attr;
	pthread_cond_t     *cond = malloc(sizeof(pthread_cond_t));

	DEBUG_MSG(DEBUG_SYS,T("multithread_android::EventCreate"));
	pthread_cond_init (cond, &attr);
	return cond;
}
void EventSet(void* Handle)
{
	DEBUG_MSG(DEBUG_SYS,T("multithread_android::EventSet"));
	if(Handle)
	{
		pthread_cond_signal((pthread_cond_t*)Handle);
	}
}
void EventReset(void* Handle)
{
	DEBUG_MSG(DEBUG_SYS,T("multithread_android::EventReset"));
	//ResetEvent(Handle);
}
void EventClose(void* Handle)
{
	DEBUG_MSG(DEBUG_SYS,T("multithread_android::EventClose"));
	if(Handle)
	{
		pthread_cond_destroy((pthread_cond_t*)Handle);
		free((pthread_cond_t*)Handle);
	}
}

int ThreadId()
{
	DEBUG_MSG1(DEBUG_SYS,T("multithread_android::ThreadId 0x%x"),pthread_self());
	return pthread_self();
}
void ThreadSleep(int Time)
{
	DEBUG_MSG1(DEBUG_SYS,T("multithread_android::ThreadSleep %d"),Time);
	usleep(Time*1000);
}
void ThreadTerminate(void* Handle)
{
	DEBUG_MSG1(DEBUG_SYS,T("multithread_android::ThreadTerminate Handle 0x%x"),Handle);
	if(Handle)
	{
		pthread_join(*(pthread_t*)Handle,NULL);
		free((pthread_t*)Handle);
	}
	DEBUG_MSG1(DEBUG_SYS,T("multithread_android::ThreadTerminate Handle 0x%x end"),Handle);
}

void* ThreadCreate(int(*Start)(void*),void* Parameter,int Quantum)
{
	pthread_attr_t attr;
	struct sched_param param;
	bool_t ret = 0;
	pthread_t* thread_id = (pthread_t*)malloc(sizeof(pthread_t));
	int policy = 0;
	int primax = 0;

	pthread_attr_init (&attr);

	ret = pthread_attr_setschedpolicy(&attr,SCHED_RR);

	DEBUG_MSG2(DEBUG_SYS,T("ThreadCreate::pthread_attr_setschedpolicy ret %d, policy %d"),ret,policy);

	ret = pthread_attr_getschedpolicy(&attr,&policy);

	DEBUG_MSG2(DEBUG_SYS,T("ThreadCreate::pthread_attr_getschedpolicy ret %d, policy %d"),ret,policy);

	primax = sched_get_priority_max(policy);

	param.sched_priority = primax;

	DEBUG_MSG1(DEBUG_SYS,T("ThreadCreate::sched_get_priority_max primax %d"),primax);

	ret = pthread_attr_setschedparam(&attr,&param);

	DEBUG_MSG2(DEBUG_SYS,T("ThreadCreate::pthread_attr_setschedparam ret %d,  primax %d"),ret,primax);

	ret = pthread_create(thread_id, &attr, Start, Parameter );

	DEBUG_MSG2(DEBUG_SYS,T("ThreadCreate::pthread_create ret %d, thread_id 0x%x"),ret,thread_id);

	return thread_id;
}

#endif
#endif
