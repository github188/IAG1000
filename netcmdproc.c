/* 网关命令处理函数
*/


#include "ipmain.h"
#include <commonlib.h>
#include "watch_process.h"
#include "netcmdproc.h"
#include "ipmain_para.h"
#include "alarm_process.h"
#include <sys/types.h>
#include "netinfo.h"
#include "gate_connect.h"
#include "watch_board.h"
#include "devstat.h"
//#include "infodump.h"
#include <sys/types.h>
#include <sys/stat.h>
#include "remote_file.h"
#include "xmlparser.h"
#include "alarm_process.h"
#include "gtvs_io_api.h"

static pthread_mutex_t update_mutex = PTHREAD_MUTEX_INITIALIZER;
static int update_waiting_no=0;	//记录发来过升级要求的网关数目
static gateinfo updatesw_gate_list[10];//存放发来过升级要求的网关信息

static int updating_flag=0;

static struct send_dev_trig_state_struct last_dev_trig={0,0,0,0,0,0,0,0};

#define UPDATE_UNRESPONED_INTERVAL 300

//static int usr_update_software_cmd(int fd,struct gt_usr_cmd_struct *netcmd,int env,int enc);
//static int usr_get_hq_pic_cmd(int fd,struct gt_usr_cmd_struct *netcmd);
//static int usr_query_index_cmd(int fd,struct gt_usr_cmd_struct *netcmd);

/**********************************************************************************************
 * 函数名	:init_netcmd_var()
 * 功能	:初始化网络命令处理工作相关变量
 * 输入	:无
 *返回值	:无
 **********************************************************************************************/
void init_netcmd_var(void)
{
	int i = 0;

    set_regist_flag(i,0);
    set_alarm_flag(i,1);
    set_reportstate_flag(i,0);
    set_trigin_flag(i,1);

	memset(updatesw_gate_list,0,sizeof(updatesw_gate_list));
}

int print_devid(BYTE *buf,int len)
{
        int i;
        for(i=0;i<(len-1);i++)
                printf("%02x-",buf[len-i-1]);
        printf("%02x ",buf[0]);
        return 0;
}

//远程系统设置设备时间
//重新启动相关线程
int rmt_set_time(struct tm *ntime)
{
	int ret=0;
	//int ret2;
	time_t curtime,newt;
	double diff;
	//struct tm tmt;
	if(ntime==NULL)
		return -1;
	newt = mktime(ntime);
	curtime=time(NULL);
	diff=difftime(curtime,newt);
	printf("time on dev is %d seconds later than server time\n",(int)diff);
	gtloginfo("设备时间比服务器时间快%d秒\n",(int)diff);

    //ipcall to do set time
    //
	ret = set_dev_time(ntime);

	if(ret==0)
	{
		printf("时间设置成功\n");
		gtloginfo("时间设置成功\n");
	}
	else
	{
		printf("时间设置失败 err=%d\n",ret);
		gtloginfo("时间设置失败 err=%d\n",ret);
	}
	return ret;
}



int get_dev_sitename(int dev_no,BYTE *devsite)
{
	int len;
	struct ipmain_para_struct *main_para;
	main_para=get_mainpara();
	memset(devsite,0,40);
	len=strlen(main_para->devpara[dev_no].inst_place);

	if(len>40)
		len=38; //可能是汉字
	main_para->devpara[dev_no].inst_place[len]='\0';
	memcpy(devsite,main_para->devpara[dev_no].inst_place,len+1);
	return 40;
}
//获得设备的网络ip地址(不是局域网地址)


//获得设备的网络ip地址(不是局域网地址)
static  int get_sys_ip(DWORD *ip)
{
	in_addr_t *addr;
	//int ad;
	struct ipmain_para_struct *main_para;
	main_para=get_mainpara();
	if(main_para->wan_addr!=0)
	{
		addr=&main_para->wan_addr;
	}
	else
		addr=&main_para->lan_addr;

	memcpy(ip,addr,4);
	
	return 16;
}	


#define BUZ_TEMP "/tmp/buzgrep"
int getgrepcmd(const char *cmd)
{
	int found;
	system(cmd);
	found = get_file_lines(BUZ_TEMP);
	return found;
}


void active_buz(void)
{
	printf("active buz!\n");
		char pbuf[200];
	    int  buz_playnum = 0;
	    sprintf(pbuf,"ps | grep alarmbuz > %s",BUZ_TEMP);
	    buz_playnum = getgrepcmd(pbuf);
	    printf("buz num now is %d\n",buz_playnum);
	    if(!buz_playnum)
	    {		
			gtloginfo("报警提示buz输出！\n");
			system("/conf/alarmbuz.sh & >/dev/null");	
	    }
}

void kill_buz(void)
{
	char  cmdline[100];
	char buff[50]; 
	FILE *fstream=NULL;
	int  buzpid=0;

	printf("kill buz\n");

	memset(cmdline,0,100);
	memset(buff,0,sizeof(buff));  
	sprintf(cmdline,"ps | grep alarmbuz | grep sh | awk '{print $1}' ");

	if(NULL==(fstream=popen(cmdline,"r")))       
    {      
        printf("execute command failed: %s",strerror(errno));       
        return -1;       
    } 

	if(NULL!=fgets(buff, sizeof(buff), fstream))      
    {      
        printf("alarmbuz's pid is %s!\n",buff);
    }      
    else     
    {     
        pclose(fstream);     
		return;
    } 
	
	memset(cmdline,0,100);
	sprintf(cmdline,"kill %s\n",buff);
	system(cmdline);
	system("/conf/buzzer.sh");
}


/**********************************************************************************************
 * 函数名	:system_regist()
 * 功能	:将系统信息注册到远程网关服务器系统注册
 * 输入	:fd:socket连接描述符 >0表示已经建立的连接 -1表示需要让网关连
 *				接线程建立一个连接
 *			  ack:是否需要接收确认 1表示需要  0表示不需要
 *			env:签名类型,fd为正时才有意义
 *			enc:加密算法
 * 返回值	:0表示成功，负值表示失败
 **********************************************************************************************/

int  system_regist(int fd,int ack, int env, int enc,int dev_no)
{//
	int rc;
	DWORD send_buf[150];
//	char rtversion[30];
//	char hdversion[30];

//	char tbuf[256];

	struct tm *lvtime;
	//time_t ctime;
	struct mod_com_type *modcom;
	struct gt_pkt_struct *send;
	struct gt_usr_cmd_struct *cmd;
	struct ip1004_info_struct *info;
	struct ipmain_para_struct * para;
	int i;
	int *ppb;
	struct sockaddr_in peeraddr;
	int addrlen=sizeof(struct sockaddr);	
	i=getpeername(fd,(struct sockaddr *)&peeraddr,&addrlen);

	if(fd>0)//向已有连接注册
	{
		send=(struct gt_pkt_struct *)send_buf;
		cmd=(struct gt_usr_cmd_struct *)send->msg;

	}
	else
	{
		modcom=(struct mod_com_type *)send_buf;
		modcom->env = env;
		modcom->enc = enc;
		cmd=(struct gt_usr_cmd_struct *)modcom->para;
	}
	
	cmd->cmd=DEV_REGISTER;
	cmd->en_ack=ack;
	cmd->reserve0=0;
	cmd->reserve1=0;
	para=get_mainpara();
	rc=virdev_get_devid(dev_no,cmd->para);
	cmd->len=rc+SIZEOF_GT_USR_CMD_STRUCT-sizeof(cmd->para)-2+SIZEOF_IP1004_INFO_STRUCT;
	info=(struct ip1004_info_struct *)(&cmd->para[rc]);
	info-> vendor	=DEVICE_VENDOR;			//设备制造商标识(4) +设备型号标识(4)
	info->device_type=get_devtype();			//设备型号
	info->protocal_version=INTERNAL_PROTOCAL;	// 网关和前端设备通讯的协议内部版本号(无符号整数)
	rc=get_dev_sitename(dev_no,info->site_name);	//安装地点名称
	rc=get_sys_ip(&info->dev_ip);				//设备的动态ip地址
	//	BYTE reserve[12];		//为ipv6地址扩展保留
	info->dev_ip=ntohl(info->dev_ip);
 	info->boot_type= 0 ; //没有51后，无法get_regist_reason();		//设备的注册原因,16位，按位表示意义


	//以下数据应当调用相应的获取函数

	info->video_num=virdev_get_video_num(dev_no);		//设备视频输入通道数。
	printf("info->video_num is %d\n",info->video_num);
	info->com_num=virdev_get_total_com(dev_no);			// 串口数
	info->storage_type=get_hd_type()+1;// HDTYPE;		//设备的存储介质类型 0：没有 1：cf卡 2：硬盘

#ifndef FOR_PC_MUTI_TEST
	info->storage_room=get_hd_capacity();
#else
	info->storage_room=1024*1024*80;
#endif
	info->compress_num=virdev_get_videoenc_num(dev_no);	//压缩通道数	目前为1，2或5
	info->compress_type=1;	//压缩数据类型，(压缩芯片，压缩格式)
	info->audio_compress=1;	//声音输入压缩类型
	info->audio_in=virdev_get_audio_num(dev_no);			//声音输入通道数，目前为1
    info->audio_in_act=1;		//声音输入通道有效位，从右向左，1-8，0-通道无效，1-有效
	info->switch_in_num=virdev_get_trigin_num(dev_no);	//开关量输入通道数
	if(info->switch_in_num != 0)
		info->switch_in_act=para->tin_mask;//开关量输入通道有效位，从右向左，1-8，0-通道无效，1-有效
	info->switch_out_num=virdev_get_alarmout_num(dev_no);	//开关量输出通道数
	if(info->switch_out_num != 0)
		info->switch_out=para->alarm_mask;		//开关量输出通道有效位，从右向左，1-8，0-通道无效，1-有效
	info->max_pre_rec=1800;					//设备最大预录时间，以秒为单位
	info->max_dly_rec=1800;					//设备最大延时录像时间

	//设备出厂时间
	//ctime=time(NULL);
	//ptime=localtime(&ctime);
	lvtime=get_lvfac_time();
	info->year=lvtime->tm_year+1900;	//年
	info->month=lvtime->tm_mon+1;	//月
	info->day=lvtime->tm_mday; 		//日
	info->hour=lvtime->tm_hour;		//时
	info->minute=lvtime->tm_min;		//分
	info->second=lvtime->tm_sec;		//秒
	info->reserve1=0; 
	
	info->reserve2=0;

	//服务端口
	info->cmd_port=para->devpara[dev_no].cmd_port;
	//printf("send regist %d %d %d\n",info->cmd_port,info->image_port,info->audio_port);
	info->image_port=para->image_port;
	info->audio_port=para->audio_port;
	//ppb=(int*)info->ex_info;
	//*ppb = para->pb_port;
	memset(info->ex_info,0,sizeof(info->ex_info));
	
	memset(info->firmware,0,sizeof(info->firmware));
	memset(info->dev_info,0,sizeof(info->dev_info));
	if(fd>0)
	{
		rc = gt_cmd_pkt_send(fd,send,cmd->len+2,NULL,0,env,enc);
		if(rc>0)
			rc = 0;
	}
	else	
		rc=send_gate_pkt(fd,modcom,cmd->len+2,dev_no);
	//gtloginfo("向远程服务器注册\n");
	if(rc==0)
	{
#ifdef SHOW_WORK_INFO
	printf("ipmain %s send a regist info\n",devlog(dev_no));
#endif
		return 0;
	}	
#ifdef SHOW_WORK_INFO
	printf("ipmain %s use send_gate_pkt()=%d send a regist info failed!\n",devlog(dev_no),rc);
#endif
	
	return -1;

}


static DWORD dev_state_old[MAX_DEV_NO]={0};
static DWORD per_state_old[MAX_DEV_NO]={0};
static DWORD alarmin_state_old[MAX_DEV_NO] = {0};

/**********************************************************************************************
 * 函数名	:send_dev_state()
 * 功能	:发送设备状态到远程服务器
 * 输入	:fd:socket连接描述符 >0表示已经建立的连接 -1表示需要让网关连
 *				接线程建立一个连接
 *			  ack:是否需要接收确认 1表示需要  0表示不需要
 *			  required:是否强制发送 1表示强制发送 0表示只有状态变了才发送 
 *			env:签名类型,fd为正时才有意义
 *			enc:加密算法
 *返回值	:0表示成功，负值表示失败
 **********************************************************************************************/

int send_dev_state(int fd,int ack,int required, int env, int enc,int dev_no)

{
	DWORD send_buf[25];//响应命令包不会超过100字节
	struct gt_usr_cmd_struct *cmd;
	struct mod_com_type *modcom;
	struct ip1004_state_struct * gtstate;
	int rc;
	int i;
	DWORD *per_state;
	DWORD *dev_state;
	DWORD *alarmin_state;
	struct sockaddr_in peeraddr;
	int link_err;	
	int addrlen=sizeof(struct sockaddr);	
	
	//如果没有注册成功,且fd<0则不发送状态
	if((!get_regist_flag(dev_no))&&(fd<0))
	{	
		return 0;
	}	
	i=getpeername(fd,(struct sockaddr *)&peeraddr,&addrlen);
	modcom=(struct mod_com_type *)send_buf;
	modcom->env = env;
	modcom->enc = enc;
	cmd=(struct gt_usr_cmd_struct *)modcom->para;
	cmd->cmd=DEV_STATE_RETURN;
	cmd->en_ack=ack;
	cmd->reserve0=0;
	cmd->reserve1=0;
	rc=virdev_get_devid(dev_no,cmd->para);

	cmd->len=rc+SIZEOF_GT_USR_CMD_STRUCT-sizeof(cmd->para) -2+4+4+4;
	gtstate=get_ip1004_state(dev_no);
	
	pthread_mutex_lock(&gtstate->mutex);
	link_err=gtstate->reg_dev_state.link_err;
	gtstate->reg_dev_state.link_err=0;
//	gtstate->reg_dev_state.cf_err=0;//test!!!
	memcpy(&cmd->para[rc+4],(char *)&gtstate->reg_dev_state,4);
	gtstate->reg_dev_state.link_err=link_err;//连接故障不应发送给网关
	memcpy(&cmd->para[rc+8],(char *)&gtstate->alarmin_state,4);
	pthread_mutex_unlock(&gtstate->mutex);
	per_state=(DWORD*)&cmd->para[rc];
	dev_state=(DWORD*)&cmd->para[rc+4];
	alarmin_state = (DWORD*)&cmd->para[rc+8];

	//gtlogdebug("%d,%d,%08x,%08x,%08x,%08x,%d,%d\n",ack,required,dev_state_old,*dev_state,per_state_old,*per_state,(dev_state_old!=*dev_state),(per_state_old!=*per_state));

	
	if((required==1)||(dev_state_old[dev_no]!=*dev_state)||(per_state_old[dev_no]!=*per_state)||(alarmin_state_old[dev_no]!= *alarmin_state))
	{
		if(ack==1)//若需要ACK,则清标志位
			set_reportstate_flag(dev_no,0);

#ifdef SHOW_WORK_INFO
		printf("%s向远端服务器发送状态%08x,%08x,%08x\n",devlog(dev_no),(int)*per_state,(int)*dev_state,(int)*alarmin_state);
#endif
		gtloginfo("%s向远端服务器发送状态%08x,%08x,%08x\n",devlog(dev_no),(int)*per_state,(int)*dev_state,(int)*alarmin_state);
		rc=send_gate_pkt(fd,modcom,cmd->len+2,dev_no);

	}
	else
		rc=0;
		 
	dev_state_old[dev_no]=*dev_state;
	per_state_old[dev_no]=*per_state;
	alarmin_state_old[dev_no] = *alarmin_state;
	return rc;
}


/**********************************************************************************************
 * 函数名	:send_dev_req_sync_time()
 * 功能	:发送请求对时命令到网关
 * 输入	:fd:已经建立的目标socket
 *		env:签名
 *		enc:加密
 *返回值	:0表示成功负值表示失败
 **********************************************************************************************/

int send_dev_req_sync_time(int fd,int env,int enc,int dev_no)
{	
	DWORD send_buf[25];//响应命令包不会超过100字节
	struct gt_usr_cmd_struct *cmd;
	struct mod_com_type *modcom;
	int rc;
	int i;
	struct sockaddr_in peeraddr;
	int addrlen=sizeof(struct sockaddr);	
	i=getpeername(fd,(struct sockaddr *)&peeraddr,&addrlen);
	modcom=(struct mod_com_type *)send_buf;
	modcom->env = env;
	modcom->enc = enc;
	cmd=(struct gt_usr_cmd_struct *)modcom->para;
	cmd->cmd=DEV_REQ_SYNC_TIME;
	cmd->en_ack=1;
	cmd->reserve0=0;
	cmd->reserve1=0;

	rc=virdev_get_devid(dev_no,cmd->para);
	cmd->len=rc+SIZEOF_GT_USR_CMD_STRUCT-sizeof(cmd->para)-2;//+sizeof(struct send_dev_trig_state_struct);
	gtloginfo("%s发送时间同步命令!\n",devlog(dev_no));
	return send_gate_pkt(fd,modcom,cmd->len+2,dev_no);			
}

/**********************************************************************************************
 * 函数名	:send_dev_trig_state()
 * 功能	:发送设备报警信息到远程服务器
 * 输入	:fd:socket连接描述符 >0表示已经建立的连接 -1表示需要让网关连
 *				接线程建立一个连接,-2表示如果连接不上所有网关则连接
 *				紧急报警服务器
 *			  dev_rtig:报警信息结构
 *			  ack:是否需要确认 1表示需要 0表示不需要
 *				env:签名
 *				enc:加密，这两项在fd>0时有意义
 *返回值	:0表示成功，负值表示失败
 **********************************************************************************************/
int send_dev_trig_state(int fd,struct send_dev_trig_state_struct *dev_trig,int ack, int env, int enc,int dev_no)
{	
	DWORD send_buf[25];//响应命令包不会超过100字节
	struct gt_usr_cmd_struct *cmd;
	struct mod_com_type *modcom;
	int rc;
	int i;
	struct sockaddr_in peeraddr;
	int addrlen=sizeof(struct sockaddr);	
	i=getpeername(fd,(struct sockaddr *)&peeraddr,&addrlen);
	modcom=(struct mod_com_type *)send_buf;
	modcom->env = env;
	modcom->enc = enc;
	cmd=(struct gt_usr_cmd_struct *)modcom->para;
	cmd->cmd=DEV_ALARM_RETURN;
	cmd->en_ack=ack;
	cmd->reserve0=0;
	cmd->reserve1=0;
	rc=virdev_get_devid(dev_no,cmd->para);

	cmd->len=rc+SIZEOF_GT_USR_CMD_STRUCT-sizeof(cmd->para)-2+sizeof(struct send_dev_trig_state_struct);
	memcpy(&cmd->para[rc],dev_trig,sizeof(struct send_dev_trig_state_struct));
	
	gtloginfo("%s发送报警状态0x%04x,时间%04d-%02d-%02d-%02d-%02d-%02d给网关\n",devlog(dev_no),(int)dev_trig->alarmstate,dev_trig->year,dev_trig->month,dev_trig->day,dev_trig->hour,dev_trig->minute,dev_trig->second);
	memcpy(&last_dev_trig,dev_trig,sizeof(struct send_dev_trig_state_struct));
	//if(dev_no == 0)
	//	set_state_led_state(2);
	if(ack==1)//若需要ACK,则清标志位
		set_alarm_flag(dev_no,0);
	return send_gate_pkt(fd,modcom,cmd->len+2,dev_no);	

}

/**********************************************************************************************
 * 函数名	:send_gate_ack()
 * 功能	:向网关发送一条命令的响应
 * 输入	:fd:已经建立的目标socket
 *			 rec_cmd:要响应的命令
 *			 result:执行命令的结果码
 *				env:签名
 *				enc:加密，这两项在fd>0时有意义
 *返回值	:0表示成功负值表示失败
 **********************************************************************************************/
int send_gate_ack(int fd,WORD rec_cmd,WORD result,int env, int enc,int dev_no)
{
	DWORD send_buf[25];//响应命令包不会超过100字节
	struct gt_usr_cmd_struct *cmd;
	struct mod_com_type *modcom;
	int i;
	struct sockaddr_in peeraddr;
	int addrlen=sizeof(struct sockaddr);
	int rc;
	i=getpeername(fd,(struct sockaddr *)&peeraddr,&addrlen);
	modcom=(struct mod_com_type *)send_buf;
	modcom->env = env;
	modcom->enc = enc;
	cmd=(struct gt_usr_cmd_struct *)modcom->para;
	cmd->cmd=DEV_CMD_ACK;
	cmd->en_ack=0;
	cmd->reserve0=0;
	cmd->reserve1=0;
	rc= virdev_get_devid(dev_no, cmd->para);
	memcpy(&cmd->para[rc],(char *)&result,2);
	memcpy(&cmd->para[rc+2],(char *)&rec_cmd,2);
	cmd->len=rc+SIZEOF_GT_USR_CMD_STRUCT-sizeof(cmd->para) -2+4;
	send_gate_pkt(fd,modcom,cmd->len+2,dev_no);

	gtloginfo("%s向%s发送命令%04x的结果%04x",devlog(dev_no),inet_ntoa(peeraddr.sin_addr),rec_cmd,result);


	return 0;
}


//透明串口控制的命令返回
static int send_gate_serial_return(int fd,WORD ch,WORD result,WORD port, int env,int enc, int dev_no)
{
	DWORD send_buf[25];//响应命令包不会超过100字节
	struct gt_usr_cmd_struct *cmd;
	struct mod_com_type *modcom;
	struct dev_com_ret_struct *serialrt;
	int rc;
	
	modcom=(struct mod_com_type *)send_buf;
	modcom->env = env;
	modcom->enc = enc;
	cmd=(struct gt_usr_cmd_struct *)modcom->para;
	cmd->cmd=DEV_COM_PORT_RET;
	cmd->en_ack=0;
	cmd->reserve0=0;
	cmd->reserve1=0;
	rc=virdev_get_devid(dev_no,cmd->para);
	serialrt=(struct dev_com_ret_struct *)((char *)cmd->para+rc);
	serialrt->ch=ch;
	serialrt->result=result;
	serialrt->port=port;
	cmd->len=rc+SIZEOF_GT_USR_CMD_STRUCT-sizeof(cmd->para)-2+SIZEOF_DEV_COM_RET_STRUCT;
	send_gate_pkt(fd,modcom,cmd->len+2,dev_no);

#ifdef SHOW_WORK_INFO
	printf("%s向远端服务器发送透明串口控制命令返回%d\n",devlog(dev_no),result);
#endif
	gtloginfo("%s向远端服务器发送透明串口控制命令返回%d\n",devlog(dev_no),result);
	return 0;
}


static int send_gate_query_rt_return(int fd,WORD ch,WORD result, int env,int enc, int dev_no)
{
	DWORD send_buf[25];//响应命令包不会超过100字节
	struct gt_usr_cmd_struct *cmd;
	struct mod_com_type *modcom;
	struct usr_req_rt_img_answer_struct * answer;
	int rc;
	
	modcom=(struct mod_com_type *)send_buf;
	modcom->env = env;
	modcom->enc = enc;
	cmd=(struct gt_usr_cmd_struct *)modcom->para;
	cmd->cmd=USR_REQUIRE_RT_IMAGE_ANSWER;
	cmd->en_ack=0;
	cmd->reserve0=0;
	cmd->reserve1=0;
	rc=virdev_get_devid(dev_no,cmd->para);
	answer=(struct usr_req_rt_img_answer_struct *)((char *)cmd->para+rc);
	answer->query_usr_id = ch;
	cmd->len=rc+SIZEOF_GT_USR_CMD_STRUCT-sizeof(cmd->para)-2+SIZEOF_DEV_COM_RET_STRUCT;
	send_gate_pkt(fd,modcom,cmd->len+2,dev_no);

#ifdef SHOW_WORK_INFO
	printf("%s向远端服务器发送订阅视频应答命令%d\n",devlog(dev_no),result);
#endif
	gtloginfo("%s向远端服务器发送订阅视频应答命令%d\n",devlog(dev_no),result);
	return 0;
}
static int send_gate_query_pb_return(int fd,WORD ch,WORD result, int env,int enc, int dev_no)
{
	DWORD send_buf[25];//响应命令包不会超过100字节
	struct gt_usr_cmd_struct *cmd;
	struct mod_com_type *modcom;
	viewer_subsrcibe_answer_record_struct * answer;
	int rc;
	
	modcom=(struct mod_com_type *)send_buf;
	modcom->env = env;
	modcom->enc = enc;
	cmd=(struct gt_usr_cmd_struct *)modcom->para;
	cmd->cmd=USR_REQUIRE_RECORD_PLAYBACK_ANSWER;
	cmd->en_ack=0;
	cmd->reserve0=0;
	cmd->reserve1=0;
	rc=virdev_get_devid(dev_no,cmd->para);
	answer=( viewer_subsrcibe_answer_record_struct *)((char *)cmd->para+rc);
	answer->query_usr_id = ch;
	cmd->len=rc+SIZEOF_GT_USR_CMD_STRUCT-sizeof(cmd->para)-2+SIZEOF_DEV_COM_RET_STRUCT;
	send_gate_pkt(fd,modcom,cmd->len+2,dev_no);

#ifdef SHOW_WORK_INFO
	printf("%s向远端服务器发送订阅视频应答命令%d\n",devlog(dev_no),result);
#endif
	gtloginfo("%s向远端服务器发送订阅视频应答命令%d\n",devlog(dev_no),result);
	return 0;
}


//设置网关ip地址
static int usr_set_gate_ip(int fd,struct gt_usr_cmd_struct *cmd,int env,int enc,int dev_no)
{
	struct usr_set_auth_ip_struct *gatecmd;
	struct in_addr addr;
	DWORD rmt;
	WORD	result;
	int i;
	struct sockaddr_in peeraddr;
	int addrlen=sizeof(struct sockaddr);
	
	if((fd<0)||(cmd->cmd!=USR_SET_AUTH_IP))
		return -1;
	gatecmd=(struct usr_set_auth_ip_struct*)cmd->para;
	i=getpeername(fd,(struct sockaddr *)&peeraddr,&addrlen);
//	
	printf("%s(fd=%d)发来设置%s网关ip地址命令0x0101\n",inet_ntoa(peeraddr.sin_addr),fd,devlog(dev_no));
	gtloginfo("%s(fd=%d)发来设置%s网关ip地址命令0x0101\n",inet_ntoa(peeraddr.sin_addr),fd,devlog(dev_no));
	rmt=htonl(gatecmd->ip);
	memcpy(&addr,&rmt,4);
	printf("receive set ip cmd ip:%s\n",inet_ntoa(addr));

	if(set_gate_ip(dev_no,gatecmd->server_sn,&gatecmd->ip,REMOTE_GATE_CMD_PORT)<0)
	{
		result=ERR_ENC_NOT_ALLOW;
	}
	else
	{
		result=RESULT_SUCCESS;
		if(gatecmd->save_flag)
		{
			//存入flash
		switch(gatecmd->server_sn)
			{
				case 1:
				{
					if(dev_no == 0)
						save2para_file(IPMAIN_PARA_FILE,"remote_gate:rmt_gate1",inet_ntoa(addr));	
					if(dev_no == 1)
						save2para_file(IPMAIN_PARA_FILE,"remote_gate_dev2:rmt_gate1",inet_ntoa(addr));	
				}
				break;
				case 2:
				{
					if(dev_no == 0)
						save2para_file(IPMAIN_PARA_FILE,"remote_gate:rmt_gate2",inet_ntoa(addr));	
					if(dev_no == 1)
						save2para_file(IPMAIN_PARA_FILE,"remote_gate_dev2:rmt_gate2",inet_ntoa(addr));	
				}
				break;
				case 3:
				{
					if(dev_no == 0)
						save2para_file(IPMAIN_PARA_FILE,"remote_gate:rmt_gate3",inet_ntoa(addr));	
					if(dev_no == 1)
						save2para_file(IPMAIN_PARA_FILE,"remote_gate_dev2:rmt_gate3",inet_ntoa(addr));	
				}
				break;
				case 4:
				{
					if(dev_no == 0)
						save2para_file(IPMAIN_PARA_FILE,"remote_gate:rmt_gate4",inet_ntoa(addr));	
					if(dev_no == 1)
						save2para_file(IPMAIN_PARA_FILE,"remote_gate_dev2:rmt_gate4",inet_ntoa(addr));	
				}
				break;
				case 5:
				{
					if(dev_no == 0)
						save2para_file(IPMAIN_PARA_FILE,"remote_gate:alarm_gate",inet_ntoa(addr));	
					if(dev_no == 1)
						save2para_file(IPMAIN_PARA_FILE,"remote_gate_dev2:alarm_gate",inet_ntoa(addr));	
				}
				break;
				
			}
		}
	}
	//if(virdev_get_virdev_number()==2)//虚拟设备
	//	system("/ip1004/ini_conv -s");
	gtloginfo("%s网关ip地址设置完毕，结果为%d\n",devlog(dev_no),result);
	if(cmd->en_ack!=0)
		return send_gate_ack(fd,USR_SET_AUTH_IP, result,env,enc,dev_no);
	return 0;

}
static int count=60;
static int usr_set_clock(int fd,struct gt_usr_cmd_struct *cmd,int env,int enc,int dev_no)
{	//
	WORD	result;
	struct usr_set_time_struct *timeval;
	//time_t systime;
	struct tm tmt;
	//int inttime;
	int i;
	struct sockaddr_in peeraddr;
	int addrlen=sizeof(struct sockaddr);
	
	if((fd<0)||(cmd->cmd!=USR_CLOCK_SETING))
		return -1;
	//date MMDDHHMMYYYY.SS	//设置时间
	//clock -w将系统时间写入 CMOS 中
	timeval=(struct usr_set_time_struct*)cmd->para;
#ifdef SHOW_WORK_INFO
	printf("vsmain recv a set time cmd new time:%d-%02d-%02d %02d:%02d:%02d\n",
		timeval->year,timeval->month,timeval->day,timeval->hour,timeval->minute,timeval->second);
#endif	

	tmt.tm_year=timeval->year-1900;
	tmt.tm_mon=timeval->month-1;
	tmt.tm_mday=timeval->day;
	tmt.tm_hour=timeval->hour;
	tmt.tm_min=timeval->minute;
	tmt.tm_sec=timeval->second;
	i=getpeername(fd,(struct sockaddr *)&peeraddr,&addrlen);
	if(count++>=60)
	{
		gtloginfo("%s(fd=%d)设置时间0x0200，时间%d-%d-%d-%d-%d-%d\n",inet_ntoa(peeraddr.sin_addr),fd,timeval->year,timeval->month,timeval->day,timeval->hour,timeval->minute,timeval->second);

		//设置rtc时间
		if(rmt_set_time(&tmt)<0)
		{
#ifdef SHOW_WORK_INFO	
			printf("ipmain recv a bad time\n");
#endif
			result=ERR_DVC_INVALID_REQ;
			cmd->en_ack=1;
		}
		else
		{
			result=RESULT_SUCCESS;
		}
		
#ifdef SHOW_WORK_INFO
		printf("usr_set_clock result is %d\n",result);
#endif
		gtloginfo("设置时间完毕，结果为%d\n",result);
		count=0;
	}
	else
	{
			cmd->en_ack=1;
			result=RESULT_SUCCESS;
	}


	if(cmd->en_ack!=0)
		return send_gate_ack(fd,USR_CLOCK_SETING, result,env,enc,dev_no);	
	return 0;
}


//设置触发输入口属性
static int usr_set_trigger_in(int fd,struct gt_usr_cmd_struct *cmd,int env,int enc,int dev_no)
{//
	WORD	result;
	//struct ipmain_para_struct * para;
	BYTE en_ack;
	struct trig_in_attrib_struct *attrib;
	int i,ch;
	//char entry[30],vstr[30];
	//DWORD *act;
	struct sockaddr_in peeraddr;
	int addrlen=sizeof(struct sockaddr);
	//dictionary *ini;	
	//FILE *fp;
	if((fd<0)||(cmd->cmd!=USR_SET_SWITCH_IN))
		return -1;

	attrib=(struct trig_in_attrib_struct*)cmd->para;
	i=getpeername(fd,(struct sockaddr *)&peeraddr,&addrlen);
#ifdef SHOW_WORK_INFO	
	printf("%s(fd=%d)发来%s开关量输入属性设置0x0213,通道模式%x,动作0%x,1%x,2%x,3%x\n",inet_ntoa(peeraddr.sin_addr),(int)fd,devlog(dev_no),(int)attrib->ch_mode,(int)attrib->action0,(int)attrib->action1,(int)attrib->action2,(int)attrib->action3);
#endif
	gtloginfo("%s(fd=%d)发来%s开关量输入属性设置0x0213,通道模式%x,动作0%x,1%x,2%x,3%x\n",inet_ntoa(peeraddr.sin_addr),(int)fd,devlog(dev_no),(int)attrib->ch_mode,(int)attrib->action0,(int)attrib->action1,(int)attrib->action2,(int)attrib->action3);
	
	if(virdev_get_trigin_num(dev_no)==0)
	{
		result=ERR_ENC_NOT_ALLOW;
	}
	//lc do
#ifdef USE_IO	
	if(set_trigin_attrib_perbit((DWORD)(attrib->ch_mode))<0)
		result=ERR_ENC_NOT_ALLOW;
	else
	{
		en_ack=cmd->en_ack;
		
//#ifdef HQMODULE_USE
		//cmd->en_ack=0;
		//bypass_hqsave_cmd(fd,cmd);	//转发给高清晰录像模块
//#endif
		cmd->en_ack=en_ack;
		ch=attrib->ch_mode&0xff;

	if((ch)<get_trigin_num())
		{
			result=RESULT_SUCCESS;
			if(attrib->save_flag!=0)
			{//dont save for now,lc
			/*
				ini=iniparser_load(IPMAIN_PARA_FILE);
				if(ini==NULL)
				{
					//retflag=-3;
				}
				else
				{				
					para=get_mainpara();
					act=&attrib->action0;
					for(i=0;i<MAX_TRIG_EVENT;i++)
					{					
						sprintf(entry,"alarm:tin%dact%d",(attrib->ch_mode&0xff),i);
						para->tinact[ch][i]=*act;
						sprintf(vstr,"%08x",htonl(*act));
						save2para(ini,entry,vstr);
					}	
					fp=fopen(IPMAIN_PARA_FILE,"w");
					if(fp!=NULL)
					{
						iniparser_dump_ini(ini,fp);
						fclose(fp);
					}
					iniparser_freedict(ini);
					
				}	*/
			}


		}
		else
			result=ERR_ENC_NOT_ALLOW;
	}
#endif	
	gtloginfo("%s设置触发输入口属性结果为%d\n",devlog(dev_no),result);
	if(cmd->en_ack!=0)
		return send_gate_ack(fd,USR_SET_SWITCH_IN, result,env,enc,dev_no);	
	return 0;
}
//设置报警输出口属性
static int usr_set_alarm_out(int fd,struct gt_usr_cmd_struct *cmd,int env,int enc,int dev_no)
{//
	WORD	result=0;
	WORD	*attrib;
	WORD	*saveflag;
	int i;
	struct sockaddr_in peeraddr;
	int addrlen=sizeof(struct sockaddr);
	if((fd<0)||(cmd->cmd!=USR_SET_SWITCH_OUT))
		return -1;
	saveflag=(WORD*)cmd->para;
	attrib=(WORD*)&cmd->para[2];
	i=getpeername(fd,(struct sockaddr *)&peeraddr,&addrlen);
#ifdef SHOW_WORK_INFO
	printf("ipmain recv a set_alarm_attrib cmd new attrib is:%04x\n",*attrib);
#endif
	gtloginfo("%s(fd=%d)发来%s设置报警输出口属性0x0214,设置%04x\n",inet_ntoa(peeraddr.sin_addr),fd,devlog(dev_no),*attrib);
	//if(set_alarm_out_attrib(*attrib,*saveflag)<0)
	if(virdev_get_alarmout_num(dev_no)==0)
	{
		result=ERR_ENC_NOT_ALLOW;
	}
	gtloginfo("完成设置%s报警输出口属性结果为%d\n",devlog(dev_no),result);
	if(cmd->en_ack!=0)
		return send_gate_ack(fd,USR_SET_SWITCH_OUT, result,env,enc,dev_no);	
	return 0;
}
//控制报警输出口
static int usr_alarm_ctl(int fd,struct gt_usr_cmd_struct *cmd,int env,int enc,int dev_no)
{
//
	WORD	result;
	WORD	*output;
	WORD	*saveflag;
	int i;
	struct sockaddr_in peeraddr;
	int addrlen=sizeof(struct sockaddr);
	if((fd<0)||(cmd->cmd!=USR_SWITCH_OUT))
		return -1;
	saveflag=(WORD*)cmd->para;
	output=(WORD*)&cmd->para[2];
	i=getpeername(fd,(struct sockaddr *)&peeraddr,&addrlen);
#ifdef SHOW_WORK_INFO
	printf("vsmain recv a usr_alarm_ctl cmd to set output%d to %d\n",(*output&0xff),(*output>>8&0xff));
#endif
	gtloginfo("%s(fd=%d)发来设置%s控制报警输出口0x0215,%d 到 %d\n",inet_ntoa(peeraddr.sin_addr),fd,devlog(dev_no),(*output&0xff),(*output>>8&0xff));
	//if(set_alarm_state_bit((*output)>>8,*output&0xff)<0)
	if(virdev_get_alarmout_num(dev_no)==0)
	{
		result=ERR_ENC_NOT_ALLOW;
	}
	//lc do 待io封装库完成后实现
	if(set_relay_output(*output&0xff,(*output)>>8)<0)
		result=ERR_ENC_NOT_ALLOW;
	else
	{
		result=RESULT_SUCCESS;	//不能存储		
	}
	
	result=RESULT_SUCCESS;
#ifdef SHOW_WORK_INFO
	printf("%s设置控制报警输出口结果%d\n",devlog(dev_no),result);
#endif
	gtloginfo("%s设置控制报警输出口结果%d\n",devlog(dev_no),result);
	if(cmd->en_ack!=0)
		return send_gate_ack(fd,USR_SWITCH_OUT, result,env,enc,dev_no);
	return 0;
}




static int usr_query_state_cmd(int fd,struct gt_usr_cmd_struct *cmd, int env, int enc,int dev_no)
{//
	time_t rtime,curtime;
	struct tm tm,*pctime=NULL;
	int inttime;
	struct gt_time_struct *rmttime;
	int i;
	struct sockaddr_in peeraddr;
	int addrlen=sizeof(struct sockaddr);
	i=getpeername(fd,(struct sockaddr *)&peeraddr,&addrlen);
	if((fd<0)||(cmd->cmd!=USR_QUERY_STATE))
		return -1;	
	rmttime=(struct gt_time_struct *)cmd->para;
#ifdef SHOW_WORK_INFO
	printf("%s(fd=%d)发来查询%s设备状态命令0x0103\n",inet_ntoa(peeraddr.sin_addr),fd,devlog(dev_no));
#endif
	gtloginfo("%s(fd=%d)发来查询%s设备状态命令0x0103\n",inet_ntoa(peeraddr.sin_addr),fd,devlog(dev_no));
	gttime2tm(rmttime,&tm);

	rtime = mktime(&tm);
	inttime=rtime;
	if(inttime>0)
	{
		curtime=time(NULL);
		pctime=localtime(&curtime);	
		if(pctime!=NULL)
		{
			if((pctime->tm_year+1900)<2005)
			{//如果设备当前时间小于2005表示可能出错，需要对时
				gtloginfo("使用远程对时 :\n");
				gtloginfo("oldtime:%d%d%d-%d:%d:%d\n",(pctime->tm_year+1900),
													    (pctime->tm_mon+1),
													    (pctime->tm_mday),
													    (pctime->tm_hour),
													    pctime->tm_min,
													    pctime->tm_sec);
				
				gtloginfo("newtime:%d%d%d-%d:%d:%d\n",(pctime->tm_year+1900),
													    (pctime->tm_mon+1),
													    (pctime->tm_mday),
													    (pctime->tm_hour),
													    pctime->tm_min,
													    pctime->tm_sec);
													    
				//lc do 对端要求对时
				rmt_set_time(&tm);
			}
		}

	}
	//gtloginfo("被查询状态新加\n");
	send_dev_state(fd,0,1,env,enc,dev_no);	
	return 0;
}


/**********************************************************************************************
 * 函数名	:remote_cancel_alarm()
 * 功能	:处理远程发来的解除警报指令
 * 输入	:trig:按位表示的触发状态，做复位报警联动用
 * 返回值	:0表示成功，负值表示失败
 **********************************************************************************************/
int remote_cancel_alarm(DWORD trig)
{
	struct ipmain_para_struct *para;
	struct alarm_motion_struct *alarm_motion;
	int i,j,k,video_ch;
	struct ip1004_state_struct *ip1004state;
	DWORD temptrig=0;
	DWORD tempatrig=0;
	int event=0;
	int temptag=0;
	int count=0;
	unsigned int video_trig = 0;
	int cancel_ch[MAX_VIDEO_IN]={0};
	DWORD actual_trig=0;
	para=get_mainpara();
	
	//服务器会单独发来解锁命令
	//执行那些reset执行的报警联动
	for(i=0;i<(get_trigin_num());i++)
	{
		k=(i<10)?i:i+16;
		if((trig&(1<<k))==0)
			continue;
		for(j=0;j<MAX_TRIG_EVENT;j++)
		{
			event=get_alarm_event(i,2,j); 
#ifdef SHOW_WORK_INFO
			printf("复位时event为%d\n",event);
#endif
			take_alarm_action(i,event);
		}
	}

	return 0;
}

//用户取消报警
static int usr_cancel_alarm_cmd(int fd,struct gt_usr_cmd_struct *cmd,int env,int enc,int dev_no)
{
	int rc;
	WORD	result;
	int i;
	struct sockaddr_in peeraddr;
	struct usr_start_alarm_actions_yes_struct *action;
	struct ipmain_para_struct *para;
	
	int addrlen=sizeof(struct sockaddr);
	i=getpeername(fd,(struct sockaddr *)&peeraddr,&addrlen);
	
	if((fd<0)||(cmd->cmd!=USR_CANCEL_ALARM))
		return -1;	
	action=(struct usr_start_alarm_actions_yes_struct *)cmd->para;
#ifdef SHOW_WORK_INFO
	printf("%s(fd=%d)发来%s报警复位命令0x0219,报警状态0x%04x\n",inet_ntoa(peeraddr.sin_addr),fd,devlog(dev_no),(int)action->trig);
#endif
	gtloginfo("%s(fd=%d)发来%s报警复位命令0x0219,报警状态0x%04x\n",inet_ntoa(peeraddr.sin_addr),fd,devlog(dev_no),(int)action->trig);

	rc=remote_cancel_alarm(action->trig);

	para = get_mainpara();
	
	if(rc>=0)
		result=RESULT_SUCCESS;
	else
		result=ERR_DVC_INTERNAL;
	
#ifdef SHOW_WORK_INFO
	printf("报警复位结果%d\n",result);
#endif
	gtloginfo("报警复位结果%d\n",result);

	if(cmd->en_ack!=0)
		return send_gate_ack(fd,USR_CANCEL_ALARM, result,env,enc,dev_no);		

	
	return 0;
}

//控制执行报警联动命令
static int usr_start_alarm_actions_cmd(int fd,struct gt_usr_cmd_struct *cmd,int env, int enc,int dev_no)
{
	int i,event;
	struct sockaddr_in peeraddr;
	struct usr_start_alarm_actions_struct *action;
	int addrlen=sizeof(struct sockaddr);
	i=getpeername(fd,(struct sockaddr *)&peeraddr,&addrlen);
	if((fd<0)||(cmd->cmd!=START_ALARM_ACTIONS))
		return -1;	
	action=(struct usr_start_alarm_actions_struct *)cmd->para;
	event=action->event;
#ifdef SHOW_WORK_INFO
	printf("%s(fd=%d)发来控制执行报警联动命令0x0223,要求执行动作码%d\n",inet_ntoa(peeraddr.sin_addr),fd,event);
#endif
	gtloginfo("%s(fd=%d)发来控制执行报警联动命令0x0223,要求执行动作码%d\n",inet_ntoa(peeraddr.sin_addr),fd,event);
	take_alarm_action(-1,event);
	if(cmd->en_ack!=0)
		return send_gate_ack(fd,START_ALARM_ACTIONS, 0,env,enc,dev_no);	
	return 0;
}

//确认报警
static int usr_start_alarm_actions_yes_cmd(int fd,struct gt_usr_cmd_struct *cmd,int env,int enc,int dev_no)
{
	
	int i,j,k,event;
	struct sockaddr_in peeraddr;
	struct usr_start_alarm_actions_yes_struct *action;
	struct ipmain_para_struct *para;
	int addrlen=sizeof(struct sockaddr);
	i=getpeername(fd,(struct sockaddr *)&peeraddr,&addrlen);
	if((fd<0)||(cmd->cmd!=START_ALARM_ACTIONS_YES))
		return -1;	
	action=(struct usr_start_alarm_actions_yes_struct *)cmd->para;
#ifdef SHOW_WORK_INFO
	printf("%s(fd=%d)发来确认报警联动命令0x0222,报警状态0x%04x\n",inet_ntoa(peeraddr.sin_addr),fd,(int)action->trig);
#endif
	gtloginfo("%s(fd=%d)发来确认报警联动命令0x0222,报警状态0x%04x\n",inet_ntoa(peeraddr.sin_addr),fd,(int)action->trig);

	para = get_mainpara();
	
	//执行那些reset执行的报警联动
	for(i=0;i<(get_trigin_num());i++)
	{
		k=(i<10)?i:i+16;
		if((action->trig&(1<<k))==0)
			continue;
		for(j=0;j<MAX_TRIG_EVENT;j++)
		{
			event=get_alarm_event(i,1,j); 
			//printf("复位时event为%d\n",event);
			take_alarm_action(i,event);
		}
	}

	if(cmd->en_ack!=0)
		return send_gate_ack(fd,START_ALARM_ACTIONS_YES, 0,env,enc,dev_no);
	return 0;
}



static int usr_reboot_device_cmd(int fd,struct gt_usr_cmd_struct *cmd,int env,int enc,int dev_no)
{
	int i;
	int *boot_mode=NULL;
	struct sockaddr_in peeraddr;
	int rc;
	int addrlen=sizeof(struct sockaddr);
	if((fd<0)||(cmd->cmd!=USR_REBOOT_DEVICE))
		return -1;	
	i=getpeername(fd,(struct sockaddr *)&peeraddr,&addrlen);
	boot_mode=(int *)cmd->para;

#ifdef SHOW_WORK_INFO
	printf("%s(fd=%d) 发来%s重起设备命令\n",inet_ntoa(peeraddr.sin_addr),fd,*boot_mode?"硬":"软");
#endif
	gtloginfo("%s(fd=%d) 发来%s重起设备命令\n",inet_ntoa(peeraddr.sin_addr),fd,*boot_mode?"硬":"软");

	if(*boot_mode)
	{
		if(access("/tmp/hwrbt",R_OK|W_OK) == 0)
			rc = system("/tmp/hwrbt 3");
		else
			rc=system("/ip1004/hwrbt 3");
	}
	else
	{
		if(access("/tmp/swrbt",R_OK|W_OK) == 0)
			rc = system("/tmp/swrbt 3");
		else
			rc=system("/ip1004/swrbt 3");
	}
	
#ifdef SHOW_WORK_INFO
	printf("执行复位脚本rc=%d\n",rc);
#endif
	gtloginfo("执行复位脚本rc=%d\n",rc);

	if(cmd->en_ack!=0)
		return send_gate_ack(fd,USR_REBOOT_DEVICE, RESULT_SUCCESS,env,enc,dev_no);


	return 0;
}	


//软件升级完毕后响应
/*
int update_answer(int fd,WORD result,BYTE *resultinfo,int env, int enc,int dev_no)

{
	DWORD send_buf[50];//响应命令包不会超过200字节
	//BYTE  dev_buf[20];
	struct gt_usr_cmd_struct *cmd;
	struct mod_com_type *modcom;
	struct user_upd_ack_struct *upack;
	int state;
	int rc;
	int i;
	BYTE *info;
	modcom=(struct mod_com_type *)send_buf;
	modcom->env = env;
	modcom->enc = enc;
	cmd=(struct gt_usr_cmd_struct *)modcom->para;
	cmd->cmd=UPDATE_ANSWER;
	cmd->en_ack=0;
	
	cmd->reserve0=0;
	cmd->reserve1=0;
	upack=(struct user_upd_ack_struct*)cmd->para;
	rc=virdev_get_devid(dev_no,upack->dev_id);// cmd->para);

	print_devid(upack->dev_id,8);

	upack->state=result;
	upack->reserve=0;
	state=result;
	info=(BYTE*)upack->info;

	//根据state判断info中内容并写入

	sprintf(info,"%s",get_gt_errname(state));
	cmd->len=rc+SIZEOF_GT_USR_CMD_STRUCT-sizeof(cmd->para) -2+28;
	i=send_gate_pkt(fd,modcom,cmd->len+2,dev_no);

	sprintf(resultinfo,info);
	gtloginfo("%s发送升级响应，结果0x%04x\n",devlog(dev_no),result);
	return 0;
}
*/


//用户登录命令的响应，以指定的env和enc发到指定的fd
int send_dev_login_return(int fd, WORD result, int env, int enc,int dev_no)
{
	DWORD send_buf[50];
	int rc;
	struct dev_login_return *login_return;
	struct gt_pkt_struct *send=NULL;
	struct gt_usr_cmd_struct *cmd;
	struct sockaddr_in peeraddr;
	int addrlen=sizeof(struct sockaddr);
	
	getpeername(fd,(struct sockaddr *)&peeraddr,&addrlen);

	send=(struct gt_pkt_struct *)send_buf;
	cmd=(struct gt_usr_cmd_struct *)send->msg;
	cmd->cmd=DEV_LOGIN_RETURN;
	cmd->en_ack=0;
	cmd->reserve0=0;
	cmd->reserve1=0;
	login_return=(struct dev_login_return *)cmd->para;
	rc=virdev_get_devid(dev_no,login_return->dev_id);// cmd->para);

	login_return->result = result;
	login_return->reserved=0;
	login_return->login_magic = lrand48();//随机数
	cmd->len=sizeof(struct gt_usr_cmd_struct)+rc+sizeof(struct dev_login_return)-sizeof(cmd->para);
	
	rc=gt_cmd_pkt_send(fd,send,(cmd->len+2),NULL,0,env,enc);
#ifdef SHOW_WORK_INFO
	printf("%s向%s(fd=%d)发送用户登录响应命令(result=0x%04x),magic=%d\n",devlog(dev_no),inet_ntoa(peeraddr.sin_addr),fd,result,login_return->login_magic);
#endif
	gtloginfo("%s向%s(fd=%d)发送用户登录响应命令(result=0x%04x),magic=%d\n",devlog(dev_no),inet_ntoa(peeraddr.sin_addr),fd,result,login_return->login_magic);
	return 0;
	


}
//直接升级完后的响应，以指定的env和enc发到指定的fd
/*
int direct_update_answer(int fd,WORD result,BYTE *resultinfo, int env, int enc,int dev_no)
{
	
	DWORD send_buf[50];//响应命令包不会超过200字节
	struct user_upd_ack_struct *upack;
	int rc;
	BYTE *info;
	struct gt_pkt_struct *send=NULL;
	struct gt_usr_cmd_struct *cmd;
	struct sockaddr_in peeraddr;
	int addrlen=sizeof(struct sockaddr);

	getpeername(fd,(struct sockaddr *)&peeraddr,&addrlen);
	
	send=(struct gt_pkt_struct *)send_buf;
	cmd=(struct gt_usr_cmd_struct *)send->msg;
	cmd->cmd=UPDATE_ANSWER;
	cmd->en_ack=0;
	cmd->reserve0=0;
	cmd->reserve1=0;
	upack=(struct user_upd_ack_struct*)cmd->para;
	rc=virdev_get_devid(dev_no,upack->dev_id);// cmd->para);

	upack->state=result;
	upack->reserve=0;
	info=(BYTE*)upack->info;
	//根据state判断info中内容并写入
	sprintf(info,"%s",get_gt_errname(result));
	cmd->len=sizeof(struct gt_usr_cmd_struct)+rc+sizeof(struct user_upd_ack_struct)-sizeof(cmd->para);	
	rc=gt_cmd_pkt_send(fd,send,(cmd->len+2),NULL,0,env,enc);
	gtloginfo("%s向%s(fd = %d)发送直接升级命令的响应0x%04x:%s ",devlog(dev_no),inet_ntoa(peeraddr.sin_addr),fd,result,get_gt_errname(result));
	return 0;	

}
*/
//直接软件升级(non-ftp)
/*
static int update_sw_direct(int fd,struct gt_usr_cmd_struct *netcmd, int env, int enc,int dev_no)

{
	struct update_direct_struct *update_direct;
	int result=0;
	int i;
	int read_cnt = 0 ;
	int read_total = 0;
	int stateled;
	char updatedir[50];
	char gzfilename[200];
	BYTE buf[8192];
	time_t timep;
	int fail_cnt =0;
	struct tm *p;
	int write_cnt=0;
	FILE *fp;
	char updateinfo[20];
	struct sockaddr_in peeraddr;
	
	char cmd[100];
	int addrlen=sizeof(struct sockaddr);
		
	if((fd<0)||(netcmd->cmd!=UPDATE_SOFTWARE_DIRECT))
		return -1;	
	//取发送者
	i=getpeername(fd,(struct sockaddr *)&peeraddr,&addrlen);//获取连接对方的ip地址
	update_direct = (struct update_direct_struct*)netcmd->para;
	gtloginfo("%s 发来要求%s直接升级命令,类型为%d,文件大小%d\n",inet_ntoa(peeraddr.sin_addr),devlog(dev_no),update_direct->type, update_direct->filesize);

	//////lc do 2013-7-24 先删除ip1004下内容，再判断空间是否足够
	//lc do 20131107 改变升级方式，ip1004目录不动，使用剩下的36M空间中的32M，挂载一个独占的内存文件系统，用来升级
#if 1	
	sprintf(cmd,"mkdir -p %s","/ip1004/");
	i=system(cmd);   
	sprintf(cmd,"mkdir -p %s/temp","/ip1004");  
	i=system(cmd);
	i=chdir("/ip1004/");
	sprintf(cmd,"cp /ip1004/hardreboot /tmp/ -frv");
	system(cmd);
	sprintf(cmd,"cp /ip1004/hwrbt /tmp/ -frv");
	system(cmd);
	sprintf(cmd,"rm -rf %s/*","/ip1004");
	system(cmd);
#endif

	//检查是否空间足够
	result = -check_update_space(update_direct->filesize, updatedir); //result保持为正值
	if(result != RESULT_SUCCESS)
	{
		if(netcmd->en_ack!=0)
		i=direct_update_answer(fd,result,get_gt_errname(result),env,enc,dev_no);

		return 0;
	}


	
	//检查是否在升级
	if(updating_flag == 0 )
	{	
		updating_flag =1;
		
	}
	else
	{
		if(update_waiting_no >= 10)
		{
			gtloginfo("%s:同时要求升级的命令太多,返回BUSY\n",devlog(dev_no));
			result = ERR_DVC_BUSY;
			return direct_update_answer(fd,result,get_gt_errname(result),env,enc,dev_no);
		}
		
		pthread_mutex_lock(&update_mutex);
		updatesw_gate_list[update_waiting_no].dev_no = dev_no;
		updatesw_gate_list[update_waiting_no].gatefd = fd;
		updatesw_gate_list[update_waiting_no].env = env;
		updatesw_gate_list[update_waiting_no].enc = enc;
		update_waiting_no++;
		pthread_mutex_unlock(&update_mutex);
		printf("%s: direct updating !\n",devlog(dev_no));
		gtloginfo("%s:已在升级,升级完毕后统一返回\n",devlog(dev_no));
		result = 0;
		i=direct_update_answer(fd,result,get_gt_errname(result),env,enc,dev_no);
		return 0;
	}	

	
	
	
	if(netcmd->en_ack!=0)
		i=direct_update_answer(fd,result,get_gt_errname(result),env,enc,dev_no);

	
	if(result !=  RESULT_SUCCESS)
		return 0;
	
    //开始准备收数据
   	net_set_recv_timeout(fd,10);
    time(&timep);
    p=localtime(&timep);
    sprintf(gzfilename,"%s/up_direct_%04d%02d%02d.tar.gz",updatedir,1900+p->tm_year,1+p->tm_mon,p->tm_mday);

	gtloginfo("直接升级:准备开始接收数据存到%s\n",gzfilename);
	fp = fopen(gzfilename,"w+");
	while(read_total < update_direct->filesize)
	{
		read_cnt = net_read_buf(fd, buf,8192);
		if(read_cnt > 0)
		{
			fail_cnt=0;				//zw-20071204误删除后重新添加
			read_total += read_cnt;
			write_cnt = fwrite(buf,1,read_cnt,fp);
			if(write_cnt== read_cnt)
				continue;
			else
			{
				gtloginfo("直接升级中,写入文件%s失败,ferrno %d,退出\n",gzfilename,ferror(fp));
				result = ERR_DVC_INTERNAL;
				goto cleanup_and_return;
			}
		}
		//zw-20071204误删除后重新添加---->
		else
		{
			if(read_cnt==-EAGAIN)
			{///接收超时
				if(++fail_cnt < 30) //shixin changed from 3
					continue;
				//超过6秒则不再重试
			}
			//其他错误直接退出
		}
		//zw-20071204误删除后重新添加<----
		
	
		
		printf("直接升级中,读%d字节后read_buf返回%d,退出\n",read_total,read_cnt);
		gtlogerr("直接升级中,读%d字节后read_buf返回%d,退出\n",read_total,read_cnt);
		result = ERR_DVC_WRONG_SIZE;

cleanup_and_return://做好善后工作后，告知网关并退出
		fclose(fp);
		sprintf(cmd,"rm -rf %s",gzfilename);
		system(cmd);
		if(netcmd->en_ack!=0) //告知网关
			i=direct_update_answer(fd,result,get_gt_errname(result),env,enc,dev_no);
		pthread_mutex_lock(&update_mutex);
		for(i=0;i<update_waiting_no;i++)//逐个返回之前由于busy没返回的升级结果
		{
			if(updatesw_gate_list[i].gatefd <= 0)
				update_answer(updatesw_gate_list[i].gatefd,result,updateinfo,updatesw_gate_list[i].env,updatesw_gate_list[i].enc,updatesw_gate_list[i].dev_no);
			else
				direct_update_answer(updatesw_gate_list[i].gatefd,result,get_gt_errname(result),updatesw_gate_list[i].env,updatesw_gate_list[i].enc,updatesw_gate_list[i].dev_no);
		}
		update_waiting_no = 0;
		pthread_mutex_unlock(&update_mutex);
		updating_flag=0;
		//lc add
		system("/tmp/hardreboot");
		return -result;
	}
	
	fclose(fp);
	printf("%s直接升级接收文件%s成功\n",devlog(dev_no),gzfilename);
	gtloginfo("%s直接升级接收文件%s成功\n",devlog(dev_no),gzfilename);

	 //得到了大小相符的.tar.gz文件
	//stateled=get_current_stateled();//开始闪烁state灯，以防被拔电
	//set_state_led_state(1);	

	//lc 2014-2-21 从接受文件开始计时，停止发送心跳，5分钟变为间隔
	//使能心跳功能
#ifdef ARCH_3520A	
	update_set_com_mode(1,UPDATE_UNRESPONED_INTERVAL);
#endif	 
	result = direct_update_software(gzfilename,updatedir);
	//set_state_led_state(stateled);
	i=direct_update_answer(fd,result,get_gt_errname(result),env,enc,dev_no);

	pthread_mutex_lock(&update_mutex);
	for(i=0;i<update_waiting_no;i++)//逐个返回之前由于busy没返回的升级结果
	{
		if(updatesw_gate_list[i].gatefd <= 0)
			update_answer(updatesw_gate_list[i].gatefd,result,updateinfo,updatesw_gate_list[i].env,updatesw_gate_list[i].enc,updatesw_gate_list[i].dev_no);
		else
			direct_update_answer(updatesw_gate_list[i].gatefd,result,get_gt_errname(result),updatesw_gate_list[i].env,updatesw_gate_list[i].enc,updatesw_gate_list[i].dev_no);
	}
	update_waiting_no = 0;
	pthread_mutex_unlock(&update_mutex);

#ifdef SHOW_WORK_INFO
    printf("%s sending update_answer %d\n",devlog(dev_no),result);
#endif    
   //记日志

   	gtloginfo("%s直接升级结束，返回为 0x%04x, 结果为%s",devlog(dev_no),result,get_gt_errname(result));
	updating_flag = 0;
	return 0;
}
*/
//用户登录设备
static int usr_login_device_cmd(int fd,struct gt_usr_cmd_struct *netcmd, int env, int enc,int dev_no)

{
	struct usr_login_device *usr_login;
	
	int result=0;
	int i;
	struct sockaddr_in peeraddr;
	int addrlen=sizeof(struct sockaddr);
		
	if((fd<0)||(netcmd->cmd!=USR_LOGIN_DEVICE))
		return -1;	
	//取发送者
	i=getpeername(fd,(struct sockaddr *)&peeraddr,&addrlen);//获取连接对方的ip地址
	usr_login = (struct usr_login_device*)netcmd->para;
#ifdef SHOW_WORK_INFO
	printf("%s 登录%s,用户信息:%s\n",inet_ntoa(peeraddr.sin_addr),devlog(dev_no),usr_login->username);
#endif
	gtloginfo("%s 登录%s,用户信息:%s\n",inet_ntoa(peeraddr.sin_addr),devlog(dev_no),usr_login->username);


	if(netcmd->en_ack!=0)
		i=send_dev_login_return(fd,result,env,enc,dev_no);

	return 0;
	
}


//用户登录命令的响应，以指定的env和enc发到指定的fd
int send_dev_ip_return(int fd, WORD result, int env, int enc,int dev_no)
{

	DWORD send_buf[50];
	int rc;
       struct dev_ip_return *ip_return=NULL;
	struct gt_pkt_struct *send=NULL;
	struct gt_usr_cmd_struct *cmd;
	struct sockaddr_in peeraddr;
       struct sockaddr_in localaddr;

       
	int addrlen=sizeof(struct sockaddr);
	
	rc=getpeername(fd,(struct sockaddr *)&peeraddr,&addrlen);

       addrlen=sizeof(struct sockaddr);
       rc=getsockname(fd,(struct sockaddr *)&localaddr,&addrlen);
       memset(send_buf,0,sizeof(send_buf));
	send=(struct gt_pkt_struct *)send_buf;
	cmd=(struct gt_usr_cmd_struct *)send->msg;
	cmd->cmd=DEV_IP_RETURN;
	cmd->en_ack=0;
	cmd->reserve0=0;
	cmd->reserve1=0;
	ip_return=(struct dev_ip_return *)cmd->para;
	rc=virdev_get_devid(dev_no,ip_return->dev_id);// cmd->para);
	ip_return->result = result;
	ip_return->reserved1=0;

       memcpy(&ip_return->client_ip,&peeraddr.sin_addr,sizeof(ip_return->client_ip));
       memcpy(&ip_return->device_ip,&localaddr.sin_addr,sizeof(ip_return->device_ip));
       ip_return->client_ip=htonl(ip_return->client_ip);///转换成主机序
       ip_return->device_ip=htonl(ip_return->device_ip);

    
	cmd->len=sizeof(struct gt_usr_cmd_struct)+rc+sizeof(struct dev_ip_return)-sizeof(cmd->para);
	
	rc=gt_cmd_pkt_send(fd,send,(cmd->len+2),NULL,0,env,enc);

	return rc;
	


}

//用户查询自己的ip地址
static int usr_require_self_ip_cmd(int fd,struct gt_usr_cmd_struct *netcmd, int env, int enc,int dev_no)

{
	
	int result=0;
	
	if((fd<0)||(netcmd->cmd!=USR_REQUIRE_SELF_IP))
		return -1;	
	return send_dev_ip_return(fd,result,env,enc,dev_no);


}
/*
//软件升级
static int usr_update_software_cmd(int fd,struct gt_usr_cmd_struct *netcmd,int env,int enc,int dev_no)
{
	struct update_software_struct *update;
	char msg[300];
	char userid[13];
	char password[13];
	char FTPip[50],*FTPipPtr;
	char rebootcmd[100];
	unsigned long int FTPipaddress;
	char *filepath;
	int result;
	int i;
	int stateled;
	struct in_addr *addr;
	struct sockaddr_in peeraddr;
	char updateinfo[20];
	int addrlen=sizeof(struct sockaddr);
		
	if((fd<0)||(netcmd->cmd!=USER_UPDATE))
		return -1;	

	memset(msg,0,300);
	//取发送者
	i=getpeername(fd,(struct sockaddr *)&peeraddr,&addrlen);//获取连接对方的ip地址
	printf("usr_update_software_cmd remote rc=%d %s\n",i,inet_ntoa(peeraddr.sin_addr));
	
	update=(struct update_software_struct*)netcmd->para;
	memcpy(userid,update->userid,12);
	memcpy(password,update->password,12);
	userid[12]='\0';
	password[12]='\0';
	FTPipaddress=(unsigned long int)update->FTPip;

	FTPipaddress=htonl(FTPipaddress);
	addr=(struct in_addr *)&FTPipaddress;
	FTPipPtr=inet_ntoa(*addr);
	memcpy(FTPip,FTPipPtr,sizeof(FTPip));
	


	filepath=(char *)update->filepath;

	gtloginfo("%s(fd=%d) 发来%s升级命令,类型为 %d,路径为%s:%d/%s\n",inet_ntoa(peeraddr.sin_addr),fd,devlog(dev_no),update->type,FTPip,update->ftpport,filepath);
//add by wsy 05.09.26,判断filepath是否有"/"
//13.06.21 lc do 新设备中wget不支持-T超时，去掉
	if(index(filepath,'/')==filepath)
		sprintf(msg,"wget -c ftp://%s:%s@%s:%d%s",userid,password,FTPip,update->ftpport,filepath);
	else
		sprintf(msg,"wget -c ftp://%s:%s@%s:%d/%s",userid,password,FTPip,update->ftpport,filepath);

	printf("usr_update_software_cmd msg is %s\n",msg);

	if(netcmd->en_ack!=0)
		i=send_gate_ack(fd,USER_UPDATE,0,env,enc,dev_no);

	if(updating_flag==0)
	{
		updating_flag=1;

		//lc to do 设置升级中状态灯
//		stateled=get_current_stateled();
		//开始闪烁state灯，以防被拔电
		//set_state_led_state(1);
#ifdef ARCH_3520A		
		result=-update_software(update->filesize,msg,UPDATE_UNRESPONED_INTERVAL);
#else  
		result=-update_software(update->filesize,msg,-1);
#endif
		//set_state_led_state(stateled);
		updating_flag=0;
		i=update_answer(-1,result,updateinfo,env,enc,dev_no);
		pthread_mutex_lock(&update_mutex);
		for(i=0;i<update_waiting_no;i++)//逐个返回之前由于busy没返回的升级结果
		{
			if((updatesw_gate_list[i].gatefd) <= 0)
			{
				update_answer(updatesw_gate_list[i].gatefd,result,updateinfo,updatesw_gate_list[i].env,updatesw_gate_list[i].enc,updatesw_gate_list[i].dev_no);
				
			}
			else
			{
				direct_update_answer(updatesw_gate_list[i].gatefd,result,get_gt_errname(result),updatesw_gate_list[i].env,updatesw_gate_list[i].enc,updatesw_gate_list[i].dev_no);
			}
		}
		update_waiting_no = 0;
		pthread_mutex_unlock(&update_mutex);
	}	
	else
	{	
		if(update_waiting_no >= 10)
		{
			gtloginfo("同时要求升级的命令太多,返回BUSY\n");
			return update_answer(-1,ERR_DVC_BUSY,updateinfo,env,enc,dev_no);
		}
		
		pthread_mutex_lock(&update_mutex);
		updatesw_gate_list[update_waiting_no].dev_no = dev_no;
		updatesw_gate_list[update_waiting_no].gatefd = -1;
		updatesw_gate_list[update_waiting_no].env = env;
		updatesw_gate_list[update_waiting_no].enc = enc;
		update_waiting_no++;
		pthread_mutex_unlock(&update_mutex);
		gtloginfo("已在升级,升级完毕后统一返回\n");
		return 0;
	}
	
#ifdef SHOW_WORK_INFO
    printf("sending update_answer %d\n",result);
#endif    
   //记日志
   	gtloginfo("升级结束，返回为 0x%04x, 结果为%s",result,updateinfo);
	if((result==0))//remed by shixin &&(i==0))
	{
		if (update->reset_flag==0) 
		{
				gtloginfo("升级完成后退出ipmain\n");
				sleep(30);
				exit(0);
		}
		else if (update->reset_flag==1) 
		{
			sleep(2);
			//lc do
			system("/tmp/hardreboot &");
			printf("/tmp/hardreboot & done!\n");
			//send_require_reset();
		}
	}
	else
	{
		gtlogerr("升级失败，设备重启\n");
		system("/tmp/hardreboot &");
	}

	return 0;
}
*/
static int usr_require_rt(int fd,struct gt_usr_cmd_struct *cmd,int env,int enc,int dev_no)
{

	WORD	result;
	BYTE * guid ; 
	//struct in_addr  *addr;

	struct usr_req_rt_img_struct * query_cmd = (struct usr_req_rt_img_struct *)cmd->para;
	DWORD  peer_ip = htonl(query_cmd->remoteip);

	struct in_addr  *addr = (struct in_addr *)&peer_ip ;

	guid = query_cmd->dev_id;
	
	if(query_cmd->mode == 1) //query av
	{

		if(query_cmd->audio_flag == 1)
		{
				printf("IAG 收到请求音视频命令 rtmp://%s:%d/realplay/%02x%02x%02x%02x%02x%02x%02x%02x/ch%d\n",\
				inet_ntoa(*addr), \
				query_cmd->remoteport,\
				guid[0],guid[1],guid[2],guid[3],guid[4],guid[5],guid[6],guid[7],\
				query_cmd->channel);
		}
		else
		{
				printf("IAG 收到请求视频命令 rtmp://%s:%d/realplay/%02x%02x%02x%02x%02x%02x%02x%02x/ch%d\n",\
				inet_ntoa(*addr), \
				query_cmd->remoteport,\
				guid[0],guid[1],guid[2],guid[3],guid[4],guid[5],guid[6],guid[7],\
				query_cmd->channel);

		
		}


	}
	else
	{
		printf("IAG 收到退订视频命令 rtmp://%s:%d/realplay/%02x%02x%02x%02x%02x%02x%02x%02x/ch%d\n",\
				inet_ntoa(*addr), \
				query_cmd->remoteport,\
				guid[0],guid[1],guid[2],guid[3],guid[4],guid[5],guid[6],guid[7],\
				query_cmd->channel);



	}

	result=RESULT_SUCCESS;
	if(cmd->en_ack!=0)

		return send_gate_ack(fd,USR_REQUIRE_RT_IMAGE, result,env,enc,dev_no);
		//return send_gate_query_rt_return(fd,query_cmd->channel,result,env,enc,dev_no);
}


static int usr_require_pb(int fd,struct gt_usr_cmd_struct *cmd,int env,int enc,int dev_no)
{
	WORD	result;

	BYTE * guid ; 

	viewer_subscribe_record_struct  * query_cmd = (viewer_subscribe_record_struct  *)cmd->para;
	DWORD  peer_ip = htonl(query_cmd->remoteip);

	struct in_addr  *addr = (struct in_addr *)&peer_ip ;

	guid = query_cmd->dev_id;
	timepoint_struct * start_time = &query_cmd->starttime;
	timepoint_struct * end_time = &query_cmd->endtime;
	
	if(query_cmd->mode == 1) //query av
	{

			printf("IAG 收到请求录像点播命令 rtmp://%s:%d/playback/%02x%02x%02x%02x%02x%02x%02x%02x/ch%d/index%d\n",\
			inet_ntoa(*addr), \
			query_cmd->remoteport,\
			guid[0],guid[1],guid[2],guid[3],guid[4],guid[5],guid[6],guid[7],\
			query_cmd->channel,\
			query_cmd->stream_idx);

			printf("\n时间 %02d-%02d-%02d %02d:%02d:%02d ~~~~~~~~%02d-%02d-%02d %02d:%02d:%02d\n",\
						start_time->year,start_time->month,start_time->day,\
						start_time->hour,start_time->minute,start_time->second,\
						end_time->year,end_time->month,end_time->day,\
						end_time->hour,end_time->minute,end_time->second);
			printf("\n命令字 %d  速率%d\n",query_cmd->ctl_cmd,query_cmd->speed);

	}



	else
	{
			printf("IAG 收到退订录像点播命令 rtmp://%s:%d/playback/%02x%02x%02x%02x%02x%02x%02x%02x/ch%d/index%d\n",\
			inet_ntoa(*addr), \
			query_cmd->remoteport,\
			guid[0],guid[1],guid[2],guid[3],guid[4],guid[5],guid[6],guid[7],\
			query_cmd->channel,\
			query_cmd->stream_idx);

	}


	result=RESULT_SUCCESS;
	if(cmd->en_ack!=0)

		return send_gate_ack(fd,USR_REQUIRE_RECORD_PLAYBACK, result,env,enc,dev_no);

	return 0;

}

static int usr_stop_pb(int fd,struct gt_usr_cmd_struct *cmd,int env,int enc,int dev_no)
{

	WORD	result;

	viewer_unsubscribe_record_struct * query_cmd = (viewer_unsubscribe_record_struct *)cmd->para;

	printf("IAG 收到停止视频命令 id = %d \n",query_cmd->query_usr_id);	



	result=RESULT_SUCCESS;
	if(cmd->en_ack!=0)
		return send_gate_ack(fd,USR_REQUIRE_RT_IMAGE_ANSWER, result,env,enc,dev_no);
	return 0;


}




//static int regist_cnt=0;//shixintest
/**********************************************************************************************
 * 函数名	:netcmd_second_proc()
 * 功能	:处理远程网络连接的秒处理程序
 * 输入	:无
 *返回值	:无
 **********************************************************************************************/

void netcmd_second_proc(void)
{
	struct ip1004_state_struct *stat;
	int dev_no = 0;
	
	//struct send_dev_trig_state_struct *dev_trig;
	//for(dev_no=0;dev_no <virdev_get_virdev_number();dev_no++)
	{
	
		stat=get_ip1004_state(dev_no);	
		//gtloginfo("second_proc\n");
		if(get_regist_flag(dev_no)==0)
		{//还没有注册上
			if(++stat->regist_timer>REGIST_TIMEOUT)
			{
				stat->regist_timer=0;
				//gtloginfo("试图发送注册信息\n");
				system_regist(-1,1,0,0,dev_no);		//发送自己的注册信息
				//send_dev_state(-1,1);	//发送自己的状态
				//send_dev_trig_state(-1,1);//发送报警状态
			}
		}
		else
		{//已经注册上
			if(get_reportstate_flag(dev_no)==0)
			{				
					if(++stat->reportstate_timer>REPORTSTATE_TIMEOUT)
					{
						stat->reportstate_timer=0;
						//gtloginfo("再次试图发送自己的状态信息新加\n");
						send_dev_state(-1,1,1,0,0,dev_no);	//发送自己的状态
					}
			}

			if(get_alarm_flag(dev_no)==0)
			{
				if(++stat->alarm_timer>ALARM_TIMEOUT)
				{
					stat->alarm_timer=0;
						
					//gtloginfo("再次试图发送报警信息\n");
					send_dev_trig_state(-1,&last_dev_trig,1,0,0,dev_no);	//发送报警状态
				}	
			}
			
			if(get_trigin_flag(dev_no)==0)
			{
				//gtloginfo("stat->trigin_timer %d\n", stat->trigin_timer);
				if(++stat->trigin_timer>TRIGIN_TIMEOUT)
				{
					stat->trigin_timer=0;
					send_alarmin_state_change(stat->old_alarmin_state, stat->alarmin_state, stat->alarmin_change_time,dev_no);
				}	
			}
		}
	}
}



static int usr_ack_cmd(int fd,struct gt_usr_cmd_struct *cmd,int env, int enc,int dev_no)
{
	struct usr_cmd_ack_struct *ack;
	int i;
	char ackinfo[200];
	struct sockaddr_in peeraddr;
	int addrlen=sizeof(struct sockaddr);
	struct ipmain_para_struct *mainpara;
	struct alarm_motion_struct *alarmmotion;
	
	if((fd<0)||(cmd->cmd!=USR_CMD_ACK))
		return -1;
	ack=(struct usr_cmd_ack_struct*)cmd->para;
	i=getpeername(fd,(struct sockaddr *)&peeraddr,&addrlen);
	switch(ack->rec_cmd)
	{
		case DEV_REGISTER:
			
		if(ack->result==RESULT_SUCCESS)
		{
			set_regist_flag(dev_no,1);//注册成功
#ifdef SHOW_WORK_INFO
			printf("%s(fd=%d) 发来ACK告知%s注册成功\n",inet_ntoa(peeraddr.sin_addr),fd,devlog(dev_no));
#endif
			gtloginfo("%s(fd=%d) 发来ACK告知%s注册成功\n",inet_ntoa(peeraddr.sin_addr),fd,devlog(dev_no));
		}
		else 
		{
#ifdef SHOW_WORK_INFO
			printf("%s(fd=%d) 发来ACK告知%s注册失败,错误码0x%04x\n",inet_ntoa(peeraddr.sin_addr),fd,devlog(dev_no),ack->result);
#endif
			gtloginfo("%s(fd=%d) 发来ACK告知%s注册失败,错误码0x%04x\n",inet_ntoa(peeraddr.sin_addr),fd,devlog(dev_no),ack->result);
		}
		break;
		case DEV_STATE_RETURN: //报告状态
			if(ack->result==RESULT_SUCCESS)
			{
				set_reportstate_flag(dev_no,1);
#ifdef SHOW_WORK_INFO
				printf("%s(fd=%d) 发来ACK告知%s报告状态成功\n",inet_ntoa(peeraddr.sin_addr),fd,devlog(dev_no));
#endif
				gtloginfo("%s(fd=%d) 发来ACK告知%s报告状态成功\n",inet_ntoa(peeraddr.sin_addr),fd,devlog(dev_no));
			}
			else 
			{
#ifdef SHOW_WORK_INFO
				printf("%s(fd=%d) 发来ACK告知%s报告状态失败,错误码0x%04x\n",inet_ntoa(peeraddr.sin_addr),fd,devlog(dev_no),ack->result);
#endif
				gtloginfo("%s(fd=%d) 发来ACK告知%s报告状态失败,错误码0x%04x\n",inet_ntoa(peeraddr.sin_addr),fd,devlog(dev_no),ack->result);
			}
		break;
		case DEV_ALARM_RETURN:	//发案报警
			//lc do 将报警信息存入数据库
			if(ack->result==RESULT_SUCCESS)
			{	
				mainpara=get_mainpara();
				set_alarm_flag(dev_no,1);
#ifdef SHOW_WORK_INFO
				printf("%s(fd=%d) 发来ACK告知%s远程报警成功\n",inet_ntoa(peeraddr.sin_addr),fd,devlog(dev_no));
#endif
				gtloginfo("%s(fd=%d) 发来ACK告知%s远程报警成功\n",inet_ntoa(peeraddr.sin_addr),fd,devlog(dev_no));
			}
			else 
				{
					gtloginfo("%s(fd=%d) 发来ACK告知%s报警失败,错误码0x%04x\n",inet_ntoa(peeraddr.sin_addr),fd,devlog(dev_no),ack->result);
				}
		break;
		case DEV_QUERY_TRIG_RETURN:	//报端子输入状态变化
			set_trigin_flag(dev_no,1);
			//lc do 将报警信息存入数据库
			if(ack->result==RESULT_SUCCESS)
			{
#ifdef SHOW_WORK_INFO
				printf("%s(fd=%d) 发来ACK告知%s报端子输入状态成功\n",inet_ntoa(peeraddr.sin_addr),fd,devlog(dev_no));
#endif
				gtloginfo("%s(fd=%d) 发来ACK告知%s报端子输入状态成功\n",inet_ntoa(peeraddr.sin_addr),fd,devlog(dev_no));
			}
			else 
			{
#ifdef SHOW_WORK_INFO
				printf("%s(fd=%d) 发来ACK告知%s报端子输入状态失败,错误码0x%04x\n",inet_ntoa(peeraddr.sin_addr),fd,devlog(dev_no),ack->result);
#endif
				gtloginfo("%s(fd=%d) 发来ACK告知%s报端子输入状态失败,错误码0x%04x\n",inet_ntoa(peeraddr.sin_addr),fd,devlog(dev_no),ack->result);
			}
		break;
		case	USR_RW_DEV_PARA:
			{
#ifdef SHOW_WORK_INFO
				printf("%s(fd=%d) 发来对%sUSR_RW_DEV_PARA 的ACK result=0x%x\n",inet_ntoa(peeraddr.sin_addr),fd,devlog(dev_no),ack->result);
#endif
				gtloginfo("%s(fd=%d) 发来对%sUSR_RW_DEV_PARA 的ACK result=0x%x\n",inet_ntoa(peeraddr.sin_addr),fd,devlog(dev_no),ack->result);
			}
		break;
		case 	DEV_POSITION_RETURN:
//				gtloginfo("%s(fd=%d) 发来对%DEV_POSITION_RETURN 的ACK result=0x%x\n",inet_ntoa(peeraddr.sin_addr),fd,devlog(dev_no),ack->result);
		break;
		case 	DEV_PARA_RETURN:
#ifdef SHOW_WORK_INFO
				printf("%s(fd=%d) 发来对%sDEV_PARA_RETURN 的ACK result=0x%x\n",inet_ntoa(peeraddr.sin_addr),fd,devlog(dev_no),ack->result);
#endif
				gtloginfo("%s(fd=%d) 发来对%sDEV_PARA_RETURN 的ACK result=0x%x\n",inet_ntoa(peeraddr.sin_addr),fd,devlog(dev_no),ack->result);
		break;
		default:
#ifdef SHOW_WORK_INFO
				printf("%s(fd=%d) 发来对%s不支持的ACK cmd=0x%x,result 0x%x(%s)\n",inet_ntoa(peeraddr.sin_addr),fd,devlog(dev_no),ack->rec_cmd,ack->result,get_gt_errname(ack->result));
#endif
				gtloginfo("%s(fd=%d) 发来对%s不支持的ACK cmd=0x%x,result 0x%x(%s)\n",inet_ntoa(peeraddr.sin_addr),fd,devlog(dev_no),ack->rec_cmd,ack->result,get_gt_errname(ack->result));
		break;
	}
	return 0;
}



/**********************************************************************************
 *      函数名: fill_trigin_return_xml()
 *      功能:  按指定的xml格式填充xmlbuf内的相关信息
 *      输入:	xmlbuf:待填充的缓冲区指针
 *				newtrig:当前的端子输入状态
 *				oldtrig:此前的端子输入状态
 *				time:发生变化的时间，time_t类型
 *      输出:   无
 *      返回值: xmlbuf中的有效字节数，负值表示失败
 *********************************************************************************/
int fill_trigin_return_xml(OUT char *xmlbuf, IN DWORD newtrig, IN DWORD oldtrig, IN time_t time)
{
	int i;
	int oldbit=0, newbit=0;
	BYTE changed_trig[200];
	IXML_Document* doc;  
	IXML_Node  *root, *value;
 	IXML_Element *Ele, *info;
 	char buf[50];
 	char timestr[200];
 	struct tm *p;
 	
	if((xmlbuf == NULL))
		return -EINVAL;
	if(newtrig == oldtrig)
		return 0;
	
	sprintf(changed_trig, "<changed_trigs> </changed_trigs>");
    if(ixmlParseBufferEx(  changed_trig, &doc ) != IXML_SUCCESS )
    {
     	printf("parse %s failed\n", changed_trig);   
        return 0;
    }
   
   	
	p=localtime(&time);
   	sprintf(timestr,"%4d-%02d-%02d %02d:%02d:%02d",1900+p->tm_year,1+p->tm_mon,p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);
   	for(i=0; i<32; i++)
	{
		oldbit = (oldtrig>>i) & 1;
		newbit = (newtrig>>i) & 1;
		if(oldbit!=newbit)
		{
			root = ixmlNode_getFirstChild( ( IXML_Node * ) doc );
    		if(root == NULL)
    			printf("null!\n");
     		Ele = ixmlDocument_createElement( doc, "trig" );
      		ixmlNode_appendChild( root,(IXML_Node * ) Ele);
     		
     		info = ixmlDocument_createElement( doc, "id" );
     		ixmlNode_appendChild( (IXML_Node * )Ele,(IXML_Node * ) info);
      		sprintf(buf,"%d",i);
      		value = ixmlDocument_createTextNode(doc,buf );
      		ixmlNode_appendChild( (IXML_Node * )info, value);
      		
      		info = ixmlDocument_createElement( doc, "state" );
     		ixmlNode_appendChild( (IXML_Node * )Ele,(IXML_Node * ) info);
     		sprintf(buf,"%d",newbit);
      		value = ixmlDocument_createTextNode(doc,buf );
      		ixmlNode_appendChild( (IXML_Node * )info, value);
      		
      		info = ixmlDocument_createElement( doc, "time" );
     		ixmlNode_appendChild( (IXML_Node * )Ele,(IXML_Node * ) info);
      		value = ixmlDocument_createTextNode(doc,timestr);
      		ixmlNode_appendChild( (IXML_Node * )info, value);
   
   		}
	}
	
	sprintf(xmlbuf,ixmlDocumenttoString(doc));
	printf("\n\n\n");
	printf("length is %d, result is \n%s\n",strlen(xmlbuf),xmlbuf);
	printf("\n\n\n");

	
	//ixmlNode_free(value);
	//ixmlDocument_free(doc);
	
	
	//printf("here\n");
	return strlen(xmlbuf);

}


/**********************************************************************************
 *      函数名: send_alarmin_state_change()
 *      功能:  主动发送端子输入的变化状态
 *      输入:	old_alarmin，变化前的输入状态
 *				new_alarmin,变化后的输入状态
 *				time，发生变化的时间
 *      输出:   无
 *      返回值:0成功，负值失败
 *********************************************************************************/

int send_alarmin_state_change( DWORD old_alarmin, DWORD new_alarmin,time_t time, int dev_no)

{
	DWORD send_buf[200];							//响应命令包不会超过800字节
	struct gt_usr_cmd_struct *cmd=NULL;
	struct dev_query_trig_return *trig_return=NULL;	
	int rc=0;
	struct mod_com_type *modcom;
	
		
	memset(send_buf,0,sizeof(send_buf));			//初始化发送缓冲区
	modcom = (struct mod_com_type *)send_buf;
	modcom->env = 0;
	modcom->enc = 0;
	
	cmd=(struct gt_usr_cmd_struct *)modcom->para;	//为cmd分配内存空间
	cmd->cmd=DEV_QUERY_TRIG_RETURN;
	cmd->en_ack=1;
	
	
	
	trig_return=(struct dev_query_trig_return *)((char *)cmd->para);		//为trig_return分配内存空间
	rc=get_devid(trig_return->dev_id);	
	
	trig_return->result=0;
	trig_return->reserve1=0;
	trig_return->alarmin = new_alarmin;
	
	trig_return->changed_info_len = fill_trigin_return_xml(trig_return->changed_info,new_alarmin,old_alarmin, time);
	if(trig_return->changed_info_len <0) //failed
	trig_return->changed_info_len = 0;
	
	cmd->len=SIZEOF_GT_USR_CMD_STRUCT-sizeof(cmd->para)-2+SIZEOF_DEV_QUERY_TRIG_RETURN+trig_return->changed_info_len-4;
	
	set_trigin_flag(dev_no,0);
	rc=	send_gate_pkt(-1,modcom,cmd->len+2,dev_no);

	printf("%s向远端服务器主动发送输入端子状态变化,0x%08x->0x%08x,rc=%d\n",devlog(dev_no),old_alarmin,new_alarmin,rc);
	gtloginfo("%s向远端服务器主动发送输入端子状态变化,0x%08x->0x%08x,rc=%d\n",devlog(dev_no),old_alarmin,new_alarmin,rc);

	return 0;



}

/**********************************************************************************
 *      函数名: send_query_trigstate_return()
 *      功能:  发送查询设备端子输入及变化状态响应
 *      输入:	fd		用户连接的tcp句柄
 *				result	操作结果
 *				env		接收到的命令使用的数字信封格式
 *				enc		刚接收到的命令使用的加密类型
 *      输出:   无
 *      返回值:0成功，负值失败
 *********************************************************************************/
int send_query_trigstate_return(int fd,DWORD result,int env,int enc,int dev_no)
{
	DWORD send_buf[200];							//响应命令包不会超过800字节
	struct gt_usr_cmd_struct *cmd=NULL;
	struct dev_query_trig_return *trig_return=NULL;	
	int rc;
	struct gt_pkt_struct *send;
	
		
	memset(send_buf,0,sizeof(send_buf));			//初始化发送缓冲区
	send = (struct gt_pkt_struct *)send_buf;
	cmd=(struct gt_usr_cmd_struct *)send->msg;	//为cmd分散内存空间
	cmd->cmd=DEV_QUERY_TRIG_RETURN;
	cmd->en_ack=0;
		
	trig_return=(struct dev_query_trig_return *)((char *)cmd->para);		//为trig_return分配内存空间
	rc=virdev_get_devid(dev_no,trig_return->dev_id);	

	
	trig_return->result=result;
	trig_return->reserve1=0;

	trig_return->alarmin = get_ip1004_state(dev_no)->alarmin_state;

	trig_return->changed_info_len = 0;
		
	cmd->len=SIZEOF_GT_USR_CMD_STRUCT-sizeof(cmd->para)-2+SIZEOF_DEV_QUERY_TRIG_RETURN+trig_return->changed_info_len-4;
	
	rc=gt_cmd_pkt_send(fd,send,(cmd->len+2),NULL,0,env,enc);
	
	printf("向远端服务器发送 查询%s端子输入状态命令返回%d,状态0x%08x,rc=%d\n",devlog(dev_no),result,trig_return->alarmin,rc);
	gtloginfo("向远端服务器发送 查询%s端子输入状态命令返回%d,状态0x%08x,rc=%d\n",devlog(dev_no),result,trig_return->alarmin,rc);

	return 0;
}

/**********************************************************************************
 *      函数名: usr_query_trigstate()
 *      功能:   查询设备报警状态
 *      输入:	fd		用户连接的tcp句柄
 *				cmd		命令结构
 *				env		接收到的命令使用的数字信封格式
 *				enc		刚接收到的命令使用的加密类型
 *      输出:   无
 *      返回值:0成功，负值失败
 *********************************************************************************/

static int usr_query_trigstate(int fd,struct gt_usr_cmd_struct* cmd,int env,int enc,int dev_no)

{
	struct sockaddr_in peeraddr;
	int i;
	
	int addrlen=sizeof(struct sockaddr);
	i=getpeername(fd,(struct sockaddr *)&peeraddr,&addrlen);

	if(cmd->cmd!=USR_QUERY_TRIGSTATE )
	{
		return -1;
	}


#ifdef SHOW_WORK_INFO
		printf("ipmain recv USR_QUERY_TRIGSTATE cmd for DEV %d!\n",dev_no);
#endif	
		gtloginfo("%s(fd=%d)发来 查询%s端子输入状态命令0x%04x\n",inet_ntoa(peeraddr.sin_addr),fd,devlog(dev_no),USR_QUERY_TRIGSTATE );




	if(virdev_get_trigin_num(dev_no)==0)
		send_query_trigstate_return(fd,ERR_DVC_NO_TRIG,env,enc,dev_no);
	else
		send_query_trigstate_return(fd,RESULT_SUCCESS,env,enc,dev_no);

	return 0;

}


/**********************************************************************************
 *      函数名: usr_query_regist()
 *      功能:  	通过本命令查询注册消息，设备收到后应返回注册
 *				命令进行注册
 *      输入:	fd		用户连接的tcp句柄
 *				cmd		命令结构
 *				env		接收到的命令使用的数字信封格式
 *				enc		刚接收到的命令使用的加密类型
 *      输出:   无
 *      返回值:0成功，负值失败
 *********************************************************************************/
static int usr_query_regist(int fd,struct gt_usr_cmd_struct* cmd,int env,int enc,int dev_no)
{
	struct usr_query_regist *usr_regist=NULL;
	struct sockaddr_in peeraddr;
	int i;
	int ret;
		
	int addrlen=sizeof(struct sockaddr);
	i=getpeername(fd,(struct sockaddr *)&peeraddr,&addrlen);

	if(cmd->cmd!=USR_QUERY_REGIST )
	{
		return -1;
	}
#ifdef SHOW_WORK_INFO
	printf("ipmain recv USR_QUERY_REGIST  cmd!\n");
#endif
		
	//解析用户命令
	usr_regist=(struct usr_query_regist *)cmd->para;

#ifdef SHOW_WORK_INFO
	printf("%s(fd=%d)发来注册查询命令0x%04x\n",inet_ntoa(peeraddr.sin_addr),fd,USR_QUERY_REGIST);
#endif
	gtloginfo("%s(fd=%d)发来注册查询命令0x%04x\n",inet_ntoa(peeraddr.sin_addr),fd,USR_QUERY_REGIST);

	ret=system_regist(fd,0,env,enc,dev_no);

	if(ret<0)
	{
#ifdef SHOW_WORK_INFO
		printf("设备注册失败，错误码:%d\n",ret);
#endif
		gtlogerr("设备注册失败，错误码:%d\n",ret);
	}

	return 0;
}




/**********************************************************************************************
 * 函数名	:process_netcmd()
 * 功能	:处理远程计算机发来的命令
 * 输入	:fd:已连接的socket描述符
 *			 cmd:刚接收到的命令结构指针
 *			 env:接收到的命令使用的数字信封格式
 *			 enc:刚接收到的命令使用的加密类型
 *返回值	:无
 **********************************************************************************************/

void process_netcmd(int fd,struct gt_usr_cmd_struct* cmd,int env,int enc,int dev_no)
{
	
	int i;
	struct sockaddr_in peeraddr;
	int addrlen=sizeof(struct sockaddr);
	i=getpeername(fd,(struct sockaddr *)&peeraddr,&addrlen);
//#ifdef SHOW_WORK_INFO
	printf("recv cmd[fd=%d]:0x%04x\n",fd,cmd->cmd);
//#endif

	switch(cmd->cmd)
	{		
		case USR_SET_AUTH_IP:
			usr_set_gate_ip(fd,cmd,env,enc,dev_no);
		break;//o
		case USR_CLOCK_SETING:
			usr_set_clock(fd,cmd,env,enc,dev_no);
		break;
		case USR_SET_SWITCH_IN:
			usr_set_trigger_in(fd,cmd,env,enc,dev_no);//o notest
		break;
		//lc to do 设置报警输出属性
		/*
		case USR_SET_SWITCH_OUT:
			usr_set_alarm_out(fd,cmd,env,enc,dev_no);//o notest	
		break;
		*/
		case USR_SWITCH_OUT:
			usr_alarm_ctl(fd,cmd,env,enc,dev_no);
		break;		
		case USR_CANCEL_ALARM:
			usr_cancel_alarm_cmd(fd,cmd,env,enc,dev_no);	
		break;
		case START_ALARM_ACTIONS_YES:
			usr_start_alarm_actions_yes_cmd(fd,cmd,env,enc,dev_no);
		break;	
		case START_ALARM_ACTIONS:
				usr_start_alarm_actions_cmd(fd,cmd,env,enc,dev_no);
		break;
	
		//查询报警状态	add-20071115
		case USR_QUERY_TRIGSTATE:
				usr_query_trigstate(fd,cmd,env,enc,dev_no);
		break;

		//注册查询add-20071115
		case USR_QUERY_REGIST:
				usr_query_regist(fd,cmd,env,enc,dev_no);
		break;
			
		case USR_QUERY_STATE:
				usr_query_state_cmd(fd,cmd,env,enc,dev_no);
		break;
		
		case USR_REBOOT_DEVICE:
				usr_reboot_device_cmd(fd,cmd,env,enc,dev_no);
		break;
		case USR_CMD_ACK:
				usr_ack_cmd(fd,cmd,env,enc,dev_no);	
		break;
		//lc do 设备升级功能
	/*	
		case USER_UPDATE:              //升级软件
				usr_update_software_cmd(fd,cmd,env,enc,dev_no);
		break;
		case UPDATE_SOFTWARE_DIRECT: //直接给设备升级
				update_sw_direct(fd,cmd,env,enc,dev_no);
		break;
	*/	

		case USR_RW_DEV_PARA://访问配置文件
				usr_rw_para_file(fd,cmd,env,enc,dev_no);
		break;
		case USR_LOGIN_DEVICE://用户登录设备
				usr_login_device_cmd(fd,cmd,env,enc,dev_no);
		break;
		case USR_REQUIRE_SELF_IP://用户查询自己的ip地址
				usr_require_self_ip_cmd(fd,cmd,env,enc,dev_no);
		break;	

		case USR_REQUIRE_RT_IMAGE: //用户请求音视频
				usr_require_rt(fd,cmd,env,enc,dev_no);
		break;
		case USR_REQUIRE_RECORD_PLAYBACK: //用户请求录像回放
				usr_require_pb(fd,cmd,env,enc,dev_no);
		break;
		case USR_STOP_RECORD_PLAYBACK: //用户停止录像回放
				usr_stop_pb(fd,cmd,env,enc,dev_no);
		break;
		
		
		
				
		default:
			printf("ipmain recv unknown gatecmd:0x%04x\n",cmd->cmd);	
			gtlogwarn("%s发来不支持的网络命令0x%04x\n",inet_ntoa(peeraddr.sin_addr),cmd->cmd);
			send_gate_ack(fd,cmd->cmd,ERR_EVC_NOT_SUPPORT,env,enc,dev_no);	
		break;
	}

}





