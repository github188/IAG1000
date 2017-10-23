/** @file	       gtsocket.c
 *   @brief 	�ṩ����ֲ��socket��������ӿ�
 *   @auth      shixin
 *   @date 	2007.06
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
//windows
#include <stdio.h>
#include <stdlib.h>
struct _RPC_ASYNC_STATE;	///<��ֹ�����뾯��
#include <winsock2.h>
#include <Ws2tcpip.h>
#pragma comment(lib,"ws2_32.lib")
#define close(a)		closesocket(a)			///<�ر�socket
#define write(a, b, c)	send(a, b, c, 0)		///<д����
#define read(a, b, c)	recv(a, b, c, 0)		///<������

#else
//linux
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/fcntl.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
//#include <errno.h>
#endif

#include "gtsocket.h"
#ifndef INT_MAX
#define INT_MAX 2147483647
#endif

#ifndef TIOCOUTQ
#define TIOCOUTQ      0x5411
#endif
#ifndef INADDR_NONE
#define INADDR_NONE     0xffffffff
#endif

#if 0 //def _WIN32
#undef	errno
#define errno			WSAGetLastError()		///<������
#endif

#ifdef _WIN32
//platform SDK #include <mstcpip.h>
  #pragma comment (lib,"ws2_32")

  //   New   WSAIoctl   Options     
  #define   SIO_RCVALL                         _WSAIOW(IOC_VENDOR,1)  
  #define   SIO_RCVALL_MCAST             _WSAIOW(IOC_VENDOR,2)  
  #define   SIO_RCVALL_IGMPMCAST     _WSAIOW(IOC_VENDOR,3)  
  #define   SIO_KEEPALIVE_VALS         _WSAIOW(IOC_VENDOR,4)  
  #define   SIO_ABSORB_RTRALERT       _WSAIOW(IOC_VENDOR,5)  
  #define   SIO_UCAST_IF                     _WSAIOW(IOC_VENDOR,6)  
  #define   SIO_LIMIT_BROADCASTS     _WSAIOW(IOC_VENDOR,7)  
  #define   SIO_INDEX_BIND                 _WSAIOW(IOC_VENDOR,8)  
  #define   SIO_INDEX_MCASTIF           _WSAIOW(IOC_VENDOR,9)  
  #define   SIO_INDEX_ADD_MCAST       _WSAIOW(IOC_VENDOR,10)  
  #define   SIO_INDEX_DEL_MCAST       _WSAIOW(IOC_VENDOR,11)   
#endif
/** 
 *   @brief     ��ָ���ĵ�ַ�Ͷ˿��ϴ���tcp����socket
 *   @param  svr_addr ���������ĵ�ַ INADDR_ANY��ʾ�����е�ַ����
 *   @param  port   �����Ķ˿ں�
 *   @return   ��ֵ��ʾ������socket������,��ֵ��ʾʧ��
 */ 
SOCK_FD create_tcp_listen_port(unsigned long svr_addr,int port)
{
    SOCK_FD                      fd;
    struct sockaddr_in           svr;
#ifdef _WIN32
    {
        WORD wVersionRequested;
        WSADATA wsaData;
        int err;
 
        wVersionRequested = MAKEWORD( 2, 0 );
 
        err = WSAStartup( wVersionRequested, &wsaData );
        if ( err != 0 ) {
            /* Tell the user that we couldn't find a usable */
            /* WinSock DLL.                               */
            //printf("can't initialize socket library\n");
			errno=WSAGetLastError();
			return -1;
        }
    }
#endif

    fd=socket(AF_INET,SOCK_STREAM,0);
    if(fd==INVALID_SOCKET)
    {
        //printf("can't create socket:%s!\n",strerror(errno));
#ifdef _WIN32
		errno=WSAGetLastError();
#endif
        return INVALID_SOCKET;
    }    
///�����������ֹ��,���������Եڶ��ο������������õȴ�һ��ʱ��
 	net_activate_reuseaddr(fd);	
	memset(& svr, 0 ,sizeof(struct sockaddr_in));
 	svr.sin_family=AF_INET; 
 	svr.sin_port=htons((unsigned short)port); 
 	svr.sin_addr.s_addr=htonl(svr_addr); 
 	if(bind(fd,(struct sockaddr *)&svr,sizeof(svr))<0) 
  	{   	    
	      //  printf("Bind Error:%s\n\a",strerror(errno)); 	
#ifdef _WIN32
		errno=WSAGetLastError();
#endif
            close(fd);
	        return INVALID_SOCKET;
  	} 
	net_set_noblock(fd,0);
    return fd;
}
/** 
 *   @brief     �ر��Ѿ���������������
 *   @param  fd:�Ѿ��򿪵�tcp����������
 *   @return   0��ʾ�ɹ�,��ֵ��ʾʧ��
 */ 
int	net_close(SOCK_FD fd)
{
	int ret;
	ret=close(fd);
	if(ret!=0)
	{
#ifdef _WIN32
		errno=WSAGetLastError();
#endif
	}
	return ret;
}

/** 
 *   @brief     �����������ӽ������ݵĳ�ʱʱ��(������ʽ��)
 *   @param  fd:�Ѿ��򿪵�tcp����������
 *   @param  second:���ճ�ʱʱ������
 *   @return   0��ʾ�ɹ�,��ֵ��ʾʧ��
 */ 
int net_set_recv_timeout(SOCK_FD fd,int second)
{
	int ret;
#ifdef _WIN32	
	int	timeout;
#else
	struct timeval timeout;
#endif

#ifdef _WIN32
	timeout=second*1000;
#else	
	timeout.tv_sec=second;
	timeout.tv_usec=0;
#endif
	ret = setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
	if(ret!=0)
	{
#ifdef _WIN32
		errno=WSAGetLastError();
#endif
	}
	return	ret;
}

/** 
 *   @brief    �����������ӷ������ݵĳ�ʱʱ��(������ʽ��)
 *   @param  fd:�Ѿ��򿪵�tcp����������
 *   @param  second:���ͳ�ʱʱ������
 *   @return   0��ʾ�ɹ�,��ֵ��ʾʧ��
 */ 
int net_set_send_timeout(SOCK_FD fd,int second)
{
	int	ret;
#ifdef _WIN32	
	int	timeout;
#else
	struct timeval timeout;
#endif

#ifdef _WIN32
	timeout=second*1000;
#else	
	//timeout.tv_sec=second;
	//timeout.tv_usec=0;
	timeout.tv_sec=0;
	timeout.tv_usec= second;
#endif
	ret = setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout));
	if(ret!=0)
	{
#ifdef _WIN32
		errno=WSAGetLastError();
#endif
	}
	return ret;
}

/** 
 *   @brief    �������ӵ�tcp����Ϊ���ж���̽��
 *   @param  fd:�Ѿ��򿪵�tcp����������
 *   @return   0��ʾ�ɹ�,��ֵ��ʾʧ��
 */ 
int	net_activate_keepalive(SOCK_FD fd)
{
	int ret;
    int keepalive = 1;
    ret = setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (char*)&keepalive, sizeof(keepalive));
	if(ret!=0)
	{
#ifdef _WIN32
		errno=WSAGetLastError();
#endif
	}
	return ret;
}
typedef struct      
{  
	u_long     onoff;  
	u_long     keepalivetime;  
	u_long     keepaliveinterval;  
}TCP_KEEPALIVE;  
/** 
 *   @brief    ����tcp���ӵ�keepalive��ʱʱ��
 *   @param	   fd:�Ѿ��򿪵�tcp����������
 *	 @param	   retrys:���Դ���
 *	 @param	   timout:��һ�γ�ʱ��ʱ��(��)
 *	 @param	   interval:���ʱ��(��)
 *   @return   0��ʾ�ɹ�,��ֵ��ʾʧ��
 */ 
int	net_set_keepalive_time(SOCK_FD fd,int retrys,int timeout,int interval)
{
#ifdef _WIN32
	TCP_KEEPALIVE   inKeepAlive   =   {0};   //�������  
	unsigned   long   ulInLen   =   sizeof(TCP_KEEPALIVE);    

	TCP_KEEPALIVE   outKeepAlive   =   {0};   //�������  
	unsigned   long   ulOutLen   =   sizeof(TCP_KEEPALIVE);    

	unsigned   long   ulBytesReturn   =   0;    

	//����socket��keep   aliveΪ5�룬���ҷ��ʹ���Ϊ3��(Ĭ��)      
	inKeepAlive.onoff=1;    
	inKeepAlive.keepaliveinterval=timeout*1000;   //��ʼ�״�KeepAlive̽��ǰ��TCP�ձ�ʱ��
	inKeepAlive.keepalivetime=interval*1000;   // ����KeepAlive̽����ʱ����   
	if(WSAIoctl(
		(unsigned   int)fd,
		SIO_KEEPALIVE_VALS,      
		(LPVOID)&inKeepAlive,ulInLen,      
		(LPVOID)&outKeepAlive,ulOutLen,      
		&ulBytesReturn,   
		NULL,   NULL)   
		==   SOCKET_ERROR)    
	{                    
		printf("WSAIoctl   failed.   error   code(%d)!\n",WSAGetLastError());  
		return -1;
	}   
	return 0;
#else
	//linux
//	#include   <netinet/tcp.h>  
//	����  
	////KeepAliveʵ��  
	//�������Ҫ����ACE,���û�а���ACE,������õ���ACE�����ĳ�linux��Ӧ�Ľӿ�  
	int   keepAlive   =   1;//�趨KeepAlive  
	int   keepIdle   =   timeout;//5;//��ʼ�״�KeepAlive̽��ǰ��TCP�ձ�ʱ��  
	int   keepInterval   =   interval;//5;//����KeepAlive̽����ʱ����  
	int   keepCount   =   retrys;//3;//�ж��Ͽ�ǰ��KeepAlive̽�����  
	printf("idle=%d interval=%d count=%d\n",keepIdle,keepInterval,keepCount);

	if(setsockopt(fd,SOL_SOCKET,SO_KEEPALIVE,(void*)&keepAlive,sizeof(keepAlive))   ==   -1)  
	{  
	  printf("setsockopt   SO_KEEPALIVE   error!\n");  
	  return -1;
	}  

	if(setsockopt(fd,SOL_TCP,TCP_KEEPIDLE,(void   *)&keepIdle,sizeof(keepIdle))   ==   -1)  
	{  
	  printf("setsockopt   TCP_KEEPIDLE   error!\n");
	  return -1;
	}  

	if(setsockopt(fd,SOL_TCP,TCP_KEEPINTVL,(void   *)&keepInterval,sizeof(keepInterval))   ==   -1)  
	{  
	  printf("setsockopt   TCP_KEEPINTVL   error!\n");
	  return -1;
	}  

	if(setsockopt(fd,SOL_TCP,TCP_KEEPCNT,(void   *)&keepCount,sizeof(keepCount))   ==   -1)  
	{  
	  printf("setsockopt   TCP_KEEPCNT   error!\n");
	  return -1;
	}   
	return 0;
#endif
}
/** 
 *   @brief    ������socket����Ϊ�����÷�ʽ,��ֹ�����˳��ڶ�������ʱ������
 *   @param  fd:����socket
 *   @return   0��ʾ�ɹ�,��ֵ��ʾʧ��
 */ 
int net_activate_reuseaddr(SOCK_FD fd)
{
  int	ret;
  int reuseaddr = 1;
  ret = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char*)&reuseaddr,sizeof(reuseaddr));
  if(ret!=0)
  {
#ifdef _WIN32
	errno=WSAGetLastError();
#endif
  }
  return ret;
}

/** 
 *   @brief    �������ӵ�tcp����Ϊ����nagle�㷨(���������ݰ�)
 *   @param  d:�Ѿ��򿪵�tcp����������
 *   @return   0��ʾ�ɹ�,��ֵ��ʾʧ��
 */ 
int net_set_nodelay(SOCK_FD fd)
{
  int ret;
  int nodelay = 1;
  ret = setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char*)&nodelay, sizeof(nodelay));
  if(ret!=0)
  {
#ifdef _WIN32
		errno=WSAGetLastError();
#endif
  }
  return ret;
}

#if 0
int net_activate_oobinline(SOCK_FD fd)
{
  int oob_inline = 1;
 return setsockopt(fd, SOL_SOCKET, SO_OOBINLINE, &oob_inline, sizeof(oob_inline));
}


int net_set_iptos_throughput(SOCK_FD fd)
{
  int tos = IPTOS_THROUGHPUT;
  /* Ignore failure to set (maybe this IP stack demands privilege for this) */
  return setsockopt(fd, IPPROTO_IP, IP_TOS, &tos, sizeof(tos));
}
#endif
/** 
 *   @brief    �����������ӵ�linger����,������close�ر�socketǰ�Ƿ�û�з�����
 *   @param  fd:�Ѿ��򿪵�tcp����������
 *   @param  en:0��ʾ��ʹ��linger
 *                     1��ʾʹ��linger  
 *   @return   0��ʾ�ɹ�,��ֵ��ʾʧ��
 */ 
int net_set_linger(SOCK_FD fd,int en)
{
//  	int retval;
	int	ret;
  	struct linger the_linger;
	if(en)
	{
  		the_linger.l_onoff = 1;
  		the_linger.l_linger = (unsigned short)INT_MAX;
	}
	else
	{
		the_linger.l_onoff = 0;
  		the_linger.l_linger = 0;
	}
  	ret = setsockopt(fd, SOL_SOCKET, SO_LINGER, (char*)&the_linger,sizeof(the_linger));
	if(ret!=0)
	{
#ifdef _WIN32
		errno=WSAGetLastError();
#endif
	}
	return ret;
}

/** 
 *   @brief    ����tcp���ͻ������ĳ���
 *   @param  fd:�Ѿ��򿪵�tcp����������
 *   @param  size:Ҫ���õĳ���
 *   @return   0��ʾ�ɹ�,��ֵ��ʾʧ��
 */ 
int net_set_tcp_sendbuf_len(SOCK_FD fd,int size)
{
	int bufsize;
	int ret;
	if((fd<0)||(size<0))
		return -1;
	bufsize=size/2;
	//lensize=sizeof(bufsize);
	ret= setsockopt(fd, SOL_SOCKET, SO_SNDBUF, (char*)&bufsize, sizeof(bufsize));	
	if(ret!=0)
	{
#ifdef _WIN32
		errno=WSAGetLastError();
#endif
	}
	return ret;
}
/** 
 *   @brief    ��ȡtcp���ͻ������ĳ���
 *   @param  fd:�Ѿ��򿪵�tcp����������
 *   @return   ��ֵ��ʾ���ͻ������ĳ��ȸ�ֵ��ʾ����
 */ 
int net_get_tcp_sendbuf_len(SOCK_FD fd)
{
	int ret;
	int bufsize,lensize;
	lensize=sizeof(bufsize);
	ret=getsockopt(fd, SOL_SOCKET, SO_SNDBUF, (char*)&bufsize, &lensize);
	if(ret==0)
	{
		return bufsize;
	}
	else
	{
#ifdef _WIN32
		errno=WSAGetLastError();
#endif
		return -1;
	}
}
/** 
 *   @brief    ����tcp���ջ������ĳ���
 *   @param  fd:�Ѿ��򿪵�tcp����������
 *   @param  size:Ҫ���õĳ���
 *   @return   0��ʾ�ɹ�,��ֵ��ʾʧ��
 */ 
int net_set_tcp_recvbuf_len(SOCK_FD fd,int size)
{
	int ret;
	int bufsize;
	if((fd<0)||(size<0))
	{
		errno=EINVAL;
		return -1;
	}
	bufsize=size/2;
	ret = setsockopt(fd, SOL_SOCKET, SO_RCVBUF, (char*)&bufsize, sizeof(bufsize));	
	if(ret!=0)
	{
#ifdef _WIN32
		errno=WSAGetLastError();
#endif
	}
	return ret;
}

/** 
 *   @brief    ��ȡtcp���ջ������ĳ���
 *   @param  fd:�Ѿ��򿪵�tcp����������
 *   @return   ��ֵ��ʾ���ջ������ĳ��ȸ�ֵ��ʾ����
 */ 
int net_get_tcp_recvbuf_len(SOCK_FD fd)
{
	int bufsize,lensize;
	int ret;
	lensize=sizeof(bufsize);
	ret=getsockopt(fd, SOL_SOCKET, SO_RCVBUF, (char*)&bufsize, &lensize);
	if(ret==0)
		return bufsize;
	else
	{
#ifdef _WIN32
		errno=WSAGetLastError();
#endif
		return ret;
	}
}
/** 
 *   @brief    ����tcp���ͻ���������(select���õ�)
 *   @param  fd:�Ѿ��򿪵�tcp����������
 *   @param  size:Ҫ���õĳ���
 *   @return   0��ʾ�ɹ�,��ֵ��ʾʧ��
 */ 
int	net_set_sock_send_low(SOCK_FD fd,int size)
{
	int ret;
	int bufsize;
	if((fd<0)||(size<0))
	{
		errno=EINVAL;
		return -1;
	}
	bufsize=size;
	ret = setsockopt(fd, SOL_SOCKET, SO_SNDLOWAT, (char*)&bufsize, sizeof(bufsize));	
	if(ret!=0)
	{
#ifdef _WIN32
		errno=WSAGetLastError();
#endif
	}
	return ret;
}
/** 
 *   @brief    ��ȡtcp���ͻ���������
 *   @param  fd:�Ѿ��򿪵�tcp����������
 *   @return   ��ֵ��ʾ���ͻ���������,��ֵ��ʾ����
 */ 
int	net_get_sock_send_low(SOCK_FD fd)
{
	int bufsize,lensize;
	int ret;
	lensize=sizeof(bufsize);
	ret=getsockopt(fd, SOL_SOCKET, SO_SNDLOWAT, (char*)&bufsize, &lensize);
	if(ret==0)
		return bufsize;
	else
	{
#ifdef _WIN32
		errno=WSAGetLastError();
#endif
		return ret;	
	}
}
/** 
 *   @brief    ��ȡtcp���ջ���������
 *   @param  fd:�Ѿ��򿪵�tcp����������
 *   @return   ��ֵ��ʾ���ͻ���������,��ֵ��ʾ����
 */ 
int	net_get_sock_recv_low(SOCK_FD fd)
{
	int bufsize,lensize;
	int ret;
	lensize=sizeof(bufsize);
	ret=getsockopt(fd, SOL_SOCKET, SO_RCVLOWAT, (char*)&bufsize, &lensize);
	if(ret==0)
		return bufsize;
	else
	{
#ifdef _WIN32
		errno=WSAGetLastError();
#endif
		return ret;	
	}
}
/** 
 *   @brief    ����tcp���ջ���������(select���õ�)
 *   @param  fd:�Ѿ��򿪵�tcp����������
 *   @param  size:Ҫ���õĳ���
 *   @return   0��ʾ�ɹ�,��ֵ��ʾʧ��
 */ 
int	net_set_sock_recv_low(SOCK_FD fd ,int size)
{
	int	ret;
	int bufsize;
	if((fd<0)||(size<0))
	{
		errno=EINVAL;
		return -1;
	}
	bufsize=size;
	ret = setsockopt(fd, SOL_SOCKET, SO_RCVLOWAT, (char*)&bufsize, sizeof(bufsize));	
	if(ret!=0)
	{
#ifdef _WIN32
		errno=WSAGetLastError();
#endif
	}
	return ret;
}

/** 
 *   @brief    ��ȡ���ջ������е���Ч�ֽ���
 *   @param  fd:�Ѿ��򿪵�tcp����������
 *   @return   ��ֵ��ʾ���ջ������е���Ч�ֽ���,��ֵ��ʾ����
 */ 
#define get_fd_in_buffer_num get_sock_in_buffer_num
int get_sock_in_buffer_num(SOCK_FD fd)
{//��ȡ���ջ���������Ч����
	int ret;
	unsigned long cnt=100;
#ifdef _WIN32
	ret=ioctlsocket(fd,FIONREAD,&cnt);
#else
	ret=ioctl(fd,FIONREAD,&cnt);
#endif
	if(ret<0)
	{
#ifdef _WIN32
		errno=WSAGetLastError();
#endif
		if(errno>0)
			return -errno;
		else
			return -1;
	}
	else
	{
		return (int)cnt;
	}
}

/** 
 *   @brief    ��ȡ�ӷ��ͻ������е���Ч�ֽ���
 *   @param  fd:�Ѿ��򿪵�tcp����������
 *   @return   ��ֵ��ʾ���ͻ������е���Ч�ֽ���,��ֵ��ʾ����
 */ 
int	get_sock_out_buffer_num(SOCK_FD fd)
{//��ȡ���ͻ�������δ���ͳ���������

#ifndef _WIN32
	int ret;
	int cnt=0;
	ret=ioctl(fd,TIOCOUTQ  ,&cnt);
	if(ret<0)
		return -1;
	else
		return cnt;
#else
///#warn "can't support this function in windows"
	///TODO Ѱ��windows�µ����Ʒ...
	return 0;
#endif
}

/** 
 *   @brief    ��ȡԶ��tcp���ӵĵ�ַ�ַ���
 *   @param  fd:�Ѿ��򿪵�tcp����������
 *   @return   Զ�����ӵ�ַ�ַ��� NULL��ʾʧ��,���������errno
 */ 
char *get_peer_ip_str(SOCK_FD fd)
{
	int Ret;
	struct sockaddr_in RmtAddr;
	int AddrLen=sizeof(struct sockaddr);
	
	Ret=getpeername(fd,(struct sockaddr *)&RmtAddr,&AddrLen);
	if(Ret==0)
		return inet_ntoa(RmtAddr.sin_addr);
	else
	{
#ifdef _WIN32
		errno=WSAGetLastError();
#endif
		return NULL;
	}
}

/** 
 *   @brief    ��ȡһ��tcp���ӵı���ip��ַ�ַ���
 *   @param  fd:�Ѿ��򿪵�tcp����������
 *   @return   �������ӵ�ַ�ַ��� NULL��ʾʧ��,���������errno
 */ 
char *get_local_ip_str(SOCK_FD fd)
{
    int ret;
    struct sockaddr_in local_addr;
    int     addrlen=sizeof(struct sockaddr_in);
    ret=getsockname(fd,(struct sockaddr *)&local_addr,&addrlen);
    if(ret==0)
        return inet_ntoa(local_addr.sin_addr);
    else
    {
#ifdef _WIN32
		errno=WSAGetLastError();
#endif
		return NULL;    
    }
}
/** 
 *   @brief    ��ȡԶ��tcp���ӵĵ�ַ
 *   @param	   fd:�Ѿ��򿪵�tcp����������
 *	 @param		addr:׼�����ip��ַ��ָ��
 *   @return   0��ʾ�ɹ�����ֵ��ʾʧ��
 */ 
int	get_peer_ip(SOCK_FD fd,struct sockaddr_in *addr)
{
	int	ret;
	int addrlen=sizeof(struct sockaddr);
	if(addr==NULL)
	{
		//errno=EINVAL;
		return -1;
	}
	ret=getpeername(fd,(struct sockaddr *)addr,&addrlen);
	if(ret!=0)
	{
#ifdef _WIN32
		errno=WSAGetLastError();
#endif
	}
	return ret;
}
/** 
 *   @brief    ��ȡtcp���ӵı��ص�ַ
 *   @param	   fd:�Ѿ��򿪵�tcp����������
 *	 @param		addr:׼�����ip��ַ��ָ��
 *   @return   0��ʾ�ɹ�����ֵ��ʾʧ��
 */ 
int	get_local_ip(SOCK_FD fd,struct sockaddr_in *addr)
{
	int	ret;
	int addrlen=sizeof(struct sockaddr);
	if(addr==NULL)
	{
		//errno=EINVAL;
		return -1;
	}
	ret=getsockname(fd,(struct sockaddr *)addr,&addrlen);
	if(ret!=0)
	{
#ifdef _WIN32
		errno=WSAGetLastError();
#endif
	}
	return ret;
}


/** 
 *   @brief    �����������ӵ�����ģʽ
 *   @param  fd:�Ѿ��򿪵�tcp����������
 *   @param  en:0��ʾʹ������ģʽ
 *                     1��ʾʹ��l������ģʽ
 *   @return   0��ʾ�ɹ�,��ֵ��ʾʧ��
 */ 

int net_set_noblock(SOCK_FD fd,int en)
#ifdef _WIN32
{//windows
	int ret;
	ret = ioctlsocket(fd,FIONBIO,(unsigned long*)&en);  
	if(ret!=0)
	{
#ifdef _WIN32
		errno=WSAGetLastError();
#endif
	}
	return ret;
}
#else
{//linux
  	int curr_flags = fcntl(fd, F_GETFL);
  	if (curr_flags<0)
  	{
  	  	return -1;
  	}
	if(en)
	{
  		curr_flags |= O_NONBLOCK;
	}
	else
	{
		curr_flags &= ~O_NONBLOCK;
	}
  	return fcntl(fd, F_SETFL, curr_flags);
  
}
#endif

/** 
 *   @brief    ����ʱ������connect,����timeout������������������connect��ȫһ��
 *   @param  fd:�Ѿ������õ�socket
 *   @param  serv_addr:Ҫ���ӵĵ�ַ���˿ں�
 *   @param  addrlen:serv_addr�ṹ�ĳ���
 *   @param  timeout:��ʱ����ʱ��(��)
 *   @return   0��ʾ�ɹ�,��ֵ��ʾʧ��
 */ 
int  connect_timeout(SOCK_FD  fd,  const  struct sockaddr *serv_addr, socklen_t    addrlen,int timeout)
{
	int rc,ret;
	int sockerr;
	socklen_t sklen;
	fd_set writefds;
	struct timeval	timeval;
	if((fd<0)||(serv_addr==NULL))
	{
		errno=EINVAL;
		return -1;
	}
	rc=net_set_noblock(fd,1);
	if(rc<0)
	{
#ifdef _WIN32
		errno=WSAGetLastError();
#endif
		return -1;
	}//
	FD_ZERO(&writefds);
	rc=-1;
	do{	

		errno=0;
 		rc=connect(fd,serv_addr,addrlen);
        if(rc<0)
		{
#ifdef _WIN32
		errno=WSAGetLastError();
#endif

#ifndef _WIN32
			if(errno==EINPROGRESS)	//linux
#endif
			{
				while(1)
				{
				printf("���ڽ�������,timeout=%d\n",timeout);
				timeval.tv_sec=timeout;
				timeval.tv_usec=0;
				FD_SET(fd,&writefds);
				ret=select(fd+1,NULL,&writefds,NULL,&timeval);
				if(ret==0)
				{
					rc=-1;
					//printf("���ӳ�ʱ\n");
#ifdef _WIN32
					errno=WSAGetLastError();
#endif
					break;
				}

				if(FD_ISSET(fd,&writefds))
				{
					sockerr=-1;
					sklen=sizeof(int);
                //�����һ��һ��Ҫ����Ҫ��Է���ǽ 
					ret=getsockopt(fd, SOL_SOCKET, SO_ERROR,(void*)&sockerr, &sklen);
					//printf("�յ� fd�����ź�sockerr=%d %s\n",sockerr,strerror(sockerr));
					if(sockerr==0)
					{
						rc=0;
						//printf("���ӳɹ�!!!!!!!!!!!!!!!\n");
						break;
					}
					else if(sockerr!=EINPROGRESS)
					{
						rc=-1;
						errno=sockerr;
						break;
					}
					continue;
				}		
				else
				{
					rc=-1;
					break;
				}
				}
			}
			//else
			//	printf("���Ӵ���%d\n",errno);
		}
	}while(0);
#ifdef _WIN32
		errno=WSAGetLastError();
#endif
	net_set_noblock(fd,0);
	return rc;
}

/** 
 *   @brief    ����Զ��tcp�����ַ
 *   @param  addr_str:����Զ�̷����ַ���ַ���
 *   @param  port:Զ��tcp����Ķ˿ں�
 *   @param   timeout:tcp���ӵĳ�ʱʱ��(��)
 *   @return   ��ֵ��ʾ�����Ӻõ�����������ֵ��ʾ����
 */ 
SOCK_FD tcp_connect_addr(const char *addr_str,int port,int timeout)
{
	SOCK_FD	rmt_sock=-1;
	int	ret=-1;
	//char *remoteHost = NULL;
	struct hostent *hostPtr = NULL;
	 struct sockaddr_in serverName = { 0 };
	if((addr_str==NULL)||(port<0)||(timeout<0))
	{
		errno=EINVAL;
		return -1;
	}
#ifdef _WIN32
	   {
        WORD wVersionRequested;
        WSADATA wsaData;
        int err;
 
        wVersionRequested = MAKEWORD( 2, 0 );
 
        err = WSAStartup( wVersionRequested, &wsaData );
        if ( err != 0 ) {
            /* Tell the user that we couldn't find a usable */
            /* WinSock DLL.                               */
            //printf("can't initialize socket library\n");
			errno=WSAGetLastError();
			return -1;
        }
    }

#endif


	hostPtr = gethostbyname(addr_str); /* struct hostent *hostPtr. */ 
    if (NULL == hostPtr)
    {
		hostPtr = gethostbyaddr(addr_str, strlen(addr_str), AF_INET);
        if (NULL == hostPtr) 
        {
#ifdef _WIN32
			errno=WSAGetLastError();
#endif
//        	printf("Error resolving server address:%s\n",addr_str);
			return -1;
        }			
    }
	rmt_sock=socket(PF_INET, SOCK_STREAM,IPPROTO_TCP);
	if(rmt_sock<0)
	{
		//printf("create socket err! rmt_sock=%d errno=%d:%s\n",rmt_sock,errno,strerror(errno));
#ifdef _WIN32
		errno=WSAGetLastError();
#endif
			return -1;
	}
	serverName.sin_family = AF_INET;
    serverName.sin_port = htons((unsigned short)port);
    (void) memcpy(&serverName.sin_addr,hostPtr->h_addr,hostPtr->h_length);
    ret = connect_timeout(rmt_sock,(struct sockaddr*) &serverName,sizeof(serverName),timeout);
    if (ret<0)
    {
#ifdef _WIN32
		errno=WSAGetLastError();
#endif
    	close(rmt_sock);	
		return -1;
   	}
	return rmt_sock;	
		
}


/** 
 *   @brief    ����Զ��tcp�����ַ
 *   @param  addr_str:����Զ�̷����ַ���ַ���
 *   @param  port:Զ��tcp����Ķ˿ں�
 *   @param   timeout:tcp���ӵĳ�ʱʱ��(��)
 *   @return   ��ֵ��ʾ�����Ӻõ�����������ֵ��ʾ����
 */ 
SOCK_FD tcp_connect_block(const char *addr_str,int port,int timeout)
{
	SOCK_FD	rmt_sock=-1;
	int	ret=-1;
	//char *remoteHost = NULL;
	struct hostent *hostPtr = NULL;
	 struct sockaddr_in serverName = { 0 };
	if((addr_str==NULL)||(port<0)||(timeout<0))
	{
		errno=EINVAL;
		return -1;
	}
#ifdef _WIN32
	   {
        WORD wVersionRequested;
        WSADATA wsaData;
        int err;
 
        wVersionRequested = MAKEWORD( 2, 0 );
 
        err = WSAStartup( wVersionRequested, &wsaData );
        if ( err != 0 ) {
            /* Tell the user that we couldn't find a usable */
            /* WinSock DLL.                               */
            //printf("can't initialize socket library\n");
			errno=WSAGetLastError();
			return -1;
        }
    }

#endif


	hostPtr = gethostbyname(addr_str); /* struct hostent *hostPtr. */ 
    if (NULL == hostPtr)
    {
		hostPtr = gethostbyaddr(addr_str, strlen(addr_str), AF_INET);
        if (NULL == hostPtr) 
        {
#ifdef _WIN32
			errno=WSAGetLastError();
#endif
//        	printf("Error resolving server address:%s\n",addr_str);
			return -1;
        }			
    }
	rmt_sock=socket(PF_INET, SOCK_STREAM,IPPROTO_TCP);
	if(rmt_sock<0)
	{
		//printf("create socket err! rmt_sock=%d errno=%d:%s\n",rmt_sock,errno,strerror(errno));
#ifdef _WIN32
		errno=WSAGetLastError();
#endif
			return -1;
	}
	serverName.sin_family = AF_INET;
    serverName.sin_port = htons((unsigned short)port);
    (void) memcpy(&serverName.sin_addr,hostPtr->h_addr,hostPtr->h_length);
    ret = connect(rmt_sock,(struct sockaddr*) &serverName,sizeof(serverName));
    if (ret<0)
    {
#ifdef _WIN32
		errno=WSAGetLastError();
#endif
    	close(rmt_sock);	
		return -1;
   	}
	return rmt_sock;	
		
}


/** 
 *   @brief    ��������������ȫ��д��һ������������
 *   @param  fd:Ŀ���ļ�������
 *   @param  *buf:ָ��Ҫ���͵Ļ�������ָ��
 *   @param  len:Ҫ���͵Ļ������е���Ч��Ϣ�ֽ���
 *   @return   ��ֵ��ʾ�ɹ�д����ֽ���,��ֵ�������쳣,���������
 */ 
int net_write_buf(SOCK_FD fd,void *buf,int len)
{
	int left,writted=0; 
	char *wp; 
	int err;
	wp=buf; 
	left=len; 
	while(left>0) 
	{ 
		/* ��ʼд*/ 
		writted=write(fd,wp,left); 
		if(writted<=0) /* ������*/ 
		{ 
#ifdef _WIN32
			errno=WSAGetLastError();
#endif
			err=errno;
			if(err==EINTR) /* �жϴ��� ���Ǽ���д*/ 
				writted=0; 
#ifndef _WIN32
			else if(err==EPIPE)	//�������ӳ�����
			{
				return -EPIPE;
			}
#endif
			else
			{
				if(err>0)	/* �������� û�а취*/ 
					return -err;
				else
					return -1;

			}
		}
		left-=writted; 
		wp+=writted; /* ��ʣ�µĵط�����д */ 
	} 
	return(writted);
}


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
int net_read_buf(SOCK_FD fd,void *buf,int len) 
{ 
	int left; 
	int ret; 
	int	err;
	char *rp; 

	left=len; 
	rp=buf;
	while(left>0) 
	{ 
		ret=read(fd,rp,left); 
		printf("ret=%d\n",ret);
		if(ret<=0) 
		{ 
#ifdef _WIN32
			errno=WSAGetLastError();
			err=WSAGetLastError();
#else
			err=errno;
#endif
			if(ret==0)
                        return -140;    ///Զ�̶Ͽ�����
			if(err==EINTR)
			{
				ret=0;
				continue;
			}			
			else if(err==ETIMEDOUT)
			{
#ifdef _WIN32
				//���ճ�ʱ
				if(left<len)
					return (len-left);
				else
					return -EAGAIN;
#else
				//keepalive��ʱ
				return (0-ETIMEDOUT);
#endif
			}
			else if(err==EHOSTUNREACH)
				return (0-EHOSTUNREACH);
			else
			{
				//���ճ�ʱ
				if(err==EAGAIN)
				{				
					if(left<len)
						return (len-left);
					else
						return -EAGAIN;
				}
				if(err>0)
					return -err;
				else
					return -1;
			}
		} 
		left-=ret; 
		rp+=ret; 
	} 
	return(len-left); 
}

/*
	����˵��:
	����tcp���ӵ�����,һ������connect��accept�ճɹ��󣬰�����
	��keepalive��ʱ������Ϊ15�뷢��һ��̽�����ʧ�ܺ���10������3��
	�ر�nagle�㷨
	�ر�linger

	recv_timeout��ʾ��������û����Ӧ��ʱʱ��
	����0��ʾ�ɹ�,��ֵ��ʾʧ��
*/
int set_tcp_socket_attr(int fd,int recv_timeout)
{
	int ret=0;
        ret+=net_activate_keepalive(fd);
	ret+=net_set_keepalive_time(fd,3,15,10);
       ret+=net_set_recv_timeout(fd,recv_timeout);
    ret+=net_set_nodelay(fd);        
    ret+=net_set_linger(fd,0);
	if(ret==0)
		return 0;
	else
		return -1;
}


/** 
 *   @brief    ����һ��UDP������(���ͺͽ���)
 *   @param  ip_str :׼�������ĵ�ַ�ַ���
 *   @param  port    :׼�������Ķ˿ں�
 *   @return    ��ֵ��ʾʧ�ܣ�����ֵ��ʾ�ļ�������       
 */ 
int udp_create(IN char *ip_str,IN int port)
{
    int fd;
     struct sockaddr_in addr;
#ifdef _WIN32
    {
        WORD wVersionRequested;
        WSADATA wsaData;
        int err;
 
        wVersionRequested = MAKEWORD( 2, 0 );
 
        err = WSAStartup( wVersionRequested, &wsaData );
        if ( err != 0 ) {
            /* Tell the user that we couldn't find a usable */
            /* WinSock DLL.                               */
            //printf("can't initialize socket library\n");
			errno=WSAGetLastError();
			return -1;
        }
    }
#endif


     
    fd =socket(AF_INET,SOCK_DGRAM,0); //����socket
    if(fd == -1)
    {
            perror("Opening socket");
            return -1;
    }    
    memset(&addr,0,sizeof(addr));
    addr.sin_family= AF_INET;
    addr.sin_addr.s_addr=inet_addr(ip_str); //����ַ���˿�
    addr.sin_port=htons((short)port);

   if(net_activate_reuseaddr(fd)<0)
    {
            perror("setsocketopt:SO_REUSEADDR");
            return -1;
    }
    if(bind(fd,(struct sockaddr *)&addr,sizeof(addr))<0)
    {
        perror("bind");
        return -1;
    }

    return fd;
    
}

/** 
 *   @brief    �����������鲥����
 *   @param  fd ֮ǰ��udp_create��ȡ��������
 *   @param  multicast_addr :�鲥��ַ
 *   @return    0��ʾ�ɹ�����ֵ��ʾʧ��
 */ 
int udp_add_multicast(int fd,char *multicast_addr)
{
    int loop;
    struct ip_mreq command;
	int ret;
#if 0
#ifdef _WIN32
	//���ø��׽���Ϊ�㲥���ͣ�
	bool opt=true;
	if(setsockopt(mfd,SOL_SOCKET,SO_BROADCAST,(char FAR *)&opt,sizeof(opt)) == SOCKET_ERROR )
	{
		printf
	}
#endif
#endif               
    loop=1;
    if(setsockopt(fd,IPPROTO_IP,IP_MULTICAST_LOOP,(char*)&loop,sizeof(loop))<0)
    {
            perror("setsocketopt:IP_MULTICAST_LOOP");
            return -1;
    }
    //join multicast group
    command.imr_multiaddr.s_addr = inet_addr(multicast_addr);
    command.imr_interface.s_addr = htonl(INADDR_ANY);

    if(command.imr_multiaddr.s_addr == -1)
    {
        perror("not a legal multicast address!");
        //exit(1);
        return -1;
    }

    if(setsockopt(fd,IPPROTO_IP,IP_ADD_MEMBERSHIP,(char*)&command,sizeof(command))<0)
    {
        perror("setsockopt:IP_ADD_MEMBERSHIP");
        return -1;
    }
    else
        printf("IP_ADD_MEMBERSHIP success \n");    
    return 0;
}
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
int udp_send_data(IN int fd, IN void *data, IN int  len, IN int flags, IN struct sockaddr_in *to)
{
    socklen_t tolen=sizeof(struct sockaddr);
    return sendto(fd,data,len,flags,(struct sockaddr *)to, tolen);
}


/** 
 *   @brief    ����һ��UDP����
 *   @param  fd ֮ǰ��udp_create��ȡ��������
 *   @param  buf:׼��������ݵĻ�����
 *   @param  len:����������
 *   @param  flags:����������ʱд0
 *   @param  from:���յ������ݵ�Զ�̵�ַ
 *   @return   ��ֵ��ʾ���յ�buf�е���Ч�ֽ�������ֵ��ʾʧ��
 */ 
int udp_recv_data(IN int fd,OUT void *buf, IN int len, IN int flags, OUT struct sockaddr_in *from)
{
    socklen_t recvlen=sizeof(struct sockaddr);     
    return recvfrom(fd,buf,len,flags,(struct sockaddr *)from,&recvlen);
}

