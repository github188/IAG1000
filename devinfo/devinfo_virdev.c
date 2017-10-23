/*
	���������豸(.virdev > 1���豸)���豸��Ϣ����
							Feb 2009
*/

#ifndef DEVINFO_VIRDEV_H
#define DEVINFO_VIRDEV_H

#include <devinfo.h>
#include <devinfo_virdev.h>
#include <guid.h>
static char virdev1_guid_str[64];

/**********************************************************************************************
 * ������	:virdev_get_virdev_number()
 * ����	:Ӧ�ó�����û���豸�ں��������豸����(Ӧ���Ѿ����ù���init_devinfo())
 * ����	: ��
 * ��� 	:
 * ����ֵ	:��ֵ��ʾ�����豸��������ֵ��ʾ����
  **********************************************************************************************/
int virdev_get_virdev_number(void)
{
	int devtype;

	devtype=get_devtype();

	if((devtype==T_GTVS3022)||(devtype==T_GTMV3122))
		return 2;
	else
		return 1;
}

/**********************************************************************************************
 * ������	:virdev_get_devid()
 * ����	:Ӧ�ó�����û�������豸��dev_id(Ӧ���Ѿ����ù���init_devinfo())
 * ����	: virdev_no,�����豸��(��0��virdev_get_virdev_number()-1)
 * ��� 	:buf:Ӧ�ó�����Ҫ���devid�Ļ�������ַ,����ʱ���buf�ĳ��ȱ����㹻������DEV_GUID_BYTE
 * ����ֵ	:��ֵ��ʾ��䵽buf�е���Ч�ֽ�������ֵ��ʾ����
  **********************************************************************************************/
int virdev_get_devid(int virdev_no,unsigned char *buf)
{
	int ret;
	int devtype;

	ret=get_devid(buf);
	if(ret<=0)
		return ret;
	
	devtype=get_devtype();
	if((devtype==T_GTVS3022)||(devtype==T_GTMV3122))
	{
		if(virdev_no!=0)
		{
			buf[ret-1]+=0x01;	//���λ���ֽ�+1
		}
	}
	return ret;
}


/**********************************************************************************************
 * ������	:virdev_get_devid_str()
 * ����	:Ӧ�ó�����û�������豸��guid���ַ���
 * ����	:virdev_no,�����豸��(��0��virdev_get_virdev_number()-1)
 * ����ֵ	:ָ������guid��Ϣ���ַ���ָ��
  **********************************************************************************************/
char* virdev_get_devid_str(int virdev_no)
{
	int ret;
	char idbuf[32];
	struct GT_GUID	*pid=(struct GT_GUID*)idbuf;
	char *pguid=get_devid_str();
	int devtype;
	
	
	devtype=get_devtype();
	if((devtype==T_GTVS3022)||(devtype==T_GTMV3122))
	{
		if(virdev_no!=0)
		{
			ret=virdev_get_devid(virdev_no,idbuf);
			guid2hex(*pid,virdev1_guid_str);	
			pguid=virdev1_guid_str;
		}
		
	}
	return pguid;
}



/**********************************************************************************************
 * ������	:virdev_get_total_com()
 * ����	:��ȡָ�������豸�Ĵ�����
 * ����	:virdev_no,�����豸��(��0��virdev_get_virdev_number()-1)
 * ����ֵ	:��ֵ��ʾ�豸���ܴ�����,��ֵ��ʾ����
 **********************************************************************************************/
int virdev_get_total_com(int virdev_no)
{
	int devtype;
	
	devtype=get_devtype();
	if((devtype==T_GTVS3022)||(devtype==T_GTMV3122))
	{
		return 1;
	}
	else
		return get_total_com();
}


/**********************************************************************************************
 * ������	:virdev_get_video_num()
 * ����	:��ȡָ�������豸�ɽ������Ƶ����
 * ����	:virdev_no,�����豸��(��0��virdev_get_virdev_number()-1)
 * ����ֵ	:�ɽ������Ƶ��
 **********************************************************************************************/
int virdev_get_video_num(int virdev_no)
{
	int devtype;
	
	devtype=get_devtype();
	if((devtype==T_GTVS3022)||(devtype==T_GTMV3122))
	{
		return 1;
	}
	else
		return get_video_num();
	
}
/**********************************************************************************************
 * ������	:virdev_get_videoenc_num()
 * ����	:��ȡָ�������豸��Ƶ��������
 * ����	:virdev_no,�����豸��(��0��virdev_get_virdev_number()-1)
 * ����ֵ	:��Ƶ��������
 **********************************************************************************************/
int virdev_get_videoenc_num(int virdev_no)
{
	int devtype;

	devtype=get_devtype();
	if((devtype==T_GTVS3022)||(devtype==T_GTMV3122))
	{
		return 1;
	}
	else
		return get_videoenc_num();
}


/**********************************************************************************************
 * ������	:virdev_get_trigin_num()
 * ����	:��ȡָ�������豸���������
 * ����	:virdev_no,�����豸��(��0��virdev_get_virdev_number()-1)
 * ����ֵ	:ָ�������豸�ϵ������������
 **********************************************************************************************/
int	virdev_get_trigin_num(int virdev_no)
{
	int devtype;
	
	devtype=get_devtype();
	if((devtype==T_GTVS3022)||(devtype==T_GTMV3122))
	{
		if(virdev_no!=0)
			return 0;
	}
	return get_trigin_num();	
}
/**********************************************************************************************
 * ������	:virdev_get_alarmout_num()
 * ����	:��ȡָ�������豸���������
 * ����	:virdev_no,�����豸��(��0��virdev_get_virdev_number()-1)
 * ����ֵ	:ָ�������豸�ϵ����������
 **********************************************************************************************/
int virdev_get_alarmout_num(int virdev_no)
{
	int devtype;
	
	devtype=get_devtype();
	
	if((devtype==T_GTVS3022)||(devtype==T_GTMV3122))
        {
                if(virdev_no!=0)
                        return 0;
        }
	return get_alarmout_num();
}



/**********************************************************************************************
 * ������	:virdev_get_audio_num()
 * ����	:��ȡָ�������豸��Ƶͨ������
 * ����	:virdev_no,�����豸��(��0��virdev_get_virdev_number()-1)
 * ���� :ָ�������豸��Ƶͨ������
 **********************************************************************************************/
int virdev_get_audio_num(int virdev_no)
{
	int devtype;
	
	devtype=get_devtype();
	if((devtype==T_GTVS3022)||(devtype==T_GTMV3122))
        {
                if(virdev_no!=0)
                        return 0;
        }
	return get_audio_num();
}



#endif




