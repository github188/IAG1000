#include "ipmain.h"
#include "ipmain_para.h"
#include <gate_cmd.h>
#include "netcmdproc.h"
#include "math.h"
#include "watch_board.h"
#include "devstat.h"
#include "gtvs_io_api.h"

#define TRIG_ACT_RECORD	1	//ִ�д����¼�
#define TRIG_ACT_ALARM		2	//ִ�б����¼�
#define TRIG_SWITCH_VIDEO   3   //�л�������Ƶ�˿�

/**********************************************************************************************
 * ������	:is_alarm_interval()
 * ����	:  ������ǰʱ���Ƿ���ָ�����ͺͱ�ŵĶ���/�ƶ���ⱨ��ʱ�����
 * ����	:	type: 1��ʾ���Ӵ�����2��ʾ�ƶ����
 			number: ���Ӵ������ƶ����ı��
 * ����ֵ	:1��ʾ�ڣ�0��ʾ���ڣ���ֵ��ʾ����
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
		case(1): //����
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
			
	//�ж�ʱ��
	if(p!=NULL)
	{
		timenow=p->tm_hour*60+p->tm_min;
		if(timestart<timestop)
		{	
			if((timenow>=timestart)&&(timenow<=timestop))//�ڱ���ʱ����ڣ�����
				 return 1;		
		}
    	if(timestart>timestop)
		{	
			//gtloginfo("start %d now %d stop %d\n",timestart,timenow,timestop);
			if((0<=timenow)&&(1440>=timenow)&&((timenow<=timestop)||(timenow>=timestart)))	//�ڱ���ʱ����ڱ���
				return 1;
		}
		if(timestart==timestop)
		{
			return 1; //ȫ�챨��
		}
	}
	
	return 0;
}


/**********************************************************************************************
 * ������	:get_alarm_event()
 * ����	:��ȡ���������¼�
 * ����	:ch:����ͨ����0~(TOTAL_TRIG_IN-1)��ʾ���Ӵ�����TOTAL_TRIG_IN~(TOTAL_TRIG_IN+3)��ʾ�ƶ�����
 *			 type:�����¼�����
 *				 0:����ִ���¼���1:ȷ��ִ���¼���2:��λִ���¼�
 * ����ֵ	:���������������¼����
 **********************************************************************************************/
int get_alarm_event(int ch, int type, int event) 
	{
	struct ipmain_para_struct * para;
	struct alarm_motion_struct *alarm_motion;
	struct alarm_trigin_struct *trigin;
	if((ch>=(get_trigin_num())+(get_video_num()))||((event+1)>MAX_TRIG_EVENT)||(type>2))
		//chΪ0-9,eventΪ0-4,typeΪ0-2,�ֱ��Ӧim,ack,rst
		return 0;
	para=get_mainpara();
	alarm_motion=&para->alarm_motion;
	if(ch<get_trigin_num())//��������
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
 * ������	:take_alarm_action()
 * ����	:�����¼�����ִ�о���Ķ���
 * ����	:event:�����¼����
 * ����ֵ	:����ʾ�ɹ�����ֵ��ʾ����
 **********************************************************************************************/
int take_alarm_action(int alarm_chn,int event) 
{
	//lc do ִ�б�����������
	
	int out,value,vch;
	struct ipmain_para_struct *para;
	struct alarm_motion_struct *alarm_motion;

	
	para=get_mainpara();
	alarm_motion=&para->alarm_motion;

	if(event == 0)
		return 0;
	
	
	if((event<9)&&(event>0))	//�������
	{

		
		out=(event-1)/2; //����ö˿ں�
		value= event%2; //���ֵ
		//lc do
		set_relay_output(out,value);
		printf("���������¼�����%d���%d\n",out,value);
		gtloginfo("���������¼�����%d���%d\n",out,value);
		return 0;
	}
	if(event==40)	//������ʾ
	{
	    gtloginfo("��Ƶ�ļ������\n");
	}
	
	
	if((event<=58)&&(event>50)) //��ͬһ����������������෴���������ڿ���
	{
		out=(event-51)/2; //����ö˿ں�
		value= event%2; //�������ֵ
		//lc do
		set_relay_output(out,value);
		usleep(500000); //���500����
		set_relay_output(out,1-value);
		//set_alarm_state_bit(1-value,out);
		usleep(500000); //���500����
		set_relay_output(out,value);
		//set_alarm_state_bit(value,out);
		printf("���������¼�,����%d���%d->%d->%d\n",out,value,1-value,value);
		gtloginfo("���������¼�,����%d���%d->%d->%d\n",out,value,1-value,value);
		return 0;
	}
	gtloginfo("δ����ı��������¼�:%d\n",event);
	
	return 0; //Ŀǰ����,0��Ч��9-29��Ч,41-50, 59-��������Ч
		
}

/**********************************************************************************************
 * ������	:process_trigin_event()
 * ����	:�����ⲿ���Ӵ������������¼�
 * ����	:trig:��λ��ʾ�Ķ��Ӵ���״̬������ʾ�д�������ʾû��
 *		 oldtrig: �ϴεĴ���״̬�������ж��Ƿ�����������
 *		 timep: ����������ʱ��,time_t����
 * ����ֵ	:����ʾ�ɹ�����ֵ��ʾ����Ӧ�����Ƿ��أ�
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
 	DWORD actual_diff =0 ; //��¼ip1004state->trigstate�ı仯
	DWORD actual_trig = 0; //��¼Ӧ�ñ��ǵ�ip1004state->trigstate��Ķ��Ӵ������
	DWORD newbit = 0;
	int oldtrigstate = 0; //��¼��ǰip1004state->trigstate�Ķ��Ӵ������
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
		if(((trig>>i)&1) == 0) //��·�޴���
		{
			continue;	
		}
		//�д������ж��Ƿ���Ч
		
		//trigin=&alarm_motion->trigin[i];
		//printf("��·�д���[%d],%d,%s\n",i,__LINE__,__FILE__);
		
		/*
		if(trigin->setalarm!=1)
		{
			if(((oldtrig>>i)&1) == 0) //�ոշ����仯
			{
				printf("��%d·�ⲿ����������������\n",i);
				gtloginfo("��%d·�ⲿ����������������\n",i);//wsy,��Ч��������,¼��
			}
			continue;
		}
		//wsy add,�ж�ʱ���
		alarmflag = is_alarm_interval(1,i);
		if(alarmflag!=1)//ʱ��β���
		{
			if(((oldtrig>>i)&1) == 0) //�ոշ����仯
			{
				printf("��%d·����ʱ�䲻��%02d:%02d-%02d:%02d�ڣ��ʲ�����\n",i,trigin->starthour,trigin->startmin,trigin->endhour,trigin->endmin);
				gtloginfo("��%d·����ʱ�䲻��%02d:%02d-%02d:%02d�ڣ��ʲ�����\n",i,trigin->starthour,trigin->startmin,trigin->endhour,trigin->endmin);
			}
			continue;
		}
		*/
		
		//��Ч����
		k=(i<10)?i:i+16;
		actual_trig |= 1<<k;
		//���Ƿ��������Ĵ���
		if(((oldtrigstate>>k)&1) == 1) //��ǰ������
		{
			continue;
		}	
		gtloginfo("��%d·�ⲿ������Ч����\n",i);
		printf("��%d·�ⲿ������Ч����\n",i);
		
		//�������������,wsy moved here
		/*
		for(j=0;j<MAX_TRIG_EVENT;j++)
		{
			event=get_alarm_event(i,0,j);
			take_alarm_action(i,event);
		}
		*/
	}
	
	//д��ip1004state��ȥ
	actual_diff = oldtrigstate ^ actual_trig;
	
	if(actual_diff != 0 )
	{
		pthread_mutex_lock(&ip1004state->mutex);
		printf("cp to new reg_trig_state\n");
		memcpy(&ip1004state->reg_trig_state, &actual_trig, sizeof(DWORD));
		pthread_mutex_unlock(&ip1004state->mutex);
	}

	if((actual_trig !=0)&&(actual_diff != 0))
	{//������һ������������Ҫ����,�Ǳ�����־��ץͼ,¼��,������
		printf("add new alarm state !\n");
		
		get_dev_trig(0,&dev_trig);
		send_dev_trig_state(-2,&dev_trig,1,0,0,0);
		//sprintf(alarminfo,"[ALARM] TRIG:0x%04x\n",(int)dev_trig.alarmstate);
	}	

	return 0;
}



