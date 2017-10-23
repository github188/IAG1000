#include "ipmain.h"
#include "devstat.h"
#include "ipmain_para.h"
#include "devinfo_virdev.h"

static struct ip1004_state_struct  ip1004_state[1];//设备的状态
/**********************************************************************************************
 * 函数名	:init_devstat()
 * 功能	:初始化设备状态结构
 * 输入	:无
 * 返回值	:无
 **********************************************************************************************/
void init_devstat(void)
{
	int i;

	memset((void*)&ip1004_state[0],0,sizeof(struct ip1004_state_struct));
	pthread_mutex_init(&ip1004_state[0].mutex, NULL);//使用缺省设置
	ip1004_state[0].reg_dev_state.link_err=1;
	ip1004_state[0].regist_timer=REGIST_TIMEOUT-5;

}

/**********************************************************************************************
 * 函数名	:get_ip1004_state()
 * 功能	:获取设备的状态信息结构指针
 * 输入	:无
 * 返回值	:描述设备状态的结构的指针
 **********************************************************************************************/
struct ip1004_state_struct * get_ip1004_state(int dev_no)
{
	return &ip1004_state[dev_no];
}

/**********************************************************************************************
 * 函数名	:get_gate_connect_flag()
 * 功能	:获取与网关连通的标志
 * 输入	:无
 * 返回值	:１表示已经与网关连通
 *		　   ０表示还没有与网关连通
 **********************************************************************************************/
int get_gate_connect_flag(int dev_no)
{
	struct ip1004_state_struct *stat;
	stat=get_ip1004_state(dev_no);
	return stat->gate_connect_flag;
}
/**********************************************************************************************
 * 函数名	:set_gate_connect_flag()
 * 功能	:设置已经与网关连通标志
 * 输入	:flag:要设置的标志的值　1表示已经与网关连通
 * 返回值	:无
 **********************************************************************************************/
void set_gate_connect_flag(int dev_no, int flag)
{//设置连通网关标志(连通网关不等于注册成功!!)
	struct ip1004_state_struct *stat;
	stat=get_ip1004_state(dev_no);
	pthread_mutex_lock(&stat->mutex);
	if(flag==0)
	{
		stat->gate_connect_flag=0;		
	}
	else
		stat->gate_connect_flag=1;
	pthread_mutex_unlock(&stat->mutex);
}
/**********************************************************************************************
 * 函数名	:set_regist_flag()
 * 功能	:设置注册成功标志 1表示成功
 * 输入	:flag:要设置的注册标志的值 1表示成功
 * 返回值	:无
 **********************************************************************************************/
void set_regist_flag(int dev_no, int flag)
{
	struct ip1004_state_struct *stat;
	stat=get_ip1004_state(dev_no);
	pthread_mutex_lock(&stat->mutex);
	if(flag==0)
	{
		stat->regist_flag=0;		
	}
	else
	{
		stat->regist_flag=1;
	}
	stat->regist_timer=REGIST_TIMEOUT-5*(dev_no+1);
	pthread_mutex_unlock(&stat->mutex);
}

/**********************************************************************************************
 * 函数名	:set_reportstate_flag()
 * 功能	:设置报告状态成功标志 1表示成功
 * 输入	:flag:要设置的报告状态标志的值 1表示成功
 * 返回值	:无
 **********************************************************************************************/
void set_reportstate_flag(int dev_no, int flag)
{
	struct ip1004_state_struct *stat;	
	stat=get_ip1004_state(dev_no);
	pthread_mutex_lock(&stat->mutex);
	if(flag==0)
	{
		stat->reportstate_flag=0;		
	}
	else
	{
		stat->reportstate_flag=1;
		//stat->reportstate_timer=REPORTSTATE_TIMEOUT-5;
	}
	pthread_mutex_unlock(&stat->mutex);
}

/**********************************************************************************************
 * 函数名	:set_alarm_flag()
 * 功能	:设置报警成功标志 1表示成功
 * 输入	:flag:要设置的报警标志的值 1表示成功
 * 返回值	:无
 **********************************************************************************************/
void set_alarm_flag(int dev_no, int flag)
{
	struct ip1004_state_struct *stat;
	stat=get_ip1004_state(dev_no);
	pthread_mutex_lock(&stat->mutex);
	if(flag==0)
	{
		stat->alarm_flag=0;	
		
	}
	else
	{
		stat->alarm_flag=1;
		//stat->alarm_timer=ALARM_TIMEOUT-5;
	}
	printf("dev %d alarm_flag now %d\n", dev_no,stat->alarm_flag);
	pthread_mutex_unlock(&stat->mutex);
}


/**********************************************************************************************
 * 函数名	:set_trigin_flag()
 * 功能	:设置报端子输入状态成功标志 1表示成功
 * 输入	:flag:要设置的报端子输入状态的值 1表示成功
 * 返回值	:无
 **********************************************************************************************/
void set_trigin_flag(int dev_no,int flag)
{
	struct ip1004_state_struct *stat;
	stat=get_ip1004_state(dev_no);
	pthread_mutex_lock(&stat->mutex);
	if(flag==0)
	{
		stat->trigin_flag=0;		
	}
	else
		stat->trigin_flag=1;
	stat->trigin_timer=0;
	printf("dev %d trigin_flag now %d\n", dev_no,stat->trigin_flag);
	pthread_mutex_unlock(&stat->mutex);
}

/**********************************************************************************************
 * 函数名	:get_regist_flag()
 * 功能	:获取注册成功标志 
 * 输入	:无
 * 返回值	:１表示注册成功０表示没有注册成功
 **********************************************************************************************/
int get_regist_flag(int dev_no)
{
	struct ip1004_state_struct *stat;
	int i;
	stat=get_ip1004_state(dev_no);
	pthread_mutex_lock(&stat->mutex);
	i=stat->regist_flag;
	pthread_mutex_unlock(&stat->mutex);
	return i;
}
/**********************************************************************************************
 * 函数名	:get_reportstate_flag()
 * 功能	:获取报告状态成功标志 
 * 输入	:无
 * 返回值	:１表示报告状态成功　０表示没有成功
 **********************************************************************************************/
int get_reportstate_flag(int dev_no)
{
	struct ip1004_state_struct *stat;
	int i;
	stat=get_ip1004_state(dev_no);
	pthread_mutex_lock(&stat->mutex);
	i=stat->reportstate_flag;
	pthread_mutex_unlock(&stat->mutex);
	return i;
}	
/**********************************************************************************************
 * 函数名	:get_alarm_flag()
 * 功能	:获取报警成功标志 
 * 输入	:无
 * 返回值	:１表示报警成功　０表示没有成功
 **********************************************************************************************/
int get_alarm_flag(int dev_no)
{
	struct ip1004_state_struct *stat;
	int i;
	stat=get_ip1004_state(dev_no);
	pthread_mutex_lock(&stat->mutex);
	i=stat->alarm_flag;
	pthread_mutex_unlock(&stat->mutex);
	return i;
}
/**********************************************************************************************
 * 函数名	:get_trigin_flag()
 * 功能	:获取报输入端子变化成功标志 
 * 输入	:无
 * 返回值	:１表示报输入端子变化成功　０表示没有成功
 **********************************************************************************************/
int get_trigin_flag(int dev_no)
{
	struct ip1004_state_struct *stat;
	int i;
	stat=get_ip1004_state(dev_no);
	pthread_mutex_lock(&stat->mutex);
	i=stat->trigin_flag;
	pthread_mutex_unlock(&stat->mutex);
	return i;
}

/**********************************************************************************************
 * 函数名	:set_alarm_out_stat()
 * 功能	:将端子输出状态设置到设备状态结构中
 * 输入	:stat:端子的输出状态
 * 返回值	:无
 **********************************************************************************************/
void set_alarm_out_stat(int dev_no,int stat)
{
	ip1004_state[dev_no].alarm_out_stat=stat;
}








