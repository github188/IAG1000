///////��io�ڿ������������api��Ŀǰ�����̵���,���Ź�,������

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

#include <gtlog.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include"gtvs_io_api.h"
#include "devinfo.h"
#include <linux/i2c-dev.h>
	 	
static int higpio_fd=-1;		//��ʼ���ļ�������
static int gpio_fd=-1;	 // input fd	
static int gpio_mmap_fd = -1; //mmap fd
static int gpio_mmap_reg = NULL;


static int gpio_mmap(void)
{
	if ((gpio_mmap_fd = open(MMAP_PATH, O_RDWR)) < 0) {
		fprintf(stderr, "unable to open mmap file");
		return -1;
    }
	gpio_mmap_reg = (uint8_t*) mmap(NULL, 1024, PROT_READ | PROT_WRITE,
	MAP_FILE | MAP_SHARED, gpio_mmap_fd, 0x10000000);
	if (gpio_mmap_reg == MAP_FAILED) {
		perror("foo");
		fprintf(stderr, "failed to mmap");
		gpio_mmap_reg = NULL;
		close(gpio_mmap_fd);
		return -1;
	}
   
    return 0;
}
int mt76x8_get_reg(int reg)
{
	uint32_t tmp = 0;
    tmp = *(volatile uint32_t *)(gpio_mmap_reg + reg);
    return tmp;
   
}
     
int mt76x8_set_reg(int reg,unsigned int data)
{    
	*(volatile uint32_t *)(gpio_mmap_reg + reg) = data;
    return 0;
}
	     
			     
void mt76x8_gpio_set_pin_direction(int pin, int is_output)
{
	uint32_t tmp;
	/* MT7621, MT7628 */
    if (pin <= 31) {
	    tmp = *(volatile uint32_t *)(gpio_mmap_reg + RALINK_REG_PIODIR);
        if (is_output)
	        tmp |=  (1u << pin);
        else
	        tmp &= ~(1u << pin);
        *(volatile uint32_t *)(gpio_mmap_reg + RALINK_REG_PIODIR) = tmp;
	} else if (pin <= 63) {
		tmp = *(volatile uint32_t *)(gpio_mmap_reg + RALINK_REG_PIO6332DIR);
	    if (is_output)
		    tmp |=  (1u << (pin-32));
	    else
		    tmp &= ~(1u << (pin-32));
	    *(volatile uint32_t *)(gpio_mmap_reg + RALINK_REG_PIO6332DIR) = tmp;
	} else if (pin <= 95) {
		tmp = *(volatile uint32_t *)(gpio_mmap_reg + RALINK_REG_PIO9564DIR);
	    if (is_output)
		    tmp |=  (1u << (pin-64));
	    else
	        tmp &= ~(1u << (pin-64));
	        *(volatile uint32_t *)(gpio_mmap_reg + RALINK_REG_PIO9564DIR) = tmp;
	}
}
			     
void mt76x8_gpio_set_pin_value(int pin, int value)
{
	uint32_t tmp;
   
    printf("pin=%d,value=%d\n",pin,value);
   /* MT7621, MT7628 */
	if (pin <= 31) {
		tmp = (1u << pin);
	if (value)
		*(volatile uint32_t *)(gpio_mmap_reg + RALINK_REG_PIOSET) = tmp;
	else
		*(volatile uint32_t *)(gpio_mmap_reg + RALINK_REG_PIORESET) = tmp;
	} else if (pin <= 63) {
		tmp = (1u << (pin-32));
	    if (value)
		    *(volatile uint32_t *)(gpio_mmap_reg + RALINK_REG_PIO6332SET) = tmp;
	    else
	        *(volatile uint32_t *)(gpio_mmap_reg + RALINK_REG_PIO6332RESET) = tmp;
	    } else if (pin <= 95) {
		tmp = (1u << (pin-64));
	    if (value)
		    *(volatile uint32_t *)(gpio_mmap_reg + RALINK_REG_PIO9564SET) = tmp;
	    else
	        *(volatile uint32_t *)(gpio_mmap_reg + RALINK_REG_PIO9564RESET) = tmp;
	    }
}



/**********************************************************************************************
 * ������	:init_vs3iodrv()
 * ����	:	�򿪲���ʼ����io�ܽſ��ƵĿ��Ź�,�̵���,�������豸
 * ����	:	
 * ����ֵ	:0��ʾ�ɹ���ֵ��ʾʧ��
 **********************************************************************************************/

int	init_gpiodrv()
{
	unsigned int data = 0;
	//init input
	gpio_fd = open("/dev/i2c-0",O_RDWR);
	if (gpio_fd < 0) 
	{
		printf("Error opening i2c-0: %s\n", strerror(errno));
		return 1;
	}
	if (ioctl(gpio_fd, I2C_SLAVE, I2C_ADDR) < 0)
	{
	
		printf("ioctl error: %s\n", strerror(errno));
		return 1;
	}
	//init output
	if(gpio_mmap())
		printf("IO mmap error\n");

	data = mt76x8_get_reg(I2S_MUX_REG);
	data|= (1u<<6);
	data =mt76x8_set_reg(I2S_MUX_REG, data);

	mt76x8_gpio_set_pin_direction(37, 1);	
	mt76x8_gpio_set_pin_direction(11, 1);	




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
 *			value:	1��ʾ�̵���������
 *					0��ʾ�̵����ָ�
 *
 * ����ֵ	:0��ʾ�ɹ���ֵ��ʾʧ��
 **********************************************************************************************/

int set_relay_output(int ch, int value)
{

	if( ch == 0 )	
		mt76x8_gpio_set_pin_value(37, value);

	else if (ch == 1)
		mt76x8_gpio_set_pin_value(11, value);

	else
	    printf("CH %d not support set output\n",ch);

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
	if(read(gpio_fd, status, 1)!=1)
	{
		printf("Error readding from gpio:%s\n",strerror(errno));	
	
	}
	
	
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
