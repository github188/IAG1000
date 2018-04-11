/*
        ��������51ģ��ͨѶ������
*/

#include "ipmain.h"

#include <errno.h>   
#include "watch_board.h"
#include "commonlib.h"
#include <netinet/in.h>	//ntohs
#include <gt_com_api.h>
#include "netcmdproc.h"
#include "watch_process.h"
#include "alarm_process.h"
#include "ipmain_para.h"
#include "devstat.h"
#include "gate_cmd.h"
//#include "gtvs_io_api.h"

static pthread_t watchbd_thread_id=-1;
static int ipalarm_fd[4] = {-1};


//ȡ��ǰʱ��ʹ���״̬�����dev_trig�ṹ
int get_dev_trig(int dev_no,struct send_dev_trig_state_struct *dev_trig)
{
	time_t timep;
	struct tm *p;
	
	DWORD year;
	BYTE month,day,hour,min,sec;
	struct ip1004_state_struct *gtstate;
	if(dev_trig==NULL)
		return -1;
	gtstate=get_ip1004_state(dev_no);

	pthread_mutex_lock(&gtstate->mutex);
	memcpy(&dev_trig->alarmstate,(char *)&gtstate->reg_trig_state,4);
	pthread_mutex_unlock(&gtstate->mutex);

  	
	time(&timep);
	p=localtime(&timep);
	year=1900+p->tm_year;
	memcpy(&(dev_trig->year),&year,2);

	month=1+p->tm_mon;
	memcpy(&(dev_trig->month),&month,1);
	day=p->tm_mday;
	hour=p->tm_hour;
	min=p->tm_min;
	sec=p->tm_sec;
	memcpy(&(dev_trig->day),&day,1);
	memcpy(&(dev_trig->hour),&hour,1);
	memcpy(&(dev_trig->minute),&min,1);
	memcpy(&(dev_trig->second),&sec,1);
	return 0;
}


static DWORD oldalarmin = 0;
/**********************************************************************************************
 * ������	:watch_board_second_proc()
 * ����	:����ι�����봦����
 * ����	:��
 * ����ֵ	:��
 **********************************************************************************************/
void watch_board_second_proc(void)
{
	time_t timenow;
	timenow=time((time_t *)NULL);	
	process_trigin_event(get_ip1004_state(0)->alarmin_state,oldalarmin,timenow);
	oldalarmin = get_ip1004_state(0)->alarmin_state;
}

static const int alarm_port[4] = {5655,5656,5657,5658};
static fd_set ipalarm_read_fds;
static int ipalarm_maxfds = 0;

int init_ipalarm_fdset()
{
	int i;
	FD_ZERO(&ipalarm_read_fds);
	for (i=0; i<get_video_num(); ++i)
	{
		if(ipalarm_fd[i]>0)
		  FD_SET(ipalarm_fd[i],&ipalarm_read_fds);

		if(ipalarm_maxfds <= ipalarm_fd[i])    
		  ipalarm_maxfds = ipalarm_fd[i];
	}

	return 0;
}

//���������ݵ��߳�
#define USE_IO
static void *watch_board_thread(void *para)
{
	DWORD oldstate = 0;//��¼�ɵĶ�������״̬���뱨���޹�
	DWORD oldpower = 0;
	DWORD newstate = 0;
	DWORD trig_diff = 0; //��¼�¾�����״̬�Ĳ���
	DWORD powerstate = 0;
	struct ipmain_para_struct *mainpara; 
	int i;
	time_t timep;
	struct ip1004_state_struct *ip1004state;
	printf("watch_board_thread running...\n");
	gtloginfo("start watch_board_thread...\n");

	mainpara = get_mainpara();
	ip1004state = get_ip1004_state(0);

	init_gpiodrv();

	while(1)
	{
		//lc do 
#ifdef USE_IO		
		read_trigin_status(&newstate);
		//process the state
		printf("newstate is %x tin mask is %x\n",newstate,get_mainpara()->tin_mask);
		newstate&=get_mainpara()->tin_mask;
		if(newstate != oldstate)
		{
			
			time(&timep);
			//����־
			printf("trigger from %02x to %02x\n",(int)oldstate,(int)newstate);
			gtloginfo("���Ӵ���״̬�ı� %02x->%02x\n",(int)oldstate,(int)newstate);
			trig_diff = newstate ^ oldstate; 	
		
			printf("ip1004state->alarmin_state=[0x%lx]...newstate=0x%lx\n",ip1004state->alarmin_state,newstate);
			gtloginfo("ip1004state->alarmin_state=[0x%lx]...newstate=0x%lx\n",ip1004state->alarmin_state,newstate);
			printf("ip1004state->old_alarmin_state=[0x%lx]...oldstate=0x%lx\n",ip1004state->old_alarmin_state,oldstate);
			gtloginfo("ip1004state->old_alarmin_state=[0x%lx]...oldstate=0x%lx\n",ip1004state->old_alarmin_state,oldstate);

			//���µ�ip1004state��ȥ
			ip1004state->alarmin_state = newstate;
			ip1004state->old_alarmin_state = oldstate;
			ip1004state->alarmin_change_time = timep;
			
			//����״̬���豸
			send_dev_state(-1,1,0,-1,-1,0);
			send_alarmin_state_change(oldstate,newstate,timep,0);
		}
		process_trigin_event(newstate,oldstate,timep);//���������
		oldstate=newstate;
#endif
		//lc do
		usleep(1000000);
	}
	
	return 0; //should never reach here
}


int init_ipalarm_fd()
{
	int i,recvfd;
	struct sockaddr_in server;
	
	for(i=0;i<4;++i)
	{
		if((recvfd=socket(AF_INET,SOCK_DGRAM,0))==-1)
		{
			perror("Creating recvfd failed.");
			exit(1);
		}
		bzero(&server,sizeof(server));
		server.sin_family = AF_INET;
		server.sin_port = htons(alarm_port[i]);
		server.sin_addr.s_addr = htonl(INADDR_ANY);
		if(bind(recvfd,(struct sockaddr *)&server,sizeof(struct sockaddr))==-1)
		{
			perror("Bind()error.");
			exit(1);

		}
		ipalarm_fd[i] = recvfd;
		printf("recv fd at %d chn is %d\n",i,recvfd);
	}

	return 0;
}





/**********************************************************************************************
 * ������	:creat_watch_board_thread()
 * ����	:�������ղ�����51����ģ�鷢����������߳�
 * ����	:attr:�߳�����
 * ����ֵ	:0��ʾ�ɹ�����ֵ��ʾʧ��
 **********************************************************************************************/
int creat_watch_board_thread(pthread_attr_t *attr)
{
	return pthread_create(&watchbd_thread_id,attr, watch_board_thread, NULL);//�������Ӷ���������߳�
}


