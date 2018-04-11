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

/****************在ulibc环境下********************************/
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

/***************在日志上记录程序退出状态*********************/
static void exit_log(int signo) {
	switch(signo)
	{
		case SIGPIPE:
			printf("ipmain process_sig_pipe \n");	
			return ;
		break;
		case SIGTERM:
			gtloginfo("ipmain 被kill,程序退出!!\n");
			exit(0);
		break;
		case SIGKILL:
			gtloginfo("ipmain SIGKILL,程序退出!!\n");
			exit(0);
		break;
		case SIGINT:
			gtloginfo("ipmain 被用户终止(ctrl-c)\n");
			exit(0);
		break;
		case SIGUSR1:
			//输出系统信息到指定文件
			//lc do 
		break;
		case SIGSEGV:
			gtloginfo("ipmain 发生段错误\n");
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
	close(0);	//保留标准输出和错误输出
	
	
	return 0;
}
*/

/**********************************************************************************************
 * 函数名	:get_gtthread_attr()
 * 功能	:获取一个默认线程属性结构
 * 输出	:attr:返回时填充好属性结构
 * 返回值	:0表示成功负值表示出错
 * 注		:用完后应该调用pthread_attr_destroy进行释放
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
	rc=pthread_attr_setdetachstate(attr,PTHREAD_CREATE_DETACHED);//分离状态
	rc=pthread_attr_setschedpolicy(attr,SCHED_OTHER);
	return 0;	
}

int main(int argc,char **argv)
{
	pthread_attr_t  thread_attr,*attr;				//线程属性结构
	
    struct ipmain_para_struct *ipmain_para;		//参数指针
	int lock_file;									//锁文件的描述符
	char pbuf[100];								//临时缓冲区
	unsigned char devid[32];								//设备guid临时缓冲区
	unsigned short serv_port;						//命令服务端口

	gtopenlog("ipmain");							//打开日志
	setsid();										//设置进程为领头进程
	show_time(NULL);								//显示系统当前时间 秒.毫秒

#ifdef DEAMON_MODE
	if(deamon_init()!=0)
	{
		printf("deamon_init error!\n");
	}
#endif

	gtloginfo("启动ipmain (ver:%s).......\n",VERSION);

	/***************注册信号处理函数**********************************/
	signal(SIGKILL,exit_log);		//kill -9信号
	signal(SIGTERM,exit_log);		//普通的kill信号
	signal(SIGINT,exit_log);		//ctrl-c信号
	signal(SIGSEGV,exit_log);		//段错误信号
	signal(SIGPIPE,exit_log);		//向已关闭的socket中写数据信号

    /**************设备信息，状态等初始化******************************/
	//todo 改为json 方式读取
	init_devinfo();					//用配置文件初始化设备信息(guid等)
	init_devstat();				//初始化设备状态
	
	get_devid(devid);
	printf("devid=%02x%02x%02x%02x%02x%02x%02x%02x\n",devid[7],devid[6],devid[5],devid[4],devid[3],devid[2],devid[1],devid[0]);
	gtloginfo("devid=%02x%02x%02x%02x%02x%02x%02x%02x\n",devid[7],devid[6],devid[5],devid[4],devid[3],devid[2],devid[1],devid[0]);
	
	//todo 从json 方式读取后端口等需要改变
	init_para();					//将参数表设置为初始值



	//todo 从json 方式读取后端口等需要改变
	readmain_para_file(IPMAIN_PARA_FILE,get_mainpara());	//从配置文件读取参数表
	
    ipmain_para=get_mainpara();

    /*******************信令工作线程************************************/
	init_netcmd_var();						//初始化和网关通讯的状态
		
	printf("prepare to start threads...\n");			
	if(get_gtthread_attr(&thread_attr)==0)
		attr=&thread_attr;
	else
		attr=NULL;
		
	creat_connect_thread(attr);				//创建网关连接线程

	//todo from mips io drivers
	creat_watch_board_thread(attr);		//创建监视输入端子的线程

	
	//todo 此为接受gtc或者sdk 连接的信息,同样可以处理网关命令
	init_mainnetcmd_threads(attr,ipmain_para->devpara[0].cmd_port,0);

	if(attr!=NULL)
	{
		pthread_attr_destroy(attr);
	}

	sleep(1);
	show_time(NULL);							//显示系统当前时间 秒.毫秒
	//todo 处理媒体配置
	init_server_para();               ///<系统参数及状态初始化
	//todo 读取配置文件应由readmain_para_file实现
	read_server_para_file();
	
	//todo 音视频数据发送线程，以后实现起来应该打包和发送一起做
	create_av_server();
	//todo 音视频命令接收线程，以后应该在create_connnect_thread/init_mainnetcmd_thread里面的接收函数里面做
	create_rtnet_av_servers();
	//todo 音频下行命令接收线程和数据收取线程，以后可能没有下行语音了
	create_rtnet_aplay_servers();

	signal(SIGUSR1,exit_log);

	//todo 增加一个nvr 连接线程
	//create_nvr_manager_thread(attr);
	//todo 增加韦根读取线程
	create_weigen_server();
	
	//保留检测线程
	watch_process_thread();					//转化为监控线程
	closelog();
	exit(-1);
}

