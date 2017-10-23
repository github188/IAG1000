/* GT1000 
 *
 *
 *
 */
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <time.h>
//#include <pthread.h>
#include <gtthread.h>
/**********************************************************************************************
 * ������	:gt_create_thread()
 * ����	:����һ������״̬���߳�
 * ����	:start_routine:�̵߳ķ�����
 *			 arg:���ݸ��̷߳������Ĳ���
 * ���	:threadid:�Ѵ������߳�id
 * ����ֵ	:0��ʾ�ɹ���ֵ��ʾ����
 **********************************************************************************************/
int  gt_create_thread(pthread_t  *threadid,void *(*start_routine)(void *), void * arg)
{
	int rc;
	pthread_attr_t attr,*at=NULL;
	if(threadid==NULL)
		return -1;
	memset((void*)&attr,0,sizeof(pthread_attr_t));
	rc=pthread_attr_init(&attr);
	
	if(rc<0)
		at=NULL;
	else
		at=&attr;
        pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
	rc=pthread_create(threadid,at,start_routine,arg);
	if(at!=NULL)
		pthread_attr_destroy(at);
	return rc;		
}

/**********************************************************************************************
 * ������	:gt_cond_wait_time()
 * ����	:�ȴ�һ��ָ������������ 
 * ����	:cond:��������ָ��
 *			 mutex:���������õ��Ļ�����ָ��
 *                   sec:��ʱʱ��(��)
 * ����ֵ	:0��ʾ�յ��ź�
 *                     ETIMEDOUT ��ʾ���ﳬʱʱ���û���յ��ź�
 *                     ��ֵ��ʾ����
 *                     
 **********************************************************************************************/
int gt_cond_wait_time(pthread_cond_t *cond,pthread_mutex_t *mutex,int sec)
{
    int ret;
    struct timespec ts;
    pthread_mutex_lock(mutex);
    ts.tv_sec = time(NULL) + sec;
    ts.tv_nsec = 0;    
    ret=pthread_cond_timedwait(cond, mutex, &ts); 
    pthread_mutex_unlock(mutex);
    return ret;
}

/**********************************************************************************************
 * ������	:gt_cond_signal()
 * ����	:��һ���������������ź�
 * ����	:cond:��������ָ��
 *			 mutex:���������õ��Ļ�����ָ��
 * ����ֵ	:0��ʾ�ɹ�
 *                   ��ֵ��ʾ����
 *                     
 **********************************************************************************************/
int gt_cond_signal(pthread_cond_t *cond,pthread_mutex_t *mutex)
{
    int ret;
    //pthread_mutex_lock(mutex);
    ret=pthread_cond_signal(cond);
    //pthread_mutex_unlock(mutex);    
    return ret;
}


#ifdef WIN32
#define u_int64_t   __int64	

#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS) || defined(__WATCOMC__)
  #define DELTA_EPOCH_IN_USEC  11644473600000000Ui64
#else
  #define DELTA_EPOCH_IN_USEC  11644473600000000ULL
#endif




static u_int64_t filetime_to_unix_epoch (const FILETIME *ft)
{
    u_int64_t res = (u_int64_t) ft->dwHighDateTime << 32;

    res |= ft->dwLowDateTime;
    res /= 10;                   /* from 100 nano-sec periods to usec */
    res -= DELTA_EPOCH_IN_USEC;  /* from Win epoch to Unix epoch */
    return (res);
}

int gettimeofday (struct timeval *tv, void *tz )
{
    FILETIME  ft;
    u_int64_t tim;
    if (!tv) {
//        errno = EINVAL;
        return (-1);
    }
    GetSystemTimeAsFileTime (&ft);
    tim = filetime_to_unix_epoch (&ft);
    tv->tv_sec  = (long) (tim / 1000000L);
    tv->tv_usec = (long) (tim % 1000000L);
    return (0);
}
#endif


