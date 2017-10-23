/** @file	       net_avstream.c
 *   @brief 	���ղ�����ͻ��˷���������Ƶ��������
 *   @date 	2007.03
 */
#include "rtimage2.h"

#include <commonlib.h>
#include <gtthread.h>
#include <netinfo.h>    
#include <gt_com_api.h>
//#include <venc_read.h>
#include "rtnet_cmd.h"
#include <gt_errlist.h>
#include <gtsocket.h>
#include <errno.h>

/** 
 *   @brief     ��ʼ����������Ƶ���з����ȫ���û�����
 *   @return   0��ʾ�ɹ�,��ֵ��ʾʧ��
 */ 
int init_net_avstream(void)
{
    tcprtimg_svr_t *p=get_rtimg_para();
    av_server_t     *svr=&p->av_server;
    av_usr_t          *usr=NULL;
    int                   i;
    p->mic_gain           = def_mic_gain;                     ///<mic����
    p->audio_gain        = def_audio_gain;	           ///<��Ƶ�������
	p->virdev_num = virdev_get_virdev_number();
	p->max_wan_usrs = p->virdev_num;
	p->max_lan_usrs    = TCPRTIMG_MAX_AVUSR_NO*p->virdev_num - p->max_wan_usrs;	    ///<���������û���
    p->max_aplay_usrs =TCPRTIMG_MAX_APLAY_NO; ///<�����Ƶ���з����û���
    p->av_svr_port       = def_rtstream_port;	        ///<����Ƶ���з���˿�
    p->audio_play_port = def_rtsnd_port;               ///<��Ƶ���з���˿�
    p->th_timeout         = def_svr_timeout;            ///<��ʱ��û�����ݽ���ʱ�жϳ�ʱ��ʱ��
    p->audio_pkt_size   = def_audio_pkt_size;       ///<Ĭ����Ƶ�������ݰ���С
    p->th_drop_p          = def_th_drop_p;		        ///<��������ݶ��ʼ����Ƶp֡����ֵ
    p->th_drop_v          =  p->th_drop_p + 10;          ///<��������ݶ��ʼ����������Ƶ����ֵ
    p->th_drop_a          = MAX_MAP_BUF_FRAMES -p->th_drop_v - 5;    ///<��ʼ�����������ݵ���ֵ
    p->eth0_addr          = get_net_dev_ip("br0");     
    p->eth0_mask         = get_net_dev_mask("br0");
    p->eth1_addr          = get_net_dev_ip("eth1");     
    p->eth1_mask         = get_net_dev_mask("eth1");
    pthread_mutex_init(&svr->l_mutex,NULL);
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_setpshared(&attr,PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&svr->s_mutex,&attr);         
    
    svr->listen_fd=-1;
    
    ///�û���Ϣ��ʼ��
  	for(i=0;i<(TCPRTIMG_MAX_AVUSR_NO*p->virdev_num + 1);i++)
      {
        usr=&svr->av_usr_list[i];
        memset((void*)usr,0,sizeof(av_usr_t));
        pthread_mutex_init(&usr->u_mutex,NULL);
        usr->fd=-1;
        usr->no=i;
        usr->pid=-1;
        usr->thread_id=-1;
        usr->magic=0x55aa;
        
    }


    return 0;
}

/** 
 *   @brief     �ж�һ����ַ�Ƿ�Ϊͬһ���������е�
 *   @param  ��Ҫ�жϵ�ip��ַ
 *   @return   1��ʾ��ͬһ����������,0��ʾ����
 */ 
int is_lan_user(in_addr_t addr)
{
    tcprtimg_svr_t *p=get_rtimg_para();
    in_addr_t          subnet;
    subnet=p->eth0_addr&p->eth0_mask;   ///<��������
    if(subnet==(p->eth0_mask&addr))
        return 1;                                         ///<��ͬһ��������
    else
    {
        if(get_eth_num()>1)
        {
            subnet=p->eth1_addr&p->eth1_mask;
            if(subnet==(p->eth1_mask&addr))
                return 1;                                         ///<��ͬһ��������            
        }
        return 0;                                                ///<����ͬһ��������
    }
}
int is_usr_valid(av_usr_t *usr)
{
	
	if(usr->used>0)
		return 1;
	else
		return 0;



}
av_usr_t * get_free_usr(int * usr_index)
{
	int i;
	tcprtimg_svr_t *p=get_rtimg_para();
	int total=p->max_lan_usrs+p->max_wan_usrs+1;
	av_usr_t *usr=NULL;
	
    av_server_t          *svr=&p->av_server;
    for(i=0;i<total;i++)  //��һ�����ڴ���æ
    {
        usr=&svr->av_usr_list[i];
		if(usr->used==0)
		{
			*usr_index=i;
			break;
		}
    }
	if(i==total)
	{
		usr=NULL;
	}
	return usr;

}

av_usr_t * find_usr(int index)
{
	int i;
	tcprtimg_svr_t *p=get_rtimg_para();
	int total=p->max_lan_usrs+p->max_wan_usrs+1;
	av_usr_t *usr=NULL;
	if(index>=total)
	{
		printf("WRONG USR INDEX\n");
		return NULL;
	}
	av_server_t 		 *svr=&p->av_server;
	usr=&svr->av_usr_list[index];
	return usr;


}
/** 
 *   @brief     ��ʼ������Ƶ�û����ݽṹ(�����������ӵ���ʱ)
 *   @param  usr �û���Ϣ�ṹָ��
 *   @return   ��
 */     
static void init_av_usr_info(av_usr_t *usr)
{
    tcprtimg_svr_t *p=get_rtimg_para();
	pthread_mutex_init(&usr->u_mutex,NULL);
    pthread_mutex_lock(&usr->u_mutex);
    
    memset(&usr->addr,0,sizeof(struct sockaddr_in));
    usr->fd=-1;
    usr->th_timeout=p->th_timeout;
    usr->timeout_cnt=0;
    usr->th_drop_p=p->th_drop_p;
    usr->th_drop_v=p->th_drop_v;
    usr->th_drop_a=p->th_drop_a;
    gettimeofday(&usr->start_time,NULL);
    memset(&usr->last_cmd_time,0,sizeof(struct timeval));
    sprintf(usr->name,"%s","newuser");
    usr->serv_stat=0;
    usr->venc_no=-1;
    usr->aenc_no=-1;
	//zsk add
	usr->stream_idx=-1;
	usr->used=0;
    memset(&usr->sock_attr,0,sizeof(socket_attrib_t));
    memset(&usr->send_info,0,sizeof(stream_send_info_t));
    usr->send_info.send_ack_flag=0;
    pthread_mutex_unlock(&usr->u_mutex);
}

/** 
 *   @brief     ��һ���û���ַ��Ϣ�����û��б�
 *   @param  usr ����û���Ϣ�Ľṹָ��
 *   @param  guest ���û���ip��ַ�Ͷ˿ں���Ϣ
 *   @return   0��ʾ�ɹ� 1��ʾæ ��ֵ��ʾ����
 *   @warnning ���ô˺���ǰ������Ӧ���Ѿ�ȡ����ý������s_mutex
 */ 
static int add_av_usr_list(av_usr_t *usr,struct sockaddr_in *guest)
{

    tcprtimg_svr_t *p=get_rtimg_para();
    av_server_t     *svr=&p->av_server;
    if((usr==NULL)||(guest==NULL))
        return -EINVAL;       

    if(is_lan_user(guest->sin_addr.s_addr))
    {
        svr->lan_usrs++;                                                         ///<�������û�����1
        if(svr->lan_usrs>p->max_lan_usrs)
        {
            svr->lan_usrs--;
            return 1;
        }
    }
    else
    {
        svr->wan_usrs++;                                                        ///<�������û�����1
        if(svr->wan_usrs>p->max_wan_usrs)
        {
            svr->wan_usrs--;
            return 1;
        }
    }
    memcpy(&usr->addr,guest,sizeof(struct sockaddr_in));    ///<�����û���Ϣ
    return 0;
    
}
/** 
 *   @brief     ������Ƶ���з�����ɾ��ָ�����û�
 *   @param  usr ����û���Ϣ�Ľṹָ��
 *   @return   0��ʾ�ɹ�  ��ֵ��ʾ����
 *   @warnning ���ô˺���ǰ������Ӧ���Ѿ�ȡ����ý������s_mutex
 */ 
static int del_av_usr_list(av_usr_t *usr)
{
    tcprtimg_svr_t *p=get_rtimg_para();
    av_server_t     *svr=&p->av_server;
    if(usr==NULL)
        return -EINVAL;
    if(is_lan_user(usr->addr.sin_addr.s_addr))
    {
        svr->lan_usrs--;
        if(svr->lan_usrs<0)
        {
            //showbug();
            //logbug();
        }
    }
    else
    {
        svr->wan_usrs--;
        if(svr->wan_usrs<0)
        {
            //showbug();
            //logbug();
        }
    }
    ///ɾ���û���Ϣ
    pthread_mutex_lock(&usr->u_mutex);
    usr->serv_stat=0;
	usr->fd=-1;
    usr->venc_no=-1;
    usr->aenc_no=-1;
	usr->stream_idx=-1;
	usr->used=0;
	memset(&usr->addr,0,sizeof(struct sockaddr_in));
    pthread_mutex_unlock(&usr->u_mutex);
    return 0;
}

/** 
 *   @brief     ��������Ƶ���з������Ӧ��Ϣ
 *   @param  fd Ŀ��������
 *   @param  result ���صĴ������
 *   @param  ��Ҫ���ڷ�����Ϣanswer_data�������Ϣ
 *   @param  datalen buf����Ч���ݵĸ���
 *   @return   ��ֵ��ʾ����,�Ǹ���ʾ�ɹ�
 */ 
int send_rtstream_ack_pkt(int fd,WORD result,char* buf,int datalen)
{
	int ret;
	DWORD	                            sendbuf[1024];                                              ///<���Ҫ�������ݵĻ�����
	struct gt_pkt_struct            *send=(struct gt_pkt_struct *)sendbuf;           ///<Ҫ���͵����ݵĽṹָ��
	struct gt_usr_cmd_struct    *cmd=(struct gt_usr_cmd_struct*)send->msg;///<����ṹָ��
	struct viewer_subscribe_cmd_answer_struct *answer=(struct viewer_subscribe_cmd_answer_struct  *)cmd->para;///<��Ӧָ��
	int answerlen;
	
	if(fd<0)
		return -EINVAL;
	cmd->cmd=VIEWER_SUBSCRIBE_VIDEO_ANSWER;
	cmd->en_ack=0;
	cmd->reserve0=0;
	cmd->reserve1=0;
	answer=(struct viewer_subscribe_cmd_answer_struct  *)cmd->para;
	ret=get_devid(answer->dev_id);
	answer->result=result;
       ret=1;           //����0д������
	memcpy((BYTE*)&answer->video_trans_id,(BYTE*)&ret,4);
	if(buf!=NULL)
	{
			answerlen=datalen;
			memcpy(answer->answer_data,buf,answerlen);
	}
	else
	{
		switch(result)
		{
			case 0:
				
			break;
			case ERR_DVC_BUSY:	
				answerlen=0;
			break;
			default:
				answerlen=0;
			break;
		}
	}
	answer->answer_data_len=answerlen;
	cmd->len=sizeof(struct viewer_subscribe_cmd_answer_struct)-sizeof(answer->answer_data)+answerlen+6;	
	ret=gt_cmd_send_noencrypt(fd,send,cmd->len+2,NULL,0);
	return ret;
}

/**
  *   @brief	�ж���������Ƶ��guid�Ƿ�Ϸ�
  *   @param	cmd_dev_id��������Ƶ�������guid
  *	@param	cmd_addr��������Ƶ���豸ip�ĵ�ַ
  *	@return	�ɹ����������豸�ţ�ʧ�ܷ��ظ�ֵ
*/
int is_guid_valid(unsigned char *cmd_dev_id, struct sockaddr_in *cmd_addr)
{
	int virdev_num;
	unsigned char virdev_guid[10][20];
	int guid_len;
	int i;


	virdev_num = virdev_get_virdev_number();		
	for(i=0; i<virdev_num; i++)
	{
		guid_len = virdev_get_devid(i, (unsigned char *)virdev_guid[i]);
		///guidƥ��Ļ������ش��豸��
		if(memcmp(virdev_guid[i], cmd_dev_id, guid_len) == 0)
		{
			return i;
		}
		else
		{
			///������һ���豸�Բ�ƥ�䣬��˵��guid���Ϸ�����ӡ�˳�
			if(i == (virdev_num-1))
			{
				printf("�յ�%s�Ķ�������,GUID��ƥ��->\n",inet_ntoa(cmd_addr->sin_addr));
				gtloginfo("�յ�%s�Ķ�������,GUID��ƥ��->\n",inet_ntoa(cmd_addr->sin_addr));
				for(i=0; i<virdev_num; i++)
				{
					printf("dev_guid=%02x%02x%02x%02x%02x%02x%02x%02x\n",virdev_guid[i][7],virdev_guid[i][6],virdev_guid[i][5],virdev_guid[i][4],virdev_guid[i][3],virdev_guid[i][2],virdev_guid[i][1],virdev_guid[i][0]);
					gtloginfo("dev_guid=%02x%02x%02x%02x%02x%02x%02x%02x\n",virdev_guid[i][7],virdev_guid[i][6],virdev_guid[i][5],virdev_guid[i][4],virdev_guid[i][3],virdev_guid[i][2],virdev_guid[i][1],virdev_guid[i][0]);
				}
      		  		
		      		printf("reg_guid=%02x%02x%02x%02x%02x%02x%02x%02x\n",*(cmd_dev_id+7),*(cmd_dev_id+6),*(cmd_dev_id+5),*(cmd_dev_id+4),*(cmd_dev_id+3),*(cmd_dev_id+2),*(cmd_dev_id+1),*(cmd_dev_id+0));
				gtloginfo("reg_guid=%02x%02x%02x%02x%02x%02x%02x%02x\n",*(cmd_dev_id+7),*(cmd_dev_id+6),*(cmd_dev_id+5),*(cmd_dev_id+4),*(cmd_dev_id+3),*(cmd_dev_id+2),*(cmd_dev_id+1),*(cmd_dev_id+0));
				return -2;
			}
		}
	}
	
	return 0;
}

/** 
 *   @brief     �������豸�н�һ���û���ַ��Ϣ�����û��б�
 *   @param  virdev_no�����豸��
 *   @param  guest ���û���ip��ַ�Ͷ˿ں���Ϣ
 *   @return   0��ʾ�ɹ� 1��ʾæ ��ֵ��ʾ����
 *   @warnning ���ô˺���ǰ������Ӧ���Ѿ�ȡ����ý������s_mutex
 */ 
static int add_virav_usr(struct sockaddr_in *guest, int virdev_no)
{
    tcprtimg_svr_t *p=get_rtimg_para();
    av_server_t     *svr=&p->av_server;
    if(guest==NULL)
        return -EINVAL;       
    if(is_lan_user(guest->sin_addr.s_addr))
    {
	if(virdev_no==0)
	{
		svr->vir0_lan_users++;
		//printf("svr->vir0_lan_users++  in  add_virav_usr  is  %d\n",svr->vir0_lan_users);///aahtest
		if(svr->vir0_lan_users > (p->max_lan_usrs/2))
		{
			//svr->vir0_lan_users--;
			return 1;
		}
	  }
	  else
	  {
		svr->vir1_lan_users++;
		//printf("svr->vir1_lan_users++  in  add_virav_usr  is  %d\n",svr->vir1_lan_users);///aahtest
		if(svr->vir1_lan_users > (p->max_lan_usrs/2))
		{
			//svr->vir1_lan_users--;
			return 2;
		}
	  }
    }
    else
   {
	if(virdev_no==0)
       {
		svr->vir0_wan_users++;
		if(svr->vir0_wan_users > (p->max_wan_usrs/2))
		{
			//svr->vir0_wan_users--;
			return 1;
		}
	}
	else
	{
		svr->vir1_wan_users++;
		if(svr->vir1_wan_users > (p->max_wan_usrs/2))
		{
			//svr->vir1_wan_users--;
			return 2;
		}
	}
    }
    return 0;
    
}
/** 
 *   @brief     �������豸����Ƶ���з�����ɾ��ָ�����û�
 *   @param  virdev_no�����豸��
 *   @param  usr����Ƶ�����û���Ϣ�ṹָ��
 *   @return   0��ʾ�ɹ� 1��ʾæ ��ֵ��ʾ����
 *   @warnning ���ô˺���ǰ������Ӧ���Ѿ�ȡ����ý������s_mutex
 */ 
static int del_virav_usr(av_usr_t *usr, int virdev_no)
{
    tcprtimg_svr_t *p=get_rtimg_para();
    av_server_t     *svr=&p->av_server;
    if(usr==NULL)
        return -EINVAL;
    if(is_lan_user(usr->addr.sin_addr.s_addr))
    {
	if(virdev_no==0)
	{
		svr->vir0_lan_users--;
		if(svr->vir0_lan_users < 0)
		{
			//showbug();
          	//	logbug();
		}
	}
       else
       {
		svr->vir1_lan_users--;
		if(svr->vir1_lan_users < 0)
		{
			//showbug();
          	//	logbug();
		}
	}
    }
    else
    {
	if(virdev_no==0)
	{
		svr->vir0_wan_users--;
		if(svr->vir0_wan_users < 0)
		{
			//showbug();
            //		logbug();
		}
	}
       else
       {
		svr->vir1_wan_users--;
		if(svr->vir1_wan_users < 0)
		{
			//showbug();
            //		logbug();
		}
	}
    }

    ///ɾ���û���Ϣ
    pthread_mutex_lock(&usr->u_mutex);
    usr->fd=-1;
    usr->serv_stat=-1;
    usr->venc_no=-1;
    usr->aenc_no=-1;
    pthread_mutex_unlock(&usr->u_mutex);
    return 0;
}


/** 
 *   @brief     ���������û�����������Ƶ�ϴ���������
 *   @param  usr �յ�������û��ṹָ��
 *   @param  cmd �������յ�������ṹָ��
 *   @param  virdev_ret�����û���	///<for 3022
 *   @return   0��ʾ���غ����ִ��,��ֵ��ʾ���غ���Ҫ�Ͽ�����
 */ 
static int proc_video_subscrib_cmd(IO av_usr_t *usr,IN struct gt_usr_cmd_struct* cmd, OUT int *virdev_ret )
{
    char                 *ptemp=NULL;
    int                    ret;
    int			virdev_num=virdev_get_virdev_number();		///<�����豸����
    tcprtimg_svr_t *p=get_rtimg_para();


    struct viewer_subscribe_cmd_struct *subscribe=(struct viewer_subscribe_cmd_struct*)cmd->para;
    if((cmd->cmd!=VIEWER_SUBSCRIBE_D1_VIDEO)&&(cmd->cmd!=VIEWER_SUBSCRIBE_VIDEO))
    {
        return -EINVAL;
    }

	ret=is_guid_valid(subscribe->dev_id, &usr->addr);
	switch(ret)
	{
		case 0:
			if(virdev_num==1)
			{

    			if(p->multichannel==1)//�µ�
    			{
					if(subscribe->channel<0||subscribe->channel>=MAX_VIDEO_ENCODER)
						usr->venc_no=0;
					else
						usr->venc_no=subscribe->channel;
					printf("\n\nsubscribe->channel=%d\n",subscribe->channel);
					
				}
    			else //�ϵ�
				{
					if(cmd->cmd==VIEWER_SUBSCRIBE_D1_VIDEO)
					{
						usr->venc_no=1;
					}
					else
					{

						usr->venc_no=0; 
					}


				} 
			}
			break;
		default:
			///guid��ƥ��
			send_rtstream_ack_pkt(usr->fd,ERR_DVC_INVALID_REQ,"invalid req guid\n",strlen("invalid req guid\n")+1);
        		usleep(500000);
        		return -2;
	}

	printf     ("�յ� %s �Ķ���ý��������(venc=%d)account=%s audioflag=0x%x \n",inet_ntoa(usr->addr.sin_addr),usr->venc_no,subscribe->account,(int)subscribe->audio_flag);
	gtloginfo("�յ� %s �Ķ���ý��������(venc=%d)account=%s audioflag=0x%x \n",inet_ntoa(usr->addr.sin_addr),usr->venc_no,subscribe->account,(int)subscribe->audio_flag);


    pthread_mutex_lock(&usr->u_mutex);
	
    ret=get_audio_num();
    if((subscribe->audio_flag==0x55aa)||(ret<=0))
    {
        usr->aenc_no=-1;                                     ///<����Ҫ����
        usr->serv_stat=1;                           
    }
	else
	{
		if(p->multichannel==1)//�µ�
		{
			usr->aenc_no=subscribe->channel;
		
				
		}
		else
		{
			//usr->aenc_no=get_net_scr_ch(usr->no);                        
		
			usr->aenc_no=0;

		}
		usr->serv_stat=3;
		printf("set serv_stat=3\n");
		
	}
    strncpy(usr->name,subscribe->account,20);   ///<�û����Ϊ19
    usr->name[19]='\0';
    ptemp=strchr(usr->name,'\n');
    if(ptemp!=NULL)
    {///ȥ���س���
        *ptemp='\0';
    }
    pthread_mutex_unlock(&usr->u_mutex);
        
    return 0;
}
/** 
 *   @brief     ���������û�����������Ƶ����
 *   @param  usr �յ�������û��ṹָ��
 *   @param  cmd �������յ�������ṹָ��
 *   @param  virdev_ret�����û���	///<for 3022
 *   @return   0��ʾ���غ����ִ��,��ֵ��ʾ���غ���Ҫ�Ͽ�����
 */ 
static int process_net_av_cmd(av_usr_t *usr,struct gt_usr_cmd_struct* cmd, OUT int *virdev_ret)
{
    int                    ret;

    if((usr==NULL)||(cmd==NULL))
        return -EINVAL;

    gettimeofday(&usr->last_cmd_time,NULL);                                 ///<���յ������ʱ��
    switch(cmd->cmd)
    {
        case VIEWER_SUBSCRIBE_D1_VIDEO:
        case VIEWER_SUBSCRIBE_VIDEO:
            ret=proc_video_subscrib_cmd(usr,cmd,virdev_ret); 
        break;
        case VIEWER_UNSUBSCRIBE_VIDEO:
                printf("usr:%d receive VIEWER_UNSUBSCRIBE_VIDEO cmd from %s\n",usr->no,inet_ntoa(usr->addr.sin_addr));	
                gtloginfo("usr:%d �յ� %s ���˶�ý�������� \n",usr->no,inet_ntoa(usr->addr.sin_addr));
                usr->serv_stat=-2;
        break;
        default:
            printf("rtimage recv unknow net cmd:0x%04x\n",cmd->cmd);
            ret=0;
        break;
    }
    
    return ret;
}


/** 
 *   @brief     ���ղ����������û�������Ƶ������߳�
 *   @return   0��ʾ�ɹ�,��ֵ��ʾʧ��
 */ 
int write_err_flag=0;
static void *rtnet_av_cmd_thread(void *para)
{
    tcprtimg_svr_t      *p=get_rtimg_para();
    av_server_t          *svr=&p->av_server;
    av_usr_t               *usr=(av_usr_t*)para;
    struct sockaddr_in guest_addr;
    fd_set                   read_fds;
    int                        accept_fd=-1,maxfd=0;
    int                        addrlen;
    int                        ret;
    int                        sel;
    struct timeval	    timeout;
    unsigned long        net_rec_buf[512];		///<�������緢�������ݵĻ�����
     int				virdev_no=-1;			///<�����豸��
    struct gt_pkt_struct* recv=(struct gt_pkt_struct*)net_rec_buf;
	
    if(usr==NULL)
    {
        printf     ("rtnet_av_cmd_thread para=NULL exit thread!!\n");
        gtloginfo("rtnet_av_cmd_thread para=NULL exit thread!!\n");
        pthread_exit(NULL);
    }
	
    printf     (" start rtnet_av_cmd_thread user:%d...\n",usr->no);
    gtloginfo(" start rtnet_av_cmd_thread user:%d...\n",usr->no);
	pthread_mutex_init(&usr->u_mutex,NULL);
    while(1)
    {
    	FD_ZERO(&read_fds);
        ///��������
        addrlen=sizeof(struct sockaddr_in);

		do
		{
			//printf("waitting for usr free usrno[%d]\n",usr->no);
			sleep(1);


		}while(is_usr_valid(usr));
        if(pthread_mutex_trylock(&svr->l_mutex)==0)
        {
        	printf("lockin usr[%d]\n",usr->no);
			pthread_mutex_lock(&usr->u_mutex);
       		usr->used=1;
       		pthread_mutex_unlock(&usr->u_mutex);
        	accept_fd = accept(svr->listen_fd, (void*)&guest_addr, &addrlen);     ///<����������    		
        }
        else
        {
            continue;
        }
        if((accept_fd<0)&&(errno==EINTR)) 
        {
        	printf("accept_fd<0\n");
            pthread_mutex_unlock(&svr->l_mutex);
            continue;
        }
         else if(accept_fd<0) 
        { 
        	printf("rtnet_av_cmd_thread accept errno=%d:%s!!\n",errno,strerror(errno));
        	gtlogerr("rtnet_av_cmd_thread accept errno=%d:%s!!\n",errno,strerror(errno));
        	pthread_mutex_unlock(&svr->l_mutex);
         	continue; 
        } 

        printf     ("come a new net_av guest:%s \n",inet_ntoa(guest_addr.sin_addr));
        gtloginfo("come a new net_av guest:%s \n",inet_ntoa(guest_addr.sin_addr));
		init_av_usr_info(usr);
		pthread_mutex_lock(&svr->s_mutex);

		ret=add_av_usr_list(usr,&guest_addr);
		pthread_mutex_unlock(&svr->s_mutex);


		pthread_mutex_unlock(&svr->l_mutex);
		net_set_linger(accept_fd,0);
		 

        if(ret!=0)
        {
            if(ret<0)
            {///����
                send_rtstream_ack_pkt(accept_fd,ERR_DVC_INTERNAL,"device internal  error\n",strlen("device internal  error\n")+1);
                sleep(1);
                close(accept_fd);
                gtloginfo("�ڲ�����,�޷����ͼ�����!\n");
                continue;
            }
            else
            { ///æ
                send_rtstream_ack_pkt(accept_fd,ERR_DVC_BUSY,"device busy\n",strlen("device busy\n")+1);
                sleep(1);
                close(accept_fd);
                gtloginfo("�豸æ,�޷����%s��ͼ�����ӷ���!\n",inet_ntoa(guest_addr.sin_addr));
                continue;
            }
            
        }

        ///set socket
        usr->fd=accept_fd; 
        usr->sock_attr.send_buf_len=200*1024;///>��ʼֵΪ128k,�������ݹ����з��ֲ��㻹��������
        ret=net_set_tcp_sendbuf_len(usr->fd,usr->sock_attr.send_buf_len);
        if(ret<0)
        {
            gtlogerr("net_set_tcp_sendbuf_len ret=%d!\n",ret);
            printf    ("net_set_tcp_sendbuf_len ret=%d!\n",ret);
            
            //showbug();
            //logbug();
            //TODO ������
        }

        usr->sock_attr.send_buf_len=net_get_tcp_sendbuf_len(usr->fd);
		/*
        ret=net_activate_keepalive(usr->fd);
        ret=net_set_recv_timeout(usr->fd,3);
        ret=net_set_nodelay(usr->fd);        
        ret=net_set_linger(usr->fd,0);
		*/
		ret=set_tcp_socket_attr(usr->fd,3);

        	printf("tcprtimage �ɹ�������һ������Ƶ���з�������(u:%d,w:%d,l:%d):%s\n",
            usr->no,svr->wan_usrs,svr->lan_usrs,inet_ntoa(usr->addr.sin_addr));
        	gtloginfo("tcprtimage �ɹ�������һ������Ƶ���з�������(u:%d,w:%d,l:%d):%s\n",
            usr->no,svr->wan_usrs,svr->lan_usrs,inet_ntoa(usr->addr.sin_addr));
        
        while(1)
        {
			//zsk add
			if(write_err_flag == 1)
			{
				gtlogerr("write data error! disconnect peer\n");

				write_err_flag = 0 ;
				break;	
			}
			//

            if((usr->fd>0)&&(accept_fd>0))
            {
                FD_SET(accept_fd,&read_fds);
            }
            else
            {
                break;//�Ѿ����Ͽ�
            }
            if(usr->serv_stat<0)
            {
                break;
            }
            timeout.tv_sec=1;
            timeout.tv_usec=0;

			if(accept_fd>maxfd)
			  maxfd=accept_fd;

            sel=select(maxfd+1,&read_fds,NULL,NULL,&timeout);
            if(sel==0&&usr->serv_stat>0)
            {

            	printf("select accept_fd û�б仯\n");
                continue;
            }
			else if(sel==0&&usr->serv_stat<0) //����������ֻ���Ӿ͹���
				break;
            
            else if(sel<0)
            {
            	//printf("here4\n");
                if(errno!=EINTR)
                    sleep(1);
                continue;
            }
			if(usr->fd<0)
            {
            	//printf("here5\n");
				gtlogerr("usr fd <0  error!\n");
                break;
            }
				errno=0;
				
            if(FD_ISSET(accept_fd,&read_fds))
            {
            	
                net_set_noblock(accept_fd,0);
                ret=gt_cmd_pkt_recv(accept_fd,recv,sizeof(net_rec_buf),NULL,0);		
                net_set_noblock(accept_fd,1);
                if(ret>=0)
                {		
                    pthread_mutex_lock(&usr->u_mutex);
                    usr->timeout_cnt=0;     ///<�����ʱ������
                    pthread_mutex_unlock(&usr->u_mutex);
                    ret=process_net_av_cmd(usr,(struct gt_usr_cmd_struct*)( recv->msg), &virdev_no);
                    if(ret<0)
                    	    break;
                }	
                else
                {
                    if(ret==-EAGAIN)
                    {///�������ʱ
                        printf     ("process_net_av_cmd �յ�һ��������������\n");
                        gtloginfo("process_net_av_cmd �յ�һ��������������\n");
                        continue;
                    }
                    else if(ret==-EINTR)
                    {
						continue;
                    }
                    else if(ret==-ETIMEDOUT)
                    {
                        printf     ("ETIMEDOUT ����:�ͻ�:%s��ͼ�����ӳ�ʱ\n",inet_ntoa(usr->addr.sin_addr));
                        gtloginfo("ETIMEDOUT ����:�ͻ�:%s��ͼ�����ӳ�ʱ\n",inet_ntoa(usr->addr.sin_addr));						
                        break;
                    }
                    else
                    {
                        if(ret==-140)
                        {
                            printf     ("ͼ������:%d(%s)���ر�,ret=%d\n",usr->fd,inet_ntoa(usr->addr.sin_addr),ret);
                            gtloginfo("ͼ������:%d(%s)���ر�,ret=%d\n",usr->fd,inet_ntoa(usr->addr.sin_addr),ret);
                        }
                        else
                        {
                            printf     ("ͼ������:%d(%s)�Ͽ�,ret=%d\n",usr->fd,inet_ntoa(usr->addr.sin_addr),ret);
                            gtloginfo("ͼ������:%d(%s)�Ͽ�,ret=%d\n",usr->fd,inet_ntoa(usr->addr.sin_addr),ret);
                        }
                        break;
                    }
                }
            }
            else
            {
                printf     ("FD_ISSET(accept_fd,&read_fds)!=1!!!\n");
                gtloginfo("FD_ISSET(accept_fd,&read_fds)!=1!!!\n");
                break;               
            }

			FD_CLR(accept_fd,&read_fds);
        }
        ///�Ͽ�����
        pthread_mutex_lock(&svr->s_mutex);
        del_av_usr_list(usr);                               ///<ɾ���û�
        pthread_mutex_unlock(&svr->s_mutex);
		FD_CLR(accept_fd,&read_fds);
        close(accept_fd);                                       ///<�ر���������
        accept_fd=-1; 
    }
    return NULL;
}

/** 
 *   @brief     ����������������Ƶ���з����socket�Լ��̳߳�
 *   @return   0��ʾ�ɹ�,��ֵ��ʾʧ��
 */ 
int create_rtnet_av_servers(void)
{
    tcprtimg_svr_t *p=get_rtimg_para();
    av_server_t     *svr=&p->av_server;
    av_usr_t          *usr=NULL;
    int                   fd;
    int                   i;

    fd = create_tcp_listen_port(INADDR_ANY,p->av_svr_port); ///<������������Ƶ����socket
    if(fd<0)
    {
        printf("can't create socket port:%d:%s exit !\n",p->av_svr_port,strerror(-fd));
        printf("can't create socket port:%d:%s exit !\n",p->av_svr_port,strerror(-fd));
        exit(1);
    }
    svr->listen_fd=fd;
    net_activate_keepalive(svr->listen_fd);          ///<̽��Է��Ͽ�
    listen(svr->listen_fd,TCPRTIMG_MAX_AVUSR_NO*p->virdev_num);            ///<��������������

    for(i=0;i<(p->max_lan_usrs+p->max_wan_usrs+1);i++)  //��һ�����ڴ���æ
    {
        usr=&svr->av_usr_list[i];
        gt_create_thread(&usr->thread_id, rtnet_av_cmd_thread, (void*)usr);
    }
    return 0;
}


