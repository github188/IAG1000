#include "ipmain.h"
//#include "commonlib.h"
#include "ipmain_para.h"
#include "mainnetproc.h"
#include "watch_board.h"
#include "watch_process.h"
#include <gt_com_api.h>
#include <devinfo.h>
#include <signal.h>
#include <dirent.h>
#include "rtimg_para2.h"
#include "avserver.h"
#include "netcmdproc.h"
#include "net_avstream.h"
#include "net_aplay.h"
#include "devstat.h"
#include "common/commonlib.h"
#include "weigen_recv.h"

#include "gate_connect.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>

/****************��ulibc������********************************/
/*
int posix_memalign(void **memptr, size_t alignment, size_t size)
{ if (alignment % sizeof(void *) != 0)
		//|| !powerof2(alignment / sizeof(void *)) != 0
		//	|| alignment == 0
	return -EINVAL;

	*memptr = memalign(alignment, size);
	return (*memptr != NULL ? 0 : ENOMEM);
}
*/

/************************************************************************************/

/***************����־�ϼ�¼�����˳�״̬*********************/
static void exit_log(int signo) {
	switch(signo)
	{
		case SIGPIPE:
			printf("ipmain process_sig_pipe \n");	
			return ;
		break;
		case SIGTERM:
			gtloginfo("ipmain ��kill,�����˳�!!\n");
			exit(0);
		break;
		case SIGKILL:
			gtloginfo("ipmain SIGKILL,�����˳�!!\n");
			exit(0);
		break;
		case SIGINT:
			gtloginfo("ipmain ���û���ֹ(ctrl-c)\n");
			exit(0);
		break;
		case SIGUSR1:
			//���ϵͳ��Ϣ��ָ���ļ�
			//lc do 
		break;
		case SIGSEGV:
			gtloginfo("ipmain �����δ���\n");
			printf("ipmain segmentation fault\n");
			exit(0);
		break;
	}
	return;
}

/*
static int deamon_init(void)
{
	pid_t pid;
	int i;
	int maxopen;
	if((pid=fork())<0)
	{
		printf("deamon fork error !\n");
		return -1;
	}
	if(pid!=0)
		exit(0);
	setsid();
	chdir("/");
	umask(0);

	maxopen=sysconf(_SC_OPEN_MAX);
	if(maxopen<0)
	{
		printf("sysconf error !\n");
		return -1;
	}
	for (i=3;i<maxopen;i++)
	{
		close(i);
	}
	close(0);	//������׼����ʹ������
	
	
	return 0;
}
*/

/**********************************************************************************************
 * ������	:get_gtthread_attr()
 * ����	:��ȡһ��Ĭ���߳����Խṹ
 * ���	:attr:����ʱ�������Խṹ
 * ����ֵ	:0��ʾ�ɹ���ֵ��ʾ����
 * ע		:�����Ӧ�õ���pthread_attr_destroy�����ͷ�
 **********************************************************************************************/
int get_gtthread_attr(pthread_attr_t *attr)
{
	int rc;
	if(attr==NULL)
		return -1;
	memset((void*)attr,0,sizeof(pthread_attr_t));
	rc=pthread_attr_init(attr);
	if(rc<0)
		return -1;
	rc=pthread_attr_setdetachstate(attr,PTHREAD_CREATE_DETACHED);//����״̬
	rc=pthread_attr_setschedpolicy(attr,SCHED_OTHER);
	return 0;	
}

int main(int argc,char **argv)
{
	pthread_attr_t  thread_attr,*attr;				//�߳����Խṹ
	
    struct ipmain_para_struct *ipmain_para;		//����ָ��
	int lock_file;									//���ļ���������
	char pbuf[100];								//��ʱ������
	unsigned char devid[32];								//�豸guid��ʱ������
	unsigned short serv_port;						//�������˿�

	gtopenlog("ipmain");							//����־
	setsid();										//���ý���Ϊ��ͷ����
	show_time(NULL);								//��ʾϵͳ��ǰʱ�� ��.����

#ifdef DEAMON_MODE
	if(deamon_init()!=0)
	{
		printf("deamon_init error!\n");
	}
#endif

	gtloginfo("����ipmain (ver:%s).......\n",VERSION);

	/***************ע���źŴ�����**********************************/
	signal(SIGKILL,exit_log);		//kill -9�ź�
	signal(SIGTERM,exit_log);		//��ͨ��kill�ź�
	signal(SIGINT,exit_log);		//ctrl-c�ź�
	signal(SIGSEGV,exit_log);		//�δ����ź�
	signal(SIGPIPE,exit_log);		//���ѹرյ�socket��д�����ź�

    /**************�豸��Ϣ��״̬�ȳ�ʼ��******************************/
	//todo ��Ϊjson ��ʽ��ȡ
	init_devinfo();					//�������ļ���ʼ���豸��Ϣ(guid��)
	init_devstat();				//��ʼ���豸״̬
	
	get_devid(devid);
	printf("devid=%02x%02x%02x%02x%02x%02x%02x%02x\n",devid[7],devid[6],devid[5],devid[4],devid[3],devid[2],devid[1],devid[0]);
	gtloginfo("devid=%02x%02x%02x%02x%02x%02x%02x%02x\n",devid[7],devid[6],devid[5],devid[4],devid[3],devid[2],devid[1],devid[0]);
	
	//todo ��json ��ʽ��ȡ��˿ڵ���Ҫ�ı�
	init_para();					//������������Ϊ��ʼֵ



	//todo ��json ��ʽ��ȡ��˿ڵ���Ҫ�ı�
	readmain_para_file(IPMAIN_PARA_FILE,get_mainpara());	//�������ļ���ȡ������
	
    ipmain_para=get_mainpara();

    /*******************������߳�************************************/
	init_netcmd_var();						//��ʼ��������ͨѶ��״̬
		
	printf("prepare to start threads...\n");			
	if(get_gtthread_attr(&thread_attr)==0)
		attr=&thread_attr;
	else
		attr=NULL;
		
	creat_connect_thread(attr);				//�������������߳�

	//todo from mips io drivers
	creat_watch_board_thread(attr);		//��������������ӵ��߳�

	
	//todo ��Ϊ����gtc����sdk ���ӵ���Ϣ,ͬ�����Դ�����������
	init_mainnetcmd_threads(attr,ipmain_para->devpara[0].cmd_port,0);

	if(attr!=NULL)
	{
		pthread_attr_destroy(attr);
	}

	sleep(1);
	show_time(NULL);							//��ʾϵͳ��ǰʱ�� ��.����
	//todo ����ý������
	init_server_para();               ///<ϵͳ������״̬��ʼ��
	//todo ��ȡ�����ļ�Ӧ��readmain_para_fileʵ��
	read_server_para_file();
	
	//todo ����Ƶ���ݷ����̣߳��Ժ�ʵ������Ӧ�ô���ͷ���һ����
	create_av_server();
	//todo ����Ƶ��������̣߳��Ժ�Ӧ����create_connnect_thread/init_mainnetcmd_thread����Ľ��պ���������
	create_rtnet_av_servers();
	//todo ��Ƶ������������̺߳�������ȡ�̣߳��Ժ����û������������
	create_rtnet_aplay_servers();

	signal(SIGUSR1,exit_log);

	//todo ����һ��nvr �����߳�
	//create_nvr_manager_thread(attr);
	//todo ����Τ����ȡ�߳�
	create_weigen_server();
	
	//��������߳�
	watch_process_thread();					//ת��Ϊ����߳�
	closelog();
	exit(-1);
}

