#ifndef _NET_LAYER_H_
#define _NET_LAYER_H_

#include<WinSock2.h>

typedef bool (__stdcall *fMessCallBack)(long lLoginID, char *pBuf,
									   unsigned long dwBufLen, long dwUser);

typedef struct 
{
	SOCKET          conn_socket;
	SOCKET			rt_socket; 
	SOCKET			pb_socket;
	HANDLE			m_handThread;
	bool			m_bExit;
	Data_CB			rt_cb;
	Data_CB			pb_cb;
	Notify_CB		not_cb;
	long			m_userData;
}NetLayer;

typedef int (CALLBACK * Data_CB )(unsigned char * data,unsigned int len,unsigned int ts,void * dwuser)

typedef int (CALLBACK * Notify_CB )(enum EVENT event,void * dwuser)

bool NetLayer_SetCallBack(NetLayer * netlayer, Data_CB realdata_callback, 
			Data_CB playback_callback, Notify_CB notify_callback, long UserData);



#endif
