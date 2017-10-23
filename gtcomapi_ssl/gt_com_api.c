/*	Ƕ��ʽ��Ƶ��������������ͨѶ�ĵײ㺯����
 *		
 *		 2005.1
 *    ���������ṩ��ʵ��Ƕ��ʽϵͳ��Զ�̼����ͨѶ�ĵײ�ӿڣ�
 *    �ϲ��Ӧ�ýӿڼ����ݽṹ�����ڱ����ʵ�ַ�Χ
 *    ���������ṩ�Ĺ�����
 *    1.���������ݱ�����GT1000��ͨѶЭ���ʽ���д�������ܣ�ǩ����crc32У�飬֮���͸�ָ����tcp�˿�
 *    2.��ָ����tcp�˿ڽ������ݰ�������crcУ�飬��֤ǩ���������ı任�����ķ��ظ����ý���                        
 *    ���յ�ʱ�򱾿����ͨ��Э��ͷ�еĶ����ж����ݰ���ʹ�õ�ǩ����ʽ�����ܷ�ʽ��
 *    ���͵�ʱ����ݲ���ѡ��ǩ����ʽ�����ܷ�ʽ��	
 */
/*
 * �������ݽṹ����
 */
#define  _CRT_SECURE_NO_WARNINGS
#define  _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <errno.h>
#include <gtsocket.h>
#include <gtlog.h>
#ifdef _WIN32
//	#pragma comment(lib,"gtsocket.lib")
#define get_unaligned(ptr)        (*(ptr))
#define put_unaligned(val, ptr) ((void)( *(ptr) = (val) ))
#else
//#include <asm/unaligned.h> 	//�����ֽڶ���
//get_unaligned(ptr); 
//put_unaligned(val, ptr); 
#endif
//#define put_unaligned(val,ptr)	*ptr=val
//#define get_unaligned(ptr)	*ptr


static inline unsigned int __get_unaligned_le32(const unsigned char *p)
{
        return p[0] | p[1] << 8 | p[2] << 16 | p[3] << 24;
}
static inline void __put_unaligned_le16(unsigned short val, unsigned char *p)
{
        *p++ = val;
        *p++ = val >> 8;
}
static inline void __put_unaligned_le32(unsigned int val, unsigned char *p)
{
        __put_unaligned_le16(val >> 16, p + 2);
        __put_unaligned_le16(val, p);
}

static inline unsigned int __get_unaligned_be32(const unsigned char *p)
{
        return p[0] << 24 | p[1] << 16 | p[2] << 8 | p[3];
}
static inline void __put_unaligned_be16(unsigned short val, unsigned char *p)
{
        *p++ = val >> 8;
        *p++ = val;
}
static inline void __put_unaligned_be32(unsigned int val, unsigned char *p)
{
        __put_unaligned_be16(val >> 16, p);
        __put_unaligned_be16(val, p + 2);
}

#ifndef __ARMEB__
#define put_unaligned(val, ptr) ({                                 \
        void *__gu_p = (ptr);                                           \
        __put_unaligned_le32((unsigned int)(val), __gu_p);            \
         (void)0; })

#define get_unaligned(ptr) ({                                 \
        __get_unaligned_le32(ptr);            })

#else
#define put_unaligned(val, ptr) ({                                 \
        void *__gu_p = (ptr);                                           \
        __put_unaligned_be32((unsigned int)(val), __gu_p);        })
#define get_unaligned(ptr) ({                                 \
        __get_unaligned_be32(ptr);            })

#endif


#include "gt_com_api.h"

#ifdef USE_SSL
#include "envelop/gt_env.h"
#endif
/***************Э��ͷ��β����*******************************/
//�������Լ�ʹ�õĶ���
#define GT_SYNC_HEAD0	'@'		//Э��ͷ��־0
#define GT_SYNC_HEAD1	0x55	//Э��ͷ��־1	
#define GT_SYNC_HEAD2	0xaa	//Э��ͷ��־2
#define GT_SYNC_HEAD3	0x55	//Э��ͷ��־3

#define GT_PKT_TAIL	0x66886623	//Э��β��־
/********************************************************************/

#ifdef USE_SSL
static int get_addon_len(BYTE auth_type,BYTE encrypt_type);
#endif

#define EBUF_ADDON			512	//�����û�����Ӧ���Ƚ��ܻ�������ĳ���

static  int print_buffer(unsigned char *buf,int len)
{
    int i;
    for(i=0;i<len;i++)
	printf("%02x,",buf[i]);
    return 0;
}
int  gt_cmd_pkt_recv_env(int fd,struct gt_pkt_struct *recv,int buf_len,void *tempbuf,int flag,int *msg_type,int *encrypt_type);
extern unsigned long tab_crc32 (const unsigned char *buf, unsigned int len);
//�����ӿڶ���
/**********************************************************************************************
 * ������	:gt_cmd_pkt_send()
 * ����	:��һ�����İ���ָ���ļ���,ǩ����ʽ�γ��������ݰ�,���͵�ָ����socket
 * ����	:fd:Ŀ��socket�ļ�������
 *			 send:ָ��Ҫ���͵�����Ļ�������ָ��
 *			 msg_len:Ҫ���͵Ļ������е���Ч��Ϣ�����ֽ���(msg�ֶ�,������session,crc,tail�ȵĳ���)
 *			 tempbuf:(����)������ʱ��NULL�Ϳ�����
 *			 flag:(����)������ʱ��0�Ϳ�����
 *			 msg_type:�������ݰ�����֤����,0��ʾ����Ҫ��֤,MSG_AUTH_SSL��ʾʹ�������ŷ⣬����ֵ����
 *			 encrypt_type:�������ݰ��ļ�������,���嶨���"���ݼ�������ѡ��"
 * ����ֵ	:��ֵ��ʾ���ͳ�ȥ���ֽ�������ֵ��ʾ����
 * ע		: ������Ӧ�������㹻��Ļ�����������Ҫ���͵�����,��������ֱ��ʹ�øû��������и������,�任�Ȳ���
 *	  		  ������ֻ�����send->msg�ֶμ��ɣ������ֶ��ɿ⺯���Լ����
 *	 		 send�Ի���������ʼ��ַ������4�ֽڶ���ģ������ϲ�Ӧ�ô�����ܻ������
 **********************************************************************************************/
int  gt_cmd_pkt_send(int fd,struct gt_pkt_struct *send,int msg_len,void *tempbuf,int flag,BYTE msg_type,BYTE encrypt_type)
{
	struct gt_pkt_struct *s=NULL;
	struct gt_pkt_head *head;
	DWORD	*ph,xor32,crcval;
	int	ret,crclen;
	WORD addon;
	void *ebuf;
#ifdef USE_SSL
	int plen, elen;
	DWORD tbuf[256];//sizeof buf;
	void *pbuf;
#endif
	int send_msglen;

	BYTE *buf;
	int mallocflag=0;//�����ڴ��־
	if(((long)send%4)!=0)
		printf("unaligned !\n");
	
	if(fd<0)
	{
		printerr("\ngt_cmd_pkt_send get fd=%d!error\n",fd);
		return -EINVAL;
	}
	if(send==NULL)
	{
		printerr("\ngt_cmd_pkt_send get NULL sendbuf!error\n");
		return -EINVAL;
	}
	ebuf=NULL;

	s=send;
	send_msglen=msg_len;
	buf=(char*)s->msg;
	if(((send_msglen)%4)!=0)   //	4�ֽڶ���
	{
		addon=4-(send_msglen)%4;
		memset((char*)(buf+send_msglen),0,addon);
	}
	else
	{
		addon=0;
	}
	send_msglen+=addon;
	addon=0;
	
	if(msg_type==GT_CMD_NO_AUTH)
	{//����Ҫʹ�������ŷ⣬������


	}
	else
	{
#ifdef USE_SSL
	    if(msg_type==MSG_AUTH_SSL)
	    {//���������ŷ�

		
	    		//s=(struct gt_pkt_struct*)calloc(4,(EBUF_ADDON+msg_len)/4);//,4);
			/*tbuf=malloc(EBUF_ADDON+msg_len);//FIXME ����һ���Ƿ���Ҫ��̬�����ڴ�
			if(tbuf==NULL)
			{
				printerr("gt_com_api gt_cmd_pkt_send not enough memory\n");
				return -1;
			}
			*/	
			if((EBUF_ADDON+msg_len)<sizeof(tbuf))
			{
				s=(struct gt_pkt_struct *)(tbuf);
				mallocflag=0;
			}
			else
			{
				ret=posix_memalign(&s,sizeof(unsigned long),EBUF_ADDON+msg_len);
				if(ret!=0)
				{
					printf("gt_cmd_pkt_send posix_memalign failed ret=%d :%s!!\n",ret,strerror(ret));
					return -ret;
				}
				else
				{
					mallocflag=1;
					printf("gt_cmd_pkt_send posix_memalign s=%x ret=%d\n",s,ret);
				}

			}
			
			ebuf=(void *)s->msg;
			elen=EBUF_ADDON+send_msglen-sizeof(s->head);
			memcpy((void*)s,(void*)&send->head,sizeof(send->head));
			
			pbuf=(void*)send->msg;
			plen=send_msglen;
			ret=env_pack(encrypt_type,pbuf,plen,ebuf,&elen);
			if(ret!=0)
			{
				printerr("gt_com_api gt_cmd_pkt_send env_pack error %d\n",ret);
                            gtlogerr("gt_cmd_pkt_send  ���ܴ��� !env_pack ret=%d etype=%d elen=%d\n",ret,encrypt_type,elen);
			
				return -EIO;
			}			
			send_msglen=elen;
			addon=0;
				
	    }
	    else
#endif
	    {
	    	printerr("gt_com_api lib can't support msg_type=0x%x now!\n",msg_type);
	    	return -EINVAL;
	    }
	}

	    head=&s->head;
	    buf=s->msg;		
	    send_msglen+=addon;
	    addon=send_msglen+sizeof(struct gt_pkt_head)-4-2+4;//-HEADFLAG(4)-LEN(2)+CRC(4)		
	    put_unaligned(addon,&head->pkt_len);
		head->pkt_len=addon;		
	    crclen=send_msglen;
		
	buf=s->msg;
	head->sync_head[0]=GT_SYNC_HEAD0;
	head->sync_head[1]=GT_SYNC_HEAD1;
	head->sync_head[2]=GT_SYNC_HEAD2;
	head->sync_head[3]=GT_SYNC_HEAD3;
	put_unaligned(GT_COM_VERSION, &head->version);
	put_unaligned(msg_type,&head->msg_type);
	put_unaligned(encrypt_type,&head->encrypt_type);
	put_unaligned(0,&head->reserve0);
	put_unaligned(0,&head->reserve1);

	ph=(DWORD *)head;
	//xor32=ph[1]^ph[2];//���ݰ���ͬ��ͷ����Ҫ�������У��
	ph++;
	xor32=(ph[0])^(ph[1]);//get_unaligned(ph)^get_unaligned(ph+1);
	//printf("ph=%x %x^%x=%x!!!!!!!!!!!!\n",(int)ph,ph[0],ph[1],xor32);
	head->xor32=xor32;//put_unaligned(xor32,&head->xor32);

	buf=s->msg;
	crcval=tab_crc32(buf,crclen);
	memcpy((void*)(buf+crclen),(void*)&crcval,sizeof(crcval));
	crclen+=sizeof(crcval);
	ph=(DWORD*)(buf+crclen);
	put_unaligned(GT_PKT_TAIL,ph);
	crclen+=4;
	crclen+=sizeof(struct gt_pkt_head);


	buf=(char*)s;
	ret=net_write_buf(fd,buf,crclen);
	//printf("send->");//shixin test!!!
	//print_buffer(buf,crclen);
	//printf("\n");
	if(ret>=0)
		ret=0;
	else
		ret=ret;
	if(mallocflag)
	{
		if(s!=NULL)
			free(s);
	}
	return ret;	
}

/*
 * ������: gt_cmd_pkt_recv()
 * ����:  ��ָ����socket����һ�����ݰ�,���н���,��֤ǩ���Ȳ�����,��������䵽recvָ��Ļ�����
 * ����ֵ:
 *         0:���յ�һ������
 *         ��ֵ:����
 *		-1:�����ݴ����������������Ѿ��Ͽ�		
 *      -2:���������
 *		-3:CRC��
 *		-4:��֧�ֵļ��ܸ�ʽ
 *		-5:���ܴ���
 *
 *         	����ֵ����
 *         recv:�Ѿ����õ����ݻ�����
 * ����:
 *         fd:  Ŀ��socket�ļ�������
 *         recv:ָ��Ҫ��Ž������ݵĻ�����,������Ӧ����һ��������������buffer
 *         buf_len:recv�������Ĵ�С,���������ݴ������ֵ��ʱ��,������Ӧ�÷��س�����Ϣ,��ʾ���������(�������Ӧ�ò��ᷢ��)
 *         flag:��Ҫ��ʲô������ִ�б�����,����ֵ��linux socket��̵���Ӧ����������ͬ,ֻҪԭ�����͸�socket��������
 *	     tempbuf:(����)������ʱ��NULL�Ϳ�����
 * ˵��:   ������Ӧ�������㹻��Ļ���������ñ�����,������������ֽ��յ����ݴ��ڻ�������С,Ӧ�����ػ����������Ϣ
 * 	   ������ֻ����ķ���ֵ��recv->msg�ֶμ��ɣ������ֶζԵ�������˵����Ч��
 *	   recv�Լ�tempbuf��������������ʼ��ַ������4�ֽڶ���ģ������ϲ�Ӧ�ô�����ܻ������
 */
int  gt_cmd_pkt_recv_env(int fd,struct gt_pkt_struct *recv,int buf_len,void *tempbuf,int flag,int *msg_type,int *encrypt_type)
{
	#define TMPBUF_LEN 1024	//Ӧ�ó����ж����tempbuf��С������������ֵ����Ҫ���·���
	#define judge_exit(r)		do	{if((r)<=0){	if((r)==0) errflag=-140;else errflag=r;	goto exit;}}while (0)	
	
	DWORD tbuf[TMPBUF_LEN/4];
	struct gt_pkt_struct *s,*s2;
	struct gt_pkt_head *head;
	DWORD	*ph,xor32,*reccrc,crcval,*tail;
	int	pkt_len,i;
	unsigned char 	*pd,*ptest;
	int elen,plen;
	int mtype,etype;
	int ret;
#ifdef USE_SSL
	void *pbuf,*ebuf;
#endif
	int mallocflag=0;
	int errflag=0;
	if(fd<0)
	{
		printerr("\ngt_cmd_pkt_recv get fd=%d!error\n",fd);
		return -EINVAL;
	}
	if(recv==NULL)
	{
		printerr("\ngt_cmd_pkt_recv get NULL recvbuf!error\n");
		return -EINVAL;
	}
	if((msg_type==NULL)||(encrypt_type==NULL))
	{
		mtype=GT_CMD_NO_AUTH;		//��û�������ŷ������²���Ҫ����
		etype=GT_CMD_NO_ENCRYPT;
		s=recv;
	}
	else
	{
		mtype=*msg_type;
		etype=*encrypt_type;
		s=(struct gt_pkt_struct *)tbuf;
		elen=buf_len;
		plen=buf_len;
		
	}
	
	head=&s->head;
	pd=(unsigned char*)head;
getheader:
//	printf("\nprepare to recv cmd:");
	pd=(char*)head->sync_head;
	ret=net_read_buf(fd,pd,1);
	judge_exit(ret);
	printf("*pd=%c\n",*pd);
	if(*pd!=GT_SYNC_HEAD0)//��ȡ���жϵ�һ��ͬ��ͷ
			//goto getheader;
		judge_exit(-6);
	pd++;
	ret=net_read_buf(fd,pd,1);
	judge_exit(ret);
	if(*pd!=GT_SYNC_HEAD1)//��ȡ���жϵڶ���ͬ��ͷ
		//goto getheader;
		judge_exit(-6);
	pd++;
	ret=net_read_buf(fd,pd,1);
	judge_exit(ret);
	if(*pd!=GT_SYNC_HEAD2)//��ȡ���жϵ�����ͬ��ͷ
		//goto getheader;
		judge_exit(-6);
	pd++;
	ret=net_read_buf(fd,pd,1);
	judge_exit(ret);
	if(*pd!=GT_SYNC_HEAD3)//��ȡ���жϵ��ĸ�ͬ��ͷ
		//goto getheader;
		judge_exit(-6);

	pd++;
	ret=net_read_buf(fd,pd,sizeof(struct gt_pkt_head)-4);
	judge_exit(ret);
	if((ret!=(sizeof(struct gt_pkt_head)-4)))  //���������ͷ(�ٳ�ͬ��ͷ���ֽ�)
		//goto getheader;
		judge_exit(-6);
//	printf("sync ok->");
	pkt_len=head->pkt_len-sizeof(struct gt_pkt_head)+4+2-4;		//ȥ��crc
	if((int)(pkt_len+sizeof(struct gt_pkt_head)+20)>buf_len)//20 �Ƕ�ӵ�
	{
		printf("too long pkt len=%d ,droped\n",pkt_len);	//���ݰ����ȳ�������������
		//goto getheader;
		judge_exit(-6);
	}
	ph=(DWORD *)(&head->pkt_len);
	xor32=get_unaligned(ph) ^ get_unaligned((ph+1));
	


/*	ptest=(char *)ph;
	for(i=0;i<(sizeof(struct gt_pkt_head)-4);i++)
	    printf("%02x,",ptest[i]);
*/
	if(xor32 != get_unaligned(&head->xor32))
	{
		//�����ͷУ�����
		printf("packet head xor error %x!=%x!\n",(int)xor32,(int)get_unaligned(&head->xor32));
		print_buffer((void*)head,sizeof(struct gt_pkt_head));
		printf("\n");
		//goto getheader;
		judge_exit(-6);
	}

	if((pkt_len+sizeof(struct gt_pkt_head)+EBUF_ADDON)>TMPBUF_LEN)
	{
		#ifdef _WIN32
			//windows
			//FIXME ������ڴ���亯��
			ret=0;
			s2=malloc(pkt_len+sizeof(struct gt_pkt_head)+EBUF_ADDON);
		#else
			//linux
			ret=posix_memalign(&s2,sizeof(unsigned long),(pkt_len+sizeof(struct gt_pkt_head)+EBUF_ADDON));
		#endif	
		if((ret!=0)||(s2==NULL))
		{
			printf("gt_cmd_pkt_recv_env posix_memalign error ret=%d:%s\n",ret,strerror(ret));
			//goto getheader;
			judge_exit(-6);
		}
		else
		{
			memcpy(s2,s,sizeof(struct gt_pkt_head));
			s=s2;
			mallocflag=1;	
			printf("gt_cmd_pkt_recv_env posix_memalign s=%x ret=%d\n",(int)s,ret);
		}
	}

		
	pd=s->msg;//��ͷ�Ѿ�������
	ret=net_read_buf(fd,pd,(pkt_len));
	judge_exit(ret);
	pd+=(pkt_len);
	reccrc=(DWORD*)pd;

	ret=net_read_buf(fd,pd,4);
	judge_exit(ret);
	pd+=4;
	
	ret=net_read_buf(fd,pd,4);
	judge_exit(ret);
	
	tail=(DWORD*)pd;
	if(get_unaligned(tail)!=GT_PKT_TAIL)//�ж��Ƿ��ǰ�β
	{
		printf("recv get %x,didn't tail\n",(int)get_unaligned(tail));
		
 		ptest=(char *)(s);
        	for(i=0;i<(int)(sizeof(struct gt_pkt_head)+pkt_len+4+4);i++)
            	printf("%02x,",ptest[i]);

        	printf("\n");

		//goto getheader;
		judge_exit(-6);
	}
	pd=(char*)s->msg;
	crcval = tab_crc32(pd,pkt_len);//�Ѿ�ȥ����crc�ĳ���
	if(crcval!=get_unaligned(reccrc))
	{//crcУ�����
		
		printf("packet receive crc error rec=%x self=%x!\n",(int)*reccrc,(int)crcval);
		judge_exit(-3);
	}	
	printf("head->msg_type is %d\n",head->msg_type);
	if(head->msg_type==GT_CMD_NO_AUTH)
	{
		//�����ʹ�������ŷ�����Ҫ���⴦��
		if(s!=recv)
		{
			memcpy(recv,s,(sizeof(struct gt_pkt_head)+pkt_len+4+4));
		}
		
	}
	
#ifdef USE_SSL
	else if(head->msg_type==MSG_AUTH_SSL)
	{
		memcpy((void*)recv,(void*)&s->head,sizeof(struct gt_pkt_head));
		pbuf=recv->msg;
		ebuf=s->msg;
		mtype=head->msg_type;
		etype=head->encrypt_type;	
		elen=head->pkt_len+2+4-4-sizeof(s->head);
		plen=buf_len;
		//gtlogerr("env_unpack etype is %d,elen is %d\n",etype,elen);
		ret=env_unpack(etype,ebuf,elen,pbuf,&plen);
		//lc to do Ŀǰ��ż��unpack����ԭ���Ѳ�

		
		if(ret!=0)
		{
			printerr("gt_com_api gt_cmd_pkt_recv_env  env_unpack ret=%d etype=%d elen=%d\n",ret,etype,elen);
                     gtlogerr("gt_cmd_pkt_recv_env  ���ܴ��� !env_unpack ret=%d etype=%d elen=%d\n",ret,etype,elen);
			judge_exit(-5);
		}
		
		//printf("recv envpkt len=%d\n",plen);
		
	}
#endif
	else
	{
		 printerr("gt_com_api lib can't support msg_type=0x%x now!\n",head->msg_type);
		 judge_exit(-4);
	}
	
	if(msg_type!=NULL)
		*msg_type=head->msg_type;
	if(encrypt_type!=NULL)
		*encrypt_type=head->encrypt_type;
	//printf("recv lib receive a cmd pkt\n");	
	errflag=0;
	
exit:
	if(mallocflag)
	{
		if(s!=NULL)
			free(s);
	}
	return errflag;	
	
}



