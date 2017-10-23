#ifndef RTIMAGE2_H_20070301
#define RTIMAGE2_H_20070301

static const char version[]="0.26";			///<程序版本号
static const int def_rtstream_port 	 = 8096;    	   ///<音视频上行服务默认端口号
static const int def_rtsnd_port 	        = 8097;		   ///<音频下行服务默认端口号

static const int def_mic_gain 		 = 8;		   ///<默认mic增益
static const int def_audio_gain 	        = 8;		   ///<摩尔音频输出增益

static const int def_svr_timeout         = 10;               ///<默认的超时时间

static const int def_audio_pkt_size    = 1024;           ///<默认音频上行数据包大小

static const int def_th_drop_p            = 30;              ///< 开始丢弃p帧视频的默认阈值

#define DEF_AUDIO_SAMRATE             (8000)                  ///<默认音频采样率
#define DEF_AUDIO_FRAGSIZE            (10)                     ///<默认音频采样块大小2^DEF_FRAGSIZE
#define DEF_AUDIO_FRAGNB               (16)                    ///<默认音频采集缓冲区块数

#define	TCPRT_MAX_VIRAVUSR_NO	10		///<3022上虚拟设备一共支持的最大音视频上行用户数

#define	TCPRTIMG_MAX_AVUSR_NO       16          ///<tcprtimage2支持的最大音视频上行用户连接数 
#define TCPRTIMG_MAX_APLAY_NO       1
#define	MAX_MAP_BUF_FRAMES            600        ///<最大的缓冲帧数(用于分配缓冲区)



#ifndef IN
    #define IN          //参数输入标志
    #define OUT       //参数输出标志
    #define IO          //参数输入输出标志
#endif



//系统头文件
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

//自行开发的头文件
#include "typedefine.h"
#include "file_def.h"
#include "devinfo.h"


#include "devinfo_virdev.h"			///<devinfo.h的扩展

#include "rtimg_para2.h"



/** 
 *   @brief     判断一个地址是否为同一个局域网中的
 *   @param  需要判断的ip地址
 *   @return   1表示是同一个局域网的,0表示不是
 */ 
int is_lan_user(in_addr_t addr);


/**
  *   @brief	判断请求音视频的guid是否合法
  *   @param	cmd_dev_id申请音视频命令里的guid
  *	@param	cmd_addr请求音视频的设备ip的地址
  *	@return	成功返回虚拟设备号，失败返回负值
*/
int is_guid_valid(unsigned char * cmd_dev_id, struct sockaddr_in * cmd_addr);

#endif
