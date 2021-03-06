#include "ipmain.h"
#include "ipmain_para.h"
#include <gate_cmd.h>
#include "netcmdproc.h"
#include "math.h"
#include "watch_board.h"
#include "devstat.h"
#include "gtvs_io_api.h"
#include "common/commonlib.h"

#define TRIG_ACT_RECORD	1	//执行触发事件
#define TRIG_ACT_ALARM		2	//执行报警事件
#define TRIG_SWITCH_VIDEO   3   //切换网络视频端口

/**********************************************************************************************
 * 函数名	:is_alarm_interval()
 * 功能	:  决定当前时间是否在指定类型和编号的端子/移动侦测报警时间段内
 * 输入	:	type: 1表示端子触发，2表示移动侦测
 			number: 端子触发和移动侦测的编号
 * 返回值	:1表示在，0表示不在，负值表示错误
 **********************************************************************************************/
int is_alarm_interval(int type, int number)
{	
	time_t timep;
	struct tm *p;
	int timenow, timestart, timestop;
	struct alarm_trigin_struct *trigin;
	struct motion_struct *motion;
	int starthour,startmin,stophour,stopmin;

	
	switch(type)
	{
		case(1): //端子
				 	if((number >= get_trigin_num())||(number < 0))
				 	return 0;
				 	trigin = &(get_mainpara()->alarm_motion.trigin[number]);
				 	starthour	= trigin->starthour;
				 	startmin 	= trigin->startmin;
				 	stophour 	= trigin->endhour;
				 	stopmin 	= trigin->endmin;
				 	break;
		default: 
					return -EINVAL;
	}	
	
	time(&timep);
	p=localtime(&timep);
	timestart=starthour*60+startmin;
	timestop=stophour*60+stopmin;
			
	//判断时间
	if(p!=NULL)
	{
		timenow=p->tm_hour*60+p->tm_min;
		if(timestart<timestop)
		{	
			if((timenow>=timestart)&&(timenow<=timestop))//在报警时间段内，报警
				 return 1;		
		}
    	if(timestart>timestop)
		{	
			//gtloginfo("start %d now %d stop %d\n",timestart,timenow,timestop);
			if((0<=timenow)&&(1440>=timenow)&&((timenow<=timestop)||(timenow>=timestart)))	//在报警时间段内报警
				return 1;
		}
		if(timestart==timestop)
		{
			return 1; //全天报警
		}
	}
	
	return 0;
}


/**********************************************************************************************
 * 函数名	:get_alarm_event()
 * 功能	:获取报警联动事件
 * 输入	:ch:触发通道号0~(TOTAL_TRIG_IN-1)表示端子触发　TOTAL_TRIG_IN~(TOTAL_TRIG_IN+3)表示移动触发
 *			 type:联动事件类型
 *				 0:立即执行事件　1:确认执行事件　2:复位执行事件
 * 返回值	:参数描述的联动事件编号
 **********************************************************************************************/
int get_alarm_event(int ch, int type, int event) 
	{
	struct ipmain_para_struct * para;
	struct alarm_motion_struct *alarm_motion;
	struct alarm_trigin_struct *trigin;
	if((ch>=(get_trigin_num())+(get_video_num()))||((event+1)>MAX_TRIG_EVENT)||(type>2))
		//ch为0-9,event为0-4,type为0-2,分别对应im,ack,rst
		return 0;
	para=get_mainpara();
	alarm_motion=&para->alarm_motion;
	if(ch<get_trigin_num())//触发端子
		trigin=&alarm_motion->trigin[ch];
	switch(type)
		{
			case(0): printf("event%d-%d\t",event,trigin->imact[event]);
					 return trigin->imact[event];break;
			case(1): return trigin->ackact[event];break;
			case(2): printf("event%d-%d\t",event,trigin->rstact[event]);
					 return trigin->rstact[event];break;
			default: printf("no result\n");	 
					return 0; break;
		}
	return 0;
}

#define APLAY_TEMP "/tmp/grepresult"
int excutegrepcmd(const char *cmd)
{
	int found;
	system(cmd);
	found = get_file_lines(APLAY_TEMP);
	return found;
}

/**********************************************************************************************
 * 函数名	:take_alarm_action()
 * 功能	:按照事件代码执行具体的动作
 * 输入	:event:联动事件编号
 * 返回值	:０表示成功，负值表示出错
 **********************************************************************************************/
int take_alarm_action(int alarm_chn,int event) 
{
	//lc do 执行报警联动动作
	
	int out,value,vch;
	struct ipmain_para_struct *para;
	struct alarm_motion_struct *alarm_motion;

	
	para=get_mainpara();
	alarm_motion=&para->alarm_motion;

	if(event == 0)
		return 0;
	
	
	if((event<9)&&(event>0))	//输出端子
	{

		
		out=(event-1)/2; //输出得端口号
		value= event%2; //输出值
		//lc do
		set_relay_output(out,value);
		printf("报警联动事件端子%d输出%d\n",out,value);
		gtloginfo("报警联动事件端子%d输出%d\n",out,value);
		return 0;
	}
	if(event==40)	//声音提示
	{
	    gtloginfo("音频文件输出！\n");
	}
	
	
	if((event<=58)&&(event>50)) //对同一端子连续输出两个相反动作，用于控制
	{
		out=(event-51)/2; //输出得端口号
		value= event%2; //最终输出值
		//lc do
		set_relay_output(out,value);
		usleep(500000); //间隔500毫秒
		set_relay_output(out,1-value);
		//set_alarm_state_bit(1-value,out);
		usleep(500000); //间隔500毫秒
		set_relay_output(out,value);
		//set_alarm_state_bit(value,out);
		printf("报警联动事件,端子%d输出%d->%d->%d\n",out,value,1-value,value);
		gtloginfo("报警联动事件,端子%d输出%d->%d->%d\n",out,value,1-value,value);
		return 0;
	}
	gtloginfo("未定义的报警联动事件:%d\n",event);
	
	return 0; //目前保留,0无效，9-29无效,41-50, 59-正无穷无效
		
}

/**********************************************************************************************
 * 函数名	:process_trigin_event()
 * 功能	:处理外部端子触发报警联动事件
 * 输入	:trig:按位表示的端子触发状态，１表示有触发０表示没有
 *		 oldtrig: 上次的触发状态，用于判断是否是新增触发
 *		 timep: 发生触发的时间,time_t变量
 * 返回值	:０表示成功，负值表示出错，应该总是返回０
 **********************************************************************************************/
int process_trigin_event(DWORD trig, DWORD oldtrig, time_t timep)
{
	int i,j,k,ret;
	int event=0;
	WORD reclen;
	char alarminfo[200];
	char takepicbuf[60];
	char remove[200];
	int alarmflag=0;
 	DWORD actual_diff =0 ; //记录ip1004state->trigstate的变化
	DWORD actual_trig = 0; //记录应该被记到ip1004state->trigstate里的端子触发情况
	DWORD newbit = 0;
	int oldtrigstate = 0; //记录此前ip1004state->trigstate的端子触发情况
	int alarmcount = 0;
	int video_ch = -1;
	DWORD temp_trig=0;
	DWORD temp_oldtrig=0;

	struct ip1004_state_struct *ip1004state;
	struct alarm_motion_struct *alarm_motion;
	struct alarm_trigin_struct *trigin;
	struct ipmain_para_struct *para;
	struct send_dev_trig_state_struct dev_trig;
	struct timeval *timeval;

	para=get_mainpara();
	alarm_motion=&para->alarm_motion;
	ip1004state=get_ip1004_state(0);

	for(i=0;i<get_trigin_num();++i)
	{
		if(i<10)
		{
			temp_trig |= (trig&(0x01<<i));
			temp_oldtrig |= (oldtrig&(0x01<<i));
		}
		else
		{
			temp_trig |= (trig&(0x01<<i))<<16;
			temp_oldtrig |= (oldtrig&(0x01<<i))<<16;
		}
	}
			
	memcpy(&oldtrigstate,&ip1004state->reg_trig_state,sizeof(int));

	for(i=0;i<get_trigin_num();i++)
	{
		if(((trig>>i)&1) == 0) //该路无触发
		{
			continue;	
		}
		//有触发，判断是否有效
		
		//trigin=&alarm_motion->trigin[i];
		//printf("该路有触发[%d],%d,%s\n",i,__LINE__,__FILE__);
		
		/*
		if(trigin->setalarm!=1)
		{
			if(((oldtrig>>i)&1) == 0) //刚刚发生变化
			{
				printf("第%d路外部触发撤防，不报警\n",i);
				gtloginfo("第%d路外部触发撤防，不报警\n",i);//wsy,有效但不报警,录像
			}
			continue;
		}
		//wsy add,判断时间段
		alarmflag = is_alarm_interval(1,i);
		if(alarmflag!=1)//时间段不合
		{
			if(((oldtrig>>i)&1) == 0) //刚刚发生变化
			{
				printf("第%d路触发时间不在%02d:%02d-%02d:%02d内，故不报警\n",i,trigin->starthour,trigin->startmin,trigin->endhour,trigin->endmin);
				gtloginfo("第%d路触发时间不在%02d:%02d-%02d:%02d内，故不报警\n",i,trigin->starthour,trigin->startmin,trigin->endhour,trigin->endmin);
			}
			continue;
		}
		*/
		
		//有效触发
		k=(i<10)?i:i+16;
		actual_trig |= 1<<k;
		//看是否是新增的触发
		if(((oldtrigstate>>k)&1) == 1) //以前就有了
		{
			continue;
		}	
		gtloginfo("第%d路外部触发有效报警\n",i);
		printf("第%d路外部触发有效报警\n",i);
		
		//处理立即处理的,wsy moved here
		/*
		for(j=0;j<MAX_TRIG_EVENT;j++)
		{
			event=get_alarm_event(i,0,j);
			take_alarm_action(i,event);
		}
		*/
	}
	
	//写到ip1004state中去
	actual_diff = oldtrigstate ^ actual_trig;
	
	if(actual_diff != 0 )
	{
		pthread_mutex_lock(&ip1004state->mutex);
		printf("cp to new reg_trig_state\n");
		memcpy(&ip1004state->reg_trig_state, &actual_trig, sizeof(DWORD));
		pthread_mutex_unlock(&ip1004state->mutex);
	}

	if((actual_trig !=0)&&(actual_diff != 0))
	{//有至少一个新增端子需要报警,记报警日志，抓图,录像,加锁等
		printf("add new alarm state !\n");
		
		get_dev_trig(0,&dev_trig);
		send_dev_trig_state(-2,&dev_trig,1,0,0,0);
		//sprintf(alarminfo,"[ALARM] TRIG:0x%04x\n",(int)dev_trig.alarmstate);
	}	

	return 0;
}



