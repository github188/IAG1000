///////用io口控制外设的驱动api，目前包括继电器,看门狗,蜂鸣器

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
	 	
static int higpio_fd=-1;		//初始化文件描述符
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
 * 函数名	:init_vs3iodrv()
 * 功能	:	打开并初始化由io管脚控制的看门狗,继电器,蜂鸣器设备
 * 输入	:	
 * 返回值	:0表示成功负值表示失败
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
 * 函数名       :clear_watchdog_cnt()
 * 功能 :       对看门狗的计数器进行清零
 * 输入 :       
 * 返回值       :0表示成功负值表示失败
 **********************************************************************************************/
/*
int clear_watchdog_cnt(void)
{
	if(vs3io_fd<0)
	{
		printf("未初始化vs3iodrv.\n");
		return -1;
	}
	//printf("用户喂狗\n");
        return ioctl(vs3io_fd,CLR_WD,NULL);
}
*/

/**********************************************************************************************
 * 函数名	:set_relay_output()
 * 功能	:	向继电器输出信号
 * 输入	:	ch:继电器的通道号,从0开始
 *			value:	1表示继电器动作，
 *					0表示继电器恢复
 *
 * 返回值	:0表示成功负值表示失败
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
		printf("未初始化gpiodrv.\n");
		return -1;
	}

	return ioctl(higpio_fd,GPIO_SET_HEARTBEAT,&level);
}
/**********************************************************************************************
 * 函数名	:read_trigin_status()
 * 功能	:	读取所有输入端子的当前触发状态,
 *			标准read接口，有数据才返回，
 *			(除了打开设备后的第一次外，每次数据有变化才从此接口返回)
 * 输出	:	status,DWORD类型指针,其指向的DWORD数值从低到高位表示端子
 *				从0到32位的状态，已做过延时处理
 *			1表示该位有触发，0表示没有
 * 返回值:  非负值表示读到的字节数，负值表示失败			
 * **********************************************************************************************/
 int  read_trigin_status(OUT DWORD *status)
{
    //ipcall to do get io info
	if(read(gpio_fd, status, 1)!=1)
	{
		printf("Error readding from gpio:%s\n",strerror(errno));	
	
	}
	
	
    return 0;                      //驱动里面来的
}

/**************************************************************************
 * 函数名	:set_trigin_attrib_perbit()
 * 功能	:设置设置端子输入属性
 * 输入	:attrib:按位表示的端子输入属性值 1表示常闭 0表示常开,缺省为全0
 * 返回值	:0表示成功，负值表示失败
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
 * 函数名	:set_trigin_delay()
 * 功能	:	设定输入端子的延时处理时间
 		(没有设定的话，对所有端子用缺省值DEF_VALID_DELAY, DEF_INVALID_DELAY)
 * 输入	:	valid_delay: 该端子从恢复到触发的延时秒数
 			invalid_delay:该端子从触发到恢复的延时秒数	
 * 返回值	:0表示成功负值表示失败
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
 * 函数名	:send_require_reset()
 * 功能	:	发出重起命令，驱动应停止喂狗，在一段时间后设备会重起
 * 输入	:	
 * 返回值	:0表示成功负值表示失败
 **********************************************************************************************/
/*
int send_require_reset(void)
{
	if(vs3io_fd<0)
	{
		printf("未初始化.\n");
		return -EINVAL;
	}

	gtloginfo("用户发出了重起命令,send_require_reset\n");
	return ioctl(vs3io_fd,RESET_CMD,NULL);
}
*/

/**************************************************************************
 * 函数名	:set_beep()
 * 功能	响蜂鸣器
 * 输入	:响的次数 
 * 返回值	:0表示成功，负值表示失败
*************************************************************************/
/*
int set_beep(int cnt)
{
	if(vs3io_fd<0)
	{
		printf("未初始化.\n");
		return -EINVAL;
	}

	return ioctl(vs3io_fd,BEEP,&cnt);

}
*/


/**********************************************************************************************
 * 函数名	:exit_gpiodrv()
 * 功能	:	打开并初始化由io管脚控制的看门狗,继电器,蜂鸣器设备
 * 输入	:	
 * 返回值	:0表示成功负值表示失败
 **********************************************************************************************/
int	exit_gpiodrv(void)
{
	if(higpio_fd<0)
        {
                printf("未初始化.\n");
                return -EINVAL;
        }

	return close(higpio_fd);
}
