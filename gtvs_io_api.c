///////��io�ڿ������������api��Ŀǰ�����̵���,���Ź�,������

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <gtlog.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include"gtvs_io_api.h"
#include "devinfo.h"
	 	
static int higpio_fd=-1;		//��ʼ���ļ�������

/**********************************************************************************************
 * ������	:init_vs3iodrv()
 * ����	:	�򿪲���ʼ����io�ܽſ��ƵĿ��Ź�,�̵���,�������豸
 * ����	:	
 * ����ֵ	:0��ʾ�ɹ���ֵ��ʾʧ��
 **********************************************************************************************/

int	init_gpiodrv()
{
	higpio_fd=open("/dev/hi_gpio",O_RDWR);
	if(higpio_fd<0)
	{
		printf("can't open gpio device !\n");
		return -1;
	}
	else
	{
		//#ifdef SHOW_WORK_INFO
		printf("open gpio device success=%d.\n",higpio_fd);
		//#endif
	}
	//lc to do Ŀǰ�����������������	
	return 0;
}


/**********************************************************************************************
 * ������       :clear_watchdog_cnt()
 * ���� :       �Կ��Ź��ļ�������������
 * ���� :       
 * ����ֵ       :0��ʾ�ɹ���ֵ��ʾʧ��
 **********************************************************************************************/
/*
int clear_watchdog_cnt(void)
{
	if(vs3io_fd<0)
	{
		printf("δ��ʼ��vs3iodrv.\n");
		return -1;
	}
	//printf("�û�ι��\n");
        return ioctl(vs3io_fd,CLR_WD,NULL);
}
*/

/**********************************************************************************************
 * ������	:set_relay_output()
 * ����	:	��̵�������ź�
 * ����	:	ch:�̵�����ͨ����,��0��ʼ
 *			result:	1��ʾ�̵���������
 *					0��ʾ�̵����ָ�
 *
 * ����ֵ	:0��ʾ�ɹ���ֵ��ʾʧ��
 **********************************************************************************************/

int set_relay_output(int ch, int result)
{

	struct relay_struct rly_info;

	if(higpio_fd<0)
	{
		printf("δ��ʼ��gpiodrv.\n");
		return -1;
	}

	rly_info.ch=ch;
	rly_info.result=result;

	return ioctl(higpio_fd,GPIO_SET_OUTPUT_VALUE,&rly_info);
}

int set_heart_beat(int level)
{
	if(higpio_fd<0)
	{
		printf("δ��ʼ��gpiodrv.\n");
		return -1;
	}

	return ioctl(higpio_fd,GPIO_SET_HEARTBEAT,&level);
}
/**********************************************************************************************
 * ������	:read_trigin_status()
 * ����	:	��ȡ����������ӵĵ�ǰ����״̬,
 *			��׼read�ӿڣ������ݲŷ��أ�
 *			(���˴��豸��ĵ�һ���⣬ÿ�������б仯�ŴӴ˽ӿڷ���)
 * ���	:	status,DWORD����ָ��,��ָ���DWORD��ֵ�ӵ͵���λ��ʾ����
 *				��0��32λ��״̬����������ʱ����
 *			1��ʾ��λ�д�����0��ʾû��
 * ����ֵ:  �Ǹ�ֵ��ʾ�������ֽ�������ֵ��ʾʧ��			
 * **********************************************************************************************/
 int  read_trigin_status(OUT DWORD *status)
{
    //ipcall to do get io info
	
    return 0;                      //������������
}

/**************************************************************************
 * ������	:set_trigin_attrib_perbit()
 * ����	:�������ö�����������
 * ����	:attrib:��λ��ʾ�Ķ�����������ֵ 1��ʾ���� 0��ʾ����,ȱʡΪȫ0
 * ����ֵ	:0��ʾ�ɹ�����ֵ��ʾʧ��
*************************************************************************/
int set_trigin_attrib_perbit(DWORD attrib)
{
	if (ioctl(higpio_fd, GPIO_SET_INPUT, &attrib) < 0)
	{
		printf("ioctl gpio setinput failed!");
		close(higpio_fd);
		return (-1);
	}

	return 0;
}


/**********************************************************************************************
 * ������	:set_trigin_delay()
 * ����	:	�趨������ӵ���ʱ����ʱ��
 		(û���趨�Ļ��������ж�����ȱʡֵDEF_VALID_DELAY, DEF_INVALID_DELAY)
 * ����	:	valid_delay: �ö��Ӵӻָ�����������ʱ����
 			invalid_delay:�ö��ӴӴ������ָ�����ʱ����	
 * ����ֵ	:0��ʾ�ɹ���ֵ��ʾʧ��
 **********************************************************************************************/

int set_trigin_delay(int valid_delay, int invalid_delay)
{	
	TimeDelay timeDelay;
	timeDelay.on_delay = invalid_delay;
	timeDelay.off_delay = valid_delay;
	if (ioctl(higpio_fd, GPIO_SET_DELAY, &timeDelay) < 0)
	{
		printf("ioctl gpio setdelay failed!");
		close(higpio_fd);
		return (-1);
	}
}


/**********************************************************************************************
 * ������	:send_require_reset()
 * ����	:	���������������Ӧֹͣι������һ��ʱ����豸������
 * ����	:	
 * ����ֵ	:0��ʾ�ɹ���ֵ��ʾʧ��
 **********************************************************************************************/
/*
int send_require_reset(void)
{
	if(vs3io_fd<0)
	{
		printf("δ��ʼ��.\n");
		return -EINVAL;
	}

	gtloginfo("�û���������������,send_require_reset\n");
	return ioctl(vs3io_fd,RESET_CMD,NULL);
}
*/

/**************************************************************************
 * ������	:set_beep()
 * ����	�������
 * ����	:��Ĵ��� 
 * ����ֵ	:0��ʾ�ɹ�����ֵ��ʾʧ��
*************************************************************************/
/*
int set_beep(int cnt)
{
	if(vs3io_fd<0)
	{
		printf("δ��ʼ��.\n");
		return -EINVAL;
	}

	return ioctl(vs3io_fd,BEEP,&cnt);

}
*/


/**********************************************************************************************
 * ������	:exit_gpiodrv()
 * ����	:	�򿪲���ʼ����io�ܽſ��ƵĿ��Ź�,�̵���,�������豸
 * ����	:	
 * ����ֵ	:0��ʾ�ɹ���ֵ��ʾʧ��
 **********************************************************************************************/
int	exit_gpiodrv(void)
{
	if(higpio_fd<0)
        {
                printf("δ��ʼ��.\n");
                return -EINVAL;
        }

	return close(higpio_fd);
}
