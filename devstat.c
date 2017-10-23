#include "ipmain.h"
#include "devstat.h"
#include "ipmain_para.h"
#include "devinfo_virdev.h"

static struct ip1004_state_struct  ip1004_state[1];//�豸��״̬
/**********************************************************************************************
 * ������	:init_devstat()
 * ����	:��ʼ���豸״̬�ṹ
 * ����	:��
 * ����ֵ	:��
 **********************************************************************************************/
void init_devstat(void)
{
	int i;

	memset((void*)&ip1004_state[0],0,sizeof(struct ip1004_state_struct));
	pthread_mutex_init(&ip1004_state[0].mutex, NULL);//ʹ��ȱʡ����
	ip1004_state[0].reg_dev_state.link_err=1;
	ip1004_state[0].regist_timer=REGIST_TIMEOUT-5;

}

/**********************************************************************************************
 * ������	:get_ip1004_state()
 * ����	:��ȡ�豸��״̬��Ϣ�ṹָ��
 * ����	:��
 * ����ֵ	:�����豸״̬�Ľṹ��ָ��
 **********************************************************************************************/
struct ip1004_state_struct * get_ip1004_state(int dev_no)
{
	return &ip1004_state[dev_no];
}

/**********************************************************************************************
 * ������	:get_gate_connect_flag()
 * ����	:��ȡ��������ͨ�ı�־
 * ����	:��
 * ����ֵ	:����ʾ�Ѿ���������ͨ
 *		��   ����ʾ��û����������ͨ
 **********************************************************************************************/
int get_gate_connect_flag(int dev_no)
{
	struct ip1004_state_struct *stat;
	stat=get_ip1004_state(dev_no);
	return stat->gate_connect_flag;
}
/**********************************************************************************************
 * ������	:set_gate_connect_flag()
 * ����	:�����Ѿ���������ͨ��־
 * ����	:flag:Ҫ���õı�־��ֵ��1��ʾ�Ѿ���������ͨ
 * ����ֵ	:��
 **********************************************************************************************/
void set_gate_connect_flag(int dev_no, int flag)
{//������ͨ���ر�־(��ͨ���ز�����ע��ɹ�!!)
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
 * ������	:set_regist_flag()
 * ����	:����ע��ɹ���־ 1��ʾ�ɹ�
 * ����	:flag:Ҫ���õ�ע���־��ֵ 1��ʾ�ɹ�
 * ����ֵ	:��
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
 * ������	:set_reportstate_flag()
 * ����	:���ñ���״̬�ɹ���־ 1��ʾ�ɹ�
 * ����	:flag:Ҫ���õı���״̬��־��ֵ 1��ʾ�ɹ�
 * ����ֵ	:��
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
 * ������	:set_alarm_flag()
 * ����	:���ñ����ɹ���־ 1��ʾ�ɹ�
 * ����	:flag:Ҫ���õı�����־��ֵ 1��ʾ�ɹ�
 * ����ֵ	:��
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
 * ������	:set_trigin_flag()
 * ����	:���ñ���������״̬�ɹ���־ 1��ʾ�ɹ�
 * ����	:flag:Ҫ���õı���������״̬��ֵ 1��ʾ�ɹ�
 * ����ֵ	:��
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
 * ������	:get_regist_flag()
 * ����	:��ȡע��ɹ���־ 
 * ����	:��
 * ����ֵ	:����ʾע��ɹ�����ʾû��ע��ɹ�
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
 * ������	:get_reportstate_flag()
 * ����	:��ȡ����״̬�ɹ���־ 
 * ����	:��
 * ����ֵ	:����ʾ����״̬�ɹ�������ʾû�гɹ�
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
 * ������	:get_alarm_flag()
 * ����	:��ȡ�����ɹ���־ 
 * ����	:��
 * ����ֵ	:����ʾ�����ɹ�������ʾû�гɹ�
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
 * ������	:get_trigin_flag()
 * ����	:��ȡ��������ӱ仯�ɹ���־ 
 * ����	:��
 * ����ֵ	:����ʾ��������ӱ仯�ɹ�������ʾû�гɹ�
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
 * ������	:set_alarm_out_stat()
 * ����	:���������״̬���õ��豸״̬�ṹ��
 * ����	:stat:���ӵ����״̬
 * ����ֵ	:��
 **********************************************************************************************/
void set_alarm_out_stat(int dev_no,int stat)
{
	ip1004_state[dev_no].alarm_out_stat=stat;
}








