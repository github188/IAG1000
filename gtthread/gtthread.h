#ifndef GT_THREAD_H
#define GT_THREAD_H
#ifdef _WIN32
#include <windows.h>
#include <pthread/pthread.h>//pthread-win32
#else
#include <unistd.h>
#include <pthread.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif
/**********************************************************************************************
 * ������	:gt_create_thread()
 * ����	:����һ������״̬���߳�
 * ����	:start_routine:�̵߳ķ�����
 *			 arg:���ݸ��̷߳������Ĳ���
 * ���	:threadid:�Ѵ������߳�id
 * ����ֵ	:0��ʾ�ɹ���ֵ��ʾ����
 **********************************************************************************************/
int  gt_create_thread(pthread_t  *threadid,void *(*start_routine)(void *), void * arg);

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
int gt_cond_wait_time(pthread_cond_t *cond,pthread_mutex_t *mutex,int sec);

/**********************************************************************************************
 * ������	:gt_cond_signal()
 * ����	:��һ���������������ź�
 * ����	:cond:��������ָ��
 *			 mutex:���������õ��Ļ�����ָ��
 * ����ֵ	:0��ʾ�ɹ�
 *                   ��ֵ��ʾ����
 *                     
 **********************************************************************************************/
int gt_cond_signal(pthread_cond_t *cond,pthread_mutex_t *mutex);

#define	GT_CreateThread	gt_create_thread
#define GT_CancelThread         pthread_cancel                      ///<ȡ���߳�
#define  gt_cancle_thread           pthread_cancel
#ifdef _WIN32
#define	sleep(s)	Sleep(1000*s)
#define msleep(s)	Sleep(s);
int gettimeofday (struct timeval *tv, void *tz );
#else
#define msleep(s)	usleep(s*1000)
#endif

#ifdef __cplusplus
}
#endif

#endif


