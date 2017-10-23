/**************************************************************************
 * 文件名 : mshm.c
 * 功能	: 将共享内存的调用接口进行包装，提供易于使
 			   用的接口   
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



#define MSHM_MAGIC      0x5500aa00              ///共享内存标志

/**************************************************************************
  *函数名	:MShmCoreCreate
  *功能	:以给定的key和usersize创建共享内存
  *参数	: mkey:共享内存的key 
  *			  usersize:用户程序需要的空间
  *			  name:要创建的共享内存的名字
  *                   u_ptr:输出值,填充用户可用的内存区域起始地址,NULL表示不需要填充
  *返回值	: 返回NULL表示有错误 错误码存在errno
  *			  返回连接好的内存地址MemCore指针,
  *			  以后用这个指针作为参数调用其它函数
  *注:本函数并不会把分配到的内存初始化为0
  *************************************************************************/
MEM_CORE * MShmCoreCreate(char *name,key_t mkey,size_t usersize,void **u_ptr)
{
	int err;
	int FirstCreateFlag=0;	//首次创建该key的共享内存标志
	MEM_CORE *mc=NULL;
	key_t semkey;
	size_t tsize;				//实际分配的共享内存大小
	size_t pagesize;
	void *area = ((void *)-1);	//连接好的内存地址
	int fdmem=-1;			//共享内存句柄
	int fdsem=-1;			//信号量句柄
	struct shmid_ds shmbuf;
	if ((usersize <= 0)||(mkey<0))
	{
		errno=EINVAL;
		return NULL;
	} 	

/******************分配共享内存***************************/
	pagesize = sysconf(_SC_PAGESIZE);					//系统页大小
    	tsize = (((usersize-1)/pagesize+1)+1)*pagesize;		//实际分配时保留一页作为共享内存描述的头
	//创建共享内存
	fdmem=shmget(mkey, tsize, (SHM_R|SHM_W|IPC_CREAT|IPC_EXCL));
	if(fdmem<0)
	{
		err=errno;
		if(err==EEXIST)
		{	//已经存在
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
	//连接共享内存
	area = (void *)shmat(fdmem, NULL, 0);
	if(area==((void*)-1))
	{
		printf("failed attach share memory 0x%x\n",mkey);
		return NULL;
	}
	if(FirstCreateFlag)
	{//如果是第一次创建则将共享内存初始化为0
		memset((void*)area,0,tsize);
	}
	//设置共享内存	
	shmctl(fdmem, IPC_STAT, &shmbuf);
	shmbuf.shm_perm.uid = getuid();
	shmbuf.shm_perm.gid = getgid();
	shmctl(fdmem, IPC_SET, &shmbuf);



	semkey=mkey;	//让信号量的key与共享内存的key相同

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


