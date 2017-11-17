#include <stdio.h>
#include "nvr_manager.h"
#include "NetLayer.h"


bool RealTime_CB(long lLoginID, char *pBuf,unsigned long dwBufLen, unsigned int frametype,long dwUser);
{
	
	//get every channel realplay  frames
	if(frametype == FRAMETYPE_A)
	{
		printf("Buf Len %d dwUser %d\n",dwBufLen,dwUser);	
	}
	
	else if (frametyoe == FRAMETYPE_V)
	{
		printf("Buf Len %d dwUser %d\n",dwBufLen,dwUser);	
	}
	nvr_device_t * device = (nvr_device_t* )dwUser;


	return 0;


}
bool PlayBack_CB(long lLoginID, char *pBuf,unsigned long dwBufLen, unsigned int frametype,long dwUser);
{


	//get every channel playback frames
	if(frametype == FRAMETYPE_A)
	{
		printf("Buf Len %d dwUser %d\n",dwBufLen,dwUser);	
	}
	
	else if (frametype == FRAMETYPE_V)
	{
		printf("Buf Len %d dwUser %d\n",dwBufLen,dwUser);	
	}

	nvr_device_t * device = (nvr_device_t* )dwUser;
	return 0;
}
bool Notify_CB(long lLoginID, char *pBuf,unsigned long dwBufLen, unsigned int frametype,long dwUser);
{


	//get every channel playback frames
	if(frametype == FRAMETYPE_A)
	{
		printf("Buf Len %d dwUser %d\n",dwBufLen,dwUser);	
	
	
	}
	
	else if (frametype == FRAMETYPE_V)
	{
	
		
		printf("Buf Len %d dwUser %d\n",dwBufLen,dwUser);	
	
	}
	return 0;
}

#define TYPE_RT 0
#define TYPE_PB 1

nvr_device_t nvr_dev;

int NVR_Module_Init(void)
{
	SOCK_FD fd = -1;
	nvr_module_open_log();

	// init media pool
	init_media_system();

	//init netlayer instance
	NetLayer_Init(&nvr_dev.netlayer);

	//create rt socket 
	fd = NetLayer_CreateSocket(RT_PORT);
	nvr_dev->netlayer->rt_socket = fd;

	//create pb socket
	fd = NetLayer_CreateSocket(PB_PORT);
	nvr_dev->netlayer->pb_socket = fd;

	NetLayer_SetCallBack(nvr_dev->netlayer, RealTime_CB, PlayBack_CB, Notify_CB, (void *)nvr_dev);

	//set rt pb cmd fds to readfds 
	NetLayer_Create_Server();
	

	return result;



}
// return 
int  NVR_Module_Connect(nvr_device_t* nvr_dev, const char* ipaddr, unsigned short port)
{

	return NetLayer_Connect(nvr_dev->netlayer, ipaddr, port);
	

}

//handle  no sense
int NVR_Start_Realplay(handle_t handle,unsigned int channel,STREAM_TYPE type,void * dwuser,handle_t handle)
{

	set_realdata_cb(cb);

	return result;

}

int NVR_Stop_Realplay(handle_t handle)
{

	return result;

}

int NVR_Start_Playpback(unsigned int channel,STREAM_TYPE type,handle_t handle,CALLBACK realdata_CB cb,void * dwuser,handle_t handle)
{

	set_playback_cb(cb);

	return result;

}

int NVR_Stop_Playback(handle_t handle)
{

	return result;

}

int NVR_Upgrade(handele_t handle,const char * pfile,fProgressCallBack cb,void * dwuser)
{

	return result;


}

int NVR_Set_DigitChnConfig(int channel,DIGITCONIFG * config)
{

	return result;

}
int NVR_Get_DigitChnConfig(int channel,DIGITCONIFG * config)
{

	return result;


}
int NVR_Module_Disconnect(handle_t handle)
{

	disconnect_nvr(handle)
	
	return result;

}


int NVR_Module_Uninit(void)
{

	nvr_module_close_log();

	uninit_meida_system();

	return result;

}


int nvr_manager_thread(void)
{
	int result  = NVR_MOUDULE_ERR;

	//start server
	NVR_Module_Init();

	while(1)
	{

		while(nvr_device->connect_status!= NVR_CONNECT_SUCCESS)
		{

			if(NVR_CONNECT_SUCCESS == NVR_Module_Connect(&nvr_dev, ipmain_para->nvr_addr, ipmain_para->nvr_port))

				nvr_device->connect_status = NVR_CONNECT_SUCCESS;

		}
		
		sleep(1);
	
	}
	

}
int create_nvr_manager_thread(attr)
{
	int                         i;
	pthread_t					thread_id;
    struct ipmain_para_struct * ipmain_para;	
	ipmain_para = get_mainpara();
	if(!ipmain_para->use_nvr)
	{
		printf("use onviflib to connect ipcs\n");	
		gt_create_thread(&thread_id,onvif_manager_thread,NULL);
		return 0;
	
	}
	else
	{

		printf("use nvr_manager to connect ipcs\n");	
		gt_create_thread(&thread_id,nvr_manager_thread,NULL);
	}
	return 0;
}
