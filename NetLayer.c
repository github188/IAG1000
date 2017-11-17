#include "stdafx.h"
#include "NetLayer.h"



NetLayer_Init(NetLayer**  netlayer)
{
	*netlayer = malloc(netlayer,0,sizeof(NetLayer));

	*netlayer->conn_socket = -1
	*netlayer->rt_socket = -1;
	*netlayer->pb_socket = -1;
	*netlayer.m_handThread=NULL;
	*netlayer.m_bExit=false;
	*netlayer.m_MainCallBack=NULL;
	*netlayer.m_SubCallBack=NULL;
	*netlayer.m_userData=0;

}
bool NetLayer_CreateSocket(unsigned short port)
{


	SOCK_FD listen_fd = create_tcp_listen_port(INADDR_ANY,port);
	
	if(SOCKET_ERROR == listen(listen_fd,10))
	{
		printf("listen Wrong");
		return false;
	}
	
	return listen_fd;
}
bool NetLayer_Connect(const char* ipaddr, unsigned short port,NetLayer * netlayer)
{
	
	SOCKET conn_fd = -1;
	conn_fd = tcp_connect_addr(ipaddr, port, timeout);
	if(fd > 0 )
		netlayer.conn_socket = conn_fd;
	else
	  printf(" Connect %s error!\n",ipaddr);

	return NET_LAYER_SUCCESS;

}
DWORD  HandIoThread(void * parameter)
{
	NetLayer *Serv=(NetLayer *)parameter;
	fd_set  fdread,	fdwrite,fdexcept;
	struct timeval   timeout;
	timeout.tv_sec=0;
	timeout.tv_usec=300;
	while(!Serv->m_bExit)
	{
		FD_ZERO(&fdread);
        FD_ZERO(&fdwrite);
        FD_ZERO(&fdexcept);
		if (Serv->m_socket)
		{
			FD_SET(Serv->m_socket,&fdread);
		}		
		
		if(Serv->m_RtSocket!=NULL)
		{
			FD_SET(Serv->m_RtSocket,&fdread);
			
		}
		if(Serv->m_PbSocket!=NULL)
		{
			FD_SET(Serv->PbSocket,&fdread);
			
		}
		
		int rc = select(0, &fdread, &fdwrite, NULL, &timeout);
		if (rc == SOCKET_ERROR)
        {
            return -1;
        }
		else if (rc == 0)
        {
            // timeout     			 
        }
        else
        {
			
			if (FD_ISSET(Serv->m_socket, &fdread))//m_socket 用来监听
			{					
				SOCKADDR_IN addr = {0};
				int	 addrLen = sizeof(SOCKADDR_IN);
				if(NULL==Serv->m_ClientSocket)
				{
					Serv->m_ClientSocket= accept(Serv->m_socket, (SOCKADDR*)&addr, &addrLen);
					unsigned long flags = 1;
				int ret = ioctlsocket(Serv->m_ClientSocket, FIONBIO, &flags);
				}
				else if(NULL==Serv->m_ClientSubSocket)
				{
					Serv->m_ClientSubSocket= accept(Serv->m_socket, (SOCKADDR*)&addr, &addrLen);
					unsigned long flags = 1;
					int ret = ioctlsocket(Serv->m_ClientSubSocket, FIONBIO, &flags);
				}
				else
				{
					return 0;
				}
				
								
			}
			
			if (FD_ISSET(Serv->m_ClientSocket, &fdread))//只有一个主客户端
			{				
				Serv->ResvM();
			}
			if (FD_ISSET(Serv->m_ClientSocket, &fdread))//只有一个子客户端
			{				
				Serv->ResvSub();
			}
			
		}
	}
	return 0;
}
bool NetLayer_Create_Server(NetLayer * netlayer)
{

		netlayer->m_handThread = gt_create_thread(&thread_id, HandIoThread, args);
}
char buf[1024*128];

bool NetLayer_ResvM()
{
	memset(buf,0,1024*128);	
	int rec=recv(m_ClientSocket,(char*)buf,sizeof(buf),0);
	if(rec==0)
	{
		closesocket(m_ClientSocket);
		m_ClientSocket=NULL;		
		//AfxMessageBox("graceful close");
		return false;
		
	}
	else if(SOCKET_ERROR==rec)
	{
		closesocket(m_ClientSocket);
		m_ClientSocket=NULL;
		//AfxMessageBox("error close");
		return false;
	}
	else
	{
		m_MainCallBack(MainData,buf,rec,m_userData);
		return true;
	}
	

	return true;
}
bool NetLayer_ResvSub()
{
	memset(buf,0,1024*128);	
	int rec=recv(m_ClientSubSocket,(char*)buf,sizeof(buf),0);
	if(rec==0)
	{
		closesocket(m_ClientSubSocket);
		m_ClientSubSocket=NULL;		
		//AfxMessageBox("graceful close");
		return false;
		
	}
	else if(SOCKET_ERROR==rec)
	{
		closesocket(m_ClientSubSocket);
		m_ClientSubSocket=NULL;
		//AfxMessageBox("error close");
		return false;
	}
	else
	{
		m_SubCallBack(SubData,buf,rec,m_userData);
		return true;
	}
	
	return true;
}

bool NetLayer_SetCallBack(NetLayer * netlayer, Data_CB realdata_callback, 
			Data_CB playback_callback, Notify_CB notify_callback, long UserData)
{

	netlayer->rt_cb= realdata_callback;
	netlayer->pb_cb= playbackdata_callback;
	netlayer->not_cb= notify_callback;
	netlayer.m_userData=UserData;
	return true;
}

bool NetLayer_Send(char* bufIN,long len)
{
	char buf[1000];//占订1000
	memset(buf,0,1000);
	memcpy(buf,bufIN,len);
	if(netlayer.conn_socket==NULL)
	{
		printf("Waiting for connect to NVR ");
		return false;
	}
	if(SOCKET_ERROR==send(netlayer.conn_socket,(char*)buf,(int)len,0))
	{
		return false;
	}
	
	return false;
}
