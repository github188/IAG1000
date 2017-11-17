#ifdef __cplusplus
extern "C" {
#endif
#ifndef SUBDEVDEF_H
#define SUBDEVDEF_H


#include <devinfo.h>


typedef struct{                 //��Ƶ�����������豸��Ϣ�ṹ
        int             type;           //���豸����
        int             no;                     //���豸��ͬ���豸�е����
        char    *name;          //���豸��
        char    *node;          //���豸�ڵ�
        char    *driver;                //���豸��������
}DevType_T;
/************************��������豸����**************************************************************/
static DevType_T QuadDev={
		//����ָ���
		.type		=	SUB_DEV_QUAD,
		.no			=	0,
		.name		=	"tw2834",
		.node		=	"/dev/quaddev",
		.driver		=	NULL		   
	};
static DevType_T Ime6410VEnc0={
		//��0·��Ƶ������
		.type		=	SUB_DEV_VENC,
		.no			=	0,
		.name		=	"ime6410_pcm",
		.node		=	"/dev/IME6410",
		.driver		=	"/gt1000/drivers/ime6410_pcm.o"
	};
static DevType_T Ime6410VEnc1={
		//��1·��Ƶ������
              .type		=	SUB_DEV_VENC,
				.no		=	1,
              .name		=	"ime6410_d1",
              .node		=	"/dev/IME6410_D1",
              .driver	=	"/gt1000/drivers/ime6410_d1.o"
        };
static DevType_T IDEDisk={
		//����
		.type		=	SUB_DEV_IDE,
		.no			=	0,
		.name		=	"ide",
		.node		=	NULL,
		.driver		=	"/hqdata"
	};
static DevType_T AudioChip={
		//��Ƶ�����оƬ
		.type		=	SUB_DEV_ACDC,
		.no			=	0,
		.name		=	"uda1341",
		.node		=	"/dev/dsp",
		.driver		=	NULL
	};
static DevType_T UART0={
		.type		=	SUB_DEV_COM,
		.no		=	0,
		.name		=	"uart0",
		.node		=	"/dev/ttyS1",
		.driver		=	NULL
	};

static DevType_T UART1={
		//����1
                .type		=	SUB_DEV_COM,
		  		.no			=	1,
                .name		=	"uart1",
                .node		=	"/dev/ttyS2",
                .driver		=	NULL
        };
static DevType_T IPUART0={
		.type		=	SUB_DEV_COM,
		.no		=	0,
		.name		=	"ipuart0",
		.node		=	"/dev/ttyAMA1",
		.driver		=	NULL
	};

static DevType_T IPUART1={
		//����1
                .type		=	SUB_DEV_COM,
		  		.no			=	1,
                .name		=	"ipuart1",
                .node		=	"/dev/ttyAMA2",
                .driver		=	NULL
        };
static DevType_T NetPort0={
		//����0
		.type		=	SUB_DEV_NETPORT,
		.no		=	0,
		.name		=	"br0",
		.node		=	"br0",
		.driver		=	"NULL"
	};
static DevType_T NetPort1={
		//����1
                .type		=	SUB_DEV_NETPORT,
		.no		=	1,
                .name		=	"eth1",
                .node		=	"eth1",
                .driver		=	"NULL"
        };

/***************************************************************************************************************/

static DevType_T QuadDev_2835={
		//����ָ���
		.type		=	SUB_DEV_QUAD,
		.no			=	0,
		.name		=	"tw2835",
		.node		=	"/dev/quaddev",
		.driver		=	NULL		   
	};

#endif
#ifdef __cplusplus
}
#endif
