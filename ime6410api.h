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

#define MEDIA_VIDEO		0x01		//视频数据
#define MEDIA_AUDIO		0x02		//音频数据

#define IDX1_VID  		0x63643030	//AVI的视频包标记
#define IDX1_AID  		0x62773130	//AVI的音频报的标记

struct NCHUNK_HDR {	//avi格式的数据块头标志结构
	unsigned long  chk_id;
	unsigned long  chk_siz;
};



struct stream_fmt_struct
{							//数据帧的结构
	struct timeval tv;			//数据产生时的时间戳
	unsigned long	channel;	//压缩通道，如果是6410则仅有一个通道，6400有4个
	unsigned short media;		//media type 音频或视频
	unsigned short  type;		//frame type	I/P/声音...
	 long len;				//frame len 后面的视频或音频祯数据的长度
	struct NCHUNK_HDR chunk;//数据块头标志，目前使用avi格式
	char data[4];				//frame data changed by shixin 060327
};

#endif


