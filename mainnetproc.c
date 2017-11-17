#include "ipmain.h"
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include "mainnetproc.h"
#include <gt_com_api.h>
#include <stdlib.h>
#include "netcmdproc.h"
#include "gate_connect.h"
#include "ipmain_para.h"
#include <gate_cmd.h>
#include <commonlib.h>
#include <gt_errlist.h>
#include "signal.h"
#include "unistd.h"
#include "netcmdproc.h"
#include "mod_socket.h"



#define MAXTHREADS 8
static int threadbusy[2]={0,0};
static pthread_mutex_t mlock[2]		={PTHREAD_MUTEX_INITIALIZER,PTHREAD_MUTEX_INITIALIZER};
static pthread_mutex_t busylock[2] 	={PTHREAD_MUTEX_INITIALIZER,PTHREAD_MUTEX_INITIALIZER};
static pthread_t netcmd_thread_id[MAXTHREADS*2];



int add_threadbusy(int devno, int value)
{
	if(pthread_mutex_lock(&busylock[devno])==0)
	{
		threadbusy[devno]=threadbusy[devno]+value;
		pthread_mutex_unlock(&busylock[devno]);
	}
	return 0;
}

static int init_mainnetvar(void)
{
	return 0;
}
/**********************************************************************************************
 * ������	:print_gate_pkt()
 * ����	:���豸������ͨѶ�����ݰ���ʾ����Ļ��(������)
 * ����	:recv:Ҫ��ӡ��ͨѶ���ݰ�
 * ����ֵ	:��
 **********************************************************************************************/
#ifdef	SHOW_GATE_CMD_REC
void print_gate_pkt(struct gt_pkt_struct* recv)
{
	BYTE *ptr;
	int status;
	ptr=(BYTE*)recv;
	print_buffer(ptr,sizeof(struct gt_pkt_head));
	printf("{");
	ptr=recv->msg;
	print_buffer(ptr,8);
	printf("--");
	status=recv->head.pkt_len+4+2-sizeof(struct gt_pkt_head)-4;//-crc;
	ptr+=8;
	print_buffer(ptr,status-8);
	printf("}");
	ptr=(BYTE*)&recv->msg[status];
	print_buffer(ptr,8);
	printf("\n");
}
#endif



//������������Ƿ�Ϸ�
//���ظ�ֵ��ʾ�Ƿ���0����ֵ��ʾ�ɹ����ص����(0,1,2..)
int check_valid_gate(int dev_no,struct sockaddr_in *guest)
{
	struct ipmain_para_struct *main_para;
	struct sockaddr_in *gate;
	int place=0;
	main_para=get_mainpara();

	for(place=0;place<10;place++)
	{
		switch(place)
		{
			case 0:
				gate=main_para->devpara[dev_no].rmt_gate0;
			break;
			case 1:
				gate=main_para->devpara[dev_no].rmt_gate1;
			break;
			case 2:
				gate=main_para->devpara[dev_no].rmt_gate2;
			break;
			case 3:
				gate=main_para->devpara[dev_no].rmt_gate3;
			break;
			case 4:
				gate=main_para->devpara[dev_no].alarm_gate;
			break;

			default:
				return -1;
			break;
		}

		if(memcmp(&guest->sin_addr.s_addr,&gate->sin_addr.s_addr,sizeof(gate->sin_addr.s_addr))==0)
		{
			return place;
		}

	}
	return -1;
}


//����socket����
int gt_set_server_sock_opt(int fd)
{
	//int addrlen;
	if(fd<0)
		return -1;
	//addrlen=1; 

	 /* �����������ֹ��,���������Եڶ��ο������������õȴ�һ��ʱ��  */ 
	net_activate_reuseaddr(fd);
	 net_activate_keepalive(fd);
//	 setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,&addrlen,sizeof(int));
	return 0;
	
}

//ÿ���߳�ִ�е�����
static void * thread_main(void *arg)
{
     //DWORD net_rec_buf[1024];
     BYTE *net_rec_buf;
     DWORD tempbuf[1024];
     int     connfd,ret;
     int 	rserver_fd=-1;
     struct gt_pkt_struct *recv;
    // void    web_child(int);
     socklen_t clilen;
     int enc,env;
     struct sockaddr_in cliaddr;
     struct ipmain_para_struct *ipmainpara;
     int thread_num;
     struct timeval	timeout;	 
     int recv_cmd_timout;//�������ʱ�ļ����� 0��ʾ��ʹ�� >0��ʾ�೤ʱ��û���յ�����
     fd_set readfds;
     int sel;	 
	 int devno;
	 int listen_fd;
	 mainnet_info *info;
	 info = (mainnet_info *)arg;
	 listen_fd 	= 	info->listen_fd;
	 devno		=	info->dev_no;
	 thread_num	=	info->thread_no;



     printf("dev %d thread %d starting\n", info->dev_no,info->thread_no);
	gtloginfo("start dev %d thread_main%d ...\n",info->dev_no,info->thread_no);


	ret=posix_memalign((void**)&net_rec_buf,sizeof(unsigned long),MAX_FILE_LEN);
	if(ret!=0)
	{
		printf("thread_main%d posix_memalign err ret=%d:%s\n",thread_num,ret,strerror(ret));
		gtloginfo("thread_main%d posix_memalign err ret=%d:%s\n",thread_num,ret,strerror(ret));
		return NULL;
	}
     while (1) 
     {
         clilen =sizeof(cliaddr);
 	  bzero(&cliaddr,sizeof(cliaddr));
	  rserver_fd=-1;
		 if(pthread_mutex_lock(&mlock[devno])==0)
         {
         	connfd = accept(listen_fd, (struct sockaddr *)&cliaddr, &clilen);
         	pthread_mutex_unlock(&mlock[devno]);
         }
	 else
	 	continue;
	 if((connfd<0)&&(errno==EINTR)) 
	          continue; 	

	 else if(connfd<0) 
	{ 
		printf("thread_main Accept Error:%s, exit!\n\a",strerror(errno)); 
		gtlogerr("thread_main accept����:%s,�˳�����\n",strerror(errno));
		exit(1);
	} 
	ret=net_activate_keepalive(connfd);
	ret=net_set_recv_timeout(connfd,3);
	ret=net_set_nodelay(connfd);
	
	 
    add_threadbusy(devno,1);
	if(threadbusy[devno] >=MAXTHREADS)
	{
		send_gate_ack(connfd,0,ERR_DVC_BUSY,get_mainpara()->rmt_env_mode,get_mainpara()->rmt_enc_mode,devno); 
		usleep(500000);
		close(connfd);
		add_threadbusy(devno,-1);
#ifdef SHOW_WORK_INFO
		printf("%sæ������� %s ����������(%d connects)",devlog(devno),inet_ntoa(cliaddr.sin_addr),threadbusy[devno]);
#endif
		gtloginfo("%sæ������� %s ����������(%d connects)",devlog(devno),inet_ntoa(cliaddr.sin_addr),threadbusy[devno]);
		continue;
	}

#ifdef	SHOW_WORK_INFO
		printf("ipmain come a new guest:%s \n",inet_ntoa(cliaddr.sin_addr));
#endif

#ifdef	SHOW_WORK_INFO
		printf("%s ����%s����˿�...\n",inet_ntoa(cliaddr.sin_addr),devlog(info->dev_no));
#endif
		gtloginfo("%s ����%s����˿�...\n",inet_ntoa(cliaddr.sin_addr),devlog(info->dev_no));
		rserver_fd=connfd;//
#ifdef	SHOW_WORK_INFO
		printf("ipmain������һ����%s����������:fd=%d(total :%d)rmt:%s\n",devlog(info->dev_no),rserver_fd,threadbusy,inet_ntoa(cliaddr.sin_addr));
#endif

		gtloginfo("ipmain������һ����%s����������:fd=%d(total :%d)rmt:%s\n",devlog(info->dev_no),rserver_fd,threadbusy,inet_ntoa(cliaddr.sin_addr));

		FD_ZERO(&readfds);
		timeout.tv_sec=1;
		timeout.tv_usec=0;
		recv_cmd_timout=1;	//��Ҫ����
		while(rserver_fd>0)
		{
			FD_SET(rserver_fd,&readfds);
			sel=select(rserver_fd+1,&readfds,NULL,NULL,&timeout);	
			if(sel==0)
			{
				timeout.tv_sec=1;
				timeout.tv_usec=0;
				if(recv_cmd_timout!=0)
				{
					if(++recv_cmd_timout>5)
					{
#ifdef	SHOW_WORK_INFO
						printf("%s ����%s��%d����û�з����κ�����,�Ͽ�����!!\n",inet_ntoa(cliaddr.sin_addr),devno,recv_cmd_timout-1);
#endif
						gtloginfo("%s ����%s��%d����û�з����κ�����,�Ͽ�����!!\n",inet_ntoa(cliaddr.sin_addr),devno,recv_cmd_timout-1);
						break;
					}
				}
				continue;
			}
			else if(sel<0)
			{
				if(errno==EINTR)
					continue;
				else
				{
#ifdef	SHOW_WORK_INFO
					printf("thread_main%d sel=%d errno=%d close connect\n",thread_num,sel,errno);
#endif
					gtloginfo("thread_main%d sel=%d errno=%d close connect\n",thread_num,sel,errno);
					break;
				}
			}
			if(!FD_ISSET(rserver_fd,&readfds))
			{
#ifdef	SHOW_WORK_INFO
				printf("thread_main unknow sel!!\n");
#endif
				gtloginfo("thread_main unknow sel!!\n");
				sleep(1);
			}
				
			recv=(struct gt_pkt_struct*)net_rec_buf;
			ipmainpara=get_mainpara();
			env=ipmainpara->rmt_env_mode;
			enc=ipmainpara->rmt_enc_mode;
			ret=gt_cmd_pkt_recv_env(rserver_fd,recv,MAX_FILE_LEN,tempbuf,0,&env,&enc);
			if(ret>=0)
			{		
#ifdef	SHOW_GATE_CMD_REC
				printf("recv gate pkt->");
				print_gate_pkt(recv);	
#endif
				if((ipmainpara->inst_ack))
				{
					if(env!=ipmainpara->rmt_env_mode)
					{
#ifdef SHOW_WORK_INFO
						printf("�յ��������ŷ��ʽ��ƥ�� set:env=%d recv:env=%d\n",ipmainpara->rmt_env_mode,env);
#endif
						gtlogwarn("�� fd:%d �յ��������ŷ��ʽ��ƥ�� set:env=%d recv:env=%d\n",rserver_fd,ipmainpara->rmt_env_mode,env);
						continue;
					}
				}
				recv_cmd_timout=0;//�Ѿ��յ���һ����������
				process_netcmd(rserver_fd,(struct gt_usr_cmd_struct*)( recv->msg),env,enc,devno);
			}
			else 
			{

				if(ret==-EAGAIN)
					continue;
			#ifdef	SHOW_WORK_INFO
				printf("remote close connection:%d\n",rserver_fd);
			#endif
				gtloginfo("Զ�̷������Ͽ�����˿�����(fd=%d) ret=%d\n",rserver_fd,ret);
				break;
			}
		}
		if(rserver_fd>0)
		{
			net_close(rserver_fd);
		}
		add_threadbusy(devno,-1);
		rserver_fd=-1;
     }
	 return NULL;
}


int thread_make(pthread_attr_t *attr,void *arg)
{
	int i;
	mainnet_info *info;
	info = (mainnet_info *)arg;
	i = info->thread_no;
	
	return pthread_create(&netcmd_thread_id[i+ (info->dev_no)*MAXTHREADS], NULL, &thread_main, (void *) arg);
}


/**********************************************************************************************
 * ������	:init_mainnetcmd_threads()
 * ����	:��������Զ���������ӵ��̳߳�
 * ����	:attr:�߳�����
 *			 para:�ṩ�������Ķ˿�ָ��
 * ����ֵ	:��
 **********************************************************************************************/
int init_mainnetcmd_threads(pthread_attr_t *attr,int port, int dev_no)
{
 	struct sockaddr_in  remote_addr;//,newguest; 
	mainnet_info *info[MAXTHREADS] ;
	int i;
	init_mainnetvar();
	
	
	int listen_fd= -1;
 	if((listen_fd=socket(AF_INET,SOCK_STREAM,0))<0) 
  	{ 
        printf("����Ϊ�������������socket:%s,�����˳�!\n\a",strerror(errno)); 
		gtlogfault("����Ϊ�������������socket,�����˳�!\n");
        exit(1); 
  	}   
	bzero(&remote_addr, sizeof(struct sockaddr_in));
 	remote_addr.sin_family=AF_INET; 
 	remote_addr.sin_port=htons(port); 
 	remote_addr.sin_addr.s_addr=htonl(INADDR_ANY); 
 	gt_set_server_sock_opt(listen_fd);
 	if(bind(listen_fd,(struct sockaddr *)&remote_addr,sizeof(remote_addr))<0) 
  	{ 
  		 gtlogfault("����Ϊ������������,��socket err:%s,�����˳�!\n",strerror(errno));
	     printf("ipmain Bind Error:%s\n\a",strerror(errno)); 
	     exit(1); 
  	} 
  	listen(listen_fd,MAXTHREADS);
	for(i=0;i<MAXTHREADS;i++)
	{
		info[i] = (mainnet_info *)malloc(sizeof(mainnet_info));
		info[i]->listen_fd 	=	listen_fd;
		info[i]->dev_no		=	dev_no;
		info[i]->thread_no	=	i ;
		thread_make(attr,(void*)(info[i]));
	}
	return 0;	
}








