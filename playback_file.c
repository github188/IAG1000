#include <signal.h>
#include <devinfo.h>
#include <commonlib.h>
#include <gtthread.h>
#include "diskinfo.h"
#include "mpdisk.h"
#include "hdutil.h"
#include "playback.h"
#include "fileindex.h"
#include "playback_file.h"
#include "gtsocket.h"
#include "gtsf.h"

extern int multichannel;

int query_index_in_partition(char *devname, char* mountpath, void *fn_arg)
{

    query_index_struct * query;
    
    if((mountpath == NULL)||(fn_arg == NULL))
    {
        return -EINVAL;
    }
        
    query = (query_index_struct *) fn_arg;
 
    if(query->index_fp == NULL)
    {
        return -EINVAL;
    }
    return fileindex_query_index(mountpath,query);
}

int query_record_timesection(char *tsname, int ch, time_t start, time_t stop)
{
    int result;//��ѯ���
    FILE *fp,*fwrite;
    struct stat statbuf;
    query_index_struct queryindex;
    char path[20];
    char cmd[200];
    char tmpname[100];//�������ǰ����������
    char  filename[MAX_FILE_NAME_SIZE];
    char  timestring[200];   	
    struct file_info_struct fileinfo;
    int     starttime = 0, preendtime = 0;
    struct tm *p_starttm,*p_endtm;
    char  tmp[200];
    char indexname[200];

    
    
    sprintf(path,"%s/index",HDSAVE_PATH);
    if(access(path,F_OK)!=0)
        mkdir(path,0755);
    sprintf(tmpname,"%s/index/%d-tmp.txt",HDSAVE_PATH,(int)start);
    fp=fopen(tmpname,"w+");
    if(fp==NULL)
    {
        printf("[Func]:%s [file open err]:%s\n", __FUNCTION__,tmpname);
        return PLAYBACK_ERR_FILE_ACCESS;
    }
    
    queryindex.index_fp =   fp;
    if(multichannel)
      queryindex.ch     =   ch;
    else
      queryindex.ch       =   -1;//ch;ip1004ֻ��4���棬ȫ����
    queryindex.start    =   start;
    queryindex.stop     =   stop;
    queryindex.trig_flag =   -1; 
    result = mpdisk_process_all_partitions(query_index_in_partition, &queryindex);

    fclose(fp);
    stat(tmpname,&statbuf);
    if(statbuf.st_size == 0)
    {

       printf("[Func]:%s [file size]:%d\n", __FUNCTION__,(int)statbuf.st_size);
       return PLAYBACK_ERR_NO_FILE;
 
    }

    printf("������ʱ�ļ�:%s\n",tmpname);
    sprintf(indexname,"%s/index/%d.txt",HDSAVE_PATH,(int)start);
    sprintf(cmd,"/ip1004/record_sort %s %s",tmpname,indexname);
    system(cmd);
    sprintf(indexname,"%s/index/%d.txt",HDSAVE_PATH,(int)start);

    printf("������ʱ�ļ�:%s\n",indexname);
    printf("ɾ����ʱ�ļ�:%s\n",tmpname);
    /*ɾ����ʱ�ļ�*/
    sprintf(cmd,"rm -rf %s",tmpname);
    system(cmd);

    /*����ʱ���*/
    fp = fopen(indexname,"r");
    if(fp == NULL)    
    {
         printf("[Func]:%s [Line]:%d [buflen]:%s\n", __FUNCTION__, __LINE__, "index is NULL");
         return PLAYBACK_ERR_NO_FILE;  
    }


    sprintf(tmpname,"%s/index/%d-time.txt",HDSAVE_PATH,(int)start);
    printf("������ʱ�ļ�:%s\n",tmpname);
    fwrite = fopen(tmpname,"w+");
    if(fwrite == NULL)
    {
        printf("[Func]:%s [file open err]:%s\n", __FUNCTION__,tmpname);
        return PLAYBACK_ERR_FILE_ACCESS;
    }
    
    while(fgets(filename,MAX_FILE_NAME_SIZE,fp) != NULL)
    {
       hdutil_filename2finfo(filename,&fileinfo);
       if(starttime == 0)
       {
            starttime =  fileinfo.stime;
            preendtime = starttime;
       }

       /*���ʱ�����3��֮����ͬһʱ���*/
       if(abs(preendtime -fileinfo.stime) > 3)
       {
            /*дʱ���starttime��endtime*/
            printf("starttime:%d~endtime:%d\n",starttime,preendtime);
            p_starttm = gmtime((const time_t  *)&starttime); /*��ȡGMTʱ��*/
            sprintf(timestring,"%04d-%02d-%02d %02d:%02d:%02d~",
            p_starttm->tm_year+1900, p_starttm->tm_mon+1, p_starttm->tm_mday,
            p_starttm->tm_hour, p_starttm->tm_min,  p_starttm->tm_sec);
            p_endtm = gmtime((const time_t  *)&preendtime); 

            sprintf(tmp,"%04d-%02d-%02d %02d:%02d:%02d",
            p_endtm->tm_year+1900, p_endtm->tm_mon+1, p_endtm->tm_mday,
            p_endtm->tm_hour, p_endtm->tm_min,  p_endtm->tm_sec);
            strcat(timestring,tmp);
            fprintf(fwrite,"%s\n",timestring);
            printf("ʱ���:%s\n",timestring);
            
            starttime = fileinfo.stime;
       }
       preendtime = fileinfo.stime  + fileinfo.len;
       
    }

    printf("starttime:%d~endtime:%d\n",starttime,preendtime);
    p_starttm = gmtime((const time_t  *)&starttime); /*��ȡGMTʱ��*/
    sprintf(timestring,"%04d-%02d-%02d %02d:%02d:%02d~",
    p_starttm->tm_year+1900, p_starttm->tm_mon+1, p_starttm->tm_mday,
    p_starttm->tm_hour, p_starttm->tm_min,  p_starttm->tm_sec);
    p_endtm = gmtime((const time_t  *)&preendtime); 

    sprintf(tmp,"%04d-%02d-%02d %02d:%02d:%02d",
    p_endtm->tm_year+1900, p_endtm->tm_mon+1, p_endtm->tm_mday,
    p_endtm->tm_hour, p_endtm->tm_min,  p_endtm->tm_sec);
    strcat(timestring,tmp);
    fprintf(fwrite,"%s\n",timestring);
    printf("ʱ���:%s\n",timestring);
    fclose(fp);
    fclose(fwrite);
    

    /*ɾ����ʱ�ļ�*/
    sprintf(cmd,"rm -rf %s",indexname);
    system(cmd);
    sprintf(tsname,"/index/%d-time.txt",(int)start);
    printf("ɾ����ʱ�ļ�:%s\n",indexname);
    printf("�����ļ�%s\n",tsname);
    
    return PLAYBACK_SUCCESS;

}


//��Ϊ�������ļ��в�,
int query_record_index(char *indexname, int queryid, int ch, time_t start, time_t stop)
{
    int result;//��ѯ���
    FILE *fp;
    struct stat statbuf;
    query_index_struct queryindex;
    char path[20];
    char cmd[200];
    char tmpname[100];//�������ǰ����������
    
    
    sprintf(path,"%s/index",HDSAVE_PATH);
    if(access(path,F_OK)!=0)
        mkdir(path,0755);
    sprintf(tmpname,"%s/index/%d-%d-tmp.txt",HDSAVE_PATH,(int)start,queryid);
    fp=fopen(tmpname,"w+");
    if(fp==NULL)
    {
        printf("[Func]:%s [file open err]:%s\n", __FUNCTION__,tmpname);
        return PLAYBACK_ERR_FILE_ACCESS;
    }
    
    queryindex.index_fp =   fp;
    if(multichannel)
      queryindex.ch       =   ch;//ip1004ֻ��4���棬ȫ����
    else
      queryindex.ch       = -1;
    queryindex.start    =   start;
    queryindex.stop     =   stop;
    queryindex.trig_flag =   -1; 
    result = mpdisk_process_all_partitions(query_index_in_partition, &queryindex);

    sleep(3);

    fclose(fp);
    stat(tmpname,&statbuf);
    if(statbuf.st_size == 0)
    {

       printf("[Func]:%s [file size]:%d\n", __FUNCTION__,(int)statbuf.st_size);
       return PLAYBACK_ERR_NO_FILE;
 
    }
 
    sprintf(indexname,"%s/index/%d-%d.txt",HDSAVE_PATH,(int)start,queryid);
    sprintf(cmd,"/ip1004/record_sort %s %s",tmpname,indexname);
    system(cmd);
    
    //sprintf(cmd,"rm -rf %s",tmpname);
    //system(cmd);    
    sprintf(indexname,"%s/index/%d-%d.txt",HDSAVE_PATH,(int)start,queryid);
    return PLAYBACK_SUCCESS;

}


int playback_openfile(struct hd_playback_struct *phdplayback)
{

    int     result;
    FILE  *fp = NULL;
    char  firstfile[MAX_FILE_NAME_SIZE];
    char  lastfile[MAX_FILE_NAME_SIZE];
    char  openfile[MAX_FILE_NAME_SIZE];   	
    struct file_info_struct firstinfo;
    int     passtime = 0;
    int     lastf;                                      /*����֡������ֻ�в��ŵ����һ���ļ�ʱʹ��*/
    avi_t * avifile =NULL;

    memset(firstfile, 0, sizeof(firstfile));
    memset(lastfile, 0, sizeof(lastfile));
    
    result = 
    query_record_index(phdplayback->Indexfilename,phdplayback->playbackindex, phdplayback->channel,
                    phdplayback->start, phdplayback->stop);

    if(result != PLAYBACK_SUCCESS)
    {
        return result;
    }

    phdplayback->frames = 0;
    phdplayback->fileindex = 0;
    phdplayback->recordfiletotal = 0;


    /*����ʱ�������ļ������ҵ�һ��¼���ļ����򿪣�
       ��λ��ʼ���ŵ�λ�ã����ֻ��һ��¼���ļ�����Ҫ��λ
       ���ŵĽ���λ��*/
    fp = fopen(phdplayback->Indexfilename,"r");
    if(fp == NULL)    
    {
         printf("[Func]:%s [Line]:%d [buflen]:%s\n", __FUNCTION__, __LINE__, "index is NULL");
         return PLAYBACK_ERR_NO_FILE;  
    }


    if(fgets(firstfile,MAX_FILE_NAME_SIZE,fp) == NULL)
    {
       printf("[Func]:%s [Line]:%d [buflen]:%s\n", __FUNCTION__, __LINE__, "index is NULL");
       return PLAYBACK_ERR_NO_FILE;
    }
    
    firstfile[(strlen(firstfile)-1)] = '\0';
    phdplayback->recordfiletotal++;
    while(fgets(lastfile,MAX_FILE_NAME_SIZE,fp) != NULL)
    {
        phdplayback->recordfiletotal++;
    }
    lastfile[(strlen(lastfile)-1)] = '\0';
	

    fclose(fp);
    printf("first record name:%s, last record name:%s. total:%d\n", firstfile,lastfile,phdplayback->recordfiletotal);

    /*��λ��ʼ֡*/
    hdutil_filename2finfo(firstfile,&firstinfo);
    if(phdplayback->start > firstinfo.stime)
    {
        passtime = phdplayback->start - firstinfo.stime;
        printf("[Func]:%s [Line]:%d [buflen]:%s,���%d��\n", __FUNCTION__, __LINE__, firstfile,passtime);
        //return PLAYBACK_ERR_FILE_ACCESS;
    }
    printf("first record name:%s. last:%s, total:%d\n", firstfile,lastfile,phdplayback->recordfiletotal);
    sprintf(openfile,"%s%s","/hqdata",firstfile);
    avifile = AVI_open_input_file(openfile, 1);
    if(avifile == NULL)
    {
        printf("[Func]:%s [Line]:%d [buflen]:%s%s\n", __FUNCTION__, __LINE__, openfile,"open error");
        return PLAYBACK_ERR_FILE_ACCESS;
    }

    /*��ȡ��ʼ����Ҫ�����֡*/
    if(passtime != 0 )
    {
        if(avifile->fps == 0)
        {
            avifile->video_pos = passtime*PLAYBACK_FRAMERATE_25;
        }
        else
        {
            avifile->video_pos = passtime*avifile->fps;
        }
     }

    if(avifile->video_pos < 0 || avifile->video_pos >= avifile->video_frames)
    {
    
        printf("[Func]:%s [frame pos]:%d [frames]:%d\n", __FUNCTION__,(int)(avifile->video_pos),(int)(avifile->video_frames));
        AVI_close(avifile);
        return PLAYBACK_ERR_FILE_ACCESS;
    }

    phdplayback->aviinfo = avifile;
    
    printf("��һ���ļ�%s,���%d�룬����%d֡���ܹ�֡��:%d\n",
        firstfile,passtime,(int)(avifile->video_pos),(int)(avifile->video_frames));


    /*ֻ��һ���ļ�,��һ���ļ��������һ���ļ�*/
    if(phdplayback->recordfiletotal == 1)
    {
        /*��λ����֡*/
        //hdutil_filename2finfo(firstfile,&firstinfo);
        /*phdplayback->stop�Ľ���ʱ������һ��*/
        passtime = phdplayback->stop+1 - firstinfo.stime;
        if(phdplayback->stop  < firstinfo.stime)
        {
            printf("[Func]:%s [Line]:%d [buflen]:%s%d\n", __FUNCTION__, __LINE__, firstfile,passtime);
            return PLAYBACK_ERR_FILE_ACCESS;
        }
        
        /*��ȡҪ�����֡*/
        if(passtime != 0 )
        {
            if(avifile->fps == 0)
            {
                lastf = passtime*PLAYBACK_FRAMERATE_25;
            }
            else
            {
                lastf = passtime*avifile->fps;
            }
            /*��Ϊǰ������Ϊ����(end+1)��������Խ��Ŀ���*/
            if(lastf > avifile->video_frames)
            {
                lastf = avifile->video_frames;
            }
            
            phdplayback->lastframe = lastf;
        }
        
    
    }

    phdplayback->fileindex++;

/*��λ�ļ�ָ��*/


    if(!avifile->video_index)
    { 
        printf("[Func]:%s [Line]:%d [buflen]:%s\n", __FUNCTION__, __LINE__, "index is NULL");
        return PLAYBACK_ERR_FILE_ACCESS;
    }

    if(avifile->video_pos < 0 || avifile->video_pos >= avifile->video_frames)
    {
    
        printf("[Func]:%s [frame pos]:%d [frames]:%d\n", __FUNCTION__,(int)(avifile->video_pos),(int)(avifile->video_frames));
        return PLAYBACK_ERR_FILE_ACCESS;
    }
/*    
    n = avifile->video_index[avifile->video_pos].len;

    if(bufflen < n)
    {
        printf("[Func]:%s [frame size]:%d [buflen]:%d\n", __FUNCTION__,(int)n, bufflen);
        return PLAYBACK_ERR_BUF_SIZE;
    }

    *keyframe = (avifile->video_index[avifile->video_pos].key==0x10) ? 1:0;
*/

    lseek(avifile->fdes, avifile->video_index[avifile->video_pos].pos-8, SEEK_SET);

/*  ��Щ�����ڶ�����֡�ĺ�������    
    if (avi_read(AVI->fdes,vidbuf,n) != n)
    {
        printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "AVI READ");
        return PLAYBACK_ERR_FILE_ACCESS;
    }

    AVI->video_pos++;
*/    

/*��λ�ļ�ָ��*/
    
    return result;


}

           
    
  /**   
      *   ��������ʱ��ļ�����õ�ʱ���   
      *   @param   struct   timeval*   resule   ���ؼ��������ʱ��   
      *   @param   struct   timeval*   x             ��Ҫ�����ǰһ��ʱ��   
      *   @param   struct   timeval*   y             ��Ҫ����ĺ�һ��ʱ��   
      *   return   -1   failure   ,0   success   
  **/   
  int   timeval_subtract(struct   timeval*   result,   struct   timeval*   x,   struct   timeval*   y)   
  {   

        if(x->tv_sec > y->tv_sec)   
            return   -1;   
    
        if((x->tv_sec == y->tv_sec) && (x->tv_usec > y->tv_usec))   
            return   -1;   
    
        result->tv_sec = (y->tv_sec-x->tv_sec);   
        result->tv_usec = (y->tv_usec-x->tv_usec);   
    
        if(result->tv_usec < 0)   
        {   
            result->tv_sec--;   
            result->tv_usec += 1000000;   
        }   
    
        return   0;   
  }   
    
void milliseconds_sleep(unsigned long mSec){
    struct timeval tv;
    tv.tv_sec=mSec/1000;
    tv.tv_usec=(mSec%1000)*1000;
    int err;
    do{
       err=select(0,NULL,NULL,NULL,&tv);
    }while(err<0 && errno==EINTR);
}

extern struct hd_playback_struct g_playback[PLAYBACK_NUM];

static unsigned short  playback_get_source()
{

    dictionary    *ini=NULL;
    FILE            *fp=NULL;
    int               port,i;
    char              secbuf[20]={0};

    ini=iniparser_load_lockfile(IPMAIN_PARA_FILE,1,&fp);
    if(ini==NULL)
    {
          printf("playback_get_port() cannot parse ini file file [%s]", IPMAIN_PARA_FILE);
          gtlogerr("playback_get_port() cannot parse ini file file [%s]", IPMAIN_PARA_FILE);
          return -1 ;
    }


    //��ǰ¼����
	for(i=0;i<PLAYBACK_NUM;i++)
	{
		sprintf(secbuf,"audio%d:interval",i);
		g_playback[i].audio_source=iniparser_getint(ini,secbuf,1);
	}
  
    if(fp!=NULL)
    {
        unlock_file(fileno(fp));
        fsync(fileno(fp));
        fclose(fp);
    }   
    iniparser_freedict(ini);
    
    return 0;
    
}


void playback_process(void * ch)
{

    int  result;
    char *pdata;
    int  isvedio;
    int  getbufflen;
    int  isIframe;
    int  speed = PLAYBACK_NSPEED;
    gtsf_stream_fmt  *pStream;
    stream_video_format_t  *pV_fmt;
    stream_audio_format_t  *pA_fmt;
    stream_format_t  *pmedia_format;
    struct   timeval   start,stop,diff;
    int  frame_interval =  40000;                           /*���۵�֡���*/
    int channel=(int)ch;

    printf("playback thread %d  start\n", channel);

	if(playback_get_source()<0)
		return;


    playback_struct *phdplayback = getplayback(channel);
    if(phdplayback == NULL)
    {
        printf("phdplayback:%d  is NULL", channel);
        return ;
    }

    pStream = (gtsf_stream_fmt *)malloc(PLAYBACK_BUFF_LEN);
    if(pStream == NULL)
    {
        printf("memory error, malloc:%d  is NULL", channel);
        return ;
    }

    gettimeofday(&start,0); 
    
    while(1)
    {
    
        /*�ȼ����û�в���,����У��ȴ���
          ��Щ����*/

         
        /*ֹͣ����ط�*/
        if(phdplayback->oper == PLAYBACK_CTRL_CLOSE)
        {
            printf("phdplayback %d stop\n", channel);
            playbackClose(channel);
        }

        /*��ͣʱ�����벻�޸�*/
        if(phdplayback->oper == PLAYBACK_CTRL_PAUSE)
        {
            printf("phdplayback %d pause\n", channel);
            usleep(40000);
            continue;
        }

        if(phdplayback->oper == PLAYBACK_CTRL_SEEK)
        {
            char indexfile[256];
            printf("phdplayback %d seek \n", channel);

            sprintf(indexfile,"rm -rf %s",phdplayback->Indexfilename);
            system(indexfile);

            if(phdplayback->aviinfo != NULL)
            {
                AVI_close(phdplayback->aviinfo);
                phdplayback->aviinfo = NULL;
            }
            result = playback_openfile(phdplayback);
            if(result != PLAYBACK_SUCCESS)
            {
                printf(" playback_openfile error:%x,read file:%d  is over \n", result,channel);
                playbackClose(channel);
            }
            phdplayback->oper = PLAYBACK_CTRL_IDLE;
            continue;
        }

        if(phdplayback->state != PLAYBACK_STAT_OK)
        {
            usleep(40000);
            continue;
        }

        
        isIframe = 0;
        pdata =  (char*)pStream + GTSF_HEAD_SIZE;     
        result = playbackreadfileframe(phdplayback, pdata, PLAYBACK_BUFF_LEN, &isvedio, 
        &getbufflen,&isIframe);

        if(result == 0)
        {
            printf(" playback,read file:%d  is over \n", channel);
            playbackClose(channel);
            continue;
        }
        else if(result < 0)
        {
            printf(" playback,read file:%d error \n", channel);
            playbackClose(channel);
            continue;
        }


        /*����֡�����ʱ�䣬ע����ֻ��I֡ʱ,����I֡�����ݻ��ӵ�*/
        if(speed != phdplayback->speed)
        {
            printf("channel :%d speed change, %d-->%d",channel, speed, phdplayback->speed);
            speed = phdplayback->speed;
            
        }         
        if(speed  == PLAYBACK_QSPEED)
        {
            frame_interval = 160000; 
        }
        else if(speed  == PLAYBACK_HSPEED)
        {
            frame_interval = 80000; 
        }
        else if(speed  == PLAYBACK_2SPEED)
        {
            frame_interval = 20000; 
        }
        else if(speed  == PLAYBACK_4SPEED)
        {
            frame_interval = 10000; 
        }
        else if(speed  == PLAYBACK_ISPEED)
        {
            frame_interval = 40000; 
        }
        else
        {
            /*NSPEED*/
            frame_interval = 40000;
        }
        

        /*���ݴ��*/
        pStream->mark = GTSF_MARK;
        pStream->encrypt_type = 0;
        pStream->len = getbufflen;
        if(isvedio == PLAYBACK_TRUE)
        {
            pStream->type = MEDIA_VIDEO;
            pmedia_format = &pStream->media_format;
            pV_fmt = (stream_video_format_t  *)&pmedia_format->v_fmt;
            pV_fmt->format = VIDEO_H264;
            if(isIframe == PLAYBACK_TRUE)
                pV_fmt->type = FRAMETYPE_I;
            else
                pV_fmt->type = FRAMETYPE_P;

            printf("pv_fmt aviinfo width is %d,height is %d\n",phdplayback->aviinfo->width,phdplayback->aviinfo->height);

            if(( phdplayback->aviinfo->width == 720)&&(phdplayback->aviinfo->height== 576))
            {
                pV_fmt->ratio = RATIO_D1_PAL;
            }
            else if(( phdplayback->aviinfo->width == 704)&&(phdplayback->aviinfo->height== 576))
            {
                pV_fmt->ratio = RATIO_D1_NTSC;
            }
            else if(( phdplayback->aviinfo->width == 352)&&(phdplayback->aviinfo->height== 288))
            {
                pV_fmt->ratio = RATIO_CIF_PAL;
            }
            else if(( phdplayback->aviinfo->width == 320)&&(phdplayback->aviinfo->height== 240))
            {
                pV_fmt->ratio = RATIO_CIF_NTSC;
            }
            else if(( phdplayback->aviinfo->width == 1280)&&(phdplayback->aviinfo->height== 720))
            {
                pV_fmt->ratio = RATIO_720P;
            }
            else if(( phdplayback->aviinfo->width == 1920)&&(phdplayback->aviinfo->height== 1080))
            {
                pV_fmt->ratio = RATIO_1080P;
            }            
        }
        else
        {
/*
	waveform.wFormatTag		=	WAVE_FORMAT_MULAW;
	waveform.nChannels		=	1;
	waveform.nSamplesPerSec	=	8000;
	waveform.nAvgBytesPerSec=	8000;
 	waveform.nBlockAlign	=	1;
	waveform.wBitsPerSample	=	8;
	waveform.cbSize			=	0;	

*/
			pStream->type = MEDIA_AUDIO;
            pmedia_format = &pStream->media_format;
            pA_fmt = (stream_audio_format_t  *)&pmedia_format->a_fmt;

			if(phdplayback->audio_source==1)
			{
				//internel
				pA_fmt->a_channel = 1;
				pA_fmt->a_wformat = 7;
				pA_fmt->a_sampling = 8000;
				pA_fmt->a_bits = 16;
				pA_fmt->a_bitrate = 64;
			}
			else
			{
				//outsource
				pA_fmt->a_channel = 2;
				pA_fmt ->a_wformat = 255;
				pA_fmt->a_sampling = 16000;
				pA_fmt->a_bits = 16;
				pA_fmt->a_bitrate = 0;

			}
            
        }
#if 0        
    {
        int i;
        char *p = (char *)pStream;
        for(i = 0;i<32;i++)
       {
                printf("%x ",p[i]);
                if(i%32 ==0)
                {
                    printf("\n");
                }
       }

    }
#endif
        if((speed  == PLAYBACK_ISPEED) &&(isIframe == 0))
        {
            printf("discard packet,channel:%d  type %d\n",channel,pStream->type);
            ;//do noting
        }
        else
        {
            result = fd_write_buf(phdplayback->socket,(char*)pStream,getbufflen+GTSF_HEAD_SIZE);
            //printf("channel:%d tcp type:%d,isIframe:%d send %d, speed:%d  result %d\n",channel, pStream->type,isIframe,getbufflen,speed,result);  
            if(result <= 0)
            {
                printf(" playback, net send:%d  is over \n", channel);
                playbackClose(channel);
                continue;
            }
            phdplayback->packetsum++;

        }
        
        /*֡�ʿ��ƣ���ʼ*/

        /*��Ƶ������*/
        if(isvedio != PLAYBACK_TRUE)
            continue;

       gettimeofday(&stop,0);
       timeval_subtract(&diff,&start,&stop);
       //printf("�ܼ���ʱ:%d��%d ΢��\n",diff.tv_sec, diff.tv_usec); 

        /*����1���ˣ�ֱ����һ��*/
        if(diff.tv_sec > 0)
        {
           gettimeofday(&start,0); 
           continue;
        }

        //printf("speed:%d frame_interval:%d, use:%d\n",speed,frame_interval,diff.tv_usec); 
        if(frame_interval > diff.tv_usec)
        {
             if(frame_interval - diff.tv_usec >9000)
                usleep(frame_interval - diff.tv_usec -9000);
        }
        gettimeofday(&start,0); 
         /*֡�ʿ��ƣ�����*/
        
    }
    
   free(pStream);
   playbackClose(channel);

       
}
void readfile_thread(int playbackIndex)
{
    int thread_node_t = -1;
    pthread_t thread_test;

    thread_node_t = GT_CreateThread(&thread_test, (void *)playback_process, (void *)playbackIndex);                  

    if(thread_node_t == 0)
    {
        printf("������%d·�ط��̳߳ɹ�\n", playbackIndex);
    }
    else 
    {
        printf("������%d·�߳�ʧ��\n", playbackIndex);
    }
}
static size_t avi_read(int fd, char *buf, size_t len)
{
   size_t n = 0;
   size_t r = 0;

   while (r < len) {
      n = read (fd, buf + r, len - r);

      if (n <= 0)
          break;
      r += n;
   }
   return r;
}

/*
static long playback_read_frame(avi_t *AVI, char *vidbuf, int bufflen,  int *keyframe)
{

    long n;

    if(AVI->mode==AVI_MODE_WRITE) 
    {
        return PLAYBACK_ERR_FILE_ACCESS; 
    }
    if(!AVI->video_index)
    { 
        printf("[Func]:%s [Line]:%d [buflen]:%s\n", __FUNCTION__, __LINE__, "index is NULL");
        return PLAYBACK_ERR_FILE_ACCESS;
    }

    if(AVI->video_pos < 0 || AVI->video_pos >= AVI->video_frames)
    {
    
        printf("[Func]:%s [frame pos]:%d [frames]:%d\n", __FUNCTION__,(int)(AVI->video_pos),(int)(AVI->video_frames));
        return PLAYBACK_ERR_FILE_ACCESS;
    }
    n = AVI->video_index[AVI->video_pos].len;

    if(bufflen < n)
    {
        printf("[Func]:%s [frame size]:%d [buflen]:%d\n", __FUNCTION__,(int)n, bufflen);
        return PLAYBACK_ERR_BUF_SIZE;
    }

    *keyframe = (AVI->video_index[AVI->video_pos].key==0x10) ? 1:0;

    lseek(AVI->fdes, AVI->video_index[AVI->video_pos].pos, SEEK_SET);
    if (avi_read(AVI->fdes,vidbuf,n) != n)
    {
        printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "AVI READ");
        return PLAYBACK_ERR_FILE_ACCESS;
    }

    AVI->video_pos++;

    return n;

}
*/


int playbackreadfileframe(playback_struct *phdplayback, void *buf, int buf_len, int 
*isvideo, int *getbuflen, int *flag)
{

    FILE *fp = NULL;
    char  filename[MAX_FILE_NAME_SIZE];
    char  openfile[MAX_FILE_NAME_SIZE];
    int     result;
    int     passtime;
    int     getfileindex = 0;
    struct file_info_struct fileinfo;
  
    

    int     lastf;                                      /*����֡������ֻ�в��ŵ����һ���ļ�ʱʹ��*/
    avi_t * avifile =NULL;

    if(phdplayback == NULL)
    {
        return PLAYBACK_ERR_PARAM;
    }

    /*�ȿ��ǲ������һ���ļ�*/
    if(phdplayback->fileindex == phdplayback->recordfiletotal)
    {
        if(phdplayback->aviinfo->video_pos >= phdplayback->lastframe)
        {
            /*���Ž���*/
            printf("���Ž���:������%d���ļ�\n",phdplayback->recordfiletotal);
            AVI_close(phdplayback->aviinfo);
            phdplayback->aviinfo = NULL;
            return 0;
        }
    }
    else
    {
        if(phdplayback->aviinfo->video_pos >= (phdplayback->aviinfo->video_frames -1))
        {
            /*����һ���ļ�����*/
            //printf("playback the %d record name:%s\n", getfileindex,filename);    
            AVI_close(phdplayback->aviinfo);
            phdplayback->aviinfo = NULL;

            /*��ȡ��һ���ļ���*/
            fp = fopen(phdplayback->Indexfilename,"r");
            if(fp != NULL)
            {

                while(fgets(filename,MAX_FILE_NAME_SIZE,fp)!=NULL)
                {
                    getfileindex++;
                    if(getfileindex == (phdplayback->fileindex + 1))
                    {
                        break;
                    }
                }
                filename[(strlen(filename)-1)] = '\0';
            }
            fclose(fp);
            printf("open the %d record name:%s\n", getfileindex,filename);            
            /*����һ���ļ�*/
            hdutil_filename2finfo(filename,&fileinfo);
            sprintf(openfile,"%s%s","/hqdata",filename);
            avifile = AVI_open_input_file(openfile, 1);
            if(avifile == NULL)
            {
                printf("[Func]:%s [Line]:%d [buflen]:%s%s\n", __FUNCTION__, __LINE__, filename,"open error");
                return PLAYBACK_ERR_FILE_ACCESS;
            }
            phdplayback->aviinfo = avifile;
            /*���һ���ļ�*/
            if(phdplayback->recordfiletotal == (phdplayback->fileindex + 1))
            {
                /*��λ����֡*/
                /*phdplayback->stop�Ľ���ʱ������һ��*/
                
                passtime = phdplayback->stop+1 - fileinfo.stime;
                if(phdplayback->stop  < fileinfo.stime)
                {
                    printf("[Func]:%s [Line]:%d [buflen]:%s%d\n", __FUNCTION__, __LINE__, filename,passtime);
                    return PLAYBACK_ERR_FILE_ACCESS;
                }
                
                /*��ȡҪ�����֡*/
                if(passtime != 0)
                {
                    if(avifile->fps == 0)
                    {
                        lastf = passtime*PLAYBACK_FRAMERATE_25;
                    }
                    else
                    {
                        lastf = passtime*avifile->fps;
                    }
                    /*��Ϊǰ������Ϊ����(end+1)��������Խ��Ŀ���*/
                    if(lastf > avifile->video_frames)
                    {
                        lastf = avifile->video_frames;
                    }
                    
                    phdplayback->lastframe = lastf;
                }
                printf("the last file, name:%s. ����:%d��,֡:%d total:%d\n", 
                filename,passtime,lastf,phdplayback->recordfiletotal);

            
            }
            phdplayback->fileindex++;
            phdplayback->aviinfo->video_pos = 0;

            if(!phdplayback->aviinfo->video_index)
            { 
                printf("[Func]:%s [Line]:%d [buflen]:%s\n", __FUNCTION__, __LINE__, "index is NULL");
                return PLAYBACK_ERR_FILE_ACCESS;
            }

            lseek(avifile->fdes, avifile->video_index[avifile->video_pos].pos-8, SEEK_SET);
  
                   
        }
    
    }

    if(phdplayback->aviinfo == NULL)
    {
        /*������һ��¼���ļ�*/
         printf("���Ŵ���,��%d ��¼���ļ�û�д�\n",phdplayback->fileindex);
         return PLAYBACK_ERR_FILE_ACCESS;
    }


//    result = playback_read_frame(phdplayback->aviinfo, buf, buf_len,  flag);
  
    result = AVI_read_data(phdplayback->aviinfo,  buf, buf_len,
                               buf, buf_len,
                              (long*)getbuflen);
/*
 * Return codes:
 *
 *    1 = video data read
 *    2 = audio data read
 *    0 = reached EOF
 *   -1 = video buffer too small
 *   -2 = audio buffer too small
 */     
    if(result == 1)
    {
        *isvideo = 1;
        if(!phdplayback->aviinfo->video_index)         
        { 
            return PLAYBACK_ERR_FILE_NOINDEX;
        }

        if(phdplayback->aviinfo->video_pos < 0 || phdplayback->aviinfo->video_pos > phdplayback->aviinfo->video_frames) 
        {
            return PLAYBACK_ERR_FILE_NOINDEX;
        }


        *flag = 
        (phdplayback->aviinfo->video_index[phdplayback->aviinfo->video_pos-1].key==0x10) ? 1:0;
        //lc inavailted
		//printf("read frame,type:%d len %d is video:%d , video frame:%d fileindex:%d total:%d\n", *flag,*getbuflen,result,phdplayback->aviinfo->video_pos,phdplayback->fileindex,phdplayback->recordfiletotal);
        if((phdplayback->aviinfo->video_pos %10 == 0)&&(*isvideo == 1))
        {
            //printf("\nread frame,len %d is video:%d , video frame:%d fileindex:%d total:%d\n", *getbuflen,result,phdplayback->aviinfo->video_pos,phdplayback->fileindex,phdplayback->recordfiletotal);
        }        
    }
    else if(result == 2)
    {
        *isvideo = 0;
    }
    else if(result == 0)
    {
        printf("file read over \n");
        return result;
    }
    else
    {
        printf("error:[Func]:%s [Line]:%d :read file errior :%d \n", __FUNCTION__, 
        __LINE__, result);
        return result;
    }

    return result;
}


int playbackclosefile(struct hd_playback_struct *phdplayback)
{
    //int ret;
    char indexfile[256];

    sprintf(indexfile,"rm -rf %s",phdplayback->Indexfilename);
    system(indexfile);

	printf("phdplayback->aviinfo is %p,socket is %d\n",phdplayback->aviinfo,phdplayback->socket);
    if(phdplayback->aviinfo != NULL)
    {
        AVI_close(phdplayback->aviinfo);
        phdplayback->aviinfo = NULL;
    }
    if(phdplayback->socket != 0)
    {
        close(phdplayback->socket);
        phdplayback->socket = 0;
    }
	//phdplayback->state=PLAYBACK_STAT_IDLE;
	
    return PLAYBACK_SUCCESS;
}


  
/*û���ã�Ϊ�˱���ͨ��*/
void set_cferr_flag(int flag)
{   


}


