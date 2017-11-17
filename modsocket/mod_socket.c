/*	ģ�����û���udp��socketͨѶ�ĺ�����
 *		wsy
 *		 2007.9
 *    ���������ṩ��ʵ�ָ�ģ�����û���udp��socketͨѶ��Ҫ����ײ�ӿ�
 *    �ϲ��Ӧ�ýӿڼ����ݽṹ�����ڱ����ʵ�ַ�Χ
 */
#ifdef _WIN32
#include <windows.h>
#include <winsock.h>
#endif
#include "mod_socket.h"					//ģ���socketͨѶ�������ֶ���
#include "gtsocket.h"
#include "gtlog.h"
#include "gtthread.h"
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#define		MOD_MULTICAST_ADDR	"227.0.0.1"		//ģ���ಥ�ĵ�ַ(����)
#define		MOD_SOCKET_PORT			0x7654		//ģ���socketͨѶ�Ķ˿�
#define		MOD_SOCKET_MAGIC			0x8135		//ģ���socketͨѶ��ħ���֣�����������ȷ����


/**********************************************************************************************
 * �����ֽṹ,��ʵ��Ӧ�������ڸ�������Ĳ���������ͬ,����ʹ�ñ�
 * �ṹ�����ʱ���������ֻ��4,
 * ʹ��ʱ��Ҫ���������ǿ��ת����ָ�򱾽ṹ��ָ���Ա�ʹ��
 **********************************************************************************************/

typedef	struct 
{
	DWORD	magic;		//0xfedc֮�࣬��ʾ����һ�����׵�����.
	DWORD	source;		//���������ģ�飬��VMMAIN_ID�ȵ�
	DWORD	target;		//���������ģ�飬��ALL_PROCESS��VMMAIN_ID��
	DWORD	reserve;
	DWORD	reserve2;	//����Ҳ�����š��������Ҫ�Ļ���������guid
	DWORD	len;		//data�е���Ч�ֽ���
	BYTE    data[4];	//����...
}mod_socket_type;

static int sendflag = 0;	//��������ʱʹ�õ�����flags,��initʱ�趨
static int recvflag = 0;	//��������ʱʹ�õ�����flags


/**********************************************************************************************
 * ������	:mod_socket_init()
 * ����	:	��ָ���Ķ˿ڽ�������udp socket��������֮
 * ����	:	send_flag,	��������ʱʹ�õ�����flags,һ���趨,��������������
 *			recv_flag,	��������ʱʹ�õ�����flags
 * ����ֵ	:��ֵ��ʾʧ��
 *			  ������  �������ļ����������Ժ�ͨ���������ͻ��������      
 **********************************************************************************************/
int mod_socket_init( IN int send_flag, IN int recv_flag)
{
	int fd = -1;
	
	fd = udp_create(MOD_MULTICAST_ADDR,MOD_SOCKET_PORT);
	if(fd < 0)
		return -EAGAIN;

	udp_add_multicast(fd, MOD_MULTICAST_ADDR);//֧�ֶಥ
	sendflag = send_flag;
	recvflag = recv_flag;

	return fd;
}

/**********************************************************************************************
 * ������	:mod_socket_send()
 * ����	:��ָ����������ַ����һ��������Ϣ
 * ����	:com_fd:�ɵ��� 'mod_socket_init()' �ķ���ֵ�õ�
 *		 target:Ŀ��ģ���id����Ϊ0���ʾ���͸�����ģ��
 *		 source:����ģ���id
 *		 cmdbuf:ָ��Ҫ���͵�����Ļ�������ָ��(�Ѿ����������Ϣ)
 *		 cmdlen:����������Ч���ݵĳ���
 * ����ֵ	:0��ʾ�ɹ�����ֵ��ʾʧ��
 **********************************************************************************************/
int mod_socket_send(IN int com_fd,IN int target,IN int source,IN void *cmdbuf,IN int cmdlen)
{	
	struct sockaddr_in addr;
	mod_socket_type *cmd;
	char buf[MAX_MOD_SOCKET_CMD_LEN];
	int len=0;
	int rc;
	
	cmd = (mod_socket_type *)buf;
	
	memset(&addr,0,sizeof(addr));
	addr.sin_family	=	AF_INET;
	addr.sin_port	=	htons(MOD_SOCKET_PORT);
#ifdef _WIN32
	addr.sin_addr.s_addr=inet_addr(MOD_MULTICAST_ADDR);
#else
	inet_aton(MOD_MULTICAST_ADDR,&addr.sin_addr);
#endif
	//bzero(cmd,sizeof(mod_socket_type));
	if(cmdbuf!= NULL)
		memcpy(cmd->data,cmdbuf,cmdlen);
	cmd->magic	=	MOD_SOCKET_MAGIC;
	cmd->source	=	source;
	cmd->target	=	target;
	cmd->len	=	cmdlen;
	//printf("mod_socket_send cmd_len=%d\n",cmdlen);
	len	=	cmdlen+sizeof(mod_socket_type)-sizeof(cmd->data);
	errno = 0;
	rc = udp_send_data(com_fd,cmd,len,sendflag,&addr);
	if(rc!=len)
		return -errno;
	else
		return 0;
}

/**********************************************************************************************
 * ������: mod_socket_req_recv()
 * ����:  ��socket�н���һ�������
 * ����:
 *          com_fd:�ɵ��� 'mod_socket_init()' �ķ���ֵ�õ�
 *		 req_id:����ʱ�Ǳ�ģ���id,���ʱ��ʾ���յ������ݰ���Ŀ��id
 *		 source: ���������ģ��id
 *		 cmdbuf:ָ��Ҫ���͵�����Ļ�������ָ��(�Ѿ����������Ϣ)
 *		 cmd_maxlen:����������󳤶�
 *		 source_addr:��ŷ���ģ���ip����ΪNULL��ʾ����ȡ
 * ����ֵ:
 *         ��ֵ:���յ�����Ϣ���ֽ���
 *         ��ֵ:����
 ************************************************************************************************/
int mod_socket_req_recv(IN int com_fd, IO int *req_id, OUT int *source, OUT void *cmdbuf,IN int cmd_maxlen, OUT char *source_addr)
{
	struct sockaddr_in addr;
	DWORD buf[MAX_MOD_SOCKET_CMD_LEN];
	mod_socket_type *cmd;
	int len;
       int  myid=*req_id;
	if(cmdbuf ==NULL )
		return -EINVAL;

	cmd = (mod_socket_type *)buf;
	memset(&addr,0,sizeof(addr));
	while(1)
	{
		len = udp_recv_data(com_fd, cmd,MAX_MOD_SOCKET_CMD_LEN,recvflag, &addr);
		if(len<=0)	//socket error
			return len;

		if((int)cmd->len > cmd_maxlen) //����ȳ����ṩ�Ļ�����
		{
		    errno=ENOBUFS;
		    return -ENOBUFS;
              }
		
		if(cmd->magic != MOD_SOCKET_MAGIC) //ħ��������
			continue;

		if(((int)cmd->target != myid)&&(cmd->target != ALL_PROCESS)&&(myid!=ALL_PROCESS))//���Ƿ����ҵ�
			continue;

              *source=cmd->source;
              *req_id=cmd->target;
		memcpy(cmdbuf,cmd->data,cmd->len);//��������
		//printf("mod_socket_req_recv cmd_len=%d\n",cmd->len);
		if(source_addr != NULL) //���source ip
		{
			memcpy((void*)source_addr,(void*)inet_ntoa(addr.sin_addr),strlen(inet_ntoa(addr.sin_addr))+1);
		}
		return (len-(sizeof(mod_socket_type)-sizeof(cmd->data)));
	}
	
}

/**********************************************************************************************
 * ������: mod_socket_recv()
 * ����:  ��socket�н���һ�������
 * ����:
 *        com_fd:�ɵ��� 'mod_socket_init()' �ķ���ֵ�õ�
 *		 myid:��ģ���id
 *		 source: ���������ģ��id
 *		 cmdbuf:ָ��Ҫ���͵�����Ļ�������ָ��(�Ѿ����������Ϣ)
 *		 cmd_maxlen:����������󳤶�
 *		 source_addr:��ŷ���ģ���ip����ΪNULL��ʾ����ȡ
 * ����ֵ:
 *         ��ֵ:���յ�����Ϣ���ֽ���
 *         ��ֵ:����
 ************************************************************************************************/
int mod_socket_recv(IN int com_fd, IN int myid, OUT int *source, OUT void *cmdbuf,IN int cmd_maxlen, OUT char *source_addr)
{
    return mod_socket_req_recv(com_fd,&myid,source,cmdbuf,cmd_maxlen,source_addr);	
}



int send_ack_to_main(int com_fd, int mod_id, int cmd, int result, gateinfo *gate)
{
	DWORD send_buf[200];
	mod_socket_cmd_type *socketcmd;
	DWORD state;	
	struct usr_cmd_ack_struct *ack;
	
	socketcmd = (mod_socket_cmd_type *)send_buf;
	memcpy(&socketcmd->gate,gate,sizeof(gateinfo));//��gateinfoԭ������
	socketcmd->cmd	=	MOD_BYPASSTO_GATE_ACK;
	ack = (struct usr_cmd_ack_struct *)socketcmd->para;
	ack->rec_cmd 	= cmd;
	ack->result 	= result;
	socketcmd->len	=	sizeof(struct usr_cmd_ack_struct);
	
	return mod_socket_send(com_fd,MAIN_PROCESS_ID,mod_id,socketcmd,sizeof(mod_socket_cmd_type)-sizeof(socketcmd->para)+socketcmd->len);
}


//����������ģ��Ĳ�ѯ���������״̬
void *recv_modsocket_thread (void *data)
{
	
	int len;
	int sourceid;
	DWORD buf[MAX_MOD_SOCKET_CMD_LEN/sizeof(DWORD)];      //shixin changed to DWORD
	char sourceaddr[100];
	mod_socket_cmd_type *modsocket;
	mod_socket_thread_data *threaddata;
	
	if(data == NULL)
		return NULL;

	threaddata = (mod_socket_thread_data *)data;
	
	printf("%s start recv_modsocket_thread!\n",threaddata->module_name);
	gtloginfo ("start recv_modsocket_thread!\n");
       
	
	while(1)
	{
		len = mod_socket_recv(threaddata->com_fd, threaddata->mod_id,&sourceid, &buf, MAX_MOD_SOCKET_CMD_LEN, sourceaddr);
		if(len > 0)
		{
			if(sourceid!=VIDEOENC_MOD_ID)
				printf("%s recved a module-cmd from id %d, ip %s\n",threaddata->module_name,sourceid,sourceaddr);
			modsocket = (mod_socket_cmd_type *)buf;
			threaddata->fn(sourceid,modsocket);
		}
              else
                {
                    sleep(1);
                }
	}
	return NULL;
}

int creat_modsocket_thread(pthread_t *thread_id, int com_fd, int mod_id, char *mod_name, int (*fn)(int sourceid, mod_socket_cmd_type *modsocket))
{
		
	mod_socket_thread_data *threaddata;
	
	threaddata = (mod_socket_thread_data *)malloc(sizeof (mod_socket_thread_data));
	threaddata->mod_id = mod_id;
	strncpy(threaddata->module_name,mod_name,MAX_MODULE_NAME_LEN); 
       threaddata->module_name[MAX_MODULE_NAME_LEN-1]='\0';
	threaddata->fn = fn;
	threaddata->com_fd = com_fd;
	return gt_create_thread(thread_id, recv_modsocket_thread, (void *)threaddata);

}

