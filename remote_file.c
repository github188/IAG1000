//��Զ�̽����ļ������ĺ���
#include "ipmain.h"
#include "remote_file.h"
#include <gate_cmd.h>
#include <devinfo.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <commonlib.h>
#include <gt_errlist.h>
#include "ipmain_para.h"
enum REMOTE_FILE_TYPE{
	RMT_FILE_ERR=1,		//�豸��������ʱ�д�����
	RMT_FILE_GTPARA=2,	//ip1004.ini .gz��ʽ
	RMT_FILE_ALARMPARA=3,	//alarm.ini .gz��ʽ
	RMT_FILE_CERT=6,			//֤���ļ�,.zip��ʽ,���������ļ�dev-gateway.crt dev-peer.crt dev-peer.key
	RMT_FILE_GTINFO=7,          //�豸������Ϣ�ļ�gtinfo.dat
};




//ֱ�������ط���һ���������Ӧ������ͨ��ͳһ����Ϣ�����߳�
int send_gate_ack_direct(int fd,WORD rec_cmd,WORD result,int env,int enc,int dev_no)
{
	DWORD send_buf[25];//��Ӧ��������ᳬ��100�ֽ�
	struct gt_pkt_struct *send=NULL;
	struct gt_usr_cmd_struct *cmd;
	struct sockaddr_in peeraddr;
	int addrlen=sizeof(struct sockaddr);
	int rc;
	getpeername(fd,(struct sockaddr *)&peeraddr,&addrlen);
	
	send=(struct gt_pkt_struct *)send_buf;
	cmd=(struct gt_usr_cmd_struct *)send->msg;
	cmd->cmd=DEV_CMD_ACK;
	cmd->en_ack=0;
	cmd->reserve0=0;
	cmd->reserve1=0;
	rc=virdev_get_devid(dev_no,cmd->para);	

	memcpy(&cmd->para[rc],(char *)&result,2);
	memcpy(&cmd->para[rc+2],(char *)&rec_cmd,2);
	cmd->len=rc+SIZEOF_GT_USR_CMD_STRUCT-sizeof(cmd->para) -2+4;
	rc=gt_cmd_pkt_send(fd,send,(cmd->len+2),NULL,0,env,enc);
#ifdef SHOW_WORK_INFO
	printf("%s��%s��������%04x�Ľ��(direct)%04x rc=%d",devlog(dev_no),inet_ntoa(peeraddr.sin_addr),rec_cmd,result,rc);
#endif
	gtloginfo("%s��%s��������%04x�Ľ��(direct)%04x rc=%d",devlog(dev_no),inet_ntoa(peeraddr.sin_addr),rec_cmd,result,rc);
	return 0;	
}


/*
	�����������ļ��������Ϣ���͸�Զ�̼����
*/
static int send_rw_para_file_err(int fd,int env,int enc,int dev_no)
{
	return send_gate_ack_direct(fd,USR_RW_DEV_PARA,ERR_DVC_UPDATE_FILE,env,enc,dev_no);
}

	    
//����Զ�̵�д����
/*
	����ֵ  -1 ��������
			    -2 ���ܴ�ָ�����ļ�
			    -3 �ļ���ʽ����(���ܽ�ѹ���ļ�)
			    -4 ֤���ļ�����
			    -5 �豸�ڲ�����
*/		
static int process_remote_write(int devno, int filetype,char *filebuf,int buflen)
{
	#define RECV_PARA_TMP "/tmp/para"
	int ret;
	char tbuf[256];
	FILE *fp=NULL;
	char *filename=NULL;		//�����ļ���������·��
	char *name=NULL;		//�����ļ���(������·��)
	char tmpfile[100];			//��ʱ������ݵ��ļ�
	char savefile[100];
	char send_dir[100];		//��ŷ����ļ���Ŀ¼	
	char *crt_file=NULL;		//Զ�̷�����֤���ļ���
	char *key_file=NULL;		//Զ�̷�����key�ļ���

	sprintf(send_dir,"%s/%d/dev%d",RECV_PARA_TMP,getpid(),devno);
#if EMBEDED==1
	//ɾ����ǰ����ʱĿ¼
	sprintf(tbuf,"rm -rf %s\n",send_dir);
	system(tbuf);
#endif		
	//������ʱĿ¼
	sprintf(tbuf,"mkdir -p %s\n",send_dir);
	system(tbuf);

	sprintf(tmpfile,"%s/rmt.dat",send_dir);
	
	fp=fopen(tmpfile,"w");
	if(fp==NULL)
	{
		printf("���ܴ�����ʱ�ļ�%s\n",tmpfile);
		gtloginfo("���ܴ�����ʱ�ļ�%s\n",tmpfile);
		return -2;
	}
	fwrite(filebuf,1,buflen,fp);
	fclose(fp);	
	switch(filetype)
	{
		case RMT_FILE_GTPARA:		//ip1004.ini
		case RMT_FILE_ALARMPARA:	//alarm.ini
			//���յ�������д��һ���ļ�
			filename = IPMAIN_PARA_FILE;
			name=strrchr(filename,'/');	//�ҵ����һ�� '/'��ȷ���ļ���
			if(name==NULL)
				name=filename;
			else
				name++;
			//����
			sprintf(savefile,"%s/%s.gz",send_dir,name);
			sprintf(tbuf,"mv %s %s",tmpfile,savefile);
			ret=system(tbuf);
			
			//��ѹ���ļ�
			sprintf(tbuf,"gunzip %s",savefile);
			ret=system(tbuf);
			if(ret!=0)
			{
				gtloginfo("��ѹ���ļ�%s����\n",savefile);
				return -3;
			}
			//���ļ�������ָ��Ŀ¼��ָ���ļ�
			sprintf(tbuf,"/ip1004/iniset %s/%s %s",send_dir,name,filename);
			//sprintf(tbuf,"cp %s/%s %s -f",send_dir,name,filename);
			ret=system(tbuf);
			gtloginfo("���������ļ�%s/%s->%s ret=%d",send_dir,name,filename,ret);
			/*
			if(virdev_get_virdev_number()==2)
			{
				sprintf(tbuf,"/ip1004/ini_conv -m");
				ret=system(tbuf);
			}
			*/
			//wsy add,ת�������ļ�
			sprintf(tbuf,"/ip1004/para_conv -s");
			ret=system(tbuf);
			printf("�豸Ӳ��λ...\n");
			gtloginfo("�豸Ӳ��λ...\n");
			system("/ip1004/hwrbt 5");
		break;
		default:
			       printf("w:��֧�ֵ��ļ�����:%d!\n",filetype);
			gtloginfo("w:��֧�ֵ��ļ�����:%d!\n",filetype);
			return -1;
		break;
	}

out:

	#if EMBEDED==1
	//ɾ���ù���Ŀ¼
		sprintf(tbuf,"rm -rf %s\n",send_dir);
		system(tbuf);
	#endif	
	
	return 0;

}


//�����ش��������ļ�ʹ�õ��ļ���ʽ 
/*�����豸�����ļ� USR_RW_DEV_PARA
ʹ�ô���������д�豸�Ĳ��������ļ���Ŀǰֻ�ڹ��̰�װ�������ʹ�ã�,
�豸�յ��������Ӧ����DEV_PARA_RETURN����
   0x0108 USR_RW_DEV_PARA FTP�ļ�����
  {
  	  type(2)         ���ͣ�2:ip1004.ini 3:alarm.ini����ֵ������
	  mode(2)		����ģʽ:0���� 1��д
	  filelen(4)		�ļ�����(��дģʽ����Ч,���Ȳ��ܳ���60k)
	  file(n)			�ļ�����(����дģʽ����Ч)
}

*/
int usr_rw_para_file(int fd,struct gt_usr_cmd_struct *cmd,int env,int enc,int dev_no)
{
	struct usr_rwdevpara_struct *set;
//	char temp_file[100];
	int filetype;
	int ret=0;
	if((fd<0)||(cmd->cmd!=USR_RW_DEV_PARA))
		return -1;
	set=(struct usr_rwdevpara_struct *)cmd->para;
#ifdef SHOW_WORK_INFO
	printf("�յ���д%s USR_RW_DEV_PARA ����type=%d mode=%d \n",devlog(dev_no),set->type,set->mode);
#endif
	gtloginfo("�յ���д%s USR_RW_DEV_PARA ����type=%d mode=%d \n",devlog(dev_no),set->type,set->mode);
	filetype=set->type;
	if(set->mode==1)
	{//д
		if(process_remote_write(dev_no,filetype,set->file,set->filelen)==0)
		{//д��ɹ�
			ret=send_gate_ack_direct(fd,USR_RW_DEV_PARA,RESULT_SUCCESS,env,enc,dev_no);
		}
		else
		{//д��ʧ��
			return send_rw_para_file_err(fd,env,enc,dev_no);	
		}
	}
	else
	{//��
		ret = send_para_to_rmt(fd,filetype,env,enc,0,dev_no);
		if(ret<0)
		{
			//SendGateAckDirect(fd,USR_RW_DEV_PARA,ERR_DVC_INTERNAL,env,enc);
			send_rw_para_file_err(fd,env,enc,dev_no);
		}
	}
	return ret;
}
#if 0
/*
	���յ����ļ���gz��ʽ��
	����һ��Զ���ļ�������
	��ŵ�Ԥ�ȶ���õ�·��
	�յ����ļ���gz��ʽ��
	type: 2:ip1004.ini 3:alarm.ini
	����ֵ  -1 ��������
			    -2 ���ܴ�ָ�����ļ�
			    -3 �ļ���ʽ����(���ܽ�ѹ���ļ�)
*/
static int RecvARmtPara(int type,char *filebuf,int filelen)
{
	#define RECV_PARA_TMP 	"/tmp/recvpara"
	char *filename=NULL;//�����ļ���������·��
	char *name=NULL;		//�����ļ���(������·��)
	char tbuf[100];
	char savefile[100];
	int ret;
	FILE *fp=NULL;
	if((filebuf==NULL)||(filelen<0))
		return -1;
	if(type==2)	
		filename=VSMAIN_PARA_FILE;
	else if(type==3)
		filename=MOTION_ALARM_PARA_FILE;
	else
	{
		gtloginfo("RecvARmtPara() ��������:��֧�ֵ�type:%d\n",type);
		return -1;
	}
	gtloginfo("׼�����������ļ�%s...\n",filename);
	name=strrchr(filename,'/');	//�ҵ����һ�� '/'��ȷ���ļ���
	if(name==NULL)
		name=filename;
	else
		name++;
	
#if EMBEDED==1
	//ɾ����ǰ����ʱĿ¼
	sprintf(tbuf,"rm -rf %s\n",RECV_PARA_TMP);
	system(tbuf);
#endif	
	//������ʱĿ¼
	sprintf(tbuf,"mkdir -p %s\n",RECV_PARA_TMP);
	system(tbuf);

	//���ļ�д��һ���ļ�
	sprintf(savefile,"%s/%s.gz",RECV_PARA_TMP,name);
	fp=fopen(savefile,"w");
	if(fp==NULL)
	{
		gtloginfo("���ܴ�����ʱ�ļ�%s\n",savefile);
		return -2;
	}
	fwrite(filebuf,1,filelen,fp);
	fclose(fp);

	//��ѹ���ļ�
	sprintf(tbuf,"gunzip %s",savefile);
	ret=system(tbuf);
	if(ret!=0)
	{
		gtloginfo("��ѹ���ļ�%s����\n",savefile);
		return -3;
	}
	//���ļ�������ָ��Ŀ¼��ָ���ļ�
	sprintf(tbuf,"cp %s/%s %s -f",RECV_PARA_TMP,name,filename);
	ret=system(tbuf);
	gtloginfo("���������ļ�%s/%s->%s ret=%d",RECV_PARA_TMP,name,filename,ret);

	//wsy add,ת�������ļ�
	sprintf(tbuf,"/ip1004/para_conv -s");
	ret=system(tbuf);
	
#if EMBEDED==1
	//ɾ���ù�����ʱĿ¼
	sprintf(tbuf,"rm -rf %s\n",RECV_PARA_TMP);
	system(tbuf);
#endif		
	
	return 0;

	
}
#endif


/*
	��һ���ļ����͵�Զ�̼����
*/
int SendAFile2Rmt(int fd,char *filename,int type,int env,int enc,int enack,int dev_no)
{
	struct return_para_struct *rtn;
	struct stat filestat;
	char *sendfile;
	FILE *fp;
	void *sendbuf=NULL;
	struct gt_pkt_struct *send=NULL;
	struct gt_usr_cmd_struct *sendcmd;

	int ret;

	if(fd<0)
		return -EINVAL;
	if(filename==NULL)
		return -EINVAL;
	sendfile=filename;

	ret=stat(sendfile,&filestat);
	if(ret!=0)
		return -ENOENT;

	fp=fopen(sendfile,"rb");
	if(fp==NULL)
		return -ENOENT;

	ret=posix_memalign(&sendbuf,sizeof(unsigned long),filestat.st_size+1024);
	if((ret!=0)||(sendbuf==NULL))
		return -ENOMEM;

	send=(struct gt_pkt_struct *)sendbuf;
	sendcmd=(struct gt_usr_cmd_struct *)send->msg;
	sendcmd->len=SIZEOF_GT_USR_CMD_STRUCT-sizeof(sendcmd->para)-2+sizeof(struct return_para_struct)-4+filestat.st_size;
	sendcmd->cmd=DEV_PARA_RETURN;
	sendcmd->en_ack=enack;
	sendcmd->reserve0=0;
	sendcmd->reserve1=0;
	rtn=(struct return_para_struct *)sendcmd->para;
	virdev_get_devid(dev_no,rtn->dev_id);
	rtn->type=type;
	rtn->filelen=filestat.st_size;
	rtn->reserve=0;
	fread(rtn->file,1,filestat.st_size,fp);
	ret=gt_cmd_pkt_send(fd,send,(sendcmd->len+2),NULL,0,env,enc);

	gtloginfo("��%s���͸�Զ�̼����ret=%d\n",sendfile,ret);
	if(sendbuf!=NULL)
		free(sendbuf);		
	if(fp!=NULL)
		fclose(fp);		
	return ret;		
}

//��һЩ������Ϣ����ini�ļ�
//�豸�̼��汾��,�豸��ǰʱ���
static int add_info2ini(char *filename)
{
	struct tm 	*ptime;
	time_t 		ctime;	
	char			timestr[50];
	dictionary      *ini;
	char			firmware[100];
	char			rtversion[30];
	char			hdversion[30];
	FILE*           lockfp=NULL;
	ini=iniparser_load_lockfile(filename,1,&lockfp);
        if (ini==NULL) 
        {
             printf("vsmain  cannot parse ini file file [%s]", filename);
             return -1 ;
        }

	//��ȡ�豸����汾��
	memset(firmware,0,sizeof(firmware));

	//��ȡ�豸��ǰʱ��
	ctime=time(NULL);
	ptime=localtime(&ctime);
	sprintf(timestr,"%04d-%02d-%02d %02d:%02d:%02d",ptime->tm_year+1900,
												    ptime->tm_mon+1,
												    ptime->tm_mday,
												    ptime->tm_hour,
												    ptime->tm_min,
												    ptime->tm_sec);
	printf("filename is %s\n",filename);
	iniparser_setstr(ini,"devinfo:firmware",firmware);
	iniparser_setstr(ini,"devinfo:cur_time",timestr);
	save_inidict_file(filename,ini,&lockfp);
	
	iniparser_freedict(ini);	
	return 0;
}

/**********************************************************************************************
 * ������	:send_para_to_rmt()
 * ����	:���豸�������ļ����͸�Զ�̼����
 * ����	:fd ���ӵ�Զ�̼�������ļ���������fd�������Ǹ�ֵ
 *			 type:�ļ� ֵ����ͬstruct usr_rwdevpara_struct�еĶ��� 
 *         			��2:ip1004.ini 3:alarm.ini����ֵ������
 *			env:ʹ�õ������ŷ��ʽ
 *			enc:Ҫʹ�õļ��ܸ�ʽ
 *			enack:�Ƿ���Ҫȷ��
 *����ֵ	:0��ʾ�ɹ���ֵ��ʾʧ��
 *			: -10 ��������
 *		   	: -11��֧�ֵĸ�ʽ
 **********************************************************************************************/
int send_para_to_rmt(int fd,int type,int env,int enc,int enack,int dev_no)
{
	#define SEND_PARA_TMP 	"/tmp/para"
	int ret;
	char tbuf[256];
	char *para_file=NULL;
	const char *file_name;
	char send_name[100];
	char send_dir[100];//��ŷ����ļ���Ŀ¼
	char tmp_file[100];// ��ʱini�ļ�
	if(fd<0)
		return -1;
	sprintf(send_dir,"%s/%d/dev%d",SEND_PARA_TMP,getpid(),dev_no);
#if EMBEDED==1
	//ɾ����ǰ����ʱĿ¼
	sprintf(tbuf,"rm -rf %s\n",send_dir);
	system(tbuf);
#endif	
	//������ʱĿ¼
	sprintf(tbuf,"mkdir -p %s\n",send_dir);
	system(tbuf);
	switch(type)
	{
		case RMT_FILE_GTPARA:
		case RMT_FILE_ALARMPARA:	//���������ļ�ip1004.ini .gz��ʽ
		para_file=IPMAIN_PARA_FILE;//MOTION_ALARM_PARA_FILE��ʹ����
			
		file_name=strrchr(para_file,'/');	//�ҵ����һ�� '/'��ȷ���ļ���
		if(file_name==NULL)
			file_name=para_file;
		else
			file_name++;

		//zw-modified 2011-11-12 ͬʱ�޸ķ����ļ��������ļ�
		add_info2ini(para_file);//zsk modified�� ��ǰ��tmp_file 

		//����һ������
		sprintf(tbuf,"cp -f %s %s/%s\n",para_file,send_dir,file_name);
		system(tbuf);
		sprintf(tmp_file,"%s/%s",send_dir,file_name);
		printf("KKK TEST %s\n",tmp_file);
		sprintf(tbuf,"gzip %s/%s\n",send_dir,file_name);
		system(tbuf);

		sprintf(send_name,"%s/%s.gz",send_dir,file_name);
		break;
		default:
			       printf("r:��֧�ֵ��ļ�����:%d!\n",type);
			gtloginfo("r:��֧�ֵ��ļ�����:%d!\n",type);
			return -1;
		break;		
	}

	ret=SendAFile2Rmt(fd,send_name,type, env, enc,enack,dev_no);
	//ɾ���ù����ļ�
	sprintf(tbuf,"rm -f %s\n",send_name);
	system(tbuf);			
	#if EMBEDED==1
	//ɾ���ù���Ŀ¼
		sprintf(tbuf,"rm -rf %s\n",send_dir);
		system(tbuf);
	#endif			

	return ret;		
}

