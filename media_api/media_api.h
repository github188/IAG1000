#ifndef MEDIA_API_H
#define MEDIA_API_H
#include <mshmpool.h>
#ifndef IN
	#define IN
	#define OUT
	#define IO
#endif
//ý�����Ͷ���
#define		MEDIA_TYPE_VIDEO		0
#define		MEDIA_TYPE_AUDIO		1


///��Ƶ��������
#define         VIDEO_MPEG4                     0
#define         VIDEO_H264                        1
#define         VIDEO_MJPEG                      2
#define AAC_AUDIO_BUFLEN 4096

//ý��״̬����
#define ENC_NO_INIT		0		//������û�г�ʼ��
#define	ENC_STAT_OK		1		//��������������
#define	ENC_STAT_ERR		2		//����������	

typedef struct{	//��Ƶ��ʽ��Ϣ�ṹ
	int format;		//��Ƶ�����ʽ      VIDEO_MPEG4��
	int ispal;			//�Ƿ���pal����Ƶ
	int v_width;		//ͼ����
	int v_height;		//ͼ��߶�
	int v_frate;		//ͼ������
	int v_buffsize;		//���黺������С
}video_format_t;

typedef struct{	//��Ƶ��ʽ��Ϣ�ṹ
	int a_wformat;	//������ʽ
	int a_sampling;	//����������
	int a_channel;	//����ͨ��
	int a_nr_frame;	//һ�����������м�������
	int a_bitrate;		//��������
	int a_bits;		//��Ƶ����λ��
	int a_frate;		//��Ƶ���ݵ�֡��(û�����м�����Ƶ����)
}audio_format_t;

typedef union{ //ý���ʽ����������
	video_format_t v_fmt;
	audio_format_t a_fmt;
}media_format_t;

typedef struct{ //ý�����Խṹ
	int				media_type;		//ý������
	int				stat;			//״̬
	media_format_t	fmt;				//ý���ʽ
}media_attrib_t;


typedef struct{ //ý��Դ�ṹ
	pthread_mutex_t	mutex;
	int				media_type;				//ý������(MEDIA_TYPE_VIDEO��MEDIA_TYPE_AUDIO)
	int				no;						//ͬ����Դ���
	int				dev_stat;				//-1��ʾ��û�����ӵ�����ı����豸����� ,0��ʾ�Ѿ�������
	pthread_t			thread_id;				//�߳�id
	int				max_data_len;			//���豸�����ݿ���󳤶�(��̬ˢ��)
	MSHM_POOL		mpool;					//ý���õ�����Դ�������
	void *			*temp_buf;				//��ȡ�����õĻ�����(��DWORD����)
	int				buflen;					//����������,��ֵ��ʾ�ڴ����ʧ��
	media_attrib_t		*attrib;					//�豸����		
}media_source_t;


#endif
