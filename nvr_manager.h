#ifndef _NVR_MANAGER_
#define _NVR_MANAGER_
#include "NetLayer.h"

typedef struct
{
	char [100] firmware;
	char [24] ipaddr;
	unsigned short port;
	NetLayer  * netlayer;
	realdata_CB rt_cb;
	playback_CB pb_cb;
	notify_CB   not_cb;
	SOCKET conn_fd;
		
	
}nvr_device_t;


enum EVENT
{
			


}

typedef int (CALLBACK * Data_CB )(unsigned char * data,unsigned int len,unsigned int ts,void * dwuser)

typedef int (CALLBACK * Notify_CB )(enum EVENT event,void * dwuser)

int NVR_Module_Init(void);

int NVR_Module_Connect(char * ipaddr,short port,notify_CB cb);

int NVR_Start_Realplay(realdata_CB cb,void * dwuser);

int NVR_Start_Playback(playback_CB cb,void * dwuser);

int NVR_Module_Disconnect(int handle);

int NVR_Module_Uninit(void);







#endif
