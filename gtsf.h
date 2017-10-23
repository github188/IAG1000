#ifndef GTSF_H
#define GTSF_H
#include <sys/time.h>
#ifdef __cplusplus
extern "C" {
#endif


#define GTSF_MARK            0XBFCF0267
#define GTSF_HEAD_SIZE            28



#define MEDIA_VIDEO		  0x01	//��Ƶ����
#define MEDIA_AUDIO	    0x02	//��Ƶ����
#define MEDIA_SPEECH		0x03	//�Խ�����
#define MEDIA_COMMAND		0x04	//����ڷ���������
#define MEDIA_STREAMID	0x05	//��Ʊ,��TCP�������������������ӷ������Ӻ��͸�������


#define FRAMETYPE_I		0x0		// IME6410 Header - I Frame
#define FRAMETYPE_P		0x1		// IME6410 Header - P Frame
#define FRAMETYPE_B		0x2
#define FRAMETYPE_PCM	0x5		// IME6410 Header - Audio Frame
#define FRAMETYPE_ADPCM	0x6		// IME6410 Header - Audio Frame
#define FRAMETYPE_AAC	0x7		// frame flag - Audio Frame
#define FRAMETYPE_MD	0x8

#define RATIO_D1_PAL       0x0      //720*576
#define RATIO_D1_NTSC      0x1      //704*576
#define RATIO_CIF_PAL      0x2      // 352*288
#define RATIO_CIF_NTSC     0x3      // 320*240


//��Ƶ��������
#define         VIDEO_MPEG4                     0
#define         VIDEO_H264                      1
#define         VIDEO_MJPEG                     2

typedef struct{	//��Ƶ��ʽ��Ϣ�ṹ
	struct timeval tv;			//���ݲ���ʱ��ʱ���
	unsigned long	Sequence;  //���к�
	unsigned char format;		//�����ʽformat
	unsigned char  type;		//frame type	I/P/B...
	unsigned char ratio;  //�ֱ���
	unsigned char recv[4];			//�Ƿ���pal����Ƶ
	
}stream_video_format_t;

typedef struct{	//��Ƶ��ʽ��Ϣ�ṹ
	struct timeval tv;			//���ݲ���ʱ��ʱ���
	unsigned short a_sampling;	//����������
	unsigned char  a_channel;	//����ͨ��
	unsigned char  a_wformat;	//������ʽ
	unsigned char  a_nr_frame;	//һ�����������м�������
	unsigned char  a_bitrate;		//��������
	unsigned char  a_bits;		//��Ƶ����λ��
	unsigned char  a_frate;		//��Ƶ���ݵ�֡��(û�����м�����Ƶ����)
	
}stream_audio_format_t;

typedef union{ //ý���ʽ����������
	stream_video_format_t v_fmt;
	stream_audio_format_t a_fmt;
}stream_format_t;




typedef struct gtsf_stream_fmt_struct
{
	unsigned long mark;               /*�����ʾ*/
	unsigned char type;		          //media type ��Ƶ����Ƶ
	unsigned char	encrypt_type;	      //�������ͣ�0�ǲ�����
	unsigned char	channel;		        //����Ƶͨ����ע����Ƶ����Ƶ��ͨ�����ܲ�һ��
	unsigned char	version;			      //ͨѶЭ��İ汾�ţ�Ŀǰû�У����Ա���
	long len;		//������֡ͷ����������ݳ��ȣ�������ܣ����ܺ�����ݳ�����data��ʼ
	stream_format_t media_format;      //���ݿ��ʽ
	char data[0];				              //frame data
}gtsf_stream_fmt;

#ifdef __cplusplus
};
#endif

#endif



