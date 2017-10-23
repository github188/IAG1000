#include "ipmain.h"
#include "netcmdproc.h"
#include "ipmain_para.h"
#include "gt_com_api.h"
#include "netinfo.h"
#include "gate_connect.h"
#include "devstat.h"
#include "confparser.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "gate_connect.h"
#include <commonlib.h>


static struct ipmain_para_struct ipmain_para;

int GATE_PERIOD_TIME=0;
static int  set_sock_addr(struct sockaddr_in *sock,char *host);


char * devlog(int dev_no)
{
    return "设备";
}

/**********************************************************************************************
 * 函数名	:init_para()
 * 功能	:系统参数初始化，将参数设置为默认值
 * 输入	:无
 * 返回值	:无
 **********************************************************************************************/
void  init_para(void)
{//
	int i,j;
	struct ipmain_para_struct *p;
	struct gate_struct *gt_list;
	char buf[30];
	memset(&ipmain_para,0,sizeof(struct ipmain_para_struct));
	p=&ipmain_para;
	int dev_no;
	dev_para_struct *devpara;
	for(dev_no = 0;dev_no < virdev_get_virdev_number(); dev_no++)
	{	
		devpara = (dev_para_struct *)&(ipmain_para.devpara[dev_no]);
		//MAX_GATE_LIST
		gt_list=get_gate_list(dev_no);
		devpara->rmt_gate0=&gt_list[0].gateaddr;
		devpara->rmt_gate1=&gt_list[1].gateaddr;
		devpara->rmt_gate2=&gt_list[2].gateaddr;
		devpara->rmt_gate3=&gt_list[3].gateaddr;
		devpara->rmt_gate4=&gt_list[4].gateaddr;
		devpara->alarm_gate=&gt_list[5].gateaddr;
		
		
		sprintf(buf,"0.0.0.0 %d",REMOTE_GATE_CMD_PORT);
		set_sock_addr(devpara->rmt_gate0,buf);
		set_sock_addr(devpara->rmt_gate1,buf);
		set_sock_addr(devpara->rmt_gate2,buf);
		set_sock_addr(devpara->rmt_gate3,buf);
		set_sock_addr(devpara->rmt_gate4,buf);
		set_sock_addr(devpara->alarm_gate,buf);

		sprintf(devpara->inst_place,"%s","未设置");
		devpara->cmd_port=DEV_MAIN_CMD_PORT;
		devpara->ip_chg_flag = 0;
		devpara->sendgateini_flag = 1;
		
	}

	GATE_PERIOD_TIME=GATE_PERIOD_TIME_DEF;
	p->rmt_env_mode=0;

	p->image_port=DEV_MAIN_IMAGE_PORT;
	p->audio_port=DEV_MAIN_AUDIO_PORT;
	p->ftp_port=DEV_DEFAULT_FTP_PORT;
	p->web_port=DEV_DEFAULT_WEB_PORT;
	p->trig_in=0;
	p->tin_mask=ALARMIN_MASK;
	p->alarm_out=0;
	p->alarm_mask=ALARMOUT_MASK;  

	p->inst_ack=0;
	p->internet_mode=0;
	
}

/**********************************************************************************************
 * 函数名	:refresh_netinfo()
 * 功能	:刷新网络状态参数
 * 输入	:无
 * 返回值	:无
 **********************************************************************************************/
void refresh_netinfo(void)
{
	in_addr_t addr;
	struct ip1004_state_struct *gtstate;
	int i = 0;
	addr=get_net_dev_ip("ppp0");
	if((int)addr==-1)
	{
		memset(&ipmain_para.wan_addr,0,sizeof(ipmain_para.wan_addr));
		//for(i=0;i<virdev_get_virdev_number();i++)
		{	
			gtstate=get_ip1004_state(i);
			pthread_mutex_lock(&gtstate->mutex);
			gtstate->reg_dev_state.link_err=1;
			pthread_mutex_unlock(&gtstate->mutex);	
		}					
 	}
	else
	{
		if(memcmp(&addr,&ipmain_para.wan_addr,sizeof(addr))!=0)
		{
			//for(i=0;i<virdev_get_virdev_number();i++)
			{
				set_regist_flag(i,0);//ip地址改变
				gtstate=get_ip1004_state(i);
				pthread_mutex_lock(&gtstate->mutex);
				gtstate->reg_dev_state.link_err=0;
				pthread_mutex_unlock(&gtstate->mutex);		
				if(get_gate_connect_flag(i))
				ipmain_para.devpara[i].ip_chg_flag=1;
			}
			printf("refresh_netinfo change ip %x->%x\n",ipmain_para.wan_addr,addr);
			memcpy(&ipmain_para.wan_addr,&addr,sizeof(addr));
		}
	}		
	
	addr=get_net_dev_ip("br0");
	if((int)addr==-1)
	{
		memset(&ipmain_para.lan_addr,0,sizeof(ipmain_para.lan_addr));		
	}
	else
	{
		memcpy(&ipmain_para.lan_addr,&addr,sizeof(addr));
		addr=get_net_dev_mask("br0");
		if((int)addr==-1)
		{
			memset(&ipmain_para.lan_mask,0,sizeof(ipmain_para.lan_mask));	
		}
		else
		{
			memcpy(&ipmain_para.lan_mask,&addr,sizeof(addr));
		}

	}
}


static int  set_sock_addr(struct sockaddr_in *sock,char *host)
{
	struct hostent *hostPtr = NULL;
	char *begin,*end;
	char buf[100];
	int i,len;
	unsigned short port;
	if((sock==NULL)||(host==NULL))
		return -1;
	sock->sin_family = AF_INET;
	len=strlen(host);
	if((len+1)>100)
		return -1;
	memcpy(buf,host,len+1);
	begin=buf;
	for(i=0;i<len;i++)//去掉字符串前面的空格
	{
		if(*begin!=' ')
			break;
		begin++;
	}
	len-=i;
#if 0
	if(len<=8)	//少于8个字节肯定是错误
	{
		return -1;
	}
#endif
	end=begin;
	for(i=0;i<len;i++)
	{
		if((*end==' ')||(*end=='\0'))
			break;
		end++;
	}
	*end='\0';
	len-=i;
	hostPtr = gethostbyname(begin); /* struct hostent *hostPtr. */ 
       if (NULL == hostPtr)
    	{
        	hostPtr = gethostbyaddr(begin, strlen(begin), AF_INET);
        	if (NULL == hostPtr) 
        	{
        		printf("Error resolving server address %s",host);
        		return -1;
        	}
      }
      memcpy(&sock->sin_addr,hostPtr->h_addr,hostPtr->h_length);	

	if(len>3)
	{
		end++;
		begin=end;
		//printf("begin2=%s,len=%d\n",begin,len);
		for(i=0;i<len;i++)
		{
			if((*begin!=' ')&&(*begin!='\0'))
				break;
			begin++;
		}
		len-=i;
		//printf("begin3=%s,len=%d\n",begin,len);
		if(len>3)
		{
			end=begin;
			for(i=0;i<len;i++)
			{
				if((*end==' ')||(*end=='\0'))
					break;
				end++;
			}
			
			*end='\0';
			//printf("end2=%s,begin=%s,len=%d,i=%d\n",end,begin,len,i);
			if(i>3)
			{
				port=atoi(begin);
				//printf("port=%d\n",port);
				sock->sin_port=htons((port));
			}
		}
	}
	return 0;
}


/**********************************************************************************************
 * 函数名	:save_setalarm_para()
 * 功能		:以整数形式的值存入ini结构中的报警布撤防参数变量名
 * 输入		:type:报警类型，0为端子触发，1为移动侦测
 *			 ch:相应的通道数
 *			 setalarm:布防(1)或撤防(0)
 *			 starthour: 布防起始小时
 *			 startmin:布防起始分钟
 *			 endhour: 布防结束小时
 *			 endmin: 布防结束分钟
 * 输出	: ini:描述ini文件的结构指针,返回时被填充新值
 * 返回值	:0表示成功，负值表示失败
 **********************************************************************************************/
int save_setalarm_para(dictionary *ini,int type, int ch,int setalarm, int starthour, int startmin, int endhour, int endmin)
{
	
	char entry[30];
	
	if(ini==NULL)
		return -EINVAL;
	if(type==0)//端子
	{
		if((ch+1)>get_trigin_num())
			return -EINVAL;
		sprintf(entry,"alarm%d:setalarm",ch);
		iniparser_setint(ini,entry,setalarm);
		if(setalarm==1)
		{
			sprintf(entry,"alarm%d:starthour",ch);
			iniparser_setint(ini,entry,starthour);
			sprintf(entry,"alarm%d:startmin",ch);
			iniparser_setint(ini,entry,startmin);
			sprintf(entry,"alarm%d:endhour",ch);
			iniparser_setint(ini,entry,endhour);
			sprintf(entry,"alarm%d:endmin",ch);
			iniparser_setint(ini,entry,endmin);
		}
	}
	if(type==1)//移动侦测
	{
		if((ch+1)>get_video_num())
			return -EINVAL;
		sprintf(entry,"motion%d:setalarm",ch);
		iniparser_setint(ini,entry,setalarm);
		if(setalarm==1)
		{
			sprintf(entry,"video%d:starthour",ch);
			iniparser_setint(ini,entry,starthour);
			sprintf(entry,"video%d:startmin",ch);
			iniparser_setint(ini,entry,startmin);
			sprintf(entry,"video%d:endhour",ch);
			iniparser_setint(ini,entry,endhour);
			sprintf(entry,"video%d:endmin",ch);
			iniparser_setint(ini,entry,endmin);
		}
	}
	return 0;
}

#include <netinet/in.h>

//从读出的字符串中解析

int string_to_actions(int *act, char *pstr)
{
	int number=0; //有效个数
	char string[201];
	char *lp;
	if ((pstr==NULL)||(act==NULL))
		return -1;
	memcpy(string,pstr,200);
	*act=atoi(string);
	//printf("get a int :%d\n",*act);
	number++;
	act++;
	lp=index(string,',');
	while((lp!=NULL) &&(number<=MAX_TRIG_EVENT))
	{	
		lp++;
		//printf("lp is %s\n,int is %d",lp,atoi(lp));
		*act=atoi(lp);
		act++;
		//printf("get a int :%d\n",*act);
		number++;
		
		lp=index(lp,',');
	}
	return number;
}

static char alarm_schdule_str[200];
/**********************************************************************************************
 * 函数名	:get_setalarm_str()
 * 功能		:获取给定的报警结构的布撤防及时间段情况
 * 输入		:报警结构alarm_trigin_struct的指针，对端子和移动侦测都适用
 * 返回值	:描述给定的报警结构的布撤防及时间段情况的字符串
 **********************************************************************************************/
char *get_setalarm_str(struct alarm_trigin_struct *trigin)
{
	
	int starttime=0;
	int endtime=0;
	
	if(trigin==NULL)
		return "不确定";
	if(trigin->enable==0)
		return "无效";
	if(trigin->setalarm==0)
		return "撤防";

	starttime=trigin->starthour*60+trigin->startmin;
	endtime=trigin->endhour*60+trigin->endmin;

	if(starttime==endtime)	//起始时间=结束时间
		return "全天布防";

	sprintf(alarm_schdule_str,"%02d:%02d~%02d:%02d布防",trigin->starthour,trigin->startmin,trigin->endhour,trigin->endmin);
	return alarm_schdule_str;
}

/**********************************************************************************************
 * 函数名	:readmain_para_file()
 * 功能	:将ip1004.ini文件中的参数信息读取到参数结构中
 * 输入	:filename:配置文件名
 * 输出	:para:返回时填充参数的结构指针
 * 返回值	:0表示成功，负值表示失败
 **********************************************************************************************/
int  readmain_para_file(char *filename,struct ipmain_para_struct *para)
{//

	struct ipmain_para_struct *p;
	dictionary      *ini;	
	FILE *lock_fp=NULL;
	struct video_para_struct *vadc;
	int i;
	char *pstr;
	//int status;
	int num;
	int ret;
	refresh_netinfo();	//读取网络信息
	if(filename==NULL)
		return -10;
	p=para;
	if(p==NULL)
		return -11;
	//ini=iniparser_load(filename);
	ini=iniparser_load_lockfile(filename,1,&lock_fp);
	if (ini==NULL) {
                printf("vsmain  cannot parse ini file file [%s]", filename);
                return -1 ;

        }
		
	pstr=iniparser_getstring(ini,"install:inst_place",NULL);
	if(pstr!=NULL)
	{
		num=strlen(pstr);
		if(num>99)
			num=99;
		p->devpara[0].inst_place[num]='\0';
		memcpy(p->devpara[0].inst_place,pstr,(num+1));
	}
	
	GATE_PERIOD_TIME=iniparser_getint(ini,"remote_gate:timeout",GATE_PERIOD_TIME_DEF);
	if(GATE_PERIOD_TIME<=0)
		GATE_PERIOD_TIME=0x7fffffff;//如果小于0则表示不需要断开连接

	//读取服务端口参数
	p->devpara[0].cmd_port = iniparser_getint(ini,"port:cmd_port",DEV_MAIN_CMD_PORT);

	p->image_port=iniparser_getint(ini,"port:image_port",DEV_MAIN_IMAGE_PORT);
	p->audio_port=iniparser_getint(ini,"port:audio_port",DEV_MAIN_AUDIO_PORT);
	p->ftp_port=iniparser_getint(ini,"port:ftp_port",DEV_DEFAULT_FTP_PORT);
	p->web_port=iniparser_getint(ini,"port:web_port",DEV_DEFAULT_WEB_PORT);
	p->telnet_port=iniparser_getint(ini,"port:telnet_port",DEV_DEFAULT_TELNET_PORT);

	pstr=iniparser_getstring(ini,"remote_gate:rmt_gate1",NULL);
	ret = set_sock_addr(p->devpara[0].rmt_gate1,pstr);
	pstr=iniparser_getstring(ini,"remote_gate:rmt_gate2",NULL);
	ret = set_sock_addr(p->devpara[0].rmt_gate2,pstr);
	pstr=iniparser_getstring(ini,"remote_gate:rmt_gate3",NULL);
	ret = set_sock_addr(p->devpara[0].rmt_gate3,pstr);
	pstr=iniparser_getstring(ini,"remote_gate:rmt_gate4",NULL);
	ret = set_sock_addr(p->devpara[0].rmt_gate4,pstr);
	pstr=iniparser_getstring(ini,"remote_gate:alarm_gate",NULL);
	set_sock_addr(p->devpara[0].alarm_gate,pstr);
	
	p->inst_ack=iniparser_getint(ini,"product:inst_ack",0);
	
    p->ini_version=iniparser_getint(ini,"alarmmotion:version",0);

	save_inidict_file(filename,ini,&lock_fp);	//zw-add 保存数据到ip1004.ini中
  	iniparser_freedict(ini);
	return 0;	
}

/**********************************************************************************************
 * 函数名	:get_mainpara()
 * 功能	:获取设备参数结构指针
 * 输入	:无
 * 返回值	:设备参数结构指针
 **********************************************************************************************/
struct ipmain_para_struct * get_mainpara(void)
{//
	return &ipmain_para;
}

/**********************************************************************************************
 * 函数名	:AddParaFileVersion()
 * 功能	:将配置文件的版本号加1
 * 输入	:type表示配置文件类型  2:ip1004.ini 3:alarm.ini
 * 返回值	:新的版本号,负值表示失败
 **********************************************************************************************/
int AddParaFileVersion(int dev_no,int type)
{
	char *filename;
	dictionary      *ini;	
	int version;
	char vbuf[30];
	if(type==2)
	{
		filename = IPMAIN_PARA_FILE;
	}
	else
	{
		return -1;
	}	
	ini=iniparser_load(filename);
        if (ini==NULL) {
                printf("vsmain  cannot parse ini file file [%s]", filename);
                return -1 ;

        }
	version=iniparser_getint(ini,"alarmversion:version",-1);
		
	if(version<0)
	{//存在节 alarmversion
		if(iniparser_find_entry(ini,"alarmversion")==1)
			version=0;
	}
	iniparser_freedict(ini);
	if(version>=0)
	{
		version++;
		sprintf(vbuf,"%d",version);
		save2para_file(filename,"alarmversion:version",vbuf);
	}
	return version;
}

/**********************************************************************************************
 * 函数名	:CheckParaFileChange()
 * 功能	:检查配置文件是否有变化
 * 输入	:type表示配置文件类型  2:ip1004.ini 3:alarm.ini
 * 返回值	:返回0表示无变化  1表示有变化 -1表示类型错误
 **********************************************************************************************/
int CheckParaFileChange(int dev_no,int type)
{
	char *filename;
	char *bakfile;
	int ret;
	if(type==2)
	{
		filename=IPMAIN_PARA_FILE;
		bakfile=IPMAIN_PARA_GATE_BAK;
	}
	else
	{
		return -1;
	}

	//比较两个文件
	ret=ini_diff(bakfile,filename);
	if(ret==0)
		return 0;
	else
		return 1;	
}


/**********************************************************************************************
 * 函数名	:CopyPara2Bak()
 * 功能	:将配置文件更新到相应的备份
 * 输入	:type:配置文件类型2:ip1004.ini 3:alarm.ini
 * 返回值	:0表示成功，负值表示失败
 **********************************************************************************************/

int CopyPara2Bak(int dev_no,int type)
{
	char *filename;
	char *bakfile;
	int ret;
	char tbuf[200];
	if(type==2)
	{
		filename=IPMAIN_PARA_FILE;
		bakfile=IPMAIN_PARA_GATE_BAK;

	}
	else
	{
		return -1;
	}
	gtloginfo("配置文件改变更新到备份:%s->%s\n",filename,bakfile);
	sprintf(tbuf,"cp -f %s %s",filename,bakfile);
	ret=system(tbuf);	
	
	return 0;
}




