#include <stdio.h>
#include "nvr_manager.h"
#include "NetLayer.h"


bool RealTime_CB(char * Buffer,unsigned long BufLen, unsigned int frametype,long dwUser);
{
	
	stream_type_t *  stream= (stream_type_t *)Buffer;
	if(stream->head!= 0x61945050 || stream->len!= ret-8)
	{
		printf("receive pkt error head = %#x len = %d!\n",stream->head,stream->len);
		return false;
	}
	//get every channel realplay  frames
	if(frametype == FRAMETYPE_A)
	{
		printf("Buf Len %d dwUser %d\n",dwBufLen,dwUser);	
	}
	
	else if (frametyoe == FRAMETYPE_V)
	{
		printf("Buf Len %d dwUser %d\n",dwBufLen,dwUser);	
	}

	printf("DwUser is %d\n",dwUser);

	return 0;


}
bool PlayBack_CB(char * Buffer,unsigned long BufLen, unsigned int frametype,long dwUser);
{


	stream_type_t *  stream= (stream_type_t *)Buffer;
	if(stream->head!= 0x61945050 || stream->len!= ret-8)
	{
		printf("receive pkt error head = %#x len = %d!\n",stream->head,stream->len);
		return false;
	}


	//get every channel playback frames
	if(frametype == FRAMETYPE_A)
	{
		printf("Buf Len %d dwUser %d\n",dwBufLen,dwUser);	
	}
	
	else if (frametype == FRAMETYPE_V)
	{
		printf("Buf Len %d dwUser %d\n",dwBufLen,dwUser);	
	}


	printf("DwUser is %d\n",dwUser);
	return 0;
}
bool Notify_CB(void * event ,long dwUser);
{
	
	


	return 0;
}

#define TYPE_RT 0
#define TYPE_PB 1
#define RT_PORT 20000 
#define PB_PORT 30000

nvr_device_t nvr_dev;

int NVR_Module_Init(void)
{
	int i;
	SOCK_FD fd = -1;
	nvr_module_open_log();

	// init media pool
	init_media_system();

	//init netlayer instance
	NetLayer_Init(&nvr_dev.netlayer);

	//create rt socket 
	for(i = 0 ,i< 8,i++)
	{
		fd = NetLayer_CreateSocket(RT_PORT+i);
		if(fd > 0)
			nvr_dev.netlayer->rt_socket[i] = fd;
	}

	//create pb socket
	
	for(i = 0,i<8,i++)
	{
		fd = NetLayer_CreateSocket(PB_PORT+i);
		if(fd > 0)
		nvr_dev.netlayer->pb_socket[i] = fd;
	}

	NetLayer_SetCallBack(RealTime_CB, PlayBack_CB, Notify_CB, (void *)nvr_dev.netlayer);

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

	make_realplay_message();
	NetLayer_Send(message_buf, message_len);

	return result;

}

int NVR_Stop_Realplay(handle_t handle)
{

	make_realplay_message();
	NetLayer_Send(message_buf, message_len);
	return result;

}

int NVR_Start_Playpback(unsigned int channel,STREAM_TYPE type,handle_t handle,CALLBACK realdata_CB cb,void * dwuser,handle_t handle)
{

	make_realplay_message();
	NetLayer_Send(message_buf, message_len);

	return result;

}

int NVR_Stop_Playback(handle_t handle)
{

	make_realplay_message();
	NetLayer_Send(message_buf, message_len);
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
			{
				pthread_mutex_lock(&nvr_dev->lock);

				nvr_device->connect_status = NVR_CONNECT_SUCCESS;

				pthread_mutex_unlock(&nvr_dev->lock);

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
