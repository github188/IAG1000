#ifndef IME6410API_H
#define IME6410API_H

#ifndef _WIN32

#include <sys/ipc.h>
//#include <ime6410.h>
#include <sys/time.h>
#else
#include <windows.h>
#endif //_WIN32

#include <pthread.h>

#define MEDIA_VIDEO		0x01		//��Ƶ����
#define MEDIA_AUDIO		0x02		//��Ƶ����

#define IDX1_VID  		0x63643030	//AVI����Ƶ�����
#define IDX1_AID  		0x62773130	//AVI����Ƶ���ı��

struct NCHUNK_HDR {	//avi��ʽ�����ݿ�ͷ��־�ṹ
	unsigned long  chk_id;
	unsigned long  chk_siz;
};



struct stream_fmt_struct
{							//����֡�Ľṹ
	struct timeval tv;			//���ݲ���ʱ��ʱ���
	unsigned long	channel;	//ѹ��ͨ���������6410�����һ��ͨ����6400��4��
	unsigned short media;		//media type ��Ƶ����Ƶ
	unsigned short  type;		//frame type	I/P/����...
	 long len;				//frame len �������Ƶ����Ƶ�����ݵĳ���
	struct NCHUNK_HDR chunk;//���ݿ�ͷ��־��Ŀǰʹ��avi��ʽ
	char data[4];				//frame data changed by shixin 060327
};

#endif


