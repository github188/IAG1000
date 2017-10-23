/* ip2004 ϵͳ�豸��Ϣ������ 
 *
 *
 *
 */
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <iniparser.h>
#include <devinfo.h>
#include "guid.h"
#include "devtype.h"

#if EMBEDED==0
//	#define FOR_PC_MUTI_TEST		//֧����ͬһ̨pc���������������
#endif

#include <file_def.h>

static int init_flag=0;			///�Ѿ����ù�init_devinfo��־
static const char version[]="0.04";	//devinfo��İ汾��
// 0.04 zw 2010-07-09 �޸�devtype_gtvs3000.h�е�GTMV3121��ideֵΪ3����ʾ�õ���TF������������SD��
// 0.03 zw 2010-06-18 ��gtvs3024L������������6�����������4
// 0.02 ���Ӷ�gtvm��֧��,���ӻ�ȡ�汾�ŵĽӿ�get_prog_ver()
// 0.01 ��ʼ��devinfo��Ϊ��̬��

#define	DEV_GUID_BYTE		8	//GUIDռ�õ��ֽ���
typedef struct{
	GTSeriesDVSR	*dvsr;					//�豸����
	unsigned char 	guid[DEV_GUID_BYTE];		//�豸GUID
	char		guid_str[DEV_GUID_BYTE*4];	//guid���ַ�������
	int		disk_capacity;			//����������MΪ��λ
	struct	tm	lv_fac_time;			//�豸����ʱ��
	char		batch_seq[100];			//�豸��������
	char		board_seq[100];			//�豸�忨����
	char 		prog_ver_str[256];              ///<Ӧ�ó���汾��xxx-xxx-xxx
	int		lfc_flag;			//�豸������־��1=������0=�ڼ� //2008-06-26 zw add
}devinfo_struct;



static devinfo_struct info={
	.dvsr			=	NULL,
	.disk_capacity		=	0,

};
static  char *def_dev_guid="0067007400000000";//{0x0,0x0,0x0,0x0,'t',0x0,'g',0x0};//static int dev_type =0; //�豸����

GTSeriesDVSR	*get_current_dvsr(void)
{
	return info.dvsr;
}
//���devinfo.ini�еı����Ƿ�Ͷ�Ӧ���ͺ���Ϣһ��
//���ظ�ֵ��ʾ���� 0��ʾ��ȫһ�� 1��ʾ��һ�£������Ѿ����õ�ini�ṹ����
static int fix_devinfo_file(dictionary      *ini,GTSeriesDVSR	*dvsr)
{
	int change_flag=0;
	int num;
	char *pstr=NULL;
	if((ini==NULL)||(dvsr==NULL))
		return -EINVAL;
	//�ͺŴ���
	num=iniparser_getint(ini,"devinfo:devtype",-1);
	if(num!=dvsr->type)
	{
		iniparser_setint(ini,"devinfo:devtype",dvsr->type);
		gtloginfo("devinfo:devtype %d->%d",num,dvsr->type);
		change_flag=1;
	}

	//����������
	num=iniparser_getint(ini,"resource:trignum",-1);
	if(num!=dvsr->trignum)
	{
		iniparser_setint(ini,"resource:trignum",dvsr->trignum);
		gtloginfo("resource:trignum %d->%d",num,dvsr->trignum);
		change_flag=1;
	}
	
	//���������
	num=iniparser_getint(ini,"resource:outnum",-1);
	if(num!=dvsr->outnum)
	{
		iniparser_setint(ini,"resource:outnum",dvsr->outnum);
		gtloginfo("resource:outnum %d->%d",num,dvsr->outnum);
		change_flag=1;
	}

	//������
	num=iniparser_getint(ini,"resource:com",-1);
	if(num!=dvsr->com)
	{
		iniparser_setint(ini,"resource:com",dvsr->com);		
		gtloginfo("resource:com %d->%d",num,dvsr->com);
		change_flag=1;
	}

	//�Ƿ��л���ָ���
	num=iniparser_getint(ini,"resource:quad",-1);
	if(num!=dvsr->quad)
	{
		iniparser_setint(ini,"resource:quad",dvsr->quad);
		gtloginfo("resource:quad %d->%d",num,dvsr->quad);
		change_flag=1;
	}

	//��Ƶ������
	num=iniparser_getint(ini,"resource:videonum",-1);
	if(num!=dvsr->videonum)
	{
		iniparser_setint(ini,"resource:videonum",dvsr->videonum);
		gtloginfo("resource:videonum %d->%d",num,dvsr->videonum);
		change_flag=1;
	}

	num=iniparser_getint(ini,"resource:audionum",-1);
        if(num!=dvsr->audionum)
        {
                iniparser_setint(ini,"resource:audionum",dvsr->audionum);
                gtloginfo("resource:audionum %d->%d",num,dvsr->audionum);
                change_flag=1;
        }

	//��Ƶ��������
	num=iniparser_getint(ini,"resource:videoencnum",-1);
	if(num!=dvsr->videoencnum)
	{
		iniparser_setint(ini,"resource:videoencnum",dvsr->videoencnum);
		gtloginfo("resource:videoencnum %d->%d",num,dvsr->videoencnum);
		change_flag=1;
	}

	//�Ƿ��д洢�豸
	num=iniparser_getint(ini,"resource:ide",-1);
	if(num!=dvsr->ide)
	{
		iniparser_setint(ini,"resource:ide",dvsr->ide);
		gtloginfo("resource:ide %d->%d",num,dvsr->ide);
		change_flag=1;
	}
/*yk del �������*/
#if 0
	//�Ƿ��д洢�豸
	num=iniparser_getint(ini,"resource:ide",-1);
	if(num!=dvsr->ide)
	{
		iniparser_setint(ini,"resource:ide",dvsr->ide);
		gtloginfo("resource:ide %d->%d",num,dvsr->ide);
		change_flag=1;
	}
#endif
#if 0
	//
	if(dvsr->ide==0)
	{
		num=iniparser_getint(ini,"resource:disk_capacity",-1);
		if(num!=0)
		{
			iniparser_setint(ini,"resource:disk_capacity",0);
			gtloginfo("resource:disk_capacity %d->%d",num,0);
			change_flag=1;
		}
	}
	
#endif
	
	//������
	num=iniparser_getint(ini,"resource:eth_port",-1);
	if(num!=dvsr->eth_port)
	{
		iniparser_setint(ini,"resource:eth_port",dvsr->eth_port);
		gtloginfo("resource:eth_port %d->%d",num,dvsr->eth_port);
		change_flag=1;
	}


	return change_flag;
	
}
/**********************************************************************************************
 * ������       :get_devinfo_version
 * ���� :	��ȡdevinfo��İ汾��Ϣ
 * ���� :��
 * ����ֵ       :����devinfo��汾��Ϣ���ַ���ָ��
  **********************************************************************************************/
const char *get_devinfo_version(void)
{
	return version;
}
/**********************************************************************************************
 * ������	:init_devinfo()
 * ����	:��ʼ���豸��Ϣ
 * ����	:��
 * ����ֵ	:0��ʾ��������ֵ��ʾ����
 * ע		:Ӧ�ó����ڸ�������ʱ����Ҫ�������������/conf/devinfo.ini�ж�ȡϵͳ��Ϣ
 *			���/conf/devinfo.ini�����ڣ�����豸��Ϣ���óɳ�ʼֵ��������-1
  **********************************************************************************************/
int init_devinfo(void)
{
	dictionary      *ini=NULL;
    	char *pstr=NULL;
    	int status;
	struct GT_GUID guid;
	int num;
//	struct tm *ptime=NULL;
//	time_t ctime;
	FILE *fp=NULL;
	int  write_file_flag=0;
	
	
	if(init_flag)
		return 0;
	init_flag=1;
	memset((void*)&info,0,sizeof(devinfo_struct));
	ini=iniparser_load_lockfile(DEVINFO_PARA_FILE,1,&fp);
	if(ini==NULL)
	{
		
                printf("init_devinfo() cannot parse ini file file [%s]", DEVINFO_PARA_FILE);
                gtlogerr("init_devinfo() cannot parse ini file file [%s]", DEVINFO_PARA_FILE);
                return -1 ;
        }
       // iniparser_dump_ini(ini,stdout); //��ini�ļ�������ʾ����Ļ�ϣ�ʵ��ʹ��ʱû���ã�Ӧȥ��
	pstr=iniparser_getstring(ini,"devinfo:devguid",def_dev_guid);
	guid=hex2guid(pstr);
	memcpy((char*)info.guid,(char*)(&guid),sizeof(guid));
	guid2hex(guid,info.guid_str);
	
	//�豸�ͺ��ַ���
	pstr=iniparser_getstring(ini,"devinfo:devtypestring",T_GTIP2000_STR);
	info.dvsr=get_dvsr_by_typestr(pstr);
	if(info.dvsr==NULL)
		info.dvsr=get_dvsr_by_typestr(T_GTIP2000_STR);//Ĭ��ΪIP2004

	//�豸�ͺŴ���
	num=iniparser_getint(ini,"devinfo:devtype",-1);
	if(num!=conv_dev_str2type(pstr))	//pstr����ͺ��ַ���
	{
		iniparser_setint(ini,"devinfo:devtype",conv_dev_str2type(pstr));
		write_file_flag=1;
	}
	

	if(fix_devinfo_file(ini,info.dvsr)==1)
		write_file_flag=1;

	pstr=iniparser_getstring(ini,"devinfo:batchseq","NULL");
	sprintf(info.batch_seq,"%s",pstr);

	pstr=iniparser_getstring(ini,"devinfo:cpuboard","NULL");
	sprintf(info.board_seq,"%s",pstr);
	//	ctime=time(NULL); //	ptime=localtime(&ctime);	//	memcpy((void*)&info.lv_fac_time,(void *)ptime,sizeof(info.lv_fac_time));


	info.lv_fac_time.tm_year=iniparser_getint(ini,"leave_fac:year",2000)-1900;
	info.lv_fac_time.tm_mon=iniparser_getint(ini,"leave_fac:mon",1)-1;
	info.lv_fac_time.tm_mday=iniparser_getint(ini,"leave_fac:day",1);
	info.lv_fac_time.tm_hour=iniparser_getint(ini,"leave_fac:hour",0);
	info.lv_fac_time.tm_min=iniparser_getint(ini,"leave_fac:min",0);
	info.lv_fac_time.tm_sec=iniparser_getint(ini,"leave_fac:sec",0);	
	info.lfc_flag=iniparser_getint(ini,"leave_fac:lfc_flag",-1);

	num=iniparser_getint(ini,"resource:disk_capacity",-1);
	info.disk_capacity=num;
	if(info.disk_capacity<0)
	{
		iniparser_setstr(ini,"resource:disk_capacity","0");
		write_file_flag=1;		
	}	

	if(write_file_flag)
		save_inidict_file(DEVINFO_PARA_FILE,ini,&fp);
	else
	{
		if(fp!=NULL)
		{
			unlock_file(fileno(fp));
			fsync(fileno(fp));
			fclose(fp);
		}	
	}

	iniparser_freedict(ini);
	return 0;
}

/**********************************************************************************************
 * ������	:get_devid()
 * ����	:Ӧ�ó�����û���豸��dev_id(������Ӧ���Ѿ����ù���init_devinfo())
 * ����	:��
 * ��� 	:buf:Ӧ�ó�����Ҫ���devid�Ļ�������ַ,����ʱ���buf�ĳ��ȱ����㹻������DEV_GUID_BYTE
 * ����ֵ	:��ֵ��ʾ��䵽buf�е���Ч�ֽ�������ֵ��ʾ����
  **********************************************************************************************/
int get_devid(unsigned char *buf)
{
    if(buf==NULL)
	    return -1;
    memcpy(buf,info.guid,DEV_GUID_BYTE);
    return DEV_GUID_BYTE;
}

/**********************************************************************************************
 * ������	:get_devid_str()
 * ����	:Ӧ�ó�����û���豸��guid���ַ���
 * ����	:��
 * ����ֵ	:ָ������guid��Ϣ���ַ���ָ��
  **********************************************************************************************/
char* get_devid_str(void)
{
	return info.guid_str;
}

/**********************************************************************************************
 * ������	:get_lfc_flag()
 * ����	:Ӧ�ó�����û���豸������־
 * ����	:��
 * ����ֵ	:1=������0=�ڼ�, -1����
  **********************************************************************************************/
int get_lfc_flag(void)
{
	return info.lfc_flag;
}


/**********************************************************************************************
 * ������	:set_devid_str()
 * ����	:Ӧ�ó�����������豸��guidֵ,�ַ�����ʽ
 * ����	:id_str:�ַ�����ʾ���豸guidֵ
 * ��� 	:
 * ����ֵ	:0��ʾ�ɹ� ��ֵ��ʾ���� -EINVAL��ʾ���������ʽ����
  **********************************************************************************************/
int set_devid_str(char *id_str)
{

	char				guid_temp[DEV_GUID_BYTE*4]; 
	struct GT_GUID 	guid;
	dictionary      		*ini=NULL;
	char 			*pstr=NULL;
	FILE 			*fp=NULL;
	
	if(id_str==NULL)
		return -EINVAL;
	guid=hex2guid(id_str);
	guid2hex(guid,guid_temp);
	if(strncasecmp(guid_temp,id_str,DEV_GUID_BYTE*2)!=0)
		return -EINVAL;	//GUIDֵ�Ƿ�
	ini=iniparser_load_lockfile(DEVINFO_PARA_FILE,1,&fp);
	if(ini==NULL)
	{
                printf("init_devinfo() cannot parse ini file file [%s]", DEVINFO_PARA_FILE);
                return -ENOENT ;
        }
	memcpy((void*)info.guid,(void*)&guid,DEV_GUID_BYTE);
	sprintf(info.guid_str,"%s",guid_temp);
	
	pstr=iniparser_getstring(ini,"devinfo:devguid","NULL");
	gtloginfo("devinfo:devguid %s->%s\n",pstr,guid_temp);
	iniparser_setstr(ini,"devinfo:devguid",guid_temp);	
	save_inidict_file(DEVINFO_PARA_FILE,ini,&fp);
	iniparser_freedict(ini);
	return 0;	
}
/**********************************************************************************************
 * ������	:set_devid()
 * ����	:Ӧ�ó�����������豸��guidֵ,�����Ƹ�ʽ
 * ����	:buf:������������guidֵ������ΪDEV_GUID_BYTE�ֽ�
 * ��� 	:
 * ����ֵ	:0��ʾ�ɹ� ��ֵ��ʾ���� -EINVAL��ʾ���������ʽ����
  **********************************************************************************************/
int set_devid(unsigned char *buf)
{
	struct GT_GUID guid;
	char id_str[DEV_GUID_BYTE*4];
	if(buf==NULL)
		return -EINVAL;
	memcpy((void*)&guid,buf,DEV_GUID_BYTE);
	guid2hex(guid,id_str);
	return set_devid_str(id_str);
}

/**********************************************************************************************
 * ������	:get_hd_capacity()
 * ����	:��ȡ�豸�Ĵ���������Ϣ(��MBΪ��λ)
 * ����	:��
 * ����ֵ	:��ֵ��ʾ�豸�Ĵ�����������ֵ��ʾ����
 **********************************************************************************************/
int get_hd_capacity(void)
{
	return info.disk_capacity;
}

/**********************************************************************************************
 * ������	:get_hd_type()
 * ����	:��ȡ��������
 * ����	:��
 * ����ֵ	:0-CF,1-HD,-1-none
 **********************************************************************************************/
int get_hd_type(void)//0-CF,1-HD,-1-none
{
	if(info.disk_capacity<200)
		return -1;
	if(info.disk_capacity>20*1024)
		return 1;
	return 0;
}


/**********************************************************************************************
 * ������	:get_hd_nodename()
 * ����	:��ȡ���̽ڵ�����
 * ����	:diskno, ���̱�ţ�0��1��2����
 * ����ֵ	:�ڵ����ƣ�NULL��ʾʧ��
 **********************************************************************************************/
char * get_hd_nodename(int diskno)
{
	switch(diskno)
	{
		case 0:	return "/dev/sda"; 
		case 1:	return "/dev/sdb";
		case 2: return "/dev/sdc";
		case 3: return "/dev/sdd";
		default:	return NULL;
	}
}







/**********************************************************************************************
 * ������	:set_hd_capacity()
 * ����	:�����豸�Ĵ�������ֵ
 * ����	:value:�����⵽�Ĵ�������ֵMBΪ��λ
 * ����ֵ	:0��ʾ�ɹ�����ֵ��ʾ����
 **********************************************************************************************/
int set_hd_capacity(int value)
{
	dictionary      *ini;
	FILE *fp=NULL;
	ini=iniparser_load_lockfile(DEVINFO_PARA_FILE,1,&fp);
    	if (ini==NULL) {
            printf("init_devinfo() cannot parse ini file file [%s]", DEVINFO_PARA_FILE);
            return -1 ;
    	}
    	if(info.disk_capacity==value)
    	{
		iniparser_freedict(ini);
		if(fp!=NULL)
		{
                        unlock_file(fileno(fp));
                        fsync(fileno(fp));
                        fclose(fp);
		}
    		return 0;
    	}
	info.disk_capacity=value;
	//gtloginfo("test!!!!!!!!����Ϊ%d\n",value);
	iniparser_setint(ini,"resource:disk_capacity",value);
	save_inidict_file(DEVINFO_PARA_FILE,ini,&fp);
	iniparser_freedict(ini);
	return 0;
}

/**********************************************************************************************
 * ������	:set_devtype_str()
 * ����	:�����豸���ͺ��ַ���
 * ����	:type_str:�����豸�ͺŵ��ַ���
 * ����ֵ	:0��ʾ�ɹ�����ֵ��ʾʧ��,-EINVAL��ʾ�����ǲ�֧�ֵ�����
 **********************************************************************************************/
int set_devtype_str(char *type)
{
	GTSeriesDVSR 	*dvsr=NULL;
	dictionary      		*ini=NULL;
	FILE				*fp=NULL;
	char				*ptr=NULL;
	if(type==NULL)
		return -EINVAL;
	dvsr=get_dvsr_by_typestr(type);
	if(dvsr==NULL)
		return -EINVAL;
	ini=iniparser_load_lockfile(DEVINFO_PARA_FILE,1,&fp);
	if(ini==NULL)
	{
                printf("init_devinfo() cannot parse ini file file [%s]", DEVINFO_PARA_FILE);
                return -ENOENT ;
        }

	ptr=iniparser_getstring(ini,"devinfo:devtypestring","NULL");
	gtloginfo("devinfo:devtypestring %s->%s",ptr,type);
	iniparser_setstr(ini,"devinfo:devtypestring",type);

	
	ptr=iniparser_getstring(ini,"devinfo:devtype","-1");
	gtloginfo("devinfo:devtypestring %d->%d",atoi(ptr),conv_dev_str2type(type));
	iniparser_setint(ini,"devinfo:devtype",conv_dev_str2type(type));
	save_inidict_file(DEVINFO_PARA_FILE,ini,&fp);
	iniparser_freedict(ini);
	return 0;	
}
/**********************************************************************************************
 * ������	:get_devtypestr()
 * ����	:Ӧ�ó�����û�������豸���͵��ַ���
 * ����	:��
 * ����ֵ	:�����豸�����ַ�����ָ��
 **********************************************************************************************/
char * get_devtype_str(void)
{
	return conv_dev_type2str(info.dvsr->type);
}
/**********************************************************************************************
 * ������	:get_devtype()
 * ����	:Ӧ�ó�����û���豸���ʹ���
 * ����	:��
 * ����ֵ	:�豸���ʹ���
 **********************************************************************************************/
int 	get_devtype(void)
{
	return info.dvsr->type;
}

/**********************************************************************************************
 * ������	:get_batch_seq()
 * ����	:��ȡ���������ַ���
 * ����	:��
 * ����ֵ	:ָ�����������ַ�����ָ��
 **********************************************************************************************/
char *get_batch_seq(void)
{
	return info.batch_seq;
}

/**********************************************************************************************
 * ������	:get_board_seq()
 * ����	:��ȡ�忨�����ַ���
 * ����	:��
 * ����ֵ	:ָ��忨�����ַ�����ָ��
 **********************************************************************************************/
char *get_board_seq(void)
{
	return info.board_seq;
}

/**********************************************************************************************
 * ������	:set_batch_seq()
 * ����	:�������������ַ���
 * ����	:����������Ϣ
 * ����ֵ	:0��ʾ�ɹ���ֵ��ʾʧ��
 **********************************************************************************************/
int set_batch_seq(char *seq)
{
	int len;
	dictionary      		*ini=NULL;
	FILE				*fp=NULL;
	char				*pstr=NULL;
	len=strlen(seq);
	if((len+1)>sizeof(info.batch_seq))
		len=sizeof(info.batch_seq)-1;
	memcpy(info.batch_seq,seq,len+1);
	info.batch_seq[len]='\0';
	ini=iniparser_load_lockfile(DEVINFO_PARA_FILE,1,&fp);
	if(ini==NULL)
	{
                printf("init_devinfo() cannot parse ini file file [%s]", DEVINFO_PARA_FILE);
                return -ENOENT ;
        }
	pstr=iniparser_getstring(ini,"devinfo:batchseq","NULL");
	gtloginfo("devinfo:batchseq %s->%s",pstr,info.batch_seq);
	iniparser_setstr(ini,"devinfo:batchseq",info.batch_seq);

	save_inidict_file(DEVINFO_PARA_FILE,ini,&fp);
	iniparser_freedict(ini);
	return 0;
	
}

/**********************************************************************************************
 * ������	:set_board_seq()
 * ����	:���ð忨�����ַ���
 * ����	:�忨������Ϣ
 * ����ֵ	:0��ʾ�ɹ���ֵ��ʾʧ��
 **********************************************************************************************/
int set_board_seq(char *seq)
{
	int len;
	dictionary      		*ini=NULL;
	FILE				*fp=NULL;
	char				*pstr=NULL;
	len=strlen(seq);
	if((len+1)>sizeof(info.board_seq))
		len=sizeof(info.board_seq)-1;
	memcpy(info.board_seq,seq,len+1);
	info.board_seq[len]='\0';
	ini=iniparser_load_lockfile(DEVINFO_PARA_FILE,1,&fp);
	if(ini==NULL)
	{
                printf("init_devinfo() cannot parse ini file file [%s]", DEVINFO_PARA_FILE);
                return -ENOENT ;
        }
	pstr=iniparser_getstring(ini,"devinfo:cpuboard","NULL");
	gtloginfo("devinfo:cpuboard %s->%s",pstr,info.board_seq);
	iniparser_setstr(ini,"devinfo:cpuboard",info.board_seq);

	save_inidict_file(DEVINFO_PARA_FILE,ini,&fp);
	iniparser_freedict(ini);
	return 0;
}
/**********************************************************************************************
 * ������	:get_lvfac_time()
 * ����	:��ȡ�豸�ĳ���ʱ��
 * ����	:��
 * ����ֵ	:�豸����ʱ��Ľṹָ��
 **********************************************************************************************/
struct tm * get_lvfac_time(void)
{
	return &info.lv_fac_time;
}

/**********************************************************************************************
 * ������	:get_lvfac_time()
 * ����	:��ȡ�豸�ĳ���ʱ��
 * ����	:��
 * ����ֵ	:0��ʾ��ȷ ��ֵ��ʾ����  -EINVAL��ʾ ��������
 **********************************************************************************************/
int  set_lvfac_time(struct tm *lv)
{
	time_t 			seconds;
	struct tm 		*t=NULL;
	char 			oldtime[100];
	char 			newtime[100];
	dictionary      		*ini=NULL;
	FILE				*fp=NULL;
	if(lv==NULL)
		return -EINVAL;
	seconds=mktime(lv);
	if((int)seconds<0)
		return -EINVAL;

	ini=iniparser_load_lockfile(DEVINFO_PARA_FILE,1,&fp);
	if(ini==NULL)
	{
                printf("init_devinfo() cannot parse ini file file [%s]", DEVINFO_PARA_FILE);
                return -ENOENT ;
        }
	



	
	t=lv;
	sprintf(newtime,"%04d-%02d-%02d %02d:%02d:%02d",t->tm_year+1900,
											t->tm_mon+1,
											t->tm_mday,
											t->tm_hour,
											t->tm_min,
											t->tm_sec);
	t=&info.lv_fac_time;
	sprintf(oldtime,"%04d-%02d-%02d %02d:%02d:%02d",t->tm_year+1900,
											t->tm_mon+1,
											t->tm_mday,
											t->tm_hour,
											t->tm_min,
											t->tm_sec);

	iniparser_setint(ini,"leave_fac:year",lv->tm_year+1900);
	iniparser_setint(ini,"leave_fac:mon",lv->tm_mon+1);
	iniparser_setint(ini,"leave_fac:day",lv->tm_mday);
	iniparser_setint(ini,"leave_fac:hour",lv->tm_hour);
	iniparser_setint(ini,"leave_fac:min",lv->tm_min);
	iniparser_setint(ini,"leave_fac:sec",lv->tm_sec);	
	iniparser_setint(ini,"leave_fac:lfc_flag",1);
	gtloginfo("leave_fac %s->%s\n",oldtime,newtime);
	
	save_inidict_file(DEVINFO_PARA_FILE,ini,&fp);
	iniparser_freedict(ini);
	memcpy((void*)&info.lv_fac_time,lv,sizeof(struct tm ));
	return 0;
}


/**********************************************************************************************
 * ������	:get_total_com()
 * ����	:��ȡ�豸���ܴ�����
 * ����	:��
 * ����ֵ	:��ֵ��ʾ�豸���ܴ�����,��ֵ��ʾ����
 **********************************************************************************************/
int get_total_com(void)
{
	return info.dvsr->com;
}

/**********************************************************************************************
 * ������	:get_video_num()
 * ����	:��ȡ�豸�ɽ������Ƶ����
 * ����	:��
 * ����ֵ	:�ɽ������Ƶ��
 **********************************************************************************************/
int get_video_num(void)
{
	return info.dvsr->videonum;
}


/**********************************************************************************************
 * ������	:get_videoenc_num()
 * ����	:��ȡ��Ƶ��������
 * ����	:��
 * ����ֵ	:��Ƶ��������
 **********************************************************************************************/
int get_videoenc_num(void)
{
	return info.dvsr->videoencnum;
}

/**********************************************************************************************
 * ������	:get_ide_flag()
 * ����	:��ȡ�豸�Ƿ���Ӳ�̱�־
 * ����	:��
 * ����ֵ	:1��ʾ��Ӳ�� ��CF�� 2��ʾ��SD�� 0��ʾû��
 **********************************************************************************************/
int	get_ide_flag(void)
{
	return info.dvsr->ide;
}

/**********************************************************************************************
 * ������	:get_trigin_num()
 * ����	:��ȡ���������
 * ����	:��
 * ����ֵ	:�豸�ϵĶ�������
 **********************************************************************************************/
int	get_trigin_num(void)
{
	return info.dvsr->trignum;
}


/**********************************************************************************************
 * ������	:get_alarmout_num()
 * ����	:��ȡ���������
 * ����	:��
 * ����ֵ	:�豸�ϵ����������
 **********************************************************************************************/
int get_alarmout_num(void)
{
	return info.dvsr->outnum;
}


/**********************************************************************************************
 * ������	:get_eth_num()
 * ����	:��ȡ������
 * ����	:��
 * ����ֵ	:�豸�ϵ�������
 **********************************************************************************************/
int get_eth_num(void)
{
	return info.dvsr->eth_port;
}


/**********************************************************************************************
 * ������	:get_disk_no()
 * ����	:��ȡ������Ŀ
 * ����ֵ	:������Ŀ
 **********************************************************************************************/
int get_disk_no(void)
{
	return info.dvsr->ide; 
}



/**********************************************************************************************
 * ������	:get_quad_flag()
 * ����	:��ȡ����ָ����Ƿ���Ч��־
 * ����	:��
 * ����ֵ	:1��ʾ�л���ָ��� 0��ʾû��
 **********************************************************************************************/
int get_quad_flag(void)
{
	return info.dvsr->quad;
}

/**********************************************************************************************
 * ������	:get_osd_flag()
 * ����	:��ȡ�Ƿ���osd��־
 * ����	:��
 * ����ֵ	:1��ʾ��0��ʾû��
 **********************************************************************************************/
int get_osd_flag(void)
{
	return info.dvsr->osd;
}

/**********************************************************************************************
 * ������	:get_cpu_iic_flag()
 * ����	:��ȡ�Ƿ�ʹ��cpu��iic���߿�����Ƶadת��оƬ
 * ����	:��
 * ����ֵ	:1��ʾʹ��0��ʾ��ʹ��
 **********************************************************************************************/
int get_cpu_iic_flag(void)
{
	return info.dvsr->use_cpu_iic;
}


/**********************************************************************************************
 * ������	:get_audio_num()
 * ����	:��ȡ��Ƶͨ������
 * ����	:��
 * ���� :��Ƶͨ������
 **********************************************************************************************/
int get_audio_num(void)
{
	return info.dvsr->audionum;
}

/***********************************************************************************************
 * ������:get_dev_family
 * ����  :��ȡ�豸�����ĸ�ϵ�еĲ�Ʒ
 * ����  :��
 * ����  :��Ʒϵ�д��� GTDEV_FAMILY_GTVS GTDEV_FAMILY_GTVM
 **********************************************************************************************/
///GTӲ����Ʒϵ�ж���
#define GTDEV_FAMILY_GTVS	0	///GTVSϵ����Ƶ������
#define GTDEV_FAMILY_GTVM	1	///GTVMϵ�����������

int get_dev_family(void)
{
	char *pstr=NULL;
        pstr=conv_dev_type2str(info.dvsr->type);
        if(strstr(pstr,"GTVM")!=NULL)
        {//GTVM
		return GTDEV_FAMILY_GTVM;
	}
	else
	{
		return GTDEV_FAMILY_GTVS;
	}
}

