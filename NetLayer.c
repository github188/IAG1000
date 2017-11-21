#include "stdafx.h"
#include "NetLayer.h"



int NetLayer_Init(NetLayer**  netlayer)
{
	* netlayer = malloc(netlayer,0,sizeof(NetLayer));
	if(* netlayer == NULL)
		return NET_LAYER_FAIL;

	* netlayer->conn_socket = -1
	* netlayer->rt_socket = -1;
	* netlayer->pb_socket = -1;
	* netlayer->m_handThread = NULL;
	* netlayer->m_bExit = false;
	* netlayer->pb_cb = NULL;
	* netlayer->rt_cb = NULL;
	* netlayer->not_cb = NULL;
	* netlayer->m_userData = 0;

}

int NVR_Module_Uninit(void);
bool NetLayer_CreateSocket(unsigned short port)
{


	/*
	SOCK_FD listen_fd = create_tcp_listen_port(INADDR_ANY,port);
	
	if(SOCKET_ERROR == listen(listen_fd,10))
	{
		printf("listen Wrong");
		return NET_LAYER_FAIL;
	}
	
	return listen_fd;
	*/
	SOCK_FD listen_fd= udp_create(INADDR_ANY, port);
	if(SOCKET_ERROR = listen_fd)
	{
		printf("listen Wrong");
		return NET_LAYER_FAIL;
	}

	return listen_fd;




}


//ipaddr 地址
//port  端口
//netlayer 实例
//return 成功 1:)
int NetLayer_Connect(const char* ipaddr, unsigned short port,NetLayer * netlayer)
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


	char framebuf[1024*128];
	unsigned int framelen = sizeof(framebuf);
	NetLayer *Serv=(NetLayer *)parameter;
	fd_set  fdread,	fdwrite,fdexcept;
	struct timeval   timeout;
	struct sockaddr_in client;
	timeout.tv_sec=0;
	timeout.tv_usec=300;
	for(i=0;i<8;i++)
	{
	
		if(maxfdx <= Serv->rt_socket[i])

			maxfds = Serv->rt_socket[i];


		if(maxfdx <= Serv->pb_socket[i])

			maxfds = Serv->pb_socket[i];
	
	}

	while(!Serv->m_bExit)
	{
		FD_ZERO(&fdread);
        FD_ZERO(&fdwrite);
        FD_ZERO(&fdexcept);
		for(i = 0;i < 8 ;i++)
		{
		
			if(Serv->rt_socket[i] > 0)

				FD_SET(Serv->rt_socket[i], &fdread);

			if(Serv->rt_socket[i] > 0)

				FD_SET(Serv->rt_socket[i], &fdread);

		
		}
		
		if(Serv->conn_socket !=-1)
		  
				FD_SET(Serv->conn_socket, &fdread);


		
		int rc = select(maxfds+1, &fdread, NULL, NULL, &timeout);
		if (rc == SOCKET_ERROR)
        {
			printf("Select return %d\n",rc);
			continue;
        }
		else if (rc == 0)
        {
            // timeout     			 
        }
        else
        {
			
			for(i = 0;i < 8;i++)
			{

				if (FD_ISSET(Serv->rt_socket[i], &fdread))//只有一个主客户端
				{
					if(udp_recv_data(Serv->rt_socket[i], framebuf, framelen, 0, &client)<0)

						printf("receive data error!\n");;

					Serv->rt_cb(framebuf, len, ts, type, i);
				}
			}
			for(i = 0;i < 8;i++)
			{

				if (FD_ISSET(Serv->rt_socket[i], &fdread))//只有一个主客户端
				{
					if(udp_recv_data(Serv->rt_socket[i], framebuf, framelen, 0, &client)<0)

						printf("receive data error!\n");;

					Serv->pb_cb(framebuf, len, ts, type, i);

				}
			}
			if(FD_ISSET(Serv->conn_socket,&fdread)
			
					Serv->not_cb(message,Serv->m_userData);
			
		}
	}
	return 0;
}
bool NetLayer_Create_Server(NetLayer * netlayer)
{

		netlayer->m_handThread = gt_create_thread(&thread_id, HandIoThread, (void * )netlayer);
}


bool NetLayer_SetCallBack(Data_CB realdata_callback, Data_CB playback_callback, Notify_CB notify_callback, void * UserData)
{

	NetLayer * netlayer = (NetLayer * )UserData;
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
