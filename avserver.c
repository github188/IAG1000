#include "rtimage2.h"
#include <media_api.h>
#include <gtthread.h>
#include <file_def.h>
#include <ime6410api.h>     ///FIXME stream_fmt_struct�ṹ����ŵ������ط�!
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
#define TCP_SEND_RESERVE_AUDIO      32*1024                                 ///<Ϊ������Ƶ���ݱ����Ļ�����
#define TCP_SEND_ADJUST_STEP         128*1024                                ///<ÿ�ε���tcp���ͻ�������С�Ĳ���
#define TCP_SEND_BUF_MAX                1024*1024                               ///<tcp���ͻ����������ֵ
#define NO_VIDEO_PIC				"/conf/novideo.264"
static unsigned char    avi_head_buf[1024*3];   ///<���aviͷ�Ļ�����
static  int             avi_head_len;                ///<avi_head_buf��ÿ��Ԫ�ص���Ч�ֽ���
static  unsigned char frame_head_buf[28];
extern unsigned long  net_pkts_sta;
struct timeval  venc_last_tv[MAX_VIDEO_ENCODER];       ///<��Ƶ������0���һ֡��ʱ���
struct timeval  venc_pb_tv[MAX_AUDIO_ENCODER];
/** 
 *   @brief     ����ָ����ű�������aviͷ��Ϣ
 *   @param  no ��Ƶ���������
 *   @param  attrib ��Ƶ���������Խ��ָ��
 *   @return   0��ʾ�ɹ���ֵ��ʾʧ��
 *                 avi ͷ��Ϣ�����avi_head_buf������
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
	davi.a_wformat=WFORMAT_AAC;	         //��Ƶ��ʽ
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
	 static FILE *afp=NULL; 	 ///<�ļ�ָ��
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
 *   @brief     ����ý�����ݽṹ������������
 *   @param  fd �Լ��򿪵�����������
 *   @param  frame ָ��Ҫ���͵����ݽṹָ��
 *   @param  v_fmt ��Ƶ�����ʽVIDEO_MPEG4, VIDEO_H264 ,VIDEO_MJPEG
 *   @return  ��ֵ��ʾ���ͳ�ȥ���ֽ���,��ֵ��ʾ����
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


	    //��������ͷ
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

	  	//����������
		ret=fd_write_buf(fd,(char *)frame,frame_len);	
		printf("����������fd=%d,ret=%d,len=%d\n",fd,ret,frame_len);
		if(ret>0)
		{
			ret+=sizeof(frame_head);
	    }
	}while(0);
	return ret;
}

////////////////////////////map���////////////////////////////////////////

/** 
 *   @brief     ����ý����Ϣӳ��������һ������Ԫ����Ϣ
 *   @param  map ָ����ý����Ϣӳ��ṹ��ָ��
 *   @param  flag Ԫ�ص�����
 *   @param  bytes �¼�Ԫ�ص��ֽ���
 *   @return   0��ʾ�ɹ� ��ֵ��ʾ����
 */ 
static inline int add_ele2map(IO stream_send_map_t *map,IN int size)
{
    map_frame_t *t_frame=&map->frame_map[map->tail];             ///<��βָ��

    t_frame->flag=0xff;
    t_frame->size=size;
    if(++map->tail>=MAX_MAP_BUF_FRAMES)
        map->tail=0;
    if(map->tail==map->head)
    {///��������,Ӧ�ò��ᷢ������������������˵����Ҫ����MAX_MAP_BUF_FRAMES
        printf("tail=head=%d ",map->tail);
        //showbug();
    }
    
	map->a_frames++;                ///<��Ƶ
    
    return 0;
}


 /** 
 *   @brief     �ӷ���ý����Ϣӳ����ɾ��ָ���ֽ���������
 *   @param  map ָ����ý����Ϣӳ��ṹ��ָ��
 *   @param  bytes Ҫɾ�����ֽ���
 *   @return   ��ֵ��ʾɾ����Ԫ����
 */ 
static inline int del_ele_from_map(stream_send_map_t *map,int bytes)
{
    int i;
    int remain=bytes;                 ///<����Ҫɾ�����ֽ���
    int dels=0;                             ///<ɾ����Ԫ����
    map_frame_t *frame;             ///<ָ��map��Ԫ�ص�ָ��
    if(map->head==map->tail)
        return 0;                           ///<���п�
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
            {///�����Ѿ�����
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
    //��������������Ϊ1
    net_set_tcp_sendbuf_len(usr->fd,1);
    ////send_len=150*1024;

    //������������Ϊ���
    send_len=p->tcp_max_buff*1024;
    ret=net_set_tcp_sendbuf_len(usr->fd,send_len);
    if(ret<0)
    {
        return -errno;
    }
    send_len=net_get_tcp_sendbuf_len(usr->fd);
    //printf("adjuest_buf to %dk!!\n",send_len/1024);
    //����Ӧ�ó���ֻ��(max*1024)*80%�Ŀռ���ã�����ʵ�����õ���max*1024
    usr->sock_attr.send_buf_len=send_len*0.8;               ///<ֻ����80%�Ŀռ����ʹ��,�����Ļ�����������
    //zw-add
    // ������ǰû�з��ͳ�ȥ�����ݰ���û�дﵽ������������80%
    if(usr->sock_attr.send_buf_remain>usr->sock_attr.send_buf_len)
    {
        printf("ԭ����send_buf_remain[%d]̫��\n",usr->sock_attr.send_buf_remain);
        //����ﵽ�˵Ļ�������ʱ��δ���͵����ݰ����������ϼ��ٵ�send_buff_len��ô��,��ҪĿ���ǽ��������Ӱ��׶�
        //����send_buf_remain,ƭһ��Ӧ�ó���
        usr->sock_attr.send_buf_remain=usr->sock_attr.send_buf_len;
       //// usr->sock_attr.send_buf_remain=usr->sock_attr.send_buf_len*2/5;
        printf("����send_buf_remain=[%d]\n",usr->sock_attr.send_buf_len);
    }

    //zw-add

    return 0;
}


 /** 
 *   @brief     ����ָ��ý������������ݸ���Ӧ���û�
 *   @param  enc ָ��ý���������ָ��
 *   @param  frame ָ��Ҫ���͵����ݽṹָ��
 *   @param  usr    �����͵��û�����
 *   @param  seq    ý�����ݵ����к�
 *   @param  flag   ý�����ݵı��
 *   @return   ��ֵ��ʾ�ɹ���ֵ��ʾ����,0��ʾʲôҲû�з���
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
    stream_send_info_t  *send_info=&usr->send_info;         ///<������Ϣ
    socket_attrib_t         *sock_attr=&usr->sock_attr;         ///<socket��Ϣ
    tcprtimg_svr_t    *p=get_rtimg_para();
	
    pthread_mutex_lock(&usr->u_mutex);
    do
    {
    	//netcmdavihead
        if(!send_info->send_ack_flag)
        {///<��û�з�����Ӧ��aviͷ

			ret=send_rtstream_ack_pkt(usr->fd,RESULT_SUCCESS,(char*)avi_head_buf,avi_head_len);
			if(ret>=0)
			{   
				///������Ӧ���ɹ�
				sock_attr->send_buffers=get_fd_out_buffer_num(usr->fd);
				add_ele2map(&send_info->map,sock_attr->send_buffers);   ///<��map�з�һ��������
				send_info->total_put=sock_attr->send_buffers;               ///<��ʼ�������ֽ���
				send_info->total_out=0;                                                 
				send_info->send_ack_flag=1;                                         ///<�Ѿ����͹��ļ�ͷ
				send_info->require_i_flag=1;
				ret=1;                                                                            ///<����һ֡���ݿ�ʼ����
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
				write_frame_data2trans(usr->fd,vframe,vlen,tv,IDX1_VID);                ///<��ý�����ݰ����͵�����
				write_frame_data2trans(usr->fd,vframe,vlen,tv,IDX1_VID);                ///<��ý�����ݰ����͵�����

				break;
			}
        }

        ///socket���ͻ������е�����
        sock_attr->send_buf_remain=get_fd_out_buffer_num(usr->fd);                    ///<socket���ͻ������ڵ��ֽ���
        send_bytes=sock_attr->send_buffers-sock_attr->send_buf_remain;              ///<�շ��ͳ�ȥ���ֽ���
        sock_attr->send_buffers=sock_attr->send_buf_remain;
        ret=del_ele_from_map(&send_info->map,send_bytes);
        send_info->total_out+=send_bytes;                                                             ///<�ܹ����ͳ�ȥ���ֽ���
        if(send_bytes<0)
        {
        	gtloginfo("send_bytes=%d\n",send_bytes);
            printf("send=%d sock_attr->send_buf_remain=%d!!!xxx!!!!!!!!!!!send_bytes=Ϊ����!!!!!!!!!!!!!!\n",send_bytes,sock_attr->send_buf_remain);
        }
        ///��Ƶ����
		{

			/*
            if(send_info->map.a_frames>40)
            {///��໺��40����Ƶ
                    printf("usr:%d too many audio pkt, drop audio:%d!!\n",usr->no,(int)len);
                    send_info->drop_a_flag=1;
                    send_info->drop_a_frames++;
                    ret=0;
                    break;                       
            }
            if((send_info->map.v_frames+send_info->map.a_frames+2)>MAX_MAP_BUF_FRAMES)
            {///Ԥ��2֡�ռ�
                    printf("usr:%d not enough map drop audio:%d!!\n",usr->no,(int)len);
                    send_info->drop_a_flag=1;
                    send_info->drop_a_frames++;
                    ret=0;
                    break;                     
            }
            if((sock_attr->send_buf_len-sock_attr->send_buf_remain-1024)<(len+sizeof(struct stream_fmt_struct)*2))
            {

                ret=adjust_usr_sock_buf(usr,TCP_SEND_ADJUST_STEP);      ///<socket����������,���ڻ�����
                if((sock_attr->send_buf_len-sock_attr->send_buf_remain-1024)<(len+sizeof(struct stream_fmt_struct)*2))
                { ///����û�з��䵽�㹻�Ļ�����
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
                {///�ָ���Ƶ����
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
		ret=write_frame_data2trans(usr->fd,frame,len,tv,IDX1_AID);                ///<��ý�����ݰ����͵�����
		if(ret>0)
		{
			sock_attr->send_buffers+=ret;                                   ///<�ѷ��뻺�������ֽ���
			add_ele2map(&send_info->map,ret);
			sock_attr->send_buf_remain+=ret;                            ///<�����������ֽ���
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
 *   @brief     ����ָ��ý������������ݸ���Ӧ���û�
 *   @param  enc ָ��ý���������ָ��
 *   @param  frame ָ��Ҫ���͵����ݽṹָ��
 *   @param  seq  ý�����ݵ����к�
 *   @param  flag   ý�����ݵı��
 *   @return   ��ֵ��ʾ�û���ý�巢�͵��û���,��ֵ��ʾ����
 */ 
static int send_media_frames
	(void *frame,int len,struct timeval *tv)
{

    tcprtimg_svr_t    *p=get_rtimg_para();
    av_usr_t            *usr=NULL;
    int                     total=sizeof(p->av_server.av_usr_list)/sizeof(av_usr_t);        ///<���û���
    int                     nums=0;                 ///<���͵��û�����
    int                     i;
    int                     ret;
	int 					no=0;
	stream_send_info_t  *send_info;         ///<������Ϣ


	
    for(i=0;i<total;i++)
    {
        usr=&p->av_server.av_usr_list[i];
		send_info=&usr->send_info;
        if(usr->serv_stat<=0)
        {
            continue;                                                         ///<��û���յ���������
        }
        else
        {

			pthread_mutex_lock(&usr->u_mutex);
			usr->venc_no=no;
			pthread_mutex_unlock(&usr->u_mutex);
            	
			if(usr->serv_stat==3)                               ///<��������Ƶ����
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
 *   @brief     ��Ƶ�����߳�
 *   @param  para ָ��������Ƶ��������ָ��
 *   @return   ��������
 */ 

int posix_memalign(void **memptr, size_t alignment, size_t size);




/** 
 *   @brief    ��ȡ��Ƶ���ݽӿ� 
 *   @param	   frame_buf Ϊ��ʵ���ݣ�������ͷ 
 *   @param	   lenΪ���ݳ���ָ��
 *   @param	   tvΪʱ���ָ��
 *   @return   >=0��ʾ�ɹ�,��ֵ��ʾʧ��
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
    int                     len = 0;                                       ///<frame_buf�ĳ���
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
			///�����ݳ���
			printf("read_audio_frame ret=%d!!\n",ret);
			if(ret!=-EINTR)
				sleep(1);
		}

    }
    return NULL;    
}



 /** 
 *   @brief     ��������Ƶ�����߳�
 *   @return   0��ʾ�ɹ�,��ֵ��ʾʧ��
 */ 
int create_av_server(void)
{
	int                         i;
	pthread_t					thread_id;
	gt_create_thread(&thread_id,aenc_server_thread,NULL);
	return 0;
}




