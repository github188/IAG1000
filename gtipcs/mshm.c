/**************************************************************************
 * �ļ��� : mshm.c
 * ����	: �������ڴ�ĵ��ýӿڽ��а�װ���ṩ����ʹ
 			   �õĽӿ�   
 *************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <string.h>
#include <pthread.h>
#include "msem.h"
#include "mshm.h"



#define MSHM_MAGIC      0x5500aa00              ///�����ڴ��־

/**************************************************************************
  *������	:MShmCoreCreate
  *����	:�Ը�����key��usersize���������ڴ�
  *����	: mkey:�����ڴ��key 
  *			  usersize:�û�������Ҫ�Ŀռ�
  *			  name:Ҫ�����Ĺ����ڴ������
  *                   u_ptr:���ֵ,����û����õ��ڴ�������ʼ��ַ,NULL��ʾ����Ҫ���
  *����ֵ	: ����NULL��ʾ�д��� ���������errno
  *			  �������Ӻõ��ڴ��ַMemCoreָ��,
  *			  �Ժ������ָ����Ϊ����������������
  *ע:������������ѷ��䵽���ڴ��ʼ��Ϊ0
  *************************************************************************/
MEM_CORE * MShmCoreCreate(char *name,key_t mkey,size_t usersize,void **u_ptr)
{
	int err;
	int FirstCreateFlag=0;	//�״δ�����key�Ĺ����ڴ��־
	MEM_CORE *mc=NULL;
	key_t semkey;
	size_t tsize;				//ʵ�ʷ���Ĺ����ڴ��С
	size_t pagesize;
	void *area = ((void *)-1);	//���Ӻõ��ڴ��ַ
	int fdmem=-1;			//�����ڴ���
	int fdsem=-1;			//�ź������
	struct shmid_ds shmbuf;
	if ((usersize <= 0)||(mkey<0))
	{
		errno=EINVAL;
		return NULL;
	} 	

/******************���乲���ڴ�***************************/
	pagesize = sysconf(_SC_PAGESIZE);					//ϵͳҳ��С
    	tsize = (((usersize-1)/pagesize+1)+1)*pagesize;		//ʵ�ʷ���ʱ����һҳ��Ϊ�����ڴ�������ͷ
	//���������ڴ�
	fdmem=shmget(mkey, tsize, (SHM_R|SHM_W|IPC_CREAT|IPC_EXCL));
	if(fdmem<0)
	{
		err=errno;
		if(err==EEXIST)
		{	//�Ѿ�����
			fdmem=shmget(mkey, 0, 0);
		
		}
		if(fdmem<0)
		{
			printf("failed create share memory 0x%x,err=%d\n",mkey,err);
			return NULL;
		}
	}
	else
	{
		FirstCreateFlag=1;
	}
	//���ӹ����ڴ�
	area = (void *)shmat(fdmem, NULL, 0);
	if(area==((void*)-1))
	{
		printf("failed attach share memory 0x%x\n",mkey);
		return NULL;
	}
	if(FirstCreateFlag)
	{//����ǵ�һ�δ����򽫹����ڴ��ʼ��Ϊ0
		memset((void*)area,0,tsize);
	}
	//���ù����ڴ�	
	shmctl(fdmem, IPC_STAT, &shmbuf);
	shmbuf.shm_perm.uid = getuid();
	shmbuf.shm_perm.gid = getgid();
	shmctl(fdmem, IPC_SET, &shmbuf);



	semkey=mkey;	//���ź�����key�빲���ڴ��key��ͬ

	fdsem=SemCreate(semkey,1);
	if(fdsem<0)
	{
		printf("failed create share memory sem: 0x%x\n",semkey);
              shmdt(area);
		return NULL;
	}
	SemSetVal(fdsem,0,1);
	mc = (MEM_CORE *)area;
	mc->mkey=mkey;
	mc->tsize     = tsize;
	mc->usize    = usersize;	
	mc->memid  = fdmem;
	mc->semid	   = fdsem;
	mc->uoffset  = pagesize;
	if(name!=NULL)
	{
		sprintf(mc->name,"%s",name);
	}
	else
		sprintf(mc->name,"%s","NONAME");
       if(u_ptr!=NULL)
       {
               *u_ptr=(void*)((void*)mc+mc->uoffset);
       }
       mc->magic=MSHM_MAGIC;
	return mc;
}


