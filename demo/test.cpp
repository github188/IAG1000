#include <stdio.h>
#include <memory.h>
#include   <unistd.h>
#include <iostream>
#include "librtmp_send264.h"
using namespace std;
#include "netsdk.h"
#include <signal.h>
#include <stdlib.h>
#include <pthread.h>
NaluUnit nalu;
static void exit_log(int signo)
{
     switch(signo)
    {
	           case SIGPIPE:
	               printf("ipmain process_sig_pipe \n");
	               return ;
	           break;
	           case SIGTERM:
	               printf("ipmain 被kill,程序退出!!\n");
					           exit(0);
					           break;
					           case SIGKILL:
					               printf("ipmain SIGKILL,程序退出!!\n");
					               exit(0);
					           break;
					           case SIGINT:
					               printf("ipmain 被用户终止(ctrl-c)\n");
					               exit(0);
					           break;
					           case SIGUSR1:
					           break;
					           case SIGSEGV:
					               printf("ipmain 发生段错误\n");
					               printf("ipmain segmentation fault\n");
					               exit(0);
					           break;
					       }
			       return;
			   }




int read_pack(NaluUnit &nalu ,char *buffer_tmp,char *buffer,int len,int * g_pose)
{
	int nalhead_pos = *g_pose;
	int naltail_pos = nalhead_pos;
	int nalustart = 0;
	while(nalhead_pos <= len)
	{
		if(buffer[nalhead_pos ++] == 0x00 &&buffer[nalhead_pos ++] == 0x00)
		{
			if(buffer[nalhead_pos ++]	== 0x01)
			{

				nalustart = 3;
				
				//got nal
				goto gotnal_head;

			}

			else
			{
			
				nalhead_pos --;	
				if(buffer[nalhead_pos++] == 0x00 && buffer[nalhead_pos++] == 0x01)
				{
					nalustart = 4;	
					//got nal
					goto gotnal_head;
				
				}
			
			}
		
		}
		else 
		  continue;
gotnal_head:

		naltail_pos = nalhead_pos;
		while(naltail_pos < len -4) //遍历内存找到下一个nalu
		{

			if(buffer[naltail_pos ++] == 0x00 && buffer[naltail_pos ++] == 0x00)
			{
				if(buffer[naltail_pos++] == 0x01)
				{


					nalu.size = naltail_pos -3- nalhead_pos;
					*g_pose = naltail_pos -3;;

					nalu.type = buffer[nalhead_pos]&0x1f;
					//if(nalu.type == 0x06)
					memcpy(buffer_tmp, &buffer[nalhead_pos], nalu.size );
					nalu.data = buffer_tmp;


					return 1;
				}

				else if(buffer[naltail_pos] == 0x00 && buffer[naltail_pos+1] == 0x01)
				{

						naltail_pos+=2;
						nalu.size = naltail_pos -4 - nalhead_pos;

						*g_pose = naltail_pos -4;
						nalu.type = buffer[nalhead_pos]&0x1f;
						memcpy(buffer_tmp, &buffer[nalhead_pos], nalu.size );
						nalu.data = buffer_tmp;

					return 1;
					
				}
				else
				{

					naltail_pos-=2 ;	
					//printf("wocaowocao pose = %d\n",naltail_pos);
				}
				
				

		
			}

		}
		//不然就等于整个数据长度
		if(naltail_pos >= len-4)
		{
			*g_pose = len;
			nalu.size  = len - nalhead_pos ;//总长度-头长度
			nalu.type = buffer[nalhead_pos]&0x1f;
			memcpy(buffer_tmp, &buffer[nalhead_pos], nalu.size );
			nalu.data = buffer_tmp;


			return 2;
		}



	}
	return 1;

}


	FILE * g_pFile;
int  NetDataCallBack( long lRealHandle, long dwDataType, unsigned char *pBuffer,long lbufsize,long dwUser)
{	
	//printf("lbufsize:%ld\n",lbufsize);
	
	BOOL bResult = TRUE;
	try
	{	
		fwrite(pBuffer,1,lbufsize,g_pFile);
	}
	catch (...)
	{
		
	}

	// it must return TRUE if decode successfully,or the SDK will consider the decode is failed
	return bResult;

}
char buffer[200*1024];
int g_send_sps = 0;
int  RealDataCallBack_V2(long lRealHandle, const PACKET_INFO_EX *pFrame, long dwUser)
{
	printf("time:%04d-%02d-%02d %02d:%02d:%02d\n",pFrame->nYear,pFrame->nMonth,pFrame->nDay,pFrame->nHour,pFrame->nMinute,pFrame->nSecond);
	//printf("size %d  type %d encodetype %d user:%d\n",pFrame->dwPacketSize ,pFrame->nPacketType ,pFrame->nEncodeType,dwUser);
	BOOL bResult = TRUE;
	//unsigned char  * data = (unsigned char *)pFrame->pPacketBuffer;
	//unsigned char  * data = (unsigned char *)buffer;

int g_pose = 0;
	try
	{	
		fwrite(pFrame->pPacketBuffer,1,pFrame->dwPacketSize,g_pFile);
		//if(pFrame->nPacketType  == 1)
			//printf("%02x %02x %02x %02x %02x %02x %02x %02x\n",data[0],data[1],data[2],data[3],data[4],data[5],data[6],data[7]);
			while(g_pose!= pFrame->dwPacketSize)
				{
					read_pack(nalu,buffer,pFrame->pPacketBuffer,pFrame->dwPacketSize,&g_pose);
					//printf("nalu type is %d len is %d gpose %d\n",nalu.type, nalu.size, g_pose);
					//printf("%02x %02x %02x %02x %02x %02x %02x %02x\n",data[0],data[1],data[2],data[3],data[4],data[5],data[6],data[7]);
					RTMP264_Send(&nalu,nalu.size);
				}

			/*
			if(g_send_sps == 0)
			
			{
				while(g_pose!= pFrame->dwPacketSize)
				{
					read_pack(nalu,buffer,pFrame->pPacketBuffer,pFrame->dwPacketSize,&g_pose);
					//printf("nalu type is %d len is %d gpose %d\n",nalu.type, nalu.size, g_pose);
					//printf("%02x %02x %02x %02x %02x %02x %02x %02x\n",data[0],data[1],data[2],data[3],data[4],data[5],data[6],data[7]);
					RTMP264_Send(&nalu,nalu.size);
				}
				g_send_sps = 1;
			}
			
			else
			{
				if(pFrame->nPacketType == 1) //I frame
				{
					nalu.type = 5;
				}
				else
				{

					nalu.type = 1;
				
				
				}
				nalu.data =  pFrame->pPacketBuffer ;
				nalu.size = pFrame->dwPacketSize;

				int ret = RTMP264_Send(&nalu,nalu.size);
				if(ret != TRUE)
				  printf("ret =%d\n",ret);
			
			
			
			}
			*/

	
	}
	catch (...)
	{

	}

	// it must return TRUE if decode successfully,or the SDK will consider the decode is failed
	return bResult;
}
void  TalkDataCallBack(LONG lTalkHandle, char *pDataBuf, long dwBufSize, char byAudioFlag, long dwUser)
{
	BOOL bResult = TRUE;
	try
	{
		printf("come TalkDataCallBack :%ld\n",dwBufSize);

		if(g_pFile)
		{
			//printf("come in fwrite");
			fwrite(pDataBuf,1,dwBufSize,g_pFile);
		}
		else
		{
			printf("###################################fWrite wrong!!!!!\n");
		}
		
	}
	catch (...)
	{
		
	}
	// it must return TRUE if decode successfully,or the SDK will consider the decode is failed
	return ;
}

long g_LoginID=0;
bool DevicCallBack(long lLoginID, char *pBuf,
				   unsigned long dwBufLen, long dwUser,int type)
{
	printf("one devic is comeing!");
		//H264_DVR_ACTIVEREG_INFO *info=(H264_DVR_ACTIVEREG_INFO *)pBuf;
		//printf("das device : %s",info->deviceSarialID);
		g_LoginID=lLoginID;
}
//#define SEARCHDevice  
//#define SearchDeviceV2
//#define  DAS
//#define Talk
//#define RealPlay
//#define Config
//#define PLAYBACK_BYNAME
//#define PLAYBACK_BYNAME_V2
//#define PlayBack_BYTIME
#define PlayBack_BYTIME_V2
void* START_ROUTINE(LPVOID lpThreadParameter)
{
	
	H264_DVR_CLIENTINFO playstru;

	playstru.nChannel = 0;
	playstru.nStream = 0;
	playstru.nMode = 0;
	long nPlayHandle = H264_DVR_RealPlay(g_LoginID,&playstru );
	printf("nPlayHandle=%ld\n",nPlayHandle);
	sleep(1000);
	//return 1;		
}
pthread_t id;
void* SearchDevice(void *arr)
{
	SDK_CONFIG_NET_COMMON_V2 m_Device[100];
	int nRetLength = 0;
	cout<<"start"<<endl;
	bool bRet= H264_DVR_SearchDevice((char*)m_Device,sizeof(SDK_CONFIG_NET_COMMON_V2)*100,&nRetLength,5000);
	if(bRet)
	{		
		cout<<"m_Device->HostIP"<<m_Device[1].HostName<<endl;
		cout<<"m_Device->HostIP2"<<m_Device[2].HostName<<endl;
		printf("H264_DVR_SearchDevice ok number is [%d]\n",nRetLength/sizeof(SDK_CONFIG_NET_COMMON_V2));
		pthread_join(id, NULL);
	}
}
static int s_flag = 0;
void pfNetComFun(SDK_CONFIG_NET_COMMON_V2 *pNetCom, unsigned long userData)
{
	printf("data============[%d]\n",s_flag);

	cout<<"m_Device->HostIP"<<pNetCom[0].HostName<<endl;
}

void DisConnect(long lLoginID, char *pchDVRIP, long nDVRPort, unsigned long dwUser)
{


	int nError = 0;		

	H264_DVR_DEVICEINFO OutDev;	
	cout << "loginid" <<lLoginID<< endl;
	cout << "ip" << pchDVRIP<< endl;
	cout << "port" << nDVRPort<< endl;
	cout << "dwuser" << dwUser<< endl;
	//g_LoginID = H264_DVR_Login((char*)pchDVRIP, 34567, (char*)"admin",(char*)"11111111",(LPH264_DVR_DEVICEINFO)(&OutDev),&nError);	
	//printf("g_LoginID=%d,nError:%d\n",g_LoginID,nError);
	

}

void SubDisConnectCallBack(long lLoginID, SubConnType type, long nChannel, long dwUser)
{
	cout << "loginid" <<lLoginID<< endl;
	cout << "type" << type << endl;
	cout << "channel" << nChannel<< endl;
	cout << "dwuser" << dwUser<< endl;
	


}

int g_end_play = 0;
void	DownLoadPosCallBack (long lPlayHandle, long lTotalSize, long lDownLoadSize, long dwUser)
{

	cout << "handle" <<lPlayHandle<< endl;
	cout << "totalsize " << lTotalSize << endl;
	cout << "download size " << lDownLoadSize<< endl;
	cout << "dwuser" << dwUser<< endl;
	if( lDownLoadSize  == -1)
	{
		int ret = 0;
		g_end_play = 1;
	}

		


}
int  main(void)
{
	signal(SIGKILL,exit_log);
	signal(SIGTERM,exit_log);
	signal(SIGINT,exit_log);
	signal(SIGSEGV,exit_log);
	signal(SIGPIPE,exit_log);
	
	g_pFile=NULL;
	while(!RTMP264_Connect("rtmp://192.168.31.105/myapp/live"))
	{
		
		sleep(1);
	}


	H264_DVR_Init(&DisConnect,NULL);
	exit(0);



#ifdef SEARCHDevice
	int ret = pthread_create(&id, NULL,SearchDevice, NULL);
	if(ret)
	{
		printf("Create pthread error\n");
	}
#endif
#ifdef SearchDeviceV2
		H264_DVR_SearchDevice_V2(pfNetComFun,0, 5000);
#endif

#ifdef DAS
	cout<<"start das"<<endl;
	bool ret =H264_DVR_StartActiveRigister(9400,DevicCallBack,1);
	cout<<"end das"<<endl;
	if(ret>0)
	{
		cout<<"********sleep 2 minit************ "<<endl;
		sleep(120);
	}	
	else
	{
		printf("Call H264_DVR_StartActiveRigister**********wrong£¡");
	}
#else
	H264_DVR_DEVICEINFO OutDev;	
	memset(&OutDev,0,sizeof(OutDev));
	int nError = 0;		
	//ret = H264_DVR_SetLocalBindAddress("192.168.1.251");
	//cout << "ret = "<< ret<<endl;
	int ret = H264_DVR_SetConnectTime(20000,10);

	cout << "ret = "<< ret<<endl;

	g_LoginID = H264_DVR_Login((char*)"192.168.1.108", 34567, (char*)"admin",(char*)"11111111",(LPH264_DVR_DEVICEINFO)(&OutDev),&nError);	
	printf("g_LoginID=%d,nError:%d\n",g_LoginID,nError);

	//ret = H264_DVR_SetKeepLifeTime(g_LoginID,1,5);

	//cout << "set keep life time ret = "<< ret<<endl;

	ret = H264_DVR_SetSubDisconnectCallBack(SubDisConnectCallBack,9999);
	cout << "set sub disconnect cb ret = "<< ret<<endl;
	/*
	SDK_DVR_WORKSTATE *pWorkState;
	H264_DVR_GetDVRWorkState(g_LoginID,pWorkState);
	cout <<"bitrate"<<pWorkState->vChnState[0].iBitrate<<endl;
	*/
	//ret = H264_DVR_GetLastError();

	//cout << "ret = "<< ret<<endl;

	
	/*if(g_LoginID)
	{
		SDK_ChannelNameConfigAll ChannelName;
		memset(&ChannelName, 0, sizeof(SDK_ChannelNameConfigAll));
		memcpy(ChannelName[0],"陈间", sizeof("陈间"));
		int nRet = H264_DVR_SetDevConfig(g_LoginID, E_SDK_CONFIG_CHANNEL_NAME, -1, (char*)&ChannelName, sizeof(SDK_ChannelNameConfigAll), 3000);
		if(nRet)
		{
			printf("设置成功了\n");
		}
	}*/
	//pthread_t handle;
	//pthread_create(&handle,NULL,START_ROUTINE,NULL);
	
#ifdef ADDUSER	
	USER_INFO userInfo;	
	strcpy(userInfo.Groupname,"admin");	
	strcpy(userInfo.memo,"");
	strcpy(userInfo.name,"12345");
	strcpy(userInfo.passWord,"");

	userInfo.reserved=false;
	userInfo.rigthNum=5;
	strcpy(userInfo.rights[0],"ShutDown");
	strcpy(userInfo.rights[1],"ChannelTitle");
	strcpy(userInfo.rights[2],"RecordConfig");
	strcpy(userInfo.rights[3],"Replay_01");
	strcpy(userInfo.rights[4],"Monitor_01");
	userInfo.shareable=true;
	long lRet2 = H264_DVR_SetDevConfig(g_LoginID, E_SDK_CONFIG_ADD_USER, -1, (char*)&userInfo, sizeof(userInfo), 10000);
	if(lRet2 >0)
	{
	   printf("add user ok\n");
	}else
	{
	   printf("add user err\n");
	}
#endif
	
#endif
	
	H264_DVR_FINDINFO findInfo;
	findInfo.nChannelN0=0;
	findInfo.nFileType=0;			//文件类型, 见SDK_File_Type
	
	time_t t = time(NULL);

	tm temp,temp1;
	tm * tt = &temp;
	localtime_r(&t,tt);
	t-=200;
	tm * tts = &temp1;
	localtime_r(&t,tts);
	

	findInfo.startTime.dwYear = tt->tm_year+1900;
	findInfo.startTime.dwMonth = tt->tm_mon+1;
	findInfo.startTime.dwDay = tt->tm_mday;
	findInfo.startTime.dwHour = 0;
	findInfo.startTime.dwMinute = 0;
	findInfo.startTime.dwSecond = 0;

	findInfo.endTime.dwYear = tt->tm_year+1900;
	findInfo.endTime.dwMonth = tt->tm_mon+1;
	findInfo.endTime.dwDay = tt->tm_mday;
	findInfo.endTime.dwHour = 23;
	findInfo.endTime.dwMinute = 59;
	findInfo.endTime.dwSecond = 59;
	
				 	g_pFile = fopen("testPlayBack", "wb+");
	H264_DVR_FILE_DATA *pData = new H264_DVR_FILE_DATA[64];
	int nFindCount = 0;
	
	long lRet= H264_DVR_FindFile(g_LoginID, &findInfo, pData, 64, &nFindCount, 3000);
	if(lRet>0&&nFindCount>0)
	{
	   	printf("search success,playback file num=%d\n", nFindCount);
#ifdef PLAYBACK_BYNAME
	 	lRet = H264_DVR_PlayBackByName(g_LoginID, &pData[0], DownLoadPosCallBack,NetDataCallBack, NULL);
		if(lRet>0)
		{
			sleep(100);
			printf("Play success\n");
		}else
		{
			printf("Play failed,lRet=%ld\n",lRet);
		}
#else

#ifdef PLAYBACK_BYNAME_V2
		lRet = H264_DVR_PlayBackByName_V2(g_LoginID, &pData[0], DownLoadPosCallBack,RealDataCallBack_V2, NULL);
		if(lRet>0)
		{
			sleep(100);
			printf("Play success\n");
		}else
		{
			printf("Play failed,lRet=%ld\n",lRet);
		}
#endif
#endif


	//»Ø·Å
					H264_DVR_FINDINFO info;
				 	memset(&info, 0, sizeof(info));
				 	info.nChannelN0=1;
				 	info.nFileType=0;
					printf(" start %d:%d:%d\n",tts->tm_hour,tts->tm_min,tts->tm_sec);
				 	info.startTime.dwYear = tts->tm_year+1900;
				 	info.startTime.dwMonth = tts->tm_mon+1;
				 	info.startTime.dwDay = tts->tm_mday;
				 	info.startTime.dwHour = tts->tm_hour;
				 	info.startTime.dwMinute = tts->tm_min;
				 	info.startTime.dwSecond = tts->tm_sec;
				 

					printf(" end %d:%d:%d\n",tt->tm_hour,tt->tm_min,tt->tm_sec);
				 	info.endTime.dwYear = tt->tm_year+1900;
				 	info.endTime.dwMonth = tt->tm_mon+1;
				 	info.endTime.dwDay = tt->tm_mday;
				 	info.endTime.dwHour = tt->tm_hour;
				 	info.endTime.dwMinute = tt->tm_min;
				 	info.endTime.dwSecond = tt->tm_sec;
				 	//g_pFile = fopen("testPlayBack", "wb+");
					long ret=0;
				 
#ifdef PlayBack_BYTIME
				 	ret=H264_DVR_PlayBackByTime(g_LoginID,&info,DownLoadPosCallBack,NetDataCallBack,1);
					if(ret)
				 	{
				 		printf("######playBackByTime######### %d\n",ret);
				 		sleep(1000);	

				 	}
				 	else
				 	{
				 		printf("#############playbackWrong#####:%ld\n",ret);
				 	}
#else

#ifdef PlayBack_BYTIME_V2


					//dwuser 要穿rtmp对象 ，因为如果回调发送失败就要重新连接rtmp server
					ret=H264_DVR_PlayBackByTime_V2(g_LoginID,&info,RealDataCallBack_V2,1,DownLoadPosCallBack,NULL);
					sleep(3);

					printf("playback hd = %d\n",ret);

					ret = H264_DVR_StopPlayBack(ret);
						printf("stop playback end ret = %d\n",ret);

						
					while(0)
					{
						sleep(5);	
						if(g_end_play ==1)
						{
							printf("stop playback start ret = %d\n",ret);
							ret = H264_DVR_StopPlayBack(ret);
							printf("stop playback end ret = %d\n",ret);
						}
	
						/*
							t = time(NULL);
							localtime_r(&t,tt);
							t-=20;
							localtime_r(&t,tts);
		
							printf(" start %d:%d:%d\n",tts->tm_hour,tts->tm_min,tts->tm_sec);
							info.startTime.dwYear = tts->tm_year+1900;
							info.startTime.dwMonth = tts->tm_mon+1;
							info.startTime.dwDay = tts->tm_mday;
							info.startTime.dwHour = tts->tm_hour;
							info.startTime.dwMinute = tts->tm_min;
							info.startTime.dwSecond = tts->tm_sec;
					 

							printf(" end %d:%d:%d\n",tt->tm_hour,tt->tm_min,tt->tm_sec);
							info.endTime.dwYear = tt->tm_year+1900;
							info.endTime.dwMonth = tt->tm_mon+1;
							info.endTime.dwDay = tt->tm_mday;
							info.endTime.dwHour = tt->tm_hour;
							info.endTime.dwMinute = tt->tm_min;
							info.endTime.dwSecond = tt->tm_sec;
						

							g_end_play = 0;
							ret=H264_DVR_PlayBackByTime_V2(g_LoginID,&info,RealDataCallBack_V2,1,DownLoadPosCallBack,NULL);
							*/

					}
					if(ret)
				 	{
				 		printf("######playBackByTime#########\n");
						//H264_DVR_SetPlayPos(g_LoginID,50);
				 		printf("######PlaybackControl pause######### \n");
						H264_DVR_PlayBackControl(ret, SDK_PLAY_BACK_PAUSE,10);


				 		sleep(5);	

				 		printf("######PlaybackControl continue######### %d\n");
						H264_DVR_PlayBackControl(ret, SDK_PLAY_BACK_CONTINUE,4);

				 		sleep(5);	


						ret = H264_DVR_PlayBackControl(ret, SDK_PLAY_BACK_SEEK,3);

				 		printf("######PlaybackContral######### seek%d\n",ret);

				 		sleep(1000);	


				 	}
				 	else
				 	{
				 		printf("#############playbackWrong#####:%ld\n",ret);
				 	}
	
				 	
#endif
#endif	
	
	}	
	
 	if(g_LoginID>0)
 	{
		 		printf("**************login ok***************\n");	
				//¶Ôœ²
#ifdef Talk
 		g_pFile = fopen("TestTalk", "wb+");			
 		long lHandle = H264_DVR_StartVoiceCom_MR(g_LoginID, TalkDataCallBack, 0);
 		if ( lHandle <= 0 )
 		{
 			printf("start talk wrong");
 		}
 		else
 		{						
 			printf("start talk ok\n");			
 			sleep(6);
 			if(H264_DVR_StopVoiceCom(lHandle))
 			{
 				printf("stop talk ok\n");
 			}
 			else
 			{
 				printf("stop wrong!");
 			}			
 		}
#endif
		//ÊµÊ±ŒàÊÓ


#ifdef RealPlay
		H264_DVR_CLIENTINFO playstru;

		playstru.nChannel = 0;
		playstru.nStream = 0;
		playstru.nMode = 0;
		long m_iPlayhandle = H264_DVR_RealPlay(g_LoginID, &playstru);	

		/*
		playstru.nChannel = 1;
		long m_iPlayhandle1 = H264_DVR_RealPlay(g_LoginID, &playstru);	
		playstru.nChannel = 2;
		long m_iPlayhandle2 = H264_DVR_RealPlay(g_LoginID, &playstru);	
		playstru.nChannel = 3;
		long m_iPlayhandle3 = H264_DVR_RealPlay(g_LoginID, &playstru);	
		playstru.nChannel = 4;
		long m_iPlayhandle4 = H264_DVR_RealPlay(g_LoginID, &playstru);	
		playstru.nChannel = 5;
		long m_iPlayhandle5 = H264_DVR_RealPlay(g_LoginID, &playstru);	
		playstru.nChannel = 6;
		long m_iPlayhandle6 = H264_DVR_RealPlay(g_LoginID, &playstru);	
		playstru.nChannel = 7;
		long m_iPlayhandle7 = H264_DVR_RealPlay(g_LoginID, &playstru);	
		*/
		
		
		
		
		
		
		if(m_iPlayhandle == 0 )
		{
			printf("start RealPlay wrong!\n");
		}
		else
		{
			g_pFile = fopen("/mnt/TestRealPlay.h264", "wb+");
			int ret = H264_DVR_SetRealDataCallBack_V2(m_iPlayhandle, RealDataCallBack_V2, 0);
			/*
			ret = H264_DVR_SetRealDataCallBack_V2(m_iPlayhandle1, RealDataCallBack_V2, 1);
			ret = H264_DVR_SetRealDataCallBack_V2(m_iPlayhandle2, RealDataCallBack_V2, 2);
			ret = H264_DVR_SetRealDataCallBack_V2(m_iPlayhandle3, RealDataCallBack_V2, 3);
			ret = H264_DVR_SetRealDataCallBack_V2(m_iPlayhandle4, RealDataCallBack_V2, 4);
			ret = H264_DVR_SetRealDataCallBack_V2(m_iPlayhandle5, RealDataCallBack_V2, 5);
			ret = H264_DVR_SetRealDataCallBack_V2(m_iPlayhandle6, RealDataCallBack_V2, 6);
			ret = H264_DVR_SetRealDataCallBack_V2(m_iPlayhandle7, RealDataCallBack_V2, 7);
			*/
			printf("start RealPlay ok! %d\n",ret);
			sleep(1000);
			if(H264_DVR_StopRealPlay(m_iPlayhandle))
			{
				printf("stop realPlay ok\n");
			}
			else
			{
				printf("stop realPlay Wrong\n");
			}

		}
#endif
		//ÍøÂçÅäÖÃ
		//

 		DWORD dwRetLen = 0;

 		int nWaitTime = 10000;

		SDK_NetRTMPConfig NetRTMPConfig = {0};
		NetRTMPConfig.bEnable = 1;
		strcpy(&NetRTMPConfig.Directory[0],"rtmp://192.168.31.105:1935/live"); 
		
		SDK_NetDecorderConfigAll_V3 NetDecorder;

		BOOL bSuccess = H264_DVR_GetDevConfig(g_LoginID,E_SDK_CONFIG_NET_DECODER_V3,-1,
 			(char *)&NetDecorder,sizeof(SDK_NetDecorderConfigAll_V3),&dwRetLen,nWaitTime);

		cout << "bSuccess GetDevConfig" <<bSuccess <<endl;

		int ret = H264_DVR_GetLastError();	

		cout<<"H264_DVR_GetLastError"<<ret<<endl;



		 bSuccess = H264_DVR_SetDevConfig(g_LoginID,E_SDK_CFG_NET_RTMP,0,
 			(char *)&NetRTMPConfig,sizeof(SDK_NetRTMPConfig),nWaitTime);

		cout << "bSuccess" <<bSuccess <<endl;

		//cout << "onvif addr:" << NetDecorder.vNetDecoderConfig[0].Address <<endl;
		//cout << "onvif protocal:" << NetDecorder.vNetDecoderConfig[0].Protocol <<endl;
		
		cout << "onvif addr:" << NetDecorder.DigitChnConf[1].NetDecorderConf[0].Address <<endl;
		cout << "onvif password:" << NetDecorder.DigitChnConf[1].NetDecorderConf[0].PassWord <<endl;
		cout << "onvif Channel:" << NetDecorder.DigitChnConf[1].NetDecorderConf[0].Channel <<endl;
		//
#ifdef Config
 		DWORD dwRetLen = 0;
 		int nWaitTime = 10000;
 		SDK_CONFIG_NET_COMMON NetWorkCfg;
 		BOOL bReboot = FALSE;
 		BOOL bSuccess = H264_DVR_GetDevConfig(g_LoginID,E_SDK_CONFIG_SYSNET,-1,
 			(char *)&NetWorkCfg,sizeof(SDK_CONFIG_NET_COMMON),&dwRetLen,nWaitTime);
 		if (bSuccess && dwRetLen == sizeof (SDK_CONFIG_NET_COMMON))
 		{
 			printf("TCPPort:%d\n",NetWorkCfg.SSLPort);			
 			NetWorkCfg.SSLPort=34567;
 			bSuccess = H264_DVR_SetDevConfig(g_LoginID,E_SDK_CONFIG_SYSNET,0,
 				(char *)&NetWorkCfg,sizeof(SDK_CONFIG_NET_COMMON),nWaitTime);
 			if (bSuccess)
 			{
 				printf("setconfig ok\n");
 			}
 			else
 			{
 				printf("setconfig wrong\n");
 			}
 
 		}
 		else
 		{
 			int len=sizeof (SDK_CONFIG_NET_COMMON);
 			printf("GetConfig Wrong:%d,RetLen:%ld  !=  %d\n",bSuccess,dwRetLen,len);
 		}
	//ÆÕÍšÅäÖÃ
 		dwRetLen = 0;
 		nWaitTime = 10000;
 		SDK_CONFIG_NORMAL NormalConfig = {0};
 		bSuccess = H264_DVR_GetDevConfig(g_LoginID, E_SDK_CONFIG_SYSNORMAL ,0, (char *)&NormalConfig ,sizeof(SDK_CONFIG_NORMAL), &dwRetLen,nWaitTime);
 		if ( bSuccess && dwRetLen == sizeof(SDK_CONFIG_NORMAL))
 		{
 			printf("############language: %d#############\n",NormalConfig.iLanguage);
 			NormalConfig.iLanguage=2;
 			BOOL bSuccess = H264_DVR_SetDevConfig(g_LoginID,E_SDK_CONFIG_SYSNORMAL,0,(char *)&NormalConfig,sizeof(SDK_CONFIG_NORMAL),nWaitTime);
 			if ( bSuccess == H264_DVR_OPT_REBOOT )
 			{
 				printf("####need reboot####\n");
				//H264_DVR_ControlDVR(g_LoginID,0);
 			}
 			else if(bSuccess>0)
 			{
 				printf("#####setconfig ok####\n");
 			}
 			else
 			{
 				printf("#####setconfig wrong####\n");
 			}
 		}

#endif


 	}
 	else
 	{
 		printf("**************login wrong************\n");	
 	}

#ifdef DAS
	cout<<"H264_DVR_StopActiveRigister1"<<endl;
	H264_DVR_StopActiveRigister();
	cout<<"H264_DVR_StopActiveRigister"<<endl;
#endif

	if(g_LoginID>0)
	{
		H264_DVR_Logout(g_LoginID);
		printf("Logout success!!!\n");
	}

	ret = H264_DVR_Cleanup();

	cout<<"H264_DVR_Cleanup"<<ret<<endl;
	ret = H264_DVR_GetLastError();	
	cout<<"H264_DVR_GetLastError"<<ret<<endl;

	if(g_pFile)
	{
		fclose(g_pFile);
	}
	printf("**************OVER************\n");
	return 0;
}
