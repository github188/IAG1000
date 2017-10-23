#include "rtimage2.h"
#include <media_api.h>
#include <gtthread.h>
#include <file_def.h>
#include <ime6410api.h>     ///FIXME stream_fmt_struct结构定义放到其它地方!
#include <AVIEncoder.h>
#include <commonlib.h>
#include <gt_errlist.h>
#include "rtnet_cmd.h"
#include "net_avstream.h"
#include <audiofmt.h>
#include <gtsocket.h>
#include <devinfo.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <time.h>
#include "avserver.h"
#define TCP_SEND_RESERVE_AUDIO      32*1024                                 ///<为发送音频数据保留的缓冲区
#define TCP_SEND_ADJUST_STEP         128*1024                                ///<每次调节tcp发送缓冲区大小的步长
#define TCP_SEND_BUF_MAX                1024*1024                               ///<tcp发送缓冲区的最大值
#define NO_VIDEO_PIC				"/conf/novideo.264"
static unsigned char    avi_head_buf[1024*3];   ///<存放avi头的缓冲区
static  int             avi_head_len;                ///<avi_head_buf中每个元素的有效字节数
static  unsigned char frame_head_buf[28];
extern unsigned long  net_pkts_sta;
struct timeval  venc_last_tv[MAX_VIDEO_ENCODER];       ///<视频编码器0最后一帧的时间戳
struct timeval  venc_pb_tv[MAX_AUDIO_ENCODER];
/** 
 *   @brief     生成指定编号编码器的avi头信息
 *   @param  no 视频编码器编号
 *   @param  attrib 视频编码器属性结果指针
 *   @return   0表示成功负值表示失败
 *                 avi 头信息填充在avi_head_buf数组中
 */
static int gen_venc_avi_head(void)
{
    int     ret;
    struct defAVIVal davi;
    memset((void*)&davi,0,sizeof(struct defAVIVal));
	//audio format
	davi.a_sampling=16000;
	davi.a_channel=2;
	davi.a_bitrate=32000;
	davi.a_wformat=WFORMAT_AAC;	         //音频格式
	davi.a_nr_frame=1;
	//video format	
	davi.ispal=1;
	davi.nr_stream=2;
	davi.v_width=704;
	davi.v_height=576;
	davi.v_frate=25;
	davi.v_buffsize=4096;
	strcpy(davi.v_avitag,"H264");
	printf("[%s:%s:%d].format=%d \n",__FILE__,__FUNCTION__,__LINE__,davi.a_wformat);
    ret=FormatAviHeadBufDef(&davi,avi_head_buf, sizeof(avi_head_buf));
    avi_head_len=ret;
    return 0;
}
int preflag[MAX_VIDEO_ENCODER]={0};
int sendflag=0;
static __inline__ void convert_intra_frame(unsigned long * flag)
{
	#define IFRAME_HEAD 0xb0010000
	*flag=IFRAME_HEAD;

}
int  write_rawaudio2file_send(void *buf,int len)
 {
	 static FILE *afp=NULL; 	 ///<文件指针
	 char filename[50];
	 int ret=0;
		
	 if(afp==NULL&&sendflag==0)
	 {
	 		sendflag=1;
		 sprintf(filename,"/mnt/zsk/test.aac",(int)getpid());
		 afp=fopen(filename,"wb");
		 if(afp==NULL)
		 {
			 printf("can't create file:%s errno=%d:%s!!\n",filename,errno,strerror(errno));
			 return -errno;
		 }
	 }
	 if(afp!=NULL)
	 {
		 ret=fwrite(buf,1,len,afp);
		 fflush(afp);
	 }
	 return ret;
	 
 }

 /** 
 *   @brief     发送媒体数据结构到网络描述符
 *   @param  fd 以及打开的网络描述符
 *   @param  frame 指向要发送的数据结构指针
 *   @param  v_fmt 视频编码格式VIDEO_MPEG4, VIDEO_H264 ,VIDEO_MJPEG
 *   @return  正值表示发送出去的字节数,负值表示出错
 */ 
typedef struct 
{
	unsigned int frame_id;
	struct timeval tv;
	unsigned int frame_size;
}frame_head;
static inline int write_frame_data2trans
	(int fd,void *frame,int frame_len,struct timeval * tv,int id)
{
	unsigned char *p;
	int ret=-1;
	char head_buf[16]={0};
	frame_head * p_head=head_buf;
    if(fd<0)
		return -EINVAL;
    do
	{


	    //发送数据头
		//p_head->frame_id=IDX1_AID;
		p_head->frame_id=id;
		p_head->frame_size=frame_len;
		p_head->tv.tv_sec=tv->tv_sec;
		p_head->tv.tv_usec=tv->tv_usec;
		ret=fd_write_buf(fd,head_buf,sizeof(frame_head));
		if(ret!=sizeof(frame_head))
		{			
			break;
		}

	  	//发送数据体
		ret=fd_write_buf(fd,(char *)frame,frame_len);	
		printf("发送数据体fd=%d,ret=%d,len=%d\n",fd,ret,frame_len);
		if(ret>0)
		{
			ret+=sizeof(frame_head);
	    }
	}while(0);
	return ret;
}

////////////////////////////map相关////////////////////////////////////////

/** 
 *   @brief     向发送媒体信息映像中增加一个数据元素信息
 *   @param  map 指向发送媒体信息映像结构的指针
 *   @param  flag 元素的类型
 *   @param  bytes 新加元素的字节数
 *   @return   0表示成功 负值表示出错
 */ 
static inline int add_ele2map(IO stream_send_map_t *map,IN int size)
{
    map_frame_t *t_frame=&map->frame_map[map->tail];             ///<队尾指针

    t_frame->flag=0xff;
    t_frame->size=size;
    if(++map->tail>=MAX_MAP_BUF_FRAMES)
        map->tail=0;
    if(map->tail==map->head)
    {///队列满了,应该不会发生这种情况，如果发生说明需要增大MAX_MAP_BUF_FRAMES
        printf("tail=head=%d ",map->tail);
        //showbug();
    }
    
	map->a_frames++;                ///<音频
    
    return 0;
}


 /** 
 *   @brief     从发送媒体信息映像中删除指定字节数的数据
 *   @param  map 指向发送媒体信息映像结构的指针
 *   @param  bytes 要删除的字节数
 *   @return   正值表示删除的元素数
 */ 
static inline int del_ele_from_map(stream_send_map_t *map,int bytes)
{
    int i;
    int remain=bytes;                 ///<还需要删除的字节数
    int dels=0;                             ///<删除的元素数
    map_frame_t *frame;             ///<指向map中元素的指针
    if(map->head==map->tail)
        return 0;                           ///<队列空
    if(bytes<=0)
        return 0;
    for(i=0;i<MAX_MAP_BUF_FRAMES;i++)
    {
        frame=&map->frame_map[map->head];
        if(frame->size<=remain)
        {
            remain-=frame->size;
            if(++map->head>=MAX_MAP_BUF_FRAMES)
                map->head=0;
            dels++;
			map->a_frames--;
            if(map->head==map->tail)
            {///队列已经空了
                //printf("map is empty!! head=tail=%d! remain=%d vframe=%d aframe=%d\n",map->head,remain,map->v_frames,map->a_frames);
                break;
            }
        }
        else
        {
            frame->size-=remain;
            break;
        }
    }
    return dels;
}
///////////////////////////////////////////////////////////////////////////////////

static int adjust_usr_sock_buf(av_usr_t *usr,int size)
{
    int ret;
    int send_len=net_get_tcp_sendbuf_len(usr->fd);
    tcprtimg_svr_t    *p=get_rtimg_para();
    
    send_len+=size;

    #if 0
    //if(send_len>TCP_SEND_BUF_MAX)
    if(send_len>200*1024)
    {
        return -ENOMEM;
    }
    #endif
    //将缓冲区长度设为1
    net_set_tcp_sendbuf_len(usr->fd,1);
    ////send_len=150*1024;

    //将缓冲区设置为最大
    send_len=p->tcp_max_buff*1024;
    ret=net_set_tcp_sendbuf_len(usr->fd,send_len);
    if(ret<0)
    {
        return -errno;
    }
    send_len=net_get_tcp_sendbuf_len(usr->fd);
    //printf("adjuest_buf to %dk!!\n",send_len/1024);
    //告诉应用程序只有(max*1024)*80%的空间可用，但是实际设置的是max*1024
    usr->sock_attr.send_buf_len=send_len*0.8;               ///<只能有80%的空间可以使用,超过的话会引起阻塞
    //zw-add
    // 看看当前没有发送出去的数据包有没有达到整个缓冲区的80%
    if(usr->sock_attr.send_buf_remain>usr->sock_attr.send_buf_len)
    {
        printf("原来的send_buf_remain[%d]太大\n",usr->sock_attr.send_buf_remain);
        //如果达到了的话，就临时将未发送的数据包个数表面上减少到send_buff_len那么长,主要目的是进入后面的扔包阶段
        //减少send_buf_remain,骗一下应用程序
        usr->sock_attr.send_buf_remain=usr->sock_attr.send_buf_len;
       //// usr->sock_attr.send_buf_remain=usr->sock_attr.send_buf_len*2/5;
        printf("调整send_buf_remain=[%d]\n",usr->sock_attr.send_buf_len);
    }

    //zw-add

    return 0;
}


 /** 
 *   @brief     发送指定媒体服务器的数据给相应的用户
 *   @param  enc 指向媒体服务器的指针
 *   @param  frame 指向要发送的数据结构指针
 *   @param  usr    被发送的用户对象
 *   @param  seq    媒体数据的序列号
 *   @param  flag   媒体数据的标记
 *   @return   正值表示成功负值表示出错,0表示什么也没有发送
 */
extern int write_err_flag;
static char vframe[4096]={0};
static int  vlen=0;
static int  send_audio_count;
static inline int send_media_frame2usr
	(av_usr_t *usr,void *frame,int len,struct timeval * tv)
{
    int ret;
    int send_bytes;
	int size;
	FILE * pfd;
    stream_send_info_t  *send_info=&usr->send_info;         ///<发送信息
    socket_attrib_t         *sock_attr=&usr->sock_attr;         ///<socket信息
    tcprtimg_svr_t    *p=get_rtimg_para();
	
    pthread_mutex_lock(&usr->u_mutex);
    do
    {
    	//netcmdavihead
        if(!send_info->send_ack_flag)
        {///<还没有发送响应的avi头

			ret=send_rtstream_ack_pkt(usr->fd,RESULT_SUCCESS,(char*)avi_head_buf,avi_head_len);
			if(ret>=0)
			{   
				///发送响应包成功
				sock_attr->send_buffers=get_fd_out_buffer_num(usr->fd);
				add_ele2map(&send_info->map,sock_attr->send_buffers);   ///<向map中放一个假数据
				send_info->total_put=sock_attr->send_buffers;               ///<初始化发送字节数
				send_info->total_out=0;                                                 
				send_info->send_ack_flag=1;                                         ///<已经发送过文件头
				send_info->require_i_flag=1;
				ret=1;                                                                            ///<从下一帧数据开始发送
				pfd=fopen(NO_VIDEO_PIC,"rb");		
				if(!pfd)
				{
					printf("Open file-> %s fail\n",NO_VIDEO_PIC);
					break;
				}
				fseek(pfd, 0 , SEEK_END);  
			    size= ftell (pfd);  
				rewind(pfd);  
				vlen = fread (vframe,1,size,pfd);  
				fclose(pfd);
			    if (vlen != size)  
			    {  
			        printf("Reading file error vlen=%d size=%d\n",vlen,size);  
					break;
				}
				usleep(10000);
				convert_intra_frame((void *)vframe);
				tv->tv_sec=1;
				tv->tv_usec=1;
				write_frame_data2trans(usr->fd,vframe,vlen,tv,IDX1_VID);                ///<将媒体数据包发送到网络
				write_frame_data2trans(usr->fd,vframe,vlen,tv,IDX1_VID);                ///<将媒体数据包发送到网络

				break;
			}
        }

        ///socket发送缓冲区中的数据
        sock_attr->send_buf_remain=get_fd_out_buffer_num(usr->fd);                    ///<socket发送缓冲区内的字节数
        send_bytes=sock_attr->send_buffers-sock_attr->send_buf_remain;              ///<刚发送出去的字节数
        sock_attr->send_buffers=sock_attr->send_buf_remain;
        ret=del_ele_from_map(&send_info->map,send_bytes);
        send_info->total_out+=send_bytes;                                                             ///<总共发送出去的字节数
        if(send_bytes<0)
        {
        	gtloginfo("send_bytes=%d\n",send_bytes);
            printf("send=%d sock_attr->send_buf_remain=%d!!!xxx!!!!!!!!!!!send_bytes=为负了!!!!!!!!!!!!!!\n",send_bytes,sock_attr->send_buf_remain);
        }
        ///音频数据
		{

			/*
            if(send_info->map.a_frames>40)
            {///最多缓冲40包音频
                    printf("usr:%d too many audio pkt, drop audio:%d!!\n",usr->no,(int)len);
                    send_info->drop_a_flag=1;
                    send_info->drop_a_frames++;
                    ret=0;
                    break;                       
            }
            if((send_info->map.v_frames+send_info->map.a_frames+2)>MAX_MAP_BUF_FRAMES)
            {///预留2帧空间
                    printf("usr:%d not enough map drop audio:%d!!\n",usr->no,(int)len);
                    send_info->drop_a_flag=1;
                    send_info->drop_a_frames++;
                    ret=0;
                    break;                     
            }
            if((sock_attr->send_buf_len-sock_attr->send_buf_remain-1024)<(len+sizeof(struct stream_fmt_struct)*2))
            {

                ret=adjust_usr_sock_buf(usr,TCP_SEND_ADJUST_STEP);      ///<socket缓冲区不足,调节缓冲区
                if((sock_attr->send_buf_len-sock_attr->send_buf_remain-1024)<(len+sizeof(struct stream_fmt_struct)*2))
                { ///还是没有分配到足够的缓冲区
                    printf("usr:%d not enough buf!%d<%d drop audio frame !!\n",usr->no,(sock_attr->send_buf_len-sock_attr->send_buf_remain-1024),(int)(len+sizeof(struct stream_fmt_struct)*2));
                    send_info->drop_a_flag=1;
                    send_info->drop_a_frames++;
                    ret=0;
                    break;                    
                }
               
            }
            if(send_info->drop_a_flag)
            {
                if(send_info->map.a_frames<10)
                {///恢复音频发送
                    send_info->drop_a_flag=0;
                }
                else
                {
                    printf("usr:%d drop a frame:%d!\n",usr->no,(int)len);
                    ret=0;
                    break;
                }
            }
			*/
           
        
        }
		ret=write_frame_data2trans(usr->fd,frame,len,tv,IDX1_AID);                ///<将媒体数据包发送到网络
		if(ret>0)
		{
			sock_attr->send_buffers+=ret;                                   ///<已放入缓冲区的字节数
			add_ele2map(&send_info->map,ret);
			sock_attr->send_buf_remain+=ret;                            ///<缓冲区生育字节数
			send_info->total_put+=ret;             
			if(++send_audio_count>16)
			{
				send_audio_count=0;
				ret = write_frame_data2trans(usr->fd,vframe,vlen,tv,IDX1_VID);            
				if(ret > 0) {
					sock_attr->send_buffers+=ret;  
					add_ele2map(&send_info->map,ret);
					sock_attr->send_buf_remain+=ret;     
				}
				else if(ret <=0)
				{


					write_err_flag = 1;
					gtloginfo("FUNCTION %s line:%d\n",__func__,__LINE__);
					printf("FUNCTION %s line:%d\n",__func__,__LINE__);
					pthread_mutex_unlock(&usr->u_mutex);
				    return ret;
				}
				ret = write_frame_data2trans(usr->fd,vframe,vlen,tv,IDX1_VID);              
				if(ret > 0) {
					sock_attr->send_buffers+=ret;  
					add_ele2map(&send_info->map,ret);
					sock_attr->send_buf_remain+=ret;     
				}
				else if(ret <=0)
				{


					write_err_flag = 1;
					gtloginfo("FUNCTION %s line:%d\n",__func__,__LINE__);
					printf("FUNCTION %s line:%d\n",__func__,__LINE__);
					pthread_mutex_unlock(&usr->u_mutex);
				    return ret;
				}
			}
		}
		else
		{
			printf("DBG write_frame_data2net ret=%d:%d:%s\n",ret,errno,strerror(errno));
			printf(":%d(%s),ret=%d\n",usr->fd,inet_ntoa(usr->addr.sin_addr),ret);
			gtloginfo(":%d(%s),ret=%d\n",usr->fd,inet_ntoa(usr->addr.sin_addr),ret);
			write_err_flag = 1;
		}
    }while(0);
    pthread_mutex_unlock(&usr->u_mutex);
	
    return ret;

}
 /** 
 *   @brief     发送指定媒体服务器的数据给相应的用户
 *   @param  enc 指向媒体服务器的指针
 *   @param  frame 指向要发送的数据结构指针
 *   @param  seq  媒体数据的序列号
 *   @param  flag   媒体数据的标记
 *   @return   正值表示用户该媒体发送的用户数,负值表示出错
 */ 
static int send_media_frames
	(void *frame,int len,struct timeval *tv)
{

    tcprtimg_svr_t    *p=get_rtimg_para();
    av_usr_t            *usr=NULL;
    int                     total=sizeof(p->av_server.av_usr_list)/sizeof(av_usr_t);        ///<总用户数
    int                     nums=0;                 ///<发送的用户计数
    int                     i;
    int                     ret;
	int 					no=0;
	stream_send_info_t  *send_info;         ///<发送信息


	
    for(i=0;i<total;i++)
    {
        usr=&p->av_server.av_usr_list[i];
		send_info=&usr->send_info;
        if(usr->serv_stat<=0)
        {
            continue;                                                         ///<还没有收到订阅命令
        }
        else
        {

			pthread_mutex_lock(&usr->u_mutex);
			usr->venc_no=no;
			pthread_mutex_unlock(&usr->u_mutex);
            	
			if(usr->serv_stat==3)                               ///<订阅了音频服务
			{
				ret=send_media_frame2usr(usr,frame,len,tv);
				if(ret>0)
					nums++;
			}
               
        }
        
    }
    return nums;
}

 /** 
 *   @brief     视频服务线程
 *   @param  para 指向描述视频编码器的指针
 *   @return   永不返回
 */ 

int posix_memalign(void **memptr, size_t alignment, size_t size);




/** 
 *   @brief    读取音频数据接口 
 *   @param	   frame_buf 为真实数据，不包括头 
 *   @param	   len为数据长度指针
 *   @param	   tv为时间戳指针
 *   @return   >=0表示成功,负值表示失败
 */ 

int  read_audio_frame(void * frame_buf,int *len,struct timeval *tv)
{
	//ipcall to do
	
	usleep(20000);
	memset(frame_buf,0,4096);
	*len=800;
	tv->tv_sec=100;
	tv->tv_usec=99;
	return *len;
	
	//printf("read_audio_frame [len]%d\t[sec]%d\t[usec]%d\n",*len,tv->tv_sec,tv->tv_usec);
}

static void *aenc_server_thread(void *para)
{
    tcprtimg_svr_t    *p=get_rtimg_para();
	char frame_buf[8000];
    int                     ret;
    int                     len = 0;                                       ///<frame_buf的长度
	struct timeval tv;
	gen_venc_avi_head();
    while(1)
    {
		ret = read_audio_frame((void *)frame_buf,&len,&tv);
		if(ret>0)
		{
			ret=send_media_frames(frame_buf,len,&tv);	  

		}
		else
		{
			///读数据出错
			printf("read_audio_frame ret=%d!!\n",ret);
			if(ret!=-EINTR)
				sleep(1);
		}

    }
    return NULL;    
}



 /** 
 *   @brief     创建音视频服务线程
 *   @return   0表示成功,负值表示失败
 */ 
int create_av_server(void)
{
	int                         i;
	pthread_t					thread_id;
	gt_create_thread(&thread_id,aenc_server_thread,NULL);
	return 0;
}




