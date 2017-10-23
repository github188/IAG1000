/** @file	       gtsocket.c
 *   @brief 	�ṩ����ֲ��socket��������ӿ�
 *   @auth      shixin
 *				�����붨���gt_errno.h
 *   @date 	2007.06
 */
#ifndef GTSOCKET_H
#define GTSOCKET_H
#ifndef IN
#define IN 
#endif

#ifndef OUT
#define OUT
#endif

#ifndef IO
#define IO
#endif

#ifdef _WIN32
//windows
//#include <winsock2.h>
#include <windows.h>
#define     SOCK_FD                 SOCKET             ///<socket������Ͷ���
#define		socklen_t				int


#include <gt_errno.h>						///<�����붨��
#else
//linux
#include <sys/socket.h>
#include <netinet/in.h>
#define     SOCK_FD                 int             ///<socket������Ͷ���
#define     INVALID_SOCKET    -1            ///<��Ч��socket��

#endif

#include <errno.h>						///<�����붨��

#ifdef __cplusplus
extern "C" {
#endif
#undef EXPORT_DLL
#ifdef _WIN32
	//windows ʹ��

	#define EXPORT_DLL __declspec(dllexport)

#else

	//linux ʹ��

	#define EXPORT_DLL

#endif
/** 
 *   @brief     ��ָ���ĵ�ַ�Ͷ˿��ϴ���tcp����socket
 *   @param  svr_addr ���������ĵ�ַ INADDR_ANY��ʾ�����е�ַ����
 *   @param  port   �����Ķ˿ں�
 *   @return   ��ֵ��ʾ������socket������,��ֵ��ʾʧ��
 */ 
EXPORT_DLL SOCK_FD create_tcp_listen_port(unsigned long svr_addr,int port);
/** 
 *   @brief     �����������ӽ������ݵĳ�ʱʱ��(������ʽ��)
 *   @param  fd:�Ѿ��򿪵�tcp����������
 *   @param  second:���ճ�ʱʱ������
 *   @return   0��ʾ�ɹ�,��ֵ��ʾʧ��
 */ 
EXPORT_DLL int net_set_recv_timeout(SOCK_FD fd,int second);
/** 
 *   @brief    �����������ӷ������ݵĳ�ʱʱ��(������ʽ��)
 *   @param  fd:�Ѿ��򿪵�tcp����������
 *   @param  second:���ͳ�ʱʱ������
 *   @return   0��ʾ�ɹ�,��ֵ��ʾʧ��
 */ 
EXPORT_DLL int net_set_send_timeout(SOCK_FD fd,int second);
/** 
 *   @brief    �������ӵ�tcp����Ϊ���ж���̽��
 *   @param  fd:�Ѿ��򿪵�tcp����������
 *   @return   0��ʾ�ɹ�,��ֵ��ʾʧ��
 */ 
EXPORT_DLL int	net_activate_keepalive(SOCK_FD fd);

/** 
 *   @brief    ����tcp���ӵ�keepalive��ʱʱ��
 *   @param	   fd:�Ѿ��򿪵�tcp����������
 *	 @param	   retrys:���Դ���
 *	 @param	   timout:��һ�γ�ʱ��ʱ��(��)
 *	 @param	   interval:���ʱ��(��)
 *   @return   0��ʾ�ɹ�,��ֵ��ʾʧ��
 */ 
int	net_set_keepalive_time(SOCK_FD fd,int retrys,int timeout,int interval);
/** 
 *   @brief    ������socket����Ϊ�����÷�ʽ,��ֹ�����˳��ڶ�������ʱ������
 *   @param  fd:����socket
 *   @return   0��ʾ�ɹ�,��ֵ��ʾʧ��
 */ 
EXPORT_DLL int net_activate_reuseaddr(SOCK_FD fd);
/** 
 *   @brief    �������ӵ�tcp����Ϊ����nagle�㷨(���������ݰ�)
 *   @param  d:�Ѿ��򿪵�tcp����������
 *   @return   0��ʾ�ɹ�,��ֵ��ʾʧ��
 */ 
EXPORT_DLL int net_set_nodelay(SOCK_FD fd);
/** 
 *   @brief    �����������ӵ�linger����,������close�ر�socketǰ�Ƿ�û�з�����
 *   @param  fd:�Ѿ��򿪵�tcp����������
 *   @param  en:0��ʾ��ʹ��linger
 *                     1��ʾʹ��linger  
 *   @return   0��ʾ�ɹ�,��ֵ��ʾʧ��
 */ 
EXPORT_DLL int net_set_linger(SOCK_FD fd,int en);
/** 
 *   @brief    �����������ӵ�����ģʽ
 *   @param  fd:�Ѿ��򿪵�tcp����������
 *   @param  en:0��ʾʹ������ģʽ
 *                     1��ʾʹ��l������ģʽ
 *   @return   0��ʾ�ɹ�,��ֵ��ʾʧ��
 */ 
EXPORT_DLL int net_set_noblock(SOCK_FD fd,int en);
/** 
 *   @brief    ����tcp���ͻ������ĳ���
 *   @param  fd:�Ѿ��򿪵�tcp����������
 *   @param  size:Ҫ���õĳ���
 *   @return   0��ʾ�ɹ�,��ֵ��ʾʧ��
 */ 
EXPORT_DLL int net_set_tcp_sendbuf_len(SOCK_FD fd,int size);
/** 
 *   @brief    ��ȡtcp���ͻ������ĳ���
 *   @param  fd:�Ѿ��򿪵�tcp����������
 *   @return   ��ֵ��ʾ���ͻ������ĳ��ȸ�ֵ��ʾ����
 */ 
EXPORT_DLL int net_get_tcp_sendbuf_len(SOCK_FD fd);
/** 
 *   @brief    ����tcp���ջ������ĳ���
 *   @param  fd:�Ѿ��򿪵�tcp����������
 *   @param  size:Ҫ���õĳ���
 *   @return   0��ʾ�ɹ�,��ֵ��ʾʧ��
 */ 
EXPORT_DLL int net_set_tcp_recvbuf_len(SOCK_FD fd,int size);
/** 
 *   @brief    ��ȡtcp���ջ������ĳ���
 *   @param  fd:�Ѿ��򿪵�tcp����������
 *   @return   ��ֵ��ʾ���ջ������ĳ��ȸ�ֵ��ʾ����
 */ 
EXPORT_DLL int net_get_tcp_recvbuf_len(SOCK_FD fd);
/** 
 *   @brief    ����tcp���ͻ���������(select���õ�)
 *   @param  fd:�Ѿ��򿪵�tcp����������
 *   @param  size:Ҫ���õĳ���
 *   @return   0��ʾ�ɹ�,��ֵ��ʾʧ��
 */ 
EXPORT_DLL int	net_set_sock_send_low(SOCK_FD fd,int size);
/** 
 *   @brief    ��ȡtcp���ͻ���������
 *   @param  fd:�Ѿ��򿪵�tcp����������
 *   @return   ��ֵ��ʾ���ͻ���������,��ֵ��ʾ����
 */ 
EXPORT_DLL int	net_get_sock_send_low(SOCK_FD fd);
/** 
 *   @brief    ��ȡtcp���ջ���������
 *   @param  fd:�Ѿ��򿪵�tcp����������
 *   @return   ��ֵ��ʾ���ͻ���������,��ֵ��ʾ����
 */ 
EXPORT_DLL int	net_get_sock_recv_low(SOCK_FD fd);
/** 
 *   @brief    ����tcp���ջ���������(select���õ�)
 *   @param  fd:�Ѿ��򿪵�tcp����������
 *   @param  size:Ҫ���õĳ���
 *   @return   0��ʾ�ɹ�,��ֵ��ʾʧ��
 */ 
EXPORT_DLL int	net_set_sock_recv_low(SOCK_FD fd ,int size);
/** 
 *   @brief    ��ȡ���ջ������е���Ч�ֽ���
 *   @param  fd:�Ѿ��򿪵�tcp����������
 *   @return   ��ֵ��ʾ���ջ������е���Ч�ֽ���,��ֵ��ʾ����
 */ 
#define get_fd_in_buffer_num get_sock_in_buffer_num
EXPORT_DLL int get_sock_in_buffer_num(SOCK_FD fd);
/** 
 *   @brief    ��ȡ�ӷ��ͻ������е���Ч�ֽ���
 *   @param  fd:�Ѿ��򿪵�tcp����������
 *   @return   ��ֵ��ʾ���ͻ������е���Ч�ֽ���,��ֵ��ʾ����
 */ 
#define get_fd_out_buffer_num   get_sock_out_buffer_num
EXPORT_DLL int	get_sock_out_buffer_num(SOCK_FD fd);

/** 
 *   @brief    ��ȡԶ��tcp���ӵĵ�ַ
 *   @param	   fd:�Ѿ��򿪵�tcp����������
 *	 @param		addr:׼�����ip��ַ��ָ��
 *   @return   0��ʾ�ɹ�����ֵ��ʾʧ��
 */ 
EXPORT_DLL int	get_peer_ip(SOCK_FD fd,struct sockaddr_in *addr);
/** 
 *   @brief    ��ȡtcp���ӵı��ص�ַ
 *   @param	   fd:�Ѿ��򿪵�tcp����������
 *	 @param		addr:׼�����ip��ַ��ָ��
 *   @return   0��ʾ�ɹ�����ֵ��ʾʧ��
 */ 
EXPORT_DLL int	get_local_ip(SOCK_FD fd,struct sockaddr_in *addr);

/** i
 *   @brief    ��ȡԶ��tcp���ӵĵ�ַ�ַ���
 *   @param  fd:�Ѿ��򿪵�tcp����������
 *   @return   Զ�����ӵ�ַ�ַ��� NULL��ʾʧ��,���������errno
 */ 
EXPORT_DLL char *get_peer_ip_str(SOCK_FD fd);

/** 
 *   @brief    ��ȡһ��tcp���ӵı���ip��ַ�ַ���
 *   @param  fd:�Ѿ��򿪵�tcp����������
 *   @return   �������ӵ�ַ�ַ��� NULL��ʾʧ��,���������errno
 */ 
EXPORT_DLL char *get_local_ip_str(SOCK_FD fd);
/** 
 *   @brief    ����ʱ������connect,����timeout������������������connect��ȫһ��
 *   @param  fd:�Ѿ������õ�socket
 *   @param  serv_addr:Ҫ���ӵĵ�ַ���˿ں�
 *   @param  addrlen:serv_addr�ṹ�ĳ���
 *   @param  timeout:��ʱ����ʱ��(��)
 *   @return   0��ʾ�ɹ�,��ֵ��ʾʧ��
 */										
EXPORT_DLL int  connect_timeout(SOCK_FD  fd,  const  struct sockaddr *serv_addr, socklen_t    addrlen,int timeout);
/** 
 *   @brief    ����Զ��tcp�����ַ
 *   @param  addr_str:����Զ�̷����ַ���ַ���
 *   @param  port:Զ��tcp����Ķ˿ں�
 *   @param   timeout:tcp���ӵĳ�ʱʱ��(��)
 *   @return   ��ֵ��ʾ�����Ӻõ�����������ֵ��ʾ����
 */ 
EXPORT_DLL SOCK_FD tcp_connect_addr(const char *addr_str,int port,int timeout);

/** 
 *   @brief    ��������������ȫ��д��һ������������
 *   @param  fd:Ŀ���ļ�������
 *   @param  *buf:ָ��Ҫ���͵Ļ�������ָ��
 *   @param  len:Ҫ���͵Ļ������е���Ч��Ϣ�ֽ���
 *   @return   ��ֵ��ʾ�ɹ�д����ֽ���,��ֵ�������쳣,���������
 */ 
EXPORT_DLL int net_write_buf(SOCK_FD fd,void *buf,int len);
/** 
 *   @brief    ��һ�����������ж�ȡָ���ֽ������뻺����
 *   @param  fd:Ŀ���ļ�������
 *   @param  *buf:ָ���Ž������ݵĻ�����ָ��
 *   @param  len:Ҫ���յ������ֽ���
 *   @return   ��ֵ��ʾ�ɹ�������ջ��������ֽ���,��ֵ��������쳣;
 *			  -EAGAIN ��ʾ���ճ�ʱ(SO_RCVTIMEO�趨��)
 *			  -ETIMEDOUT linux������keepalive���µĳ�ʱ
 *			  -ECONNRESET windows ������keepalive���µĶϿ�����������
 */ 
EXPORT_DLL int net_read_buf(SOCK_FD fd,void *buf,int len) ;
/** 
 *   @brief     �ر��Ѿ���������������
 *   @param  fd:�Ѿ��򿪵�tcp����������
 *   @return   0��ʾ�ɹ�,��ֵ��ʾʧ��
 */ 
EXPORT_DLL int	net_close(SOCK_FD fd);

/*
	����˵��:
	����tcp���ӵ�����,һ������connect��accept�ճɹ��󣬰�����
	��keepalive��ʱ������Ϊ15�뷢��һ��̽�����ʧ�ܺ���10������3��
	�ر�nagle�㷨
	�ر�linger

	recv_timeout��ʾ��������û����Ӧ��ʱʱ��
	����0��ʾ�ɹ�,��ֵ��ʾʧ��
*/
EXPORT_DLL int set_tcp_socket_attr(int fd,int recv_timeout);


///////////////////////////////////////////UDP����//////////////////////////////////////
/** 
 *   @brief    ����һ��UDP������(���ͺͽ���)
 *   @param  ip_str :׼�������ĵ�ַ�ַ���
 *   @param  port    :׼�������Ķ˿ں�
 *   @return    ��ֵ��ʾʧ�ܣ�����ֵ��ʾ�ļ�������       
 */ 
EXPORT_DLL int udp_create(IN char *ip_str,IN int port);
/** 
 *   @brief    �����������鲥����
 *   @param  fd ֮ǰ��udp_create��ȡ��������
 *   @param  multicast_addr :�鲥��ַ
 *   @return    0��ʾ�ɹ�����ֵ��ʾʧ��
 */ 
EXPORT_DLL int udp_add_multicast(int fd,char *multicast_addr);


/** 
 *   @brief    ����һ������
 *   @param  fd ֮ǰ��udp_create��ȡ��������
 *   @param  data:Ҫ���͵����ݻ�����
 *   @param  len:�������е���Ч�����ֽ���
 *   @param  flags:����������ʱд0
 *   @param  to:���͵�Ŀ�ĵ�ַ,��
 *                  to->sin_addr.s_addr=inet_addr("224.0.0.1"); //����ַ���˿�
                     to->sin_port=htons((short)6868);
 *   @return    ��ֵ��ʾ���ͳ�ȥ���ֽ�������ֵ��ʾʧ��
 */ 
EXPORT_DLL int udp_send_data(IN int fd, IN void *data, IN int  len, IN int flags, IN struct sockaddr_in *to);




/** 
 *   @brief    ����һ��UDP����
 *   @param  fd ֮ǰ��udp_create��ȡ��������
 *   @param  buf:׼��������ݵĻ�����
 *   @param  len:����������
 *   @param  flags:����������ʱд0
 *   @param  from:���յ������ݵ�Զ�̵�ַ
 *   @return   ��ֵ��ʾ���յ�buf�е���Ч�ֽ�������ֵ��ʾʧ��
 */ 
EXPORT_DLL int udp_recv_data(IN int fd,OUT void *buf, IN int len, IN int flags, OUT struct sockaddr_in *from);




#ifdef __cplusplus
}
#endif

#endif

