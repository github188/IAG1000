#ifndef SERV_INFO_H_20070302
#define SERV_INFO_H_20070302
#include <devinfo.h>
#include <media_api.h>
#include "tcprt_usr_info.h"

/** @struct av_server_t
 *  @brief  ����Զ������Ƶ���з�����Ϣ�����ݽṹ
 */
typedef struct{
    pthread_mutex_t     l_mutex;                                         ///<�����õĻ�����
	pthread_mutex_t     s_mutex;                                        ///<��������Ƶ���з����õĻ�����
	int		                lan_usrs;				               ///<��ǰ�ľ������û���
	int		                wan_usrs;				               ///<��ǰ�Ĺ������û���
    int                          listen_fd;                                        ///<���������������
	media_source_t 	  video_enc[MAX_VIDEO_ENCODER]; ///<��Ƶ������ʵ��
	int				vir0_lan_users;					///<��ǰ�����豸0�������û���
	int 				vir0_wan_users;					///<��ǰ�����豸0�������û���
	int				vir1_lan_users;					///<��ǰ�����豸1�������û���
	int				vir1_wan_users;					///<��ǰ�����豸1�������û���
	av_usr_t	av_usr_list[TCPRTIMG_MAX_AVUSR_NO+1];	 ///<����Ƶ���з����û���Ϣ�б�,��һ�����ڴ���æ
}av_server_t;

/** @struct aplay_server_t
 *   @brief  ����Զ����Ƶ���з�����Ϣ�����ݽṹ
 */
typedef struct{
    pthread_mutex_t     l_mutex;                                                             ///<�����õĻ�����
	pthread_mutex_t     s_mutex;                                                            ///<��������Ƶ���з����õĻ�����??����
	int		            usrs;				                                          ///<��ǰ������Ƶ���з�����û���
    int					listen_fd;                                                           ///<���������������
	aplay_usr_t	        usr_list[TCPRTIMG_MAX_APLAY_NO+1];	              ///<����Ƶ���з����û���Ϣ�б�,��һ�����ڴ���æ

}aplay_server_t;

/** @struct tcprtimg_svr_t
 *  @brief  tcprtimage2�����в�������Ϣ�ṹ����
 */
typedef struct{
	int		         mic_gain;		    ///<mic����
	float            ain0_gain;			
	float 			 ain1_gain;
	float            ain2_gain;
	float            ain3_gain;
	float		     audio_gain;          ///<��Ƶ�������
	int		         max_lan_usrs;	   ///<���������û���
	int		         max_wan_usrs;	   ///<���������û���
	int              max_aplay_usrs; ///<��Ƶ���з�������û���
	int		         av_svr_port;	   ///<����Ƶ���з���˿�
	int		         audio_play_port; ///<��Ƶ���з���˿�
       
    int              audio_pkt_size;    ///<��Ƶ�������ݰ���С	
	int		         th_timeout;	   ///<��ʱ��û�����ݽ���ʱ�жϳ�ʱ��ʱ��
	int		         th_drop_p;		   ///<��������ݶ��ʼ����Ƶp֡����ֵ
	int		         th_drop_v;		   ///<��������ݶ��ʼ����������Ƶ����ֵ
	int		         th_drop_a;		   ///<��ʼ�����������ݵ���ֵ
	in_addr_t         eth0_addr;           ///<����0�ľ�������ַ
	in_addr_t         eth0_mask;          ///<����0����������
	in_addr_t         eth1_addr;           ///<����0�ľ�������ַ
	in_addr_t         eth1_mask;          ///<����0����������       
	av_server_t	  	  av_server;           ///<����Ƶ�ϴ�����
	aplay_server_t    aplay_server;      ///<��Ƶ���з���   
	int				  multichannel;       //��ͨ����־���ķ������Ƕ���ͨ��
	//char		      resolved[200];   
///for 3022
	int                   virdev_num;	  ///<�豸�������豸����

////zw-20091229 Ϊ�˶Ը���cdma��ֻҪ˫��������Ҫͼ��
	struct sockaddr_in     audio_only_usr_addr[6];	///<ֻ���ñ�־,0�����ֿ���1Ϊֻ������

	int                 pkts_limit;                 ///<������������С�л��ķ�ֵ
	int                 tcp_max_buff;               ///<��������С
	int			        playback_pre;		///����ǰ��¼��ط�ʱ��
	int			        playback_dly;		///�������¼��ط�ʱ��
	int 			frame_rate;
	int            bitratecon;
	int            targetbitrate;
	int            maxbitrate;


        
}tcprtimg_svr_t;




#endif

