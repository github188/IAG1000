/** @file	tcprt_usr_info.h
 *  @brief 	����tcprtimage2�û���Ϣ�Ľṹ����
 *  @date 	2007.03
 */
#ifndef TCPRT_USR_INFO_H_20070301
#define TCPRT_USR_INFO_H_20070301
//ϵͳͷ�ļ�
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <pthread.h>

////

/** @struct socket_attrib_t
 *   @brief  ����socket���ԵĽṹ 
 */
typedef struct{
	int	send_buf_len;	        ///<���ͻ�������С
	int	recv_buf_len;	        ///<���ջ�������С

	int	 send_buf_remain;	  ///<���ͻ�����(socket)��ǰ���ֽ���(ʹ��ioctl��socket������������δ���ͳ�ȥ���ֽ���)
	int    send_buffers;             ///<���뷢�ͻ������е��ֽ���(writeʱ����,�������������,���ֵӦ����ԶС��send_buf_len)

       int    recv_buf_remain;      ///<���ջ������ڵ�ǰ�ֽ���

}socket_attrib_t;


/////////////////////////����Ƶ���з����õ��Ľṹ����/////////////////////
/** @struct map_frame_t
 *   @brief  stream_send_map_t������һ֡����Ƶ���ݵĽṹ 
 */
typedef struct{
	int	flag;		///<��� ��Ƶ����Ƶ
	int	size;		///<��֡�Ĵ�С
}map_frame_t;

/** @struct stream_send_map_t
 *   @brief  ����ý�����ݵ�socket������ӳ�������ṹ 
 */
typedef struct{
	int	              v_frames;			                          ///>������map�е���Ƶ֡��
	int	              a_frames;			                          ///>������map�е���Ƶ֡��
	int	              head;				                          ///>����������λ��
	int	              tail;				                                 ///>��������βλ��,head=tail��ʾû��map��û��Ԫ��
	map_frame_t	frame_map[MAX_MAP_BUF_FRAMES];    ///>����socket��������֡�ṹ	
}stream_send_map_t;

/** @struct stream_send_info_t
 *   @brief  ����ý�����ݵ�������Ϣ�ṹ 
 */
typedef struct{
    int    first_flag;                        ///<�״η�����Ƶ���ݱ�־
    int    send_ack_flag;               ///<�ѷ�����Ӧ��־ 0��ʾδ���� ��0��ʾ�ѷ���
	int	send_i_flag;		            ///<�ѷ���I֡��־
	int require_i_flag;                 //������i֡��־
    int    drop_p_flag;                  ///<��ʼ����P֡���ݵı�־
	int	drop_v_flag;	            ///<��ʼ������Ƶ���ݱ�־,0��ʾ����Ҫ����
	int	drop_a_flag;	            ///<��ʼ������Ƶ���ݱ�־,0��ʾ����Ҫ����
	int	drop_v_frames;	            ///<��������Ƶ֡��
	int	drop_a_frames;	            ///<��������Ƶ֡��

	int	last_v_seq;		            ///<��һ֡��Ƶ�����
	int 	last_a_seq;		            ///<��һ֡��Ƶ�����

       int     total_put;                   ///<�ܹ����뷢�ͻ������е��ֽ���
       int     total_out;                  ///<�ܹ����ͳ�ȥ���ֽ��� 
       int 		jump_flag;					//�豸�ط���ת�ı�־
       stream_send_map_t    map;     ///<���͵�ý����Ϣӳ��
}stream_send_info_t;

/** @struct av_usr_t
 *  @brief  ����Զ������Ƶ���з��������û���Ϣ�Ľṹ 
 */
typedef struct{
	unsigned long		         magic;			///<�û��ṹ��Ϣħ��,0x55aa��ʾ�Ѿ���ʼ��,����ֵ��ʾδ��ʼ��
	pthread_mutex_t            u_mutex;		///<�����û�����ʱ��Ҫ�Ļ�����
	pid_t			         pid;			///<pid �û��ӽ��̺�
       pthread_t                       thread_id;         ///<�߳�id
	int			                no;			///<�û����
	struct sockaddr_in	         addr;			///<Զ���û���ַ
	int			                fd;				///<�û�����������,��ֵ��ʾδ����
	int			                th_timeout;	///<�ж���ʱ����ֵ(���Ӻ�ʱ��û���յ������ʱ�䷢��������),����Ϊ��λ
	int	                              th_drop_p;		///<��ʼ����p֡����ֵ
	int	                              th_drop_v;		///<��ʼ����������Ƶ��
	int	                              th_drop_a;		///<��ʼ������Ƶ���ݵ���ֵ
	int			                timeout_cnt;	///<���㳬ʱ�ļ�����
	struct timeval 		  start_time;		///<���ӵ���ʼʱ��;
	struct timeval 		  last_cmd_time;	///<���һ���յ������ʱ��
	char			                name[40];		///<���ķ�����û���	
	int			                serv_stat;		///<����״̬,0:û���յ������������ 1:�Ѷ�������Ƶ���� 3:�ȶ�������Ƶ����Ҳ��������Ƶ����
	int			                venc_no;		///<�û��������ӵ���Ƶ���������
	int			                aenc_no;		///<�û��������ӵ���Ƶ���������(Ŀǰֻ��һ����Ƶ������,���Դ˲�����Ч)
	socket_attrib_t		  sock_attr;		///<socket����
	stream_send_info_t	  send_info;		///<����ý�����ݵ���Ϣ�ṹ
	int                   stream_idx;       //�����ӵ�sdk��idx
	int 				used;

}av_usr_t;
//////////////////////////////////////////////////////////////////////////////////////////////////////////




/////////////////////////��Ƶ���з����õ��Ľṹ����/////////////////////


typedef struct{
    int                                    total_recv;          ///<�ܹ��յ����ֽ���
    int                                    total_play;          ///<�ܹ����ŵ��ֽ���
    int                                    play_buf_used;  ///<�Ѿ�ʹ�õĲ��Ż������ֽ���
}stream_recv_info_t;

/** @struct aplay_usr_t
 *   @brief  ����Զ����Ƶ���з��������û���Ϣ�Ľṹ 
 */
typedef struct{
    unsigned long                    magic;                ///<�û��ṹ��Ϣħ��,0x55aa��ʾ�Ѿ���ʼ��,����ֵ��ʾδ��ʼ��
    //snd_dev_t                         *play_dev;        ///<��Ƶ�����豸ָ��
    int                           play_dev;
    pthread_mutex_t               u_mutex;		///<�����û�����ʱ��Ҫ�Ļ�����
    pid_t			                pid;			///<pid �û��ӽ��̺�
    pthread_t                          thread_id;         ///<�߳�id
    int			                no;			///<�û����
    struct sockaddr_in	         addr;			///<Զ���û���ַ
    int			                fd;				///<�û�����������,��ֵ��ʾδ����
    int			                th_timeout;	///<�ж���ʱ����ֵ(���Ӻ�ʱ��û���յ������ʱ�䷢��������),����Ϊ��λ
    int			                timeout_cnt;	///<���㳬ʱ�ļ�����
    struct timeval 		         start_time;		///<���ӵ���ʼʱ��;
    struct timeval 		         last_cmd_time;	///<���һ���յ������ʱ��
    char			                name[40];		///<���ķ�����û���	
    int			                serv_stat;		///<����״̬,0:û���յ������������ 1:�Ѿ��յ���Ƶ��������,׼���ṩ���� ��ֵ:��û������
    int			                aenc_no;		///<�û��������ӵ���Ƶ���������(Ŀǰֻ��һ����Ƶ������,���Դ˲�����Ч)
    int                                     a_fmt;               ///<��Ƶ��ʽAUDIO_PLAY_TYPE_UPCM,AUDIO_PLAY_TYPE_PCM,
    int                                     a_sam_rate;     ///<��Ƶ������
    socket_attrib_t		         sock_attr;		///<socket����
    stream_recv_info_t            recv_info;          ///<����оƬ���ݵ���Ϣ�ṹ
}aplay_usr_t;



///////////////////////////////////////////////////////////////////////////////////////

#endif

