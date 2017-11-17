/*
	����GT1000ϵͳ�����ص������Լ����̷߳������������
*/


//��ʼ�����̷߳��͸����������߳������ͨ��
#include "ipmain.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <pthread.h>
#include <gt_com_api.h>
#include "gate_connect.h"
#include "ipmain_para.h"
#include <gate_cmd.h>
#include <errno.h>
#include "gate_connect.h"
#include "netcmdproc.h"
#include "netinfo.h"
#include <commonlib.h>
#include "mainnetproc.h"
#include "devstat.h"

#include <sys/syscall.h> /*��ͷ�������*/

#define	TRANS_PKT_TO_GATEWAY	0x100	//��Ҫ���������߳̽����ݰ�ת����ȥ


pid_t gettid()
{
     return syscall(SYS_gettid);  /*������ں�*/
}

static struct gate_struct gate_list[MAX_GATE_LIST]=//ϵͳ�������б�
{       
	{PTHREAD_MUTEX_INITIALIZER,-1,0},
	{PTHREAD_MUTEX_INITIALIZER,-1,0},
	{PTHREAD_MUTEX_INITIALIZER,-1,0},
	{PTHREAD_MUTEX_INITIALIZER,-1,0},
	{PTHREAD_MUTEX_INITIALIZER,-1,0},
	{PTHREAD_MUTEX_INITIALIZER,-1,0}//alarm gate
};
/*
static struct gate_struct gate_list_dev2[MAX_GATE_LIST]=//ϵͳ�������б�
{       
	{PTHREAD_MUTEX_INITIALIZER,-1,0},
	{PTHREAD_MUTEX_INITIALIZER,-1,0},
	{PTHREAD_MUTEX_INITIALIZER,-1,0},
	{PTHREAD_MUTEX_INITIALIZER,-1,0},
	{PTHREAD_MUTEX_INITIALIZER,-1,0},
	{PTHREAD_MUTEX_INITIALIZER,-1,0}//alarm gate
};
*/

typedef struct {
DWORD buf[1024];
}buf_t;

static buf_t gaterecv_buf[MAX_DEV_NO];
static buf_t sendrmt_buf[MAX_DEV_NO];
static int gate_recv_ch[MAX_DEV_NO]={-1};
static pthread_t recv_gate_ack_id[MAX_DEV_NO]={-1};	//������IP1004�������������ӵ��߳�id
static pthread_t gate_connect_thread_id[MAX_DEV_NO]={-1};


void *recv_gate_ack_thread(void *devgate);
void *recv_gate_ack_thread(void *agate);
static void *gate_connect_thread(void *para);

/**********************************************************************************************
 * ������	:get_gate_list()
 * ����	:��ȡһ���������ؽṹ�����ָ��
 * ����	:��
 * ����ֵ	:�������ؽṹ�����ָ�룬����Ԫ�صĸ���ΪMAX_GATE_LIST
 **********************************************************************************************/
struct gate_struct *get_gate_list(int dev_no)
{
	if(dev_no == 0)
		return gate_list;
	//else
		//return gate_list_dev2;
}


static int init_gate_com_channel(void)
{
	int i = 0;

	gate_recv_ch[i]=mod_com_init(GATE_RECV_CHANNEL+i,MSG_INIT_ATTRIB);
		
	if(gate_recv_ch[i]<0)
	{
			printf("ipmain %s init_gate_com_channel error!!!\n",devlog(i));
			gtlogerr("%s��ʼ��gate_com_channelʧ��!\n",devlog(i));
			return -1;
	}
#ifdef SHOW_WORK_INFO
		printf("ipmain create a com channel %x for key:%x\n",gate_recv_ch,GATE_RECV_CHANNEL);
		gtloginfo("ipmainΪkeyֵ%x������һ��com channel %x\n",GATE_RECV_CHANNEL,gate_recv_ch);
#endif

	return 0;
}


/**********************************************************************************************
 * ������	:set_gate_ip()
 * ����	:��һ��ip��ַ���õ�����ip��ַ�б���
 * ����	:place:Ҫ���õ��������
 *			 ip:������ip��ַ��ip3,ip2,ip1,ip0��
 *			 port:�������ṩ����Ķ˿ں�
 * ����ֵ	:0��ʾ�ɹ���ֵ��ʾʧ��
 **********************************************************************************************/
int set_gate_ip(int dev_no,int place,DWORD *ip,WORD port)
{
	
	struct gate_struct *gate;
	DWORD newgate;
	WORD newport;
	if(place<0)
		return -1;
	if(place>(MAX_GATE_LIST-1))
		return -1;
	if(ip==NULL)
		return -1;
	if(dev_no == 0)
		gate=&gate_list[place];
	else
		return -1;
	pthread_mutex_lock(&gate->mutex);
	if(gate->fd>0)
	{
		close(gate->fd);
		gate->fd=-1;
		gate->period=0;		
	}
	newgate=htonl(*ip);
	newport=htons(port);
	//printf("going to memcpy\n");
	memcpy(&gate->gateaddr.sin_addr,&newgate,sizeof(DWORD));	
	//printf("finished memcpy\n");
	gate->gateaddr.sin_port=newport;
	//printf("going to unlock\n");
	pthread_mutex_unlock(&gate->mutex);
	//printf("finished unlock\n");
	//gtloginfo("��%s:%d���ӵ�����ip��ַ�б���\n",newgate,newport);
	return 0;
}



#ifdef FOR_PC_MUTI_TEST
#include <time.h>
extern int test_alarm_interval;		//���Ա�����ʱ����
extern int test_alarm_num;		//���Ա����õı�������
static int test_alarm_cnt=0;
extern int test_alarm_need_inc;		//�����������Ƿ���Ҫÿ�α�����Ϣ�仯
static struct send_dev_trig_state_struct test_dev_trig={0,0,0,0,0,0,0,0};	//�����ô���
#endif



static int gatedown_timer=0;
static int req_timesync_timer = 0; //�����ʱ��ʱ��
/**********************************************************************************************
 * ������	:gate_connect_second_proc()
 * ����	:���������̵߳��봦����
 * ����	:��	 
 * ����ֵ	:��
 **********************************************************************************************/
void gate_connect_second_proc()
{
#ifdef FOR_PC_MUTI_TEST
	DWORD	*alarm_stat=NULL;
	int		i;
	struct tm *ptime;
	time_t ctime;	
#endif
	if(get_regist_flag(0) == 1)
	{
		if(++req_timesync_timer > 3600)//ÿСʱ��һ��ʱ
		{
			req_timesync_timer = 0;
			send_dev_req_sync_time(-1, 0, 0,0);
		}
	}
	else
		req_timesync_timer = 0;

	return ;
}
//��ʼ�����������̱߳���
static int init_gate_connect_var(void)
{
	struct ipmain_para_struct *para;	
	para=get_mainpara();
	if(para==NULL)
		return -1;

	return 0;
}

//���ӵ�����
int connect_to_gate(int dev_no, struct gate_struct *gate)
{
	int rc;
	int sock;
	struct sockaddr_in *addr;
	dev_gate_struct *devgate=(dev_gate_struct*)malloc(sizeof(dev_gate_struct));
	if(gate==NULL)
		return -1;
	rc=-1;
	pthread_mutex_lock(&gate->mutex);
	do{
		if(gate->fd>0)
		{
			rc=gate->fd;
			gate->period=GATE_PERIOD_TIME;
		}
		else
		{		
#ifdef SHOW_WORK_INFO
			printf("dev %d connect_to_gate addr is %d\n",dev_no,gate->fd);
#endif
			addr=&gate->gateaddr;			
			if(addr->sin_addr.s_addr!=INADDR_ANY)	//û�����ø����ص�ַ
			{
				addr->sin_family = AF_INET;
				sock = socket(PF_INET, SOCK_STREAM,IPPROTO_TCP);
				if(sock>0)
				{

#ifdef SHOW_WORK_INFO
					printf("ipmain dev %d  connecting to gate %s:%d...",dev_no,inet_ntoa(addr->sin_addr),ntohs(addr->sin_port));
#endif
				
					//rc=connect(sock,(struct sockaddr*)addr,sizeof(struct sockaddr_in));
					rc=connect_timeout(sock,(struct sockaddr*)addr,sizeof(struct sockaddr_in),6);
                    if(rc>=0)
					{
#ifdef SHOW_WORK_INFO
						printf(" gate connect SUCCESS!\n");
#endif	
						//gtlogwarn("��������%s:%d�ɹ�\n",inet_ntoa(addr->sin_addr),ntohs(addr->sin_port));
						//break;
						rc=sock;						
						gate->fd=sock;
						gate->period=GATE_PERIOD_TIME;		
						devgate->dev_no = dev_no;
						memcpy(&devgate->gate,&gate,sizeof(gate));
						create_recv_gate_ack_thread(devgate);
						
						break;
					}
					else
					{
#ifdef SHOW_WORK_INFO
						printf("gate connect FAILED!\n");
						//gtlogwarn("��������%s:%dʧ��\n",inet_ntoa(addr->sin_addr),ntohs(addr->sin_port));
#endif
						close(sock);
						break;
					}
				}
				else
					printf("dev %d connect_to_gate can't create socket!\n",dev_no);
			}
			else
			{
				//û���������ص�ַ
#ifdef SHOW_WORK_INFO
				printf("didnt set the gateway address!!\n");
#endif
			}
		}	
	} while(0);
	pthread_mutex_unlock(&gate->mutex);	
	return rc;
}

static int find_active_gate(struct gate_struct *g_list,int num)
{
	int rc;
	int i;
	struct gate_struct *gate;
	rc=-1;
	gate=g_list;
	for(i=0;i<num;i++)
	{
		pthread_mutex_lock(&gate->mutex);
		if((gate->fd>0))//remed by shixin &&(gate->period>0))
			rc=gate->fd;
		pthread_mutex_unlock(&gate->mutex);
		if(rc>0)
			break;
		gate++;
	}
	return rc;
}
//�������б���Ѱ��һ����������,���û������һ������
//g_list:������ؽṹ���б��б��е����ظ���
//Ŀǰ���ظ������ܳ���MAX_GATE_LIST��
//�������ӵ����ص��ļ�������,����޷������򷵻ظ�ֵ
int get_active_gate(int dev_no, struct gate_struct *g_list,int num)
{
	int i,rc;
	struct gate_struct *gate;
	if(num>MAX_GATE_LIST)
		num=MAX_GATE_LIST;
	gate=g_list;
	rc=-1;
	
    rc=find_active_gate(g_list,MAX_GATE_LIST);

	if(rc<0)
	{
		gate=g_list;
		if(num<0) //�������ӽ�������������
		{
			rc=connect_to_gate(dev_no,g_list+abs(num)-1);
			if(rc>0)
				return rc;
			else
				num = abs(num)-1;
		}	
		for(i=0;i<num;i++)
		{
			rc=connect_to_gate(dev_no,gate);
			if(rc>0)
				break;
			gate++;
		}
	}
	return rc;
}



/**********************************************************************************************
 * ������	:send_gate_pkt()
 * ����	:�����õ����ݰ�ί�з����̷߳��͸�����
 * ����	:fd:��������������,-1��ʾ�ɷ����߳̽���һ����������
 *									   -2��ʾ������ط����������Ӳ�ͨ�����ӽ�������������
 *			len:Ҫ���͵����ݳ���(mod_com_type�ṹ�а���cmd�������ֶεĳ���)
 *			send:�������ݵĽṹָ��,����env,encӦ��Ҳ����
 * ����ֵ	:�������ؽṹ�����ָ�룬����Ԫ�صĸ���ΪMAX_GATE_LIST
 * ע		:���߳�Ҫ�뷢�����ݸ�Զ����������ô˽ӿ�ͳһ����
 *			 ��������Ҫ��Ҫ���͸�Զ�����ط����������ݰ�ȫ������� mod_com_type�ṹ�е�para�ֶ�����
 **********************************************************************************************/
int send_gate_pkt(int fd,struct mod_com_type *send,int len,int dev_no)

{
	int ret;
	if((dev_no >= MAX_VIRDEV_NO)||(dev_no < 0))
	{
		printf("�����豸��%d����ȷ\n",dev_no);
		gtlogerr("�����豸��%d����ȷ\n",dev_no);
		return -1;
	}
	if(gate_recv_ch[dev_no]<0)
	{
		printf("vsmain:can't send_gate_pkt() gate_recv_ch=%d\n",gate_recv_ch[dev_no]);
		gtlogerr("vsmain�޷�����send_gate_pkt(),gate_recv_chΪ%d\n",gate_recv_ch[dev_no]);
		return -1;
	}
	if(fd<0)	//��Ϊ��Ϣ���е�type����С��0����������������Ĳ���
	{
		if(fd==-2)
			fd=2;	//��Ҫ���ӽ�������������
		else
			fd=1;	//С��0�����ת��Ϊ1(��׼���),�ڽ��ն��ж��Ƿ�Ϊ1,��ȷ���Ƿ�����Ч�ļ�������
	}	
	send->target=fd;					
	send->source=dev_no;
	send->cmdlen=len+2;
	send->cmd=TRANS_PKT_TO_GATEWAY;
	ret=mod_com_send(gate_recv_ch[dev_no],send,0);
	if(ret!=0)
		gtloginfo("send_gate_pkt����%d(errno=%d\n):%s\n",ret,errno,strerror(errno));
	return ret;
}

/**********************************************************************************************
 * ������	:send_pkt_to_gate()
 * ����	:��һ�����õ����ݷ��͸�����
 * ����	:fd:�Ѿ������õ���������������
 *			 send:�Ѿ����õĴ������ݽṹָ��
 *			 len:�������ݵĳ���(len+cmd+para)
 *			flag:����������ʱд0
 *			env: 
 *			enc:�����㷨
 * ����ֵ	:�������ؽṹ�����ָ�룬����Ԫ�صĸ���ΪMAX_GATE_LIST
 **********************************************************************************************/



			
static int send_pkt_to_gate(int fd,struct gt_pkt_struct *send,int len,int flag, int env, int enc,int dev_no)
{
	//int env,enc;
	int rc;
	struct sockaddr_in addr;
	struct gt_usr_cmd_struct* cmd;
	socklen_t  addrlen=sizeof(struct sockaddr);
	struct ipmain_para_struct *para;
	DWORD remote,localip,mask;
	struct ip1004_info_struct *info;
	
	if(fd>0)
	{
		para= get_mainpara();

		cmd=(struct gt_usr_cmd_struct*)send->msg;
		switch(cmd->cmd)
		{
			case DEV_REGISTER:
				rc=getpeername(fd,(struct sockaddr *)&addr,&addrlen);//��ȡ���ӶԷ���ip��ַ
				info=(struct ip1004_info_struct *)cmd->para;
				//printf("remote rc=%d %s\n",rc,inet_ntoa(addr.sin_addr));
				if(rc==0)
				{
					mask=(DWORD)para->lan_mask;
					localip=(DWORD)para->lan_addr;
					memcpy(&remote,&addr.sin_addr,4);
					if((remote&mask)!=(localip&mask))
					{
						//����һ������
						info->dev_ip=ntohl(para->wan_addr);
						
					}
					else
					{//��1����������
						info->dev_ip=ntohl(para->lan_addr);
					}
					
				}				
			break;
			default:
			break;
		}

		//printf("para->rmt_env_mode=%d,para->rmt_enc_mode=%d\n",para->rmt_env_mode,para->rmt_enc_mode);
		//return gt_cmd_pkt_send(fd,send,len,NULL,0,(BYTE)para->rmt_env_mode,(BYTE)para->rmt_enc_mode);
		return gt_cmd_pkt_send(fd,send,len,NULL,0,(BYTE)env,(BYTE)enc);
	
	}
	else
	{
		printf("send_pkt_to_gate:fd=%d fix it\n",fd);
		gtloginfo("send_pkt_to_gate:fd=%d fix it\n",fd);
	}
	return -3;
}

//���������߳̽��������̷߳���������
static int recv_gate_cmd(struct mod_com_type *recv,int dev_no)
{
	int rc;
	rc = mod_com_recv(gate_recv_ch[dev_no],0,recv,MAX_MOD_CMD_LEN,0);
	return rc;

}

static int log_connect_ok_flag[MAX_VIRDEV_NO]={0,0};	//��¼���������سɹ���־
static int log_connect_fail_flag[MAX_VIRDEV_NO]={0,0};  //��¼����������ʧ�ܱ�־


//������յ��������̷߳����������������
int gate_connect_cmd_proc(struct mod_com_type *cmd)
{
	struct gt_pkt_struct *send;
	struct gt_usr_cmd_struct *send_cmd;
	struct sockaddr_in gate_addr;
	struct ipmain_para_struct *mainpara;
	socklen_t  addrlen;
	int rmt_fd;
	int sendlen;
	int trynum;
	int ret;
	int env,enc;
	int dev_no;
	
	dev_no = cmd->source;
	switch(cmd->cmd)
	{
		case TRANS_PKT_TO_GATEWAY:
			send=(struct gt_pkt_struct *)(sendrmt_buf[dev_no].buf);
			sendlen=cmd->cmdlen-2;	//ȥ��cmd->cmd�ĳ���
			memcpy(send->msg,cmd->para,sendlen);	

			printf("ipmain get TRANS_PKT_TO_GATEWAY cmd from source %d,target is %d\n",cmd->source,cmd->target);
			if(cmd->target<=2)	//���С�ڵ���2��ʾû��ȷ������������
			{	
				if(cmd->target==2)
					trynum=-MAX_GATE_LIST;
				else
					trynum=MAX_GATE_LIST-1;//���һ����ַ�ǽ�������������
				rmt_fd=get_active_gate(dev_no,get_gate_list(dev_no),trynum);
				
				 
				
				if(rmt_fd>0)
				{
					set_gate_connect_flag(dev_no,1);
					addrlen=sizeof(struct sockaddr_in);
					ret=getpeername(rmt_fd,(struct sockaddr *)&gate_addr,&addrlen);//��ȡ���ӶԷ���ip��ַ
					if(ret>=0)
					{
						if(!log_connect_ok_flag[dev_no])
						{
							gtloginfo("%s���ӹ�ͨ����%s:%d�ɹ�,fd=%d\n",devlog(dev_no),inet_ntoa(gate_addr.sin_addr),ntohs(gate_addr.sin_port),rmt_fd);
							log_connect_ok_flag[dev_no]=1;
							log_connect_fail_flag[dev_no]=0;  
						}
					}			
					//������Ƿ�����Ҫ���·��͵ı����Ĺ����ŵ� recv_gate_ack_thread �߳���

				}
				else
				{
					set_gate_connect_flag(dev_no,0);
				//printf("#########rmt_fd<=0\n");
					if(!log_connect_fail_flag[dev_no])
					{
						log_connect_fail_flag[dev_no]=1;
						log_connect_ok_flag[dev_no]=0;
						gtlogerr("%s�������ӵ��κι�ͨ���ط�����\n",devlog(dev_no));
					}
				}
				mainpara = get_mainpara();
				env = mainpara->rmt_env_mode;
				enc = mainpara->rmt_enc_mode;
			
			}
			else
			{
				rmt_fd=cmd->target;
				env = cmd->env;
				enc = cmd->enc;
			}
			if(rmt_fd<0)
				ret=-1;
			else
			{
				addrlen=sizeof(struct sockaddr_in);
				ret=getpeername(rmt_fd,(struct sockaddr *)&gate_addr,&addrlen);//��ȡ���ӶԷ���ip��ַ
				ret=send_pkt_to_gate(rmt_fd,send,sendlen,0,env,enc,dev_no);
				send_cmd=(struct gt_usr_cmd_struct*)send->msg;

#ifdef SHOW_WORK_INFO
				printf("%s��������:0x%04x(%s) ��%s(fd=%d) ������%d\n",devlog(dev_no),send_cmd->cmd,get_gtcmd_name(send_cmd->cmd),inet_ntoa(gate_addr.sin_addr),rmt_fd,ret);
#endif
			}
			
		break;
		default:
			printf("ipmain: dev %d gate_connect_trhead recv a unknow cmd:%x\n",dev_no,cmd->cmd);
			ret=-1;
		break;
	}
	return ret;
}


/**********************************************************************************************
 * ������	:creat_connect_thread()
 * ����	:����һ�����Ӳ��������� �����ص��߳�
 * ����	:attr:�����̵߳�����
 *		 
 * ����ֵ	:0��ʾ�ɹ�����ֵ��ʾʧ��
 **********************************************************************************************/
int creat_connect_thread(pthread_attr_t *attr)
{
	int i = 0;
	init_gate_com_channel();	//��ʼ�����߳������������߳�ͨѶ�����ͨ��
	pthread_create(&gate_connect_thread_id[i],attr, gate_connect_thread, (void*)i);//�������������Լ��������߳�ת������������͸����ص��߳�
	return 0;
}
//�������������Լ��������߳�ת������������͸����ص��߳�
static void *gate_connect_thread(void *p)
{	
	int rc;
	struct mod_com_type *recv;
	int log_errflag=0;
	int dev_no;

	
	dev_no =(int)p;


	printf("ipmain gate_connect_thread thread running...\n");
	gtloginfo("start gate_connect_thread %d...\n",dev_no);
	init_gate_connect_var();
	while(1)
	{		
		if(gate_recv_ch[dev_no]<0)
		{
			sleep(1);
			continue;
		}
		recv=(struct mod_com_type *)(gaterecv_buf[dev_no].buf);
		rc=recv_gate_cmd(recv,dev_no);
		if(rc>=0)
		{
			log_errflag=0;
			gate_connect_cmd_proc(recv);
		}
		else
		{
			if(!log_errflag)
			{
				log_errflag=1;
				gtlogerr("���������̶߳�ȡ�������:%d:%s",rc,strerror(errno));
			}
			printf("ipmain recv_gate_cmd error rc=%d %s!!\n",rc,strerror(errno));
			usleep(500000);
		}
	}
	
	return NULL;
}


/*
	���ղ�������ip1004���������ؽ��������ӵ�������
*/
int create_recv_gate_ack_thread(dev_gate_struct *devgate)
{
	int *id;
	pthread_attr_t  thread_attr,*attr;
	int dev_no;
	dev_no = devgate->dev_no;
	
	id=(int*)&recv_gate_ack_id[dev_no];
	if(*id<0)
	 {
		if(get_gtthread_attr(&thread_attr)==0)
			attr=&thread_attr;
		else
			attr=NULL;
		pthread_create(&recv_gate_ack_id[dev_no], attr, recv_gate_ack_thread, devgate);
		

		if(attr!=NULL)
		{
			pthread_attr_destroy(attr);
		}
#ifdef SHOW_WORK_INFO
		printf("dev %d create recv_gate_ack_thread thread %d\n",dev_no,*id);
#endif
	}
	else
	{
#ifdef SHOW_WORK_INFO
		printf("dev %d recv_gate_ack_thread thread already started!\n",dev_no);
#endif
		gtloginfo("dev %d recv_gate_ack_thread thread already started!\n",dev_no);
		return -1;
	}
	return 0;
}
/*
	�������ͨ����ʱ��Ҫ����һЩ��
*/
int ProcessGateConnect(int fd, int dev_no)
{//added by shixin
	struct ipmain_para_struct *para;
	//���͸����˵������ļ���������
	para= get_mainpara();

#ifndef  FOR_PC_MUTI_TEST
	if((CheckParaFileChange(dev_no,2)==1))
	{
		AddParaFileVersion(dev_no,2);
		send_para_to_rmt(fd,2,para->rmt_env_mode,para->rmt_enc_mode,1,dev_no);
		CopyPara2Bak(dev_no,2);		
		send_dev_state(fd,1,1,para->rmt_env_mode,para->rmt_enc_mode,dev_no);
	}
	else if(para->devpara[dev_no].sendgateini_flag == 1)
	{
		send_para_to_rmt(fd,2,para->rmt_env_mode,para->rmt_enc_mode,1,dev_no);
	}
	
	para->devpara[dev_no].sendgateini_flag = 0;
#endif

	
//	send_para2gate_flag3=0;
	return 0;
}


void *recv_gate_ack_thread(void *dev_gate)
{
	int netfd;
	int ret;//,quitflag;
	fd_set readfds;
	int sel;
	int env=0,enc=0;
	in_addr_t addr;
	

	struct ipmain_para_struct *para;
	struct gt_pkt_struct *recv;
	struct gate_struct *gate;
	struct timeval	timeout;
	struct sockaddr_in gate_addr;
	socklen_t addrlen;
	int temp;
 	DWORD recv_gate_ack_buffer[1024];
	DWORD	recv_gate_ack_tmp_buf[1024];
	int		connect_time=0;		//�̴߳�����ʱ��(��Ϊ��λ)

	int dev_no;
	dev_gate_struct *devgate;
	if(dev_gate==NULL)
	{
		printf("recv_gate_ack_thread's para=NULL exit thread !\n");
		return NULL;
	}
	devgate =(dev_gate_struct *)dev_gate;
	dev_no	= devgate->dev_no;	
	gate=(struct gate_struct *)devgate->gate;
#ifdef SHOW_WORK_INFO
	printf("dev %d enter recv_gate_ack_thread! fd %d\n",dev_no,gate->fd);
#endif
	gtloginfo("dev %d start recv_gate_ack_thread...\n",dev_no);
	
	memset(recv_gate_ack_buffer,0,1024*sizeof(DWORD));
	recv=(struct gt_pkt_struct*)recv_gate_ack_buffer;
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);		//�����߳�Ϊ��ȡ����
	pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
	FD_ZERO(&readfds);
	timeout.tv_sec=1;
	timeout.tv_usec=0;
	pthread_mutex_lock(&gate->mutex);
	netfd=gate->fd;	//�����ط������޸�gate->fd
	pthread_mutex_unlock(&gate->mutex);
	//quitflag=0;
	addrlen=sizeof(struct sockaddr); //modified by wsy
	getpeername(netfd,(struct sockaddr *)&gate_addr,&addrlen);
	connect_time=time(NULL);
	para= get_mainpara();
	ret=net_activate_keepalive(netfd);
	ret=net_set_keepalive_time(netfd,3,5,5);
	ret=net_set_recv_timeout(netfd,3);//FIXME
	ret=net_set_nodelay(netfd);

	ProcessGateConnect(netfd,dev_no);
	
	while(1)
	{

//	printf("gate->period=%d!!!!!!!!!\n",gate->period);
		
	    FD_ZERO(&readfds);
		if(netfd>0)
		{
			FD_SET(netfd,&readfds);
			sel=select(netfd+1,&readfds,NULL,NULL,&timeout);	
			
		}
		else
			break;
		

		if(sel==0)
		{
			//FD_ZERO(&readfds);
			timeout.tv_sec=1;
			timeout.tv_usec=0;
#if 0
                    //added by shixin
                    if((((int)time(NULL)-connect_time)%3600)==0)
                    {//����1Сʱ��һ��ʱ
						send_dev_req_sync_time(netfd,para->rmt_env_mode,para->rmt_enc_mode,dev_no);
                    }
#endif
			if(gate->fd<0)
			{
				printf("dev %d recv_gate_ack_thread gate->fd=%d break!\n",dev_no,gate->fd);
				gtloginfo("dev %d recv_gate_ack_thread gate->fd=%d break!\n",dev_no,gate->fd);
				log_connect_ok_flag[dev_no]=0;
				log_connect_fail_flag[dev_no]=0;

				break;
			}
			else
				continue;
		}
		else if(sel<0)
		{
			if(errno==EINTR)
				continue;
			else
			{
				gtloginfo("dev %d recv_gate_ack_thread sel=%d errno=%d �ر���������\n",dev_no,sel,errno);
				pthread_mutex_lock(&gate->mutex);
				//if(gate->period<=0)
				{
					gate->period=0;
					gate->fd=-1;
					log_connect_ok_flag[dev_no]=0;
					log_connect_fail_flag[dev_no]=0;
				}
				pthread_mutex_unlock(&gate->mutex);
				close(netfd);		
				break;
			}
		}
		if(FD_ISSET(netfd,&readfds))
		{			
			env=para->rmt_env_mode;
			enc=para->rmt_enc_mode;

#ifdef SHOW_WORK_INFO
			printf("gate master connect ...!!!!!!!!!!!!!!!!\n");
#endif
			ret=gt_cmd_pkt_recv_env(netfd,recv,sizeof(recv_gate_ack_buffer),recv_gate_ack_tmp_buf,0,&env,&enc);
#ifdef SHOW_WORK_INFO
			printf("gate master connect recv ret=%d ###########\n",ret);
#endif

			if(ret>=0)
			{		
#ifdef	SHOW_GATE_CMD_REC
				printf("dev %d recv gate pkt->",dev_no);
				print_gate_pkt(recv);	
#endif
				if((para->inst_ack))
				{
					if(env!=para->rmt_env_mode)
					{
#ifdef SHOW_WORK_INFO
							printf("%s�յ��������ŷ��ʽ��ƥ�� set:env=%d recv:env=%d\n",devlog(dev_no),para->rmt_env_mode,env);
#endif
							gtlogwarn("%s�յ��������ŷ��ʽ��ƥ�� set:env=%d recv:env=%d\n",devlog(dev_no),para->rmt_env_mode,env);

							continue;
					}
				}
				
				
				pthread_mutex_lock(&gate->mutex);
				gate->period=GATE_PERIOD_TIME;		//�յ�һ�������ָ���ʱʱ��
				pthread_mutex_unlock(&gate->mutex);
				process_netcmd(netfd,(struct gt_usr_cmd_struct*)( recv->msg),env,enc,dev_no);
			}
			else
			{
				
				if(ret==-EAGAIN)
					continue;
				set_gate_connect_flag(dev_no,0);
				//rc=
				addrlen=sizeof(struct sockaddr); //modified by wsy
				if(ret==-ETIMEDOUT)
				{
	#ifdef		SHOW_WORK_INFO
					 printf("ETIMEDOUT ����:%s������%s(fd=%d) ���������ӳ�ʱ,Ӧ����ע��\n",devlog(dev_no),inet_ntoa(gate_addr.sin_addr),netfd);
	#endif
					 gtlogerr("����ETIMEDOUT:%s������ %s (fd=%d)���������ӳ�ʱ,Ӧ����ע��\n",devlog(dev_no),inet_ntoa(gate_addr.sin_addr),netfd);
				}
				else if(ret==-EHOSTUNREACH)
				{
	#ifdef		SHOW_WORK_INFO
					printf("%s������ %s(fd=%d) ���������� EHOSTUNREACH ����:host unreachable,Ӧ����ע��\n",devlog(dev_no),inet_ntoa(gate_addr.sin_addr),netfd);
	#endif
					gtlogerr("%s������ %s(fd=%d) ����������host unreachable,Ӧ����ע��\n",devlog(dev_no),inet_ntoa(gate_addr.sin_addr),netfd);
				}
				else
				{
	#ifdef		SHOW_WORK_INFO
					printf("remote gate close device %d master connection,ret =%d\n",dev_no,ret);
	#endif
					temp=0-ret;
					gtloginfo("Զ�����ط�����:%s(fd=%d) �ر�%s����������,ret=%d(%s)\n",inet_ntoa(gate_addr.sin_addr),netfd,devlog(dev_no),ret,strerror(temp));
				}

				break;
			}
		}
		else
		{
#ifdef		SHOW_WORK_INFO
			printf("dev %d recv_gate_ack_thread no fd is set\n",dev_no);
#endif
			gtlogerr("dev %d recv_gate_ack_thread no fd is set\n",dev_no);
			sleep(1);
		}
		//FD_SET(netfd,&readfds);
		
	}
					
#ifdef SHOW_WORK_INFO
	printf("dev %d exit recv_gate_ack_thread connect :%d s...\n",dev_no,(int)(time(NULL)-connect_time));
#endif	
	gtloginfo("dev %d exit recv_gate_ack_thread connect :%d s...\n",dev_no,(int)(time(NULL)-connect_time));
				
	sleep(20);			//������Ҫ��һ��ʱ�����������
	
	pthread_mutex_lock(&gate->mutex);
	//if(gate->period<=0)
	{
		gate->period=0;
		gate->fd=-1;		
		close(netfd);	
		log_connect_ok_flag[dev_no]=0;
		log_connect_fail_flag[dev_no]=0;
	}
	pthread_mutex_unlock(&gate->mutex);
	set_regist_flag(dev_no,0);
	recv_gate_ack_id[dev_no]=-1;
	return NULL;
}


