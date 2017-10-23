/*	

 */
/*
 * 
 */
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <netdb.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/vfs.h>

#include <fcntl.h>      
#include <sys/file.h>
#include <termios.h>    
#include <commonlib.h>
#ifndef gtloginfo
#include <syslog.h>
//#define gtlog  syslog		//ϵͳ��־��Ϣ��¼
#define gtlog syslog
//һ������Ϣ
#define gtloginfo(args...) syslog(LOG_INFO,##args)	//��¼һ����Ϣ
//���صĴ�����Ϣ
#define gtlogfault(args...) syslog(LOG_CRIT,##args)	//
//������Ϣ
#define gtlogerr(args...) syslog(LOG_ERR,##args)	//
//������Ϣ
#define gtlogwarn(args...) syslog(LOG_WARNING,##args)

#define gtlogdebug(args...) syslog(LOG_DEBUG,##args)
#endif


#include <sys/ioctl.h>
#include <time.h>
#include <sys/time.h>
//#include <linux/rtc.h>
//#include "hi_rtc.h"
// yk del #include "gpio_i2c.h"

/*��bcd��ת��ʮ����*/
unsigned char inline bcdtobin(unsigned char hex)
{
	return (  (hex>>4)*10 + (hex&0xf) );
}

unsigned char inline bintobcd(unsigned char bin)
{
	return ( bin/10*16+bin%10  );
}

static int RTC_INIT = 0;




//��ȡ���д��̿ռ��С����MΪ��λ
long get_disk_free(char *mountpath)
{
	 struct statfs  buf;
	 if(mountpath==NULL)
	 	return -1;
	 if(statfs(mountpath,&buf)<0)
	 {
	 	printf("error at check_disk, mountpath %s\n",mountpath);
	 	//gtlogwarn("��ȡ���д��̴�Сʱʧ�ܣ�·��%s\n",mountpath);
	 	return -errno;
	 }
	return (buf.f_bavail*(buf.f_bsize>>10)>>10);
}

//��ȡ����������MΪ��λ
long get_disk_total(char *mountpath)
{
	 struct statfs  buf;
	 if(mountpath==NULL)
	 	return -1;
	 if(statfs(mountpath,&buf)<0)
	 {
	 	printf("get_disk_total error at check_disk,path:%s\n",mountpath);
	 	gtlogwarn("��ȡ�����ܴ�Сʱʧ�ܣ�·��%s,%d:%s\n",mountpath,errno,strerror(errno));
	 	return -1;
	 }
	return (buf.f_blocks*(buf.f_bsize>>10))>>10;
}



//����ļ��Ƿ���ڣ���������򷵻�1���򷵻�0
int check_file(char *file)
{
	if(file==NULL)
		return 0;
	if(access(file,F_OK)<0)
		return 0;
	return 1;
}
/**************************************************************************
  *������	:get_file_lines
  *����	:��ȡָ���ļ���������
  *����	: FileName:Ҫͳ���������ļ���
  *����ֵ	:��ֵ��ʾ�ļ�����������ֵ��ʾ����
  *ע:Ӧ�ó������в�����"grep"�������������᲻��ȷ
  *************************************************************************/
int get_file_lines(char *FileName)
{
	FILE *Fp=NULL;
	char  ReadBuf[256];
	char  *PR;
	char  *p;
	int Lines=0;
	if(FileName==NULL)
		return -EINVAL;
	Fp=fopen(FileName,"r");
	if(Fp==NULL)
		return -errno;
	//��ȡ�ļ�����
	while(1)
	{
		PR=fgets(ReadBuf,sizeof(ReadBuf),Fp);
		if(PR==NULL)
		{
			break;
		}
		p=strstr(ReadBuf,"grep");
		if(p==NULL)
			Lines++;
	}

	fclose(Fp);
	return Lines;	
}

int get_ps_threadnum(char *psName,char *FileName)
{
	FILE *Fp=NULL;
	char  ReadBuf[256];
	char  result[10];
	char  cmdline[100];
	char buff[50]; 
	char  *PR;
	char  *p;
	char  *p1;
	char  *p2;
	char  *threadid;
	char  *other;
	FILE *fstream=NULL;       
    int  threadnum=0;
	int Lines=0;
	if(FileName==NULL || psName == NULL)
		return -EINVAL;

	Fp=fopen(FileName,"r");
	if(Fp==NULL)
		return -errno;

	memset(result,0,10);
	while(1)
	{
		PR=fgets(ReadBuf,sizeof(ReadBuf),Fp);
		if(PR==NULL)
		{
			break;
		}
		p=strstr(ReadBuf,"grep");
		if(p==NULL)
		{
			//lc do �ҵ�ָ������
			//sscanf(ReadBuf," %s %s",threadid,other);
			p1=strstr(ReadBuf,"root");
			strncpy(result,ReadBuf,(p1-ReadBuf-1));
			printf("result is %s\n",result);
			Lines++;
		}
	}
	fclose(Fp);
	//lc do ȥ��result�д��ڵĿո�
	p1 = strstr(result," ");
	if(p1 != NULL)
		p2 = p1 + 1;
	else
		p2 = result;
	
	if(Lines == 0 )
	{
		printf("err no lines was found!\n");
		return -1;
	}
	memset(cmdline,0,100);
	memset(buff,0,sizeof(buff));  
	sprintf(cmdline,"cat /proc/%s/status | grep Threads | awk '{print $2}' ",p2);

	if(NULL==(fstream=popen(cmdline,"r")))       
    {      
        printf("execute command failed: %s",strerror(errno));       
        return -1;       
    } 

	if(NULL!=fgets(buff, sizeof(buff), fstream))      
    {      
        //printf("threadnum is %s\n",buff);   
        printf("Process %s has %s threads!\n",psName,buff);
        //gtloginfo("Process %s has %s threads!\n",psName,buff);
    }      
    else     
    {     
        pclose(fstream);     
        return -1;     
    } 
	
	threadnum = atoi(buff);

	pclose(fstream);
	return threadnum;		
}


//�ر������Ѿ��򿪵���Դ(����ָ��������������)
void CloseAllResExc(int Fd)
{
	int MaxOpen,i;
	MaxOpen=sysconf(_SC_OPEN_MAX);
	if(MaxOpen<0)
	{
		printf("sysconf error !\n");
	}
	//stdin stdout stderr���ر�
	for (i=3;i<MaxOpen;i++)
	{
		if(i!=Fd)
			close(i);
	}	
}

//�ر������Ѿ��򿪵���Դ
void close_all_res(void)
{
	int maxopen,i;
	maxopen=sysconf(_SC_OPEN_MAX);
	if(maxopen<0)
	{
		printf("sysconf error !\n");
	}
	//stdin stdout stderr���ر�
	for (i=3;i<maxopen;i++)
	{
		close(i);
	}	
}
int deamon_init(void)
{
	pid_t pid;
	int i;
	int maxopen;
	if((pid=fork())<0)
	{
		printf("deamon fork error !\n");
		return -1;
	}
	if(pid!=0)
		exit(0);
	setsid();
	chdir("/");
	umask(0);

	maxopen=sysconf(_SC_OPEN_MAX);
	if(maxopen<0)
	{
		printf("sysconf error !\n");
		return -1;
	}
	for (i=3;i<maxopen;i++)
	{
		close(i);
	}
	close(0);	//������׼����ʹ������
	
	
	return 0;
}

//#define DISKMAN_LOCK_FILE "/lock/vserver/diskman"
//#define IPMAIN_LOCK_FILE "/lock/vserver/vsmain"
//�����������ļ�
//���ش򿪵��ļ�������
int create_and_lockfile(char *lockfile)
{
	int lf;
	char dir[100];
	char sbuf[120];
	char *rch;
	if(lockfile==NULL)
		return -1;
	lf=open(lockfile,O_RDWR|O_CREAT,0640);//�����ļ�,������������
	if(lf<0)
	{
		strncpy(dir,lockfile,strlen(lockfile));
		rch=strrchr(dir,'/');
		if(rch!=NULL)
		{
			*rch='\0';
			sprintf(sbuf,"mkdir %s -p\n",dir);
			system(sbuf);
			
		}
		//mkdir("/lock",0770);
		//mkdir("/lock/vserver",0770);
		lf=open(lockfile,O_RDWR|O_CREAT,0640);
		if(lf<0)
		{
			printf("create lock file:%s error!\n",lockfile);
			gtloginfo("�������ļ�%sʧ��\n",lockfile);
			return -2;
		}
	}
	if(flock(lf,LOCK_EX|LOCK_NB)!=0)//�����̱�־�ļ��������Է�ֹ�������ͬһ����Ķ������
	{
		close(lf);//added
		return -1;
	}		
	return lf;
}
/**********************************************************************************************
 * ������	:create_lockfile_save_version()
 * ����	:�������ļ����������,���ش򿪵��ļ�������,���ո�ʽ��Ű汾����Ϣ
 * ����	:lockfile:Ҫ�������ļ���,
 *			 version:���̰汾���ַ���
 * ����ֵ	:��ֵ��ʾ�򿪵��ļ�������,��ֵ��ʾ����
 * ������һ�������ж��Ƿ���ĳ���̵ĸ�����������
 * �ļ���ʽ:
 *��һ��:���̺�
 *�ڶ���:version:���̰汾��
 **********************************************************************************************/
int create_lockfile_save_version(char *lockfile,char *version)
{
	char pbuf[100];
	int fd=-1;
	if(lockfile==NULL)
		return -EINVAL;
	fd=create_and_lockfile(lockfile);
	if(fd<0)
		return fd;
	
	sprintf(pbuf,"%d\nversion:%s\n",getpid(),version);
	write(fd,pbuf,strlen(pbuf)+1);					//�����̵�id�Ŵ������ļ���
	return fd;
}
#if 0
////added by lsk 2006 -11-7
//ǿ�Ƽ����ļ�  
//���� fd �ļ������� ,cmd ����, wait �ȴ���־
//����0 �ɹ� -1 ʧ��
// cmd = F_RDLCK ����ֹ �� F_WRLCK д��ֹ; F_UNLCK �������
// wait = 0 �޷��������������أ� =1 �ȴ�����
int force_lockfile(int fd, short int cmd, int wait)
{
	int ret;
	struct flock tp;
	if(fd<0)
		return -1;
//	printf("cmd = %d , F_UNLCK = %d, wrlck = %d, rdlck = %d\n", cmd, F_UNLCK, F_WRLCK,F_RDLCK);
	tp.l_type = cmd;
	tp.l_whence = SEEK_SET;
	tp.l_len = 0;
	tp.l_pid = getpid();
	if(wait ==0)
	return fcntl(fd , F_SETLK, &tp);
	if(wait ==1)
	return fcntl(fd , F_SETLKW, &tp);
	return -1;
}
#endif
/**********************************************************************************************
 * ������	:write_process_info()
 * ����	:�����̵İ汾����Ϣ��ϵͳͳһ�ĸ�ʽд�뵽һ���򿪵��ļ���
 * ����	:lfd:��д����ļ�������
 *			 version:�ַ�����ʾ�İ汾��Ϣ
 * ����ֵ	:0��ʾ�ɹ���ֵ��ʾ����
 **********************************************************************************************/
int write_process_info(int fd,char *version)
{
	char pbuf[100];
	if((fd<0)||(version==NULL))
		return -EINVAL;
	sprintf(pbuf,"%d\nversion:%s\n",getpid(),version);
	write(fd,pbuf,strlen(pbuf)+1);//�����̵�id�Ŵ������ļ���
	return 0;
}

int fd_write_buf(int fd,void *buf,int len)
{
	int left,writted; 
	char *wp; 
	int err;
	wp=buf; 
	left=len; 
	while(left>0) 
	{ 
		/* ��ʼд*/ 
		writted=write(fd,wp,left); 
		//printf("KKK fd_write_buf ret =%d,left=%d\n",writted,left);
		if(writted<=0) /* ������*/ 
		{ 
			err=errno;
			if(err==EINTR) /* �жϴ��� ���Ǽ���д*/ 
				writted=0; 
			else if(err==EPIPE)	//�������ӳ�����
			{
				return -EPIPE;
			}
			else
				return(0-err);/* �������� û�а취*/ 
	 	} 
		left-=writted; 
		wp+=writted; /* ��ʣ�µĵط�����д */ 
	} 
	return(writted);
}

/*   
	��һ���ļ������������ȡָ�����ȵ�����
*/
int fd_read_buf(int fd,void *buf,int len) 
{ 
	int left; 
	int ret; 
	char *rp; 

	left=len; 
	rp=buf;
	while(left>0) 
	{ 
		ret=read(fd,rp,left); 
		if(ret<0) 
		{ 
			if(errno==EINTR) 
				ret=0; 
			else if(errno==ETIMEDOUT)
				return (0-ETIMEDOUT);
			else if(errno==EHOSTUNREACH)
				return (0-EHOSTUNREACH);
			else
			{
				#if 0
				if(errno==EAGAIN)
				{
					printf("common lib fd_read_buf EAGAIN !!!!!\n");
					printf("buf have %d    \n",(len-left));
					rp=(char*)buf;
					for(ret=0;ret<(len-left);ret++)
						printf("0x%02x ",rp[ret]);
					printf("\n");
				}
				#endif
				return(0-errno); 
			}
		} 
		else if(ret==0) 
			//break;fixbug ����ڶ���n���ֽں�Է�close���޷���while����
			return 0;
		left-=ret; 
		rp+=ret; 
	} 
	return(len-left); 
}
#if 0
/** 
 *   @brief     ��ָ���ĵ�ַ�Ͷ˿��ϴ���tcp����socket
 *   @param  svr_addr ���������ĵ�ַ INADDR_ANY��ʾ�����е�ַ����
 *   @param  port   �����Ķ˿ں�
 *   @return   ��ֵ��ʾ������socket������,��ֵ��ʾʧ��
 */ 
#include <netinet/in.h>
int create_tcp_listen_port(unsigned long svr_addr,int port)
{
    int                         fd;
    struct sockaddr_in svr;
    fd=socket(AF_INET,SOCK_STREAM,0);
    if(fd<0)
    {
        printf("can't create socket:%s!\n",strerror(errno));
        return -errno;
    }
    
///�����������ֹ��,���������Եڶ��ο������������õȴ�һ��ʱ��
 	net_activate_reuseaddr(fd);
	
	bzero(& svr, sizeof(struct sockaddr_in));
 	svr.sin_family=AF_INET; 
 	svr.sin_port=htons(port); 
 	svr.sin_addr.s_addr=htonl(svr_addr); 

 	if(bind(fd,(struct sockaddr *)&svr,sizeof(svr))<0) 
  	{   	    
	        printf("Bind Error:%s\n\a",strerror(errno)); 	
               close(fd);
	        return -errno;
  	} 
       return fd;
}

int net_set_recv_timeout(int fd,int second)
{
	struct timeval timeout;
	timeout.tv_sec=second;
	timeout.tv_usec=0;
	return setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

}
int net_set_snd_timeout(int fd,int second)
{
	struct timeval timeout;
	timeout.tv_sec=second;
	timeout.tv_usec=0;
	return setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));

}
int	net_activate_keepalive(int fd)
{
  int keepalive = 1;
  return setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &keepalive, sizeof(keepalive));
}

int net_activate_reuseaddr(int fd)
{
  int reuseaddr = 1;
  return setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr,sizeof(reuseaddr));
}

int net_set_nodelay(int fd)
{
  int nodelay = 1;
  return setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &nodelay, sizeof(nodelay));
}
#if EMBEDED==0
int net_set_quickack(int fd)
{
	int ack=1;
	return setsockopt(fd,IPPROTO_TCP,TCP_QUICKACK,&ack,sizeof(ack));
}
#endif


int net_activate_oobinline(int fd)
{
  int oob_inline = 1;
 return setsockopt(fd, SOL_SOCKET, SO_OOBINLINE, &oob_inline, sizeof(oob_inline));
}

int net_set_iptos_throughput(int fd)
{
  int tos = IPTOS_THROUGHPUT;
  /* Ignore failure to set (maybe this IP stack demands privilege for this) */
  return setsockopt(fd, IPPROTO_IP, IP_TOS, &tos, sizeof(tos));
}

int net_set_linger(int fd,int en)
{
  	int retval;
  	struct linger the_linger;
	if(en)
	{
  		the_linger.l_onoff = 1;
  		the_linger.l_linger = INT_MAX;
	}
	else
	{
		the_linger.l_onoff = 0;
  		the_linger.l_linger = 0;
	}
  	return setsockopt(fd, SOL_SOCKET, SO_LINGER, &the_linger,sizeof(the_linger));
}

int net_set_noblock(int fd,int en)
{
  	int curr_flags = fcntl(fd, F_GETFL);
  	if (curr_flags<0)
  	{
  	  	return -1;
  	}
	if(en)
	{
  		curr_flags |= O_NONBLOCK;
	}
	else
	{
		curr_flags &= ~O_NONBLOCK;
	}
  	return fcntl(fd, F_SETFL, curr_flags);
  
}

#if 0
int net_set_tcp_keepalive_time(int fd,int second)
{//ʵ���ϵ��� time+interval*probes �Ż����
	int file;
	char buf[16];
	int time,probes,interval;
	probes=3;
	interval=10;
	time=second;
	file=open("/proc/sys/net/ipv4/tcp_keepalive_time",O_WRONLY|O_TRUNC|O_CREAT);
	if(file<0)
		return -1;
	sprintf(buf,"%d\n",time);
	write(file,buf,strlen(buf));
	close(file);
	file=open("/proc/sys/net/ipv4/tcp_keepalive_probes",O_WRONLY|O_TRUNC|O_CREAT);
	if(file<0)
		return -1;
	sprintf(buf,"%d\n",probes);
	write(file,buf,strlen(buf));
	close(file);

	file=open("/proc/sys/net/ipv4/tcp_keepalive_intvl",O_WRONLY|O_TRUNC|O_CREAT);
	if(file<0)
		return -1;
	sprintf(buf,"%d\n",interval);
	write(file,buf,strlen(buf));
	close(file);
	return 0;
	
}
#endif
int net_set_tcp_sendbuf_len(int fd,int size)
{
	int bufsize;
	int ret;
	if((fd<0)||(size<0))
		return -EINVAL;
	bufsize=size/2;
	//lensize=sizeof(bufsize);
	ret= setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &bufsize, sizeof(bufsize));	
	if(ret!=0)
		return -errno;
	return 0;
}
int net_get_tcp_sendbuf_len(int fd)
{
	int bufsize,lensize;
	int rc;
	if(fd<0)
		return -EINVAL;
	lensize=sizeof(bufsize);
	rc=getsockopt(fd, SOL_SOCKET, SO_SNDBUF, &bufsize, &lensize);
	if(rc==0)
		return bufsize;
	else
		return rc;
}
int net_set_tcp_recvbuf_len(int fd,int size)
{
	int bufsize;
	if((fd<0)||(size<0))
		return -1;
	bufsize=size/2;
	return setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &bufsize, sizeof(bufsize));	
}
int net_get_tcp_recvbuf_len(int fd)
{
	int bufsize,lensize;
	int rc;
	if(fd<0)
		return -1;
	lensize=sizeof(bufsize);
	rc=getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &bufsize, &lensize);
	if(rc==0)
		return bufsize;
	else
		return rc;
}
//���÷��ͻ���������
int	net_set_sock_send_low(int fd,int size)
{
	int bufsize;
	if((fd<0)||(size<0))
		return -1;
	bufsize=size;
	return setsockopt(fd, SOL_SOCKET, SO_SNDLOWAT, &bufsize, sizeof(bufsize));	
}
int	net_get_sock_send_low(int fd)
{
	int bufsize,lensize;
	int rc;
	if(fd<0)
		return -1;
	lensize=sizeof(bufsize);
	rc=getsockopt(fd, SOL_SOCKET, SO_SNDLOWAT, &bufsize, &lensize);
	if(rc==0)
		return bufsize;
	else
		return rc;	
}
int	net_get_sock_recv_low(int fd)
{
	int bufsize,lensize;
	int rc;
	if(fd<0)
		return -1;
	lensize=sizeof(bufsize);
	rc=getsockopt(fd, SOL_SOCKET, SO_RCVLOWAT, &bufsize, &lensize);
	if(rc==0)
		return bufsize;
	else
		return rc;	
}
//���ý��ջ���������
int	net_set_sock_recv_low(int fd,int size)
{
	int bufsize;
	if((fd<0)||(size<0))
		return -1;
	bufsize=size;
	return setsockopt(fd, SOL_SOCKET, SO_RCVLOWAT, &bufsize, sizeof(bufsize));	
}

#include <unistd.h>
#include <sys/ioctl.h>
int get_fd_in_buffer_num(int fd)
{//��ȡ���ջ���������Ч����
	int ret;
	int cnt=100;
	ret=ioctl(fd,FIONREAD,&cnt);
	if(ret<0)
		return -errno;
	else
	{
		return cnt;
	}
}
int	get_fd_out_buffer_num(int fd)
{//��ȡ���ͻ�������δ���ͳ���������
	int ret;
	int cnt=0;
	ret=ioctl(fd,TIOCOUTQ  ,&cnt);
	if(ret<0)
		return -1;
	else
		return cnt;
}




//�����ļ���������ip��ַ�ַ���
//�����ȡʧ���򷵻�NULL
char *GetPeerIpStr(int Fd)
{
	int Ret;
	struct sockaddr_in RmtAddr;
	int AddrLen=sizeof(struct sockaddr);
	
	Ret=getpeername(Fd,(struct sockaddr *)&RmtAddr,&AddrLen);
	if(Ret==0)
		return inet_ntoa(RmtAddr.sin_addr);
	else
		return NULL;
}


//����ʱ��connect
int  connect_timeout(int  fd,  const  struct sockaddr *serv_addr, socklen_t    addrlen,int timeout)
{
	int rc,ret;
	int sockerr;
	socklen_t sklen;
	fd_set writefds;
	int errtimes=0;
	struct timeval	timeval;
	if((fd<0)||(serv_addr==NULL))
		return -1;
	rc=net_set_noblock(fd,1);
	if(rc<0)
		{}//
	FD_ZERO(&writefds);
	rc=-1;
	do{		
		rc=connect(fd,serv_addr,addrlen);
		if(rc<0)
		{
			if(errno==EINPROGRESS)
			{
				while(1)
				{
				//printf("���ڽ�������,timeout=%d\n",timeout);
				timeval.tv_sec=timeout;
				timeval.tv_usec=0;
				FD_SET(fd,&writefds);
				ret=select(fd+1,NULL,&writefds,NULL,&timeval);
				if(ret==0)
				{
					rc=-1;
					//printf("���ӳ�ʱ\n");
					break;
				}
				if(FD_ISSET(fd,&writefds))
				{
					sockerr=-1;
					sklen=sizeof(int);
					getsockopt(fd, SOL_SOCKET, SO_ERROR,(void*)&sockerr, &sklen);
					//printf("�յ� fd�����ź�sockerr=%d %s\n",sockerr,strerror(sockerr));
					if(sockerr==0)
					{
						rc=0;
						//printf("���ӳɹ�!!!!!!!!!!!!!!!\n");
						break;
					}
					else
					{
						if(++errtimes>5)
							break;
					}
					continue;
				}		
				else
				{
					rc=-1;
					break;
				}
				}
			}
			//else
			//	printf("���Ӵ���%d\n",errno);
		}
	}while(0);
	net_set_noblock(fd,0);
	return rc;
}

int tcp_connect_addr(const char *addr_str,int port,int timeout)
{
	int	rmt_sock=-1;
	int	ret=-1;
	char *remoteHost = NULL;
	struct hostent *hostPtr = NULL;
	 struct sockaddr_in serverName = { 0 };
	if((addr_str==NULL)||(port<0)||(timeout<0))
		return -EINVAL;

	hostPtr = gethostbyname(addr_str); /* struct hostent *hostPtr. */ 
    	if (NULL == hostPtr)
    	{
       	 hostPtr = gethostbyaddr(addr_str, strlen(remoteHost), AF_INET);
        	if (NULL == hostPtr) 
        	{
        		printf("Error resolving server address:%s\n",addr_str);
			if(errno>0)
        			return -errno;
			else
				return -errno;
        	}			
    	}
	rmt_sock=socket(PF_INET, SOCK_STREAM,IPPROTO_TCP);
	if(rmt_sock<0)
	{
		printf("create socket err! rmt_sock=%d errno=%d:%s\n",rmt_sock,errno,strerror(errno));
		return -errno;
	}
	serverName.sin_family = AF_INET;
    	serverName.sin_port = htons(port);
    	(void) memcpy(&serverName.sin_addr,hostPtr->h_addr,hostPtr->h_length);
    	ret = connect_timeout(rmt_sock,(struct sockaddr*) &serverName,sizeof(serverName),timeout);
    	if (ret<0)
    	{
    		close(rmt_sock);
        	return -errno;
   	}
	return rmt_sock;	
		
}
#endif


#include <sys/types.h>
#include <sys/stat.h>
//��ȡ��ִ���ļ������Ƽ�·�������ExecName��
//ExecName�������ĳ���Ӧ���㹻��,��100�ֽ�
int GetExecName(char *ExecName,int BufLen)
{
	int Ret;
	pid_t Pid;
	char filename[100];
	struct stat fbuf;
	if(ExecName==NULL)
		return -EINVAL;
	Pid=getpid();
	sprintf(filename,"/proc/%d/exe",(int)Pid);
	Ret=access(filename,F_OK);
	if(Ret<0)
		return -ENOENT;
	Ret=lstat(filename,&fbuf);
	if(Ret<0)
		return -1;
	if(!S_ISLNK(fbuf.st_mode))
		return -EINVAL;
	if((fbuf.st_size+1)>BufLen)
		return -EINVAL;
	Ret=readlink(filename,ExecName,BufLen);
	if(Ret<=0)//!=fbuf.st_size)
	{
		//printf("readlink Ret=%d fbuf.st_size=%d!!\n",Ret,(int)fbuf.st_size);
		return -1;
	}
	ExecName[Ret]='\0';
	return 0;
	
}


int clr_option_ctrl_char(struct termios *options)
{
	options->c_cc[VINTR]    = 0;     /* Ctrl-c */ 
	options->c_cc[VQUIT]    = 0;     /* Ctrl-\ */
	options->c_cc[VERASE]   = 0;     /* del */
	options->c_cc[VKILL]    = 0;     /* @ */
	options->c_cc[VEOF]     = 4;     /* Ctrl-d */
	options->c_cc[VTIME]    = 0;     /* inter-character timer unused */
	                               /* ��ʹ���ַ���ļ�ʱ�� */
	options->c_cc[VSWTC]    = 0;     /* '\0' */
	options->c_cc[VSTART]   = 0;     /* Ctrl-q */ 
	options->c_cc[VSTOP]    = 0;     /* Ctrl-s */
	options->c_cc[VSUSP]    = 0;     /* Ctrl-z */
	options->c_cc[VEOL]     = 0;     /* '\0' */
	options->c_cc[VREPRINT] = 0;     /* Ctrl-r */
	options->c_cc[VDISCARD] = 0;     /* Ctrl-u */
	options->c_cc[VWERASE]  = 0;     /* Ctrl-w */
	options->c_cc[VLNEXT]   = 0;     /* Ctrl-v */
	options->c_cc[VEOL2]    = 0;     /* '\0' */
	memset(&options->c_cc,0,sizeof(options->c_cc));
	return 0;
}
//���ô���
int set_com_mode(int fd,int databits,int stopbits,int parity)
{ 
	struct termios options; 
	int ret;
#ifdef SHOW_WORK_INFO
	printf("���ô��ڣ��ļ�������%d,���ò���%d,%d,%c\n",fd,databits,stopbits,parity);
#endif
	gtloginfo("���ô��ڣ��ļ�������%d,���ò���%d,%d,%c\n",fd,databits,stopbits,parity);
	ret=tcgetattr( fd,&options);
	if  ( ret  !=  0) { 
		printf("SetupSerial 1 error!!\n"); 
		gtlogerr("�����������Ӵ����ļ�������%d,����ֵ%d\n",fd,ret);
		return(-1);  
	}
	//memset((void*)&options,0,sizeof(options));//added by shixin
	options.c_lflag=0;	//added by shixin
	options.c_iflag=0;	//added by shixin 
	options.c_cflag &= ~CSIZE; 
	switch (databits) /*......*/
	{   
	case 7:		
		options.c_cflag |= CS7; 
		break;
	case 8:     
		options.c_cflag |= CS8;
		break;   
	default:    
		fprintf(stderr,"Unsupported data size:%d set to default 8\n",databits); 
		gtlogwarn("���ô����в�֧�ֵ����ݴ�С%d����Ϊȱʡֵ8\n",databits);
		options.c_cflag |= CS8;
	break;
	}
    switch (parity) 
    {   
	case 'n':
	case 'N':    
		options.c_cflag &= ~PARENB;   /* Clear parity enable */
		options.c_iflag &= ~INPCK;     /* Enable parity checking */ 
		parity='n';
		break;  
	case 'o':   
	case 'O':     
		options.c_cflag |= (PARODD | PARENB); /* ......*/  
		options.c_iflag |= INPCK;             /* Disnable parity checking */ 
		break;  
	case 'e':  
	case 'E':   
		options.c_cflag |= PARENB;     /* Enable parity */    
		options.c_cflag &= ~PARODD;   /* ......*/     
		options.c_iflag |= INPCK;       /* Disnable parity checking */
		break;
	case 'S': 
	case 's':  /*as no parity*/   
	    	options.c_cflag &= ~PARENB;
		options.c_cflag &= ~CSTOPB;
	break;  
	default:   
		fprintf(stderr,"Unsupported parity set to default\n");  
		gtlogwarn("���ô����в�֧�ֵ�У��λ%d����Ϊȱʡֵ'N'\n",parity);
		options.c_cflag &= ~PARENB;   /* Clear parity enable */
		options.c_iflag &= ~INPCK;     /* Enable parity checking */ 
	break;
    }  
/* .....*/  
    switch (stopbits)
    {   
	case 1:    
		options.c_cflag &= ~CSTOPB;  
		break;  
	case 2:    
		options.c_cflag |= CSTOPB;  
	   break;
	default:    
		 fprintf(stderr,"Unsupported stop bits set to default 1\n"); 
		 gtlogwarn("���ô����в�֧�ֵ�ֹͣλ%d����Ϊȱʡֵ1\n",stopbits);
		 options.c_cflag &= ~CSTOPB;  
	break; 
    } 
/* Set input parity option */ 
    if (parity != 'n')   
	options.c_iflag |= INPCK; 

	
    tcflush(fd,TCIFLUSH);

    clr_option_ctrl_char(&options);
    options.c_cc[VTIME] = 150; /* ....15 seconds*/   
    options.c_cc[VMIN] = 0; /* Update the options and do it NOW */
	
    options.c_lflag  &= ~(ICANON|ECHO |ECHO| ISIG);  /*Input*///Enot term
    options.c_oflag  &= ~OPOST;   /*Output*/




    if (tcsetattr(fd,TCSANOW,&options) != 0)   
    { 
	perror("SetupSerial port");
	gtlogerr("���ô���ʱtcsetattrʧ��,�ļ�������%d\n",fd);
	return (-1);  
    } 
    return (0);  
}

static int speed_arr[] = { B115200, B57600, B38400, B19200, B9600, B4800, B2400, B1200, B300};
static int name_arr[] =  { 115200,  57600,  38400,  19200,  9600,  4800,  2400,  1200,  300 };
//���ô��ڵĲ�����
int set_com_speed(int fd, int speed)
{
	int   i; 
	int   status; 
	struct termios   Opt;
#ifdef SHOW_WORK_INFO
	printf("���ô���(fd=%d)������%d\n",fd,speed);
#endif
	gtloginfo("���ô���(fd=%d)������%d\n",fd,speed);
	tcgetattr(fd, &Opt); 
	for ( i= 0;  i < sizeof(speed_arr) / sizeof(int);  i++) 
	{ 
		if  (speed == name_arr[i]) 
		{     
			tcflush(fd, TCIOFLUSH);     
			cfsetispeed(&Opt, speed_arr[i]);  
			cfsetospeed(&Opt, speed_arr[i]);   
			status = tcsetattr(fd, TCSANOW, &Opt);  
			if  (status != 0) 
			{        
				printf("tcsetattr fd1");  
				return -errno;     
			}    
			tcflush(fd,TCIOFLUSH);   
			return 0;
		}  
	}
	return -EINVAL;
	
}

////��ȡ���̵İ汾��

int get_prog_version(char *buf,char *lockfile)
{
	char buffer[200];
	int i;
	FILE *fp;
	char *pt;
	char *ps;
	int rc;
	if(buf==NULL)
		return -1;
	if(lockfile==NULL)
		return -1;
	memcpy(buf,"0",2);
	fp=fopen(lockfile,"r");
	if(fp==NULL)
		return -1;
	rc=-1;
	for(i=0;i<10;i++)
	{
		pt=fgets(buffer,sizeof(buffer),fp);
		if(pt==NULL)
		{
			rc=-1;
			break;
		}
		ps=strstr(pt,(char*)"version:");
		if(ps==NULL)
			continue;
		pt=strchr(ps,(int)':');
		if(pt==NULL)
		{
			rc=-1;
			break;
		}		pt++;
		if(strlen(pt)>20)
		{
			rc=-1;
			break;
		}		
		memcpy(buf,pt,strlen(pt)+1);
		if((buf[strlen(pt)-1]=='\r')||(buf[strlen(pt)-1]=='\n'))
			buf[strlen(pt)-1]='\0';

//		fclose(fp);    //add by wsy
		rc=0;
		break;
	}
NOTFOUND:
	fclose(fp);
	return rc;
	
}

#define KVERSION_FILE	"/proc/version"

//��ȡ�ں˰汾���ַ���
char *get_kernel_version(void)
{
	static char kverbuf[20];
	static char *kver=NULL;
	static char tempbuf[256];
	FILE *kf=NULL;

	if(kver==NULL)
	{
		sprintf(kverbuf,"2.4");
		kf=fopen(KVERSION_FILE,"r");
		if(kf!=NULL)
		{
			//tbuf[0]='\0';
			fgets(tempbuf,sizeof(tempbuf),kf);
			if(strstr(tempbuf,"2.6.")!=NULL)
				sprintf(kverbuf,"2.6");
			fclose(kf);
			kver=kverbuf;
		}
	}
	return kver;
}

int lock_file(int fd,int wait)
{//����ָ�����������ļ�,wait=0��ʾ����޷������򷵻ش���
  //wait=1��ʾ����޷�������ȴ�
	int ret;
  	if(fd<0)
		return -EINVAL;
	if(wait)
		ret=flock(fd,LOCK_EX);
	else
		ret=flock(fd,LOCK_EX|LOCK_NB);
	if(ret<0)
		return -errno;
	else
	{
		fsync(fd);
		return ret;
	}
}
int unlock_file(int fd)
{//����ļ�������

	if(fd<0)
		return -EINVAL;
	flock(fd,LOCK_UN);
	fsync(fd);
	return 0;
}





//���ַ���ת��Ϊ16��������
DWORD atohex(char *buffer)
{
	int i,len;
	DWORD hex=0;
	char *p;
	DWORD ret;
	char ch;
	char buf[12];
	if(buffer==NULL)
		return 0;
	memcpy(buf,buffer,sizeof(buf));
	p=strrchr(buf,'x');//�ҵ����һ��x��λ��
	if(p==NULL)
	{
		p=strrchr(buf,'X');
	}	
	if(p==NULL)
		p=buf;
	else
		p++;
	len=strlen(p);

	if(len>8)
	{
		i=len-8;
		p+=i;
		len=strlen(p);	
	}
	p+=len;
	p--;
	for(i=0;i<len;i++)
	{
		ch=(char)toupper((int)*p);
		*p=ch;
		if(isdigit(*p))
		{
			ret=*p-'0';
		}
		else //��ĸ
		{
			//if(!isupper(*p))
			*p=(char)toupper((int)*p);
			ret=*p-'A'+10;
		}
		hex|=ret<<(i*4);
		p--;
	}
	return hex;	
	
}


//add by wsy@June 2006
/**********************************************************************************************
 * ������	:gttime2tm()
 * ����	:��gt_time_structתΪtm�ṹ�ĺ���
 * ����	:gttime:ָ��gt_time_struct�ṹ��ָ��
 *			:time:Ҫ�����ָ��tm�ṹ��ָ��
 * ����ֵ	:0��ʾ�ɹ�����ֵ��ʾ����
 **********************************************************************************************/
int gttime2tm (struct gt_time_struct *gttime, struct tm *time)
{	
	time->tm_year =	gttime->year-1900;
	time->tm_mon  =	gttime->month-1;
	time->tm_mday =	gttime->day;
	time->tm_hour =	gttime->hour;
	time->tm_min  =	gttime->minute;
	time->tm_sec  =	gttime->second;
	return 0;
}



/**********************************************************************************************
 * ������	:set_dev_time()
 * ����	:�����豸��ʱ��
 * ����	:ntime:ָ��Ҫ���õ�ʱ��ṹ
 * ����ֵ	:0��ʾ�ɹ�����ֵ��ʾ����
 **********************************************************************************************/
int set_dev_time(struct tm *ntime)
{
	return 0;
}


/**********************************************************************************************
 * ������	:get_percent()
 * ����	:���ݱ�����ֵ�ͷ�Χ�����������Ӧ�İٷֱ�ֵ
 * ����	:min:�����������Сֵ
 *			 max:������������ֵ
 *			 val:����ֵ
 * ����ֵ	:�ñ�����Ӧ�İٷֱ�ֵ
 **********************************************************************************************/
int get_percent(int min,int max,int val)
{
	int total;
	int cur;
	int percent;
	total=max-min+1;
	if(total<=0)
		return 50;
	cur=val-min;
	percent=(cur*100)/total;
	return percent;
}

/**********************************************************************************************
 * ������	:get_percent()
 * ����	:���ݱ����ķ�Χ�Ͱٷֱȼ����������ֵ
 * ����	:min:�����������Сֵ
 *			 max:������������ֵ
 *			 percent:�����İٷֱ�ֵ
 * ����ֵ	:�ñ�����ʵ��ֵ
 **********************************************************************************************/
int get_value(int min,int max,int percent)
{
	int total;
	int val;
	//total=max-min+1;
	total = max-min; //wsyfixed
	if(total<=0)
		return 50;//FIXME Ҫ�Ƿ�Χ�����򷵻�һ��Ĭ��ֵ
	if(percent<0)
		percent=0;
	if(percent>100)
		percent=50;//�������100% �����ó�50
	val=(percent*total)/100;
	val+=min;
	return val;
}
#if 0
////added by lsk 2006 -11-7
/**********************************************************************************************
 * ������	:force_lockfile()
 * ����	:ǿ�Ƽ����ļ�
 * ���� ��fd �ļ������� 
 *	  cmd ����
 *	  wait �ȴ���־
 *	 cmd = F_RDLCK ����ֹ �� F_WRLCK д��ֹ; F_UNLCK �������
 *	 wait = 0 �޷��������������أ� =1 �ȴ�����
 * ����ֵ	:0 �ɹ� -1 ʧ��
 **********************************************************************************************/
//ǿ�Ƽ����ļ�  
//���� fd �ļ������� ,cmd ����, wait �ȴ���־
//����0 �ɹ� -1 ʧ��
// cmd = F_RDLCK ����ֹ �� F_WRLCK д��ֹ; F_UNLCK �������
// wait = 0 �޷��������������أ� =1 �ȴ�����
int force_lockfile(int fd, short int cmd, int wait)
{	
	struct flock tp;
	tp.l_len = 0;
	tp.l_type = cmd;
	tp.l_whence = SEEK_SET; 
	tp.l_start = 0;
	tp.l_pid = getpid();
	if(wait == 0)
	return fcntl(fd, F_SETLK,&tp);

	if(wait ==1)
	return fcntl(fd, F_SETLKW,&tp);

	return fcntl(fd, F_SETLKW,&tp);
}
#endif


/**********************************************************************************************
 * ������	:show_time
 * ����	:	��ӡ��ǰʱ��
 * ����	:	infostr:Ҫ��ʾ��ʱ��֮��ĵ�����Ϣ�ַ���,����Ϊ��
 * ����ֵ	:0��ʾ�ɹ�,��ֵ��ʾ����
 **********************************************************************************************/

int show_time(char *infostr)
{
	struct timeval tv;
	struct tm *ptime;
	char pbuf[60];
	time_t ctime;
	if(gettimeofday(&tv,NULL)<0)
	{
		return -1;
	}
	ctime=tv.tv_sec;
	ptime=localtime(&ctime);
	if(ptime!=NULL)
	{
		if(infostr == NULL)
			sprintf(pbuf,"%d-%d-%d %d:%d:%d.%03d\n",ptime->tm_year+1900,ptime->tm_mon+1,ptime->tm_mday,ptime->tm_hour,ptime->tm_min,ptime->tm_sec,(int)tv.tv_usec/1000);	
		else
			sprintf(pbuf,"%d-%d-%d %d:%d:%d.%03d --%s\n",ptime->tm_year+1900,ptime->tm_mon+1,ptime->tm_mday,ptime->tm_hour,ptime->tm_min,ptime->tm_sec,(int)tv.tv_usec/1000,infostr);	
		printf("%s",pbuf);
	}
	return 0;
	
}

