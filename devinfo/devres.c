#include <stdio.h>
#include <errno.h>
#include <devinfo.h>
#include "devtype.h"
//�豸���õ��Ĺ����ڴ�key����
#define         VIDEO_ENC0_KEY          0x30000          //������0�Ĺ������key
#define         VIDEO_ENC1_KEY          0x30001          //������1�Ĺ������key
#define         VIDEO_ENC2_KEY          0x30002          //������2�Ĺ������key
#define         VIDEO_ENC3_KEY          0x30003          //������3�Ĺ������key
#define         VIDEO_ENC4_KEY          0x30004          //������4�Ĺ������key
#define         VIDEO_ENC5_KEY          0x30005          //������5�Ĺ������key

#define         AUDIO_ENC0_KEY          0x50000        //��Ƶ������0��������ݹ������key
#define         AUDIO_ENC1_KEY          0x50001	    //��Ƶ������1��������ݹ������key
#define         AUDIO_ENC2_KEY		    0x50002	    //��Ƶ������2��������ݹ������key
#define         AUDIO_ENC3_KEY          0x50003	    //��Ƶ������3��������ݹ������key
#define         AUDIO_DEC0_KEY          0x51000          //��Ƶ������0�����빲�����key
#define         AUDIO_DEC1_KEY          0x51001          //��Ƶ������0�����빲�����key
#define         AUDIO_DEC2_KEY          0x51002          //��Ƶ������0�����빲�����key
#define         AUDIO_DEC3_KEY          0x51003          //��Ƶ������0�����빲�����key




#define		HQ_ENC_QUEUE_KEY	0x60000		//������¼��ͨ��0ʹ�õ���Ϣ���е�key
//#define		HQ_ENC_QUEUE_KEY1	0x61000		//������¼��ͨ��1ʹ�õ���Ϣ���е�key
//#define		HQ_ENC_QUEUE_KEY2	0x62000		//������¼��ͨ��2ʹ�õ���Ϣ���е�key
//#define		HQ_ENC_QUEUE_KEY3	0x63000		//������¼��ͨ��3ʹ�õ���Ϣ���е�key
//#define		HQ_ENC_QUEUE_KEY4	0x64000		//������¼��ͨ��4ʹ�õ���Ϣ���е�key


#define		VIDEO_ENC0_BUF_SIZE	(1024*1024*2)	//������0ʹ�õĹ����ڴ��С
#define		VIDEO_ENC1_BUF_SIZE	(1024*1024*2)	//������1ʹ�õĹ����ڴ��С
#define		VIDEO_ENC2_BUF_SIZE	(1024*1024*2)	//������0ʹ�õĹ����ڴ��С
#define		VIDEO_ENC3_BUF_SIZE	(1024*1024*2)	//������0ʹ�õĹ����ڴ��С
#define		VIDEO_ENC4_BUF_SIZE	(1024*1024*2)	//������0ʹ�õĹ����ڴ��С



#define		VIDEO_ENC0_INI_SEC	"netencoder"
#define		VIDEO_ENC1_INI_SEC	"hqenc0"
#define		VIDEO_ENC2_INI_SEC	"hqenc1"
#define		VIDEO_ENC3_INI_SEC	"hqenc2"
#define		VIDEO_ENC4_INI_SEC	"hqenc3"



//����״̬�����ڴ涨��
#define         VSMAIN_STAT_KEY         	0x40000          //���vsmain���в�����״̬�Ĺ����ڴ�key
#define         TCPRTIMG_STAT_KEY       	0x41000          //���tcprtimg2�Ĺ���״̬�Ĺ����ڴ�key
extern	GTSeriesDVSR    *get_current_dvsr(void);



//sub_type:���豸����
//no:���豸�����
static char *get_sub_dev_node(int sub_type,int no)
{
	GTSeriesDVSR	*dvsr=get_current_dvsr();	
	int				total=dvsr->list_num;
	int				i;
	DevType_T		**list=dvsr->list;
	DevType_T		*dev=NULL;
	for(i=0;i<total;i++)
	{
		dev=list[i];
		if(dev->type==sub_type)
		{
			if(dev->no==no)
			{
				return dev->node;
			}
		}
		
	}
	return NULL;
		
}
//sub_type:���豸����
//no:���豸�����
static char *get_sub_dev_driver(int sub_type,int no)
{
	GTSeriesDVSR	*dvsr=get_current_dvsr();	
	int				total=dvsr->list_num;
	int				i;
	DevType_T		**list=dvsr->list;
	DevType_T		*dev=NULL;
	for(i=0;i<total;i++)
	{
		dev=list[i];
		if(dev->type==sub_type)
		{
			if(dev->no==no)
			{
				return dev->driver;
			}
		}
	}
	return NULL;
		
}

//sub_type:���豸����
//no:���豸�����
static char *get_sub_dev_name(int sub_type,int no)
{
	GTSeriesDVSR	*dvsr=get_current_dvsr();	
	int				total=dvsr->list_num;
	int				i;
	DevType_T		**list=dvsr->list;
	DevType_T		*dev=NULL;
	for(i=0;i<total;i++)
	{
		dev=list[i];
		if(dev->type==sub_type)
		{
			if(dev->no==no)
			{
				return dev->name;
			}
		}
	}
	return NULL;
		
}












/**********************************************************************************************^M
 * ������       :get_video_enc_key()^M
 * ���� 		:��ȡָ����ŵ���Ƶ������ʹ�õĻ���ص�key
 * ���� 		:no ��Ƶ��������
 * ����ֵ       	:�Ǹ�ֵ��ʾ��Ӧ��Ƶ��������key
 *				��ֵ��ʾ����
 *					-EINVAL��ʾ�����Ƿ�
 **********************************************************************************************/
int get_video_enc_key(int no)
{
	switch(no)
	{
		case 0:
			return VIDEO_ENC0_KEY;
		break;
		case 1:
			return VIDEO_ENC1_KEY;
		break;
		case 2:
			return VIDEO_ENC2_KEY;
		break;
		case 3:
			return VIDEO_ENC3_KEY;
		break;
		case 4:
			return VIDEO_ENC4_KEY;
		break;
		case 5:
			return VIDEO_ENC5_KEY;
		break;
		default:
			return -EINVAL;
		break;
	}
	return -EINVAL;
}

/**********************************************************************************************^M
 * ������       :get_audio_enc_key()^M
 * ���� 		:��ȡָ����ŵ���Ƶ����������ص�key
 * ���� 		:no ��Ƶ��������
 * ����ֵ       	:�Ǹ�ֵ��ʾ��Ӧ��Ƶ��������key
 *				��ֵ��ʾ����
 *					-EINVAL��ʾ�����Ƿ�
 **********************************************************************************************/
int get_audio_enc_key(int no)
{
	switch(no)
	{
		case 0:
			return AUDIO_ENC0_KEY;
		break;
		case 1:
			return AUDIO_ENC1_KEY;
		break;
		case 2:
			return AUDIO_ENC2_KEY;
		break;
		case 3:
			return AUDIO_ENC3_KEY;
		break;
		default:
			return -EINVAL;
		break;
	}
	return -EINVAL;

}

/**********************************************************************************************^M
 * ������       :get_audio_dec_key()^M
 * ���� 		:��ȡָ����ŵ���Ƶ����������ص�key
 * ���� 		:no ��Ƶ��������
 * ����ֵ       	:�Ǹ�ֵ��ʾ��Ӧ��Ƶ��������key
 *				��ֵ��ʾ����
 *					-EINVAL��ʾ�����Ƿ�
 **********************************************************************************************/
int get_audio_dec_key(int no)
{
	switch(no)
	{
		case 0:
			return AUDIO_DEC0_KEY;
		break;
		case 1:
			return AUDIO_DEC1_KEY;
		break;
		case 2:
			return AUDIO_DEC2_KEY;
		break;
		case 3:
			return AUDIO_DEC3_KEY;
		break;
		default:
			return -EINVAL;
		break;
	}
	return -EINVAL;
}


/**********************************************************************************************^M
 * ������       :get_total_hqenc_num()^M
 * ���� 		:	��ȡ¼��ͨ������
 * ���� 		:��^M
 * ����ֵ       	:��ֵ��ʾ¼��ͨ������M
 *				��ֵ��ʾ����
 **********************************************************************************************/
int get_total_hqenc_num(void)
{
	GTSeriesDVSR *dvsr=get_current_dvsr();
	return dvsr->hqencnum;
}

/**********************************************************************************************^M
 * ������       :get_hqenc_video_ch()^M
 * ���� 		:����·��ͨ���Ż�ȡ��Ӧ����Ƶ��������
 * ���� 		:no ��Ƶ��������
 * ����ֵ       	:�Ǹ�ֵ��ʾ��Ӧ��Ƶ��������key
 *				��ֵ��ʾ����
 *					-EINVAL��ʾ�����Ƿ�
 **********************************************************************************************/
int get_hqenc_video_ch(int hqch)
{
	int hqnum=get_total_hqenc_num();
	int total_enc=get_videoenc_num();
	if(hqch<0)
		return -EINVAL;
	if(hqch>=hqnum)
		return -EINVAL;
	if(hqnum<total_enc)
		return hqch+1;
	else
		return hqch;
}

//FIXME ��·����������ƵʱҪ�޸�
/**********************************************************************************************^M
 * ������       :get_net_video_ch()^M
 * ���� 		:��������ͼ��ͨ���Ż�ȡ��Ӧ����Ƶ��������
 * ���� 		:netch:������Ƶͨ����
 * ����ֵ       	:�Ǹ�ֵ��ʾ��Ӧ��Ƶ���������
 *				��ֵ��ʾ����
 *					-EINVAL��ʾ�����Ƿ�
 **********************************************************************************************/
int get_net_video_ch(int netch)
{	
	if(netch!=0)
		return -EINVAL;
	else 
		return 0;
}

/**********************************************************************************************^M
 * ������       :get_hqenc_video_key()
 * ���� 		:��ȡ������¼�����Ƶ�ɼ�key
 * ���� 		:ch:������·��ͨ����
 * ����ֵ       	:�Ǹ�ֵ��ʾ��Ӧ��Ƶ��������key
 *				��ֵ��ʾ����
 *					-EINVAL��ʾ�����Ƿ�
 **********************************************************************************************/
int get_hqenc_video_key(int ch)
{
	int hqnum=get_total_hqenc_num();
	int total_enc=get_videoenc_num();
	if((ch<0)||(ch>=hqnum))
		return -EINVAL;
	if(hqnum<total_enc)
		return get_video_enc_key(ch+1);
	else
		return get_video_enc_key(ch);
}

/**********************************************************************************************^M
 * ������       :get_hqenc_queue_key()
 * ���� 		:��ȡ������¼��ʹ�õ���Ϣ���е�key
 * ���� 		:ch:������·��ͨ����
 * ����ֵ       	:�Ǹ�ֵ��ʾ��Ӧ��Ƶ��������key
 *				��ֵ��ʾ����
 *					-EINVAL��ʾ�����Ƿ�
 **********************************************************************************************/
int get_hqenc_queue_key(int ch)
{
	int hqnum=get_total_hqenc_num();
	int total_enc=get_videoenc_num();
	if((ch<0)||(ch>=hqnum))
		return -EINVAL;
	
	switch (ch)
	{
		case 0:
			return HQ_ENC_QUEUE_KEY;
		break;
		case 1:
		case 2:
		case 3:
		case 4:
			return (HQ_ENC_QUEUE_KEY+ch*1000);
		break;
		default:
			return -EINVAL;
		break;
	}
	return -EINVAL;
}

/**********************************************************************************************^M
 * ������       :get_hqenc_audio_key()
 * ���� 		:��ȡ������¼�����Ƶ�ɼ�key
 * ���� 		:ch:������·��ͨ����
 * ����ֵ       	:�Ǹ�ֵ��ʾ��Ӧ��Ƶ��������key
 *				��ֵ��ʾ����
 *				-EINVAL��ʾ�����Ƿ�
 **********************************************************************************************/
int get_hqenc_audio_key(int ch)
{
	return get_audio_enc_key(0);	
}

/**********************************************************************************************^M
 * ������       :get_tcprtimg_stat_key()
 * ����         :��ȡ����Ƶ���з���״̬�����������Ĺ����ڴ�key
 * ����         :��
 * ����ֵ       :����Ƶ���з���״̬��key
 **********************************************************************************************/
int get_tcprtimg_stat_key(void)
{
	return TCPRTIMG_STAT_KEY;
}







/**********************************************************************************************^M
 * ������       :get_video_enc_node()
 * ���� 		:��ȡ��Ƶ�������ڵ���
 * ���� 		:no��Ƶ���������
 * ����ֵ       	:��Ƶ�������豸�ڵ��ַ���
 *				NULL��ʾʧ��(������û�г�ʼ��devinfo()�����߲�������)
 **********************************************************************************************/
char *get_video_enc_node(int no)
{
	return get_sub_dev_node(SUB_DEV_VENC,no);
}


/**********************************************************************************************^M
 * ������       :get_video_enc_driver()
 * ���� 		:��ȡ��Ƶ����������·��
 * ���� 		:no��Ƶ���������
 * ����ֵ       	:��Ƶ��������������·��
 *				NULL��ʾʧ��(������û�г�ʼ��devinfo()�����߲�������)
 **********************************************************************************************/
char *get_video_enc_driver(int no)
{
	return get_sub_dev_driver(SUB_DEV_VENC,no);
}

/**********************************************************************************************^M
 * ������       :get_video_enc_name()
 * ���� 		:��ȡ��Ƶ���������豸����(ж��ʱҪ��)
 * ���� 		:no��Ƶ���������
 * ����ֵ       	:��Ƶ��������������·��
 *				NULL��ʾʧ��(������û�г�ʼ��devinfo()�����߲�������)
 **********************************************************************************************/
char *get_video_enc_name(int no)
{
	return get_sub_dev_name(SUB_DEV_VENC,no);
}

/**********************************************************************************************^M
 * ������       :get_video_enc_ini_sec()
 * ���� 		:��ȡ��Ƶ��������ini�����ļ��д�Ų����Ľ���
 * ���� 		:no��Ƶ���������
 * ����ֵ       	:��Ӧ����Ƶ�����������ļ������ַ���
 *				NULL��ʾʧ��(������û�г�ʼ��devinfo()�����߲�������)
 **********************************************************************************************/
char *get_video_enc_ini_sec(int no)
{
	switch(no)
	{
		case 0:
			return VIDEO_ENC0_INI_SEC;
		break;
		case 1:
			return VIDEO_ENC1_INI_SEC;
		break;
		case 2:
			return VIDEO_ENC2_INI_SEC;
		break;
		case 3:
			return VIDEO_ENC3_INI_SEC;
		break;
		case 4:
			return VIDEO_ENC4_INI_SEC;
		break;
		default:
			return NULL;
		break;
	}
	return NULL;
}



