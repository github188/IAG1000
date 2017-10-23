#ifndef MEDIA_API_H
#define MEDIA_API_H
#include <mshmpool.h>
#ifndef IN
	#define IN
	#define OUT
	#define IO
#endif
//媒体类型定义
#define		MEDIA_TYPE_VIDEO		0
#define		MEDIA_TYPE_AUDIO		1


///视频编码类型
#define         VIDEO_MPEG4                     0
#define         VIDEO_H264                        1
#define         VIDEO_MJPEG                      2
#define AAC_AUDIO_BUFLEN 4096

//媒体状态定义
#define ENC_NO_INIT		0		//编码器没有初始化
#define	ENC_STAT_OK		1		//编码器工作正常
#define	ENC_STAT_ERR		2		//编码器故障	

typedef struct{	//视频格式信息结构
	int format;		//视频编码格式      VIDEO_MPEG4等
	int ispal;			//是否是pal制视频
	int v_width;		//图像宽度
	int v_height;		//图像高度
	int v_frate;		//图像珍率
	int v_buffsize;		//建议缓冲区大小
}video_format_t;

typedef struct{	//音频格式信息结构
	int a_wformat;	//声音格式
	int a_sampling;	//声音采样率
	int a_channel;	//声音通道
	int a_nr_frame;	//一包声音里面有几块数据
	int a_bitrate;		//声音码流
	int a_bits;		//音频采样位数
	int a_frate;		//音频数据的帧率(没秒钟有几包音频数据)
}audio_format_t;

typedef union{ //媒体格式定义联合体
	video_format_t v_fmt;
	audio_format_t a_fmt;
}media_format_t;

typedef struct{ //媒体属性结构
	int				media_type;		//媒体类型
	int				stat;			//状态
	media_format_t	fmt;				//媒体格式
}media_attrib_t;


typedef struct{ //媒体源结构
	pthread_mutex_t	mutex;
	int				media_type;				//媒体类型(MEDIA_TYPE_VIDEO或MEDIA_TYPE_AUDIO)
	int				no;						//同类资源编号
	int				dev_stat;				//-1表示还没有连接到具体的编码设备缓冲池 ,0表示已经连接上
	pthread_t			thread_id;				//线程id
	int				max_data_len;			//该设备的数据块最大长度(动态刷新)
	MSHM_POOL		mpool;					//媒体用到的资源共享缓冲池
	void *			*temp_buf;				//读取数据用的缓冲区(按DWORD对齐)
	int				buflen;					//缓冲区长度,负值表示内存分配失败
	media_attrib_t		*attrib;					//设备属性		
}media_source_t;


#endif
