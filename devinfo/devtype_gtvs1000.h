///GTVS1000ϵ����Ƶ�������ͺŶ���
#ifndef GTVS1000_DEV_TYPE_H
#define GTVS1000_DEV_TYPE_H
//ֻ�ܱ�devtype.c�ڰ����� include <devinfo.h>�Ͷ�����dev_name_t �����
/************************������豸�ͺŵ����豸�б�*************************************************************/
/*
	�����豸�����豸�б�ʱ��ͬ���͵Ķ���豸һ��Ҫ����˳���壬��Ime6410VEnc0һ��Ҫ��Ime6410VEnc1֮ǰ
	����һ���µ��豸�ͺ�ʱ��Ҫ�������ͺŵ�static DevType_T �ṹ�����static GTSeriesDVSR�ṹ��Դ���壬Ȼ��
	��static GTSeriesDVSR��ָ�����DEVSupportList����
*/

//GT1024
static DevType_T *GTVS1024_O_List[]=
{	//GT1024 ���豸�б�
	&UART0,
	&UART1,
	&QuadDev,
	&Ime6410VEnc0,
	&Ime6410VEnc1,
	&IDEDisk,
	&NetPort0
};

static GTSeriesDVSR     DEV_GTVS1024_O={
        .type		=	T_GTVS1024_O,      //int     type;      //#�豸�ͺ�
	 .comment	=	"������Ƶѹ��оƬ,������ָ���",
        .trignum		=	6,              //int     trignum;        	//#�豸��߱���������
	 .outnum		=	4,		   //		outnum;		//���������
        .com		=	2,              //int     com;            	//#�豸����������Ҫ�ڽ���������
        .quad		=	1,              //int     quad;           	//#�Ƿ��л���ָ�����1��ʾ�� 0��ʾû��
	 .osd		=	1,							//��osd
        .videonum	=	4,              //int     videonum;       	//#ϵͳ�����Ƶ������(���ƶ�����й�),��quad=0ʱvideonumӦ��Ϊ1 
        .videoencnum=	2,              //int     videoencnum;    //#��Ƶ����������
	 .hqencnum	=	1,			
	 .use_cpu_iic  =	0,		  //int	use_cpu_iic	��ʹ��cpuiic���߿�����Ƶadת��оƬ
        .ide			=	1,              //int     ide;   //#1��ʾ��cf����Ӳ�� 0��ʾû��
		.audionum      =   1,             //int audio
        .eth_port	=	1,              //int     eth_port;       	//#������ 1��ʾһ�� 2��ʾ����
        .list_num	=	sizeof(GTVS1024_O_List)/sizeof(DevType_T*),
        .list			=	(DevType_T**)GTVS1024_O_List        //DevType_T *List[];      //���豸�б�
};

static GTSeriesDVSR     DEV_GTVS1024_OA={
        .type		=	T_GTVS1024_OA,      //int     type;      //#�豸�ͺ�
	 .comment	=	"��GTVS1024-O��������Ƶ�Ŵ���",
        .trignum		=	6,              //int     trignum;        	//#�豸��߱���������
	 .outnum		=	4,		   //		outnum;		//���������
        .com		=	2,              //int     com;            	//#�豸����������Ҫ�ڽ���������
        .quad		=	1,              //int     quad;           	//#�Ƿ��л���ָ�����1��ʾ�� 0��ʾû��
	 .osd		=	1,							//��osd
        .videonum	=	4,              //int     videonum;       	//#ϵͳ�����Ƶ������(���ƶ�����й�),��quad=0ʱvideonumӦ��Ϊ1 
        .videoencnum=	2,              //int     videoencnum;    //#��Ƶ����������
	 .hqencnum	=	1,			
	 .use_cpu_iic  =	0,		  //int	use_cpu_iic	��ʹ��cpuiic���߿�����Ƶadת��оƬ
        .ide			=	1,              //int     ide;            	//#1��ʾ��cf����Ӳ�� 0��ʾû��
        .audionum      =   1,             //int audio
        .eth_port	=	1,              //int     eth_port;       	//#������ 1��ʾһ�� 2��ʾ����
        .list_num	=	sizeof(GTVS1024_O_List)/sizeof(DevType_T*),
        .list			=	(DevType_T**)GTVS1024_O_List        //DevType_T *List[];      //���豸�б�
};
//GT1011
static DevType_T *GTVS1011_List[]=
{	//GT1024 ���豸�б�
	&UART0,
	&UART1,
	&Ime6410VEnc0,
	&IDEDisk,
	&NetPort0,
};
static GTSeriesDVSR     DEV_GTVS1011={
        .type		=	T_GTVS1011,      //int     type;      //#�豸�ͺ�
	 .comment	=	"һ����Ƶѹ��оƬ,��������ָ���",
	 .trignum		=	6,              //int     trignum;        	//#�豸��߱���������
	 .outnum		=	4,		   //		outnum;		//���������
        .com		=	2,              //int     com;            	//#�豸����������Ҫ�ڽ���������
        .quad		=	0,              //int     quad;           	//#�Ƿ��л���ָ�����1��ʾ�� 0��ʾû��
	 .osd		=	0,							//��osd
        .videonum	=	1,              //int     videonum;       	//#ϵͳ�����Ƶ������(���ƶ�����й�),��quad=0ʱvideonumӦ��Ϊ1 
        .videoencnum=	1,              //int     videoencnum;    //#��Ƶ����������
	 .hqencnum	=	1,			
	 .use_cpu_iic  =	1,		  //int	use_cpu_iic	ʹ��cpuiic���߿�����Ƶadת��оƬ
        .ide			=	1,              //int     ide;            	//#1��ʾ��cf����Ӳ�� 0��ʾû��
        .audionum      =   1,             //int audio
        .eth_port	=	1,              //int     eth_port;       	//#������ 1��ʾһ�� 2��ʾ����
        .list_num	=	sizeof(GTVS1011_List)/sizeof(DevType_T*),
        .list			=	(DevType_T**)GTVS1011_List        //DevType_T *List[];      //���豸�б�
};
static GTSeriesDVSR     DEV_GTVS1011_A={
        .type		=	T_GTVS1011_A,      //int     type;      //#�豸�ͺ�
	 .comment	=	"һ����Ƶѹ��оƬ,��������ָ���",
	 .trignum		=	6,              //int     trignum;        	//#�豸��߱���������
	 .outnum		=	4,		   //		outnum;		//���������
        .com		=	2,              //int     com;            	//#�豸����������Ҫ�ڽ���������
        .quad		=	0,              //int     quad;           	//#�Ƿ��л���ָ�����1��ʾ�� 0��ʾû��
	 .osd		=	0,							//��osd
        .videonum	=	1,              //int     videonum;       	//#ϵͳ�����Ƶ������(���ƶ�����й�),��quad=0ʱvideonumӦ��Ϊ1 
        .videoencnum=	1,              //int     videoencnum;    //#��Ƶ����������
	 .hqencnum	=	1,			
	 .use_cpu_iic  =	1,		  //int	use_cpu_iic	��ʹ��cpuiic���߿�����Ƶadת��оƬ
        .ide			=	1,              //int     ide;            	//#1��ʾ��cf����Ӳ�� 0��ʾû��
        .audionum      =   1,             //int audio
        .eth_port	=	1,              //int     eth_port;       	//#������ 1��ʾһ�� 2��ʾ����
        .list_num	=	sizeof(GTVS1011_List)/sizeof(DevType_T*),
        .list			=	(DevType_T**)GTVS1011_List        //DevType_T *List[];      //���豸�б�
};

//GT1001
static GTSeriesDVSR     DEV_GT1001={
        .type           =       T_GT1001,      //int     type;      //#�豸�ͺ�
        .comment        =       "һ����Ƶѹ��оƬ,��������ָ���,����Ƶ����",
        .trignum        =       2,              //int     trignum;              //#�豸��߱���������
        .outnum         =       2,              //           outnum;         //���������
        .com            =       2,              //int     com;                  //#�豸����������Ҫ�ڽ���������
        .quad           =       0,              //int     quad;                 //#�Ƿ��л���ָ�����1��ʾ�� 0��ʾû��
        .osd            =       0,                                                      //��osd
        .videonum       =       1,              //int     videonum;             //#ϵͳ�����Ƶ������(���ƶ�����й�),��quad=0ʱvideonumӦ��Ϊ1 
        .videoencnum	=   	1,              //int     videoencnum;    //#��Ƶ����������
        .hqencnum      	=       1,
        .use_cpu_iic  	=       1,                //int use_cpu_iic     ��ʹ��cpuiic���߿�����Ƶadת��оƬ
        .ide            =       1,              //int     ide;                  //#1��ʾ��cf����Ӳ�� 0��ʾû��
        .audionum      =   1,             //int audio
        .eth_port       =       1,              //int     eth_port;             //#������ 1��ʾһ�� 2��ʾ����
        .list_num       =       sizeof(GTVS1011_List)/sizeof(DevType_T*),
        .list           =       (DevType_T**)GTVS1011_List        //���豸�б�,���豸��GTVS1011����ͬ
};
//GT1011-N+
static DevType_T *GTVS1011_N_List[]=
{	//GT1024 ���豸�б�
	&UART0,
	&UART1,
	&Ime6410VEnc0,
	&IDEDisk,
	&NetPort0,
	&NetPort1,
};
static GTSeriesDVSR     DEV_GTVS1011_N={
        .type		=	T_GTVS1011_N,      //int     type;      //#�豸�ͺ�
	 .comment	=	"һ����Ƶѹ��оƬ,��������ָ���,��������",
	 .trignum		=	6,              //int     trignum;        	//#�豸��߱���������
	 .outnum		=	4,		   //		outnum;		//���������
        .com		=	2,              //int     com;            	//#�豸����������Ҫ�ڽ���������
        .quad		=	0,              //int     quad;           	//#�Ƿ��л���ָ�����1��ʾ�� 0��ʾû��
	 .osd		=	0,							//��osd
        .videonum	=	1,              //int     videonum;       	//#ϵͳ�����Ƶ������(���ƶ�����й�),��quad=0ʱvideonumӦ��Ϊ1 
        .videoencnum=	1,              //int     videoencnum;    //#��Ƶ����������
	 .hqencnum	=	1,			
	 .use_cpu_iic  =	1,		  //int	use_cpu_iic	��ʹ��cpuiic���߿�����Ƶadת��оƬ
        .ide			=	1,              //int     ide;            	//#1��ʾ��cf����Ӳ�� 0��ʾû��
        .audionum      =   1,             //int audio
        .eth_port	=	2,              //int     eth_port;       	//#������ 1��ʾһ�� 2��ʾ����
        .list_num	=	sizeof(GTVS1011_N_List)/sizeof(DevType_T*),
        .list			=	(DevType_T**)GTVS1011_N_List        //DevType_T *List[];      //���豸�б�
};
static GTSeriesDVSR     DEV_GTVS1011_AN={
        .type		=	T_GTVS1011_AN,      //int     type;      //#�豸�ͺ�
	 .comment	=	"��GTVS1011-N+��������Ƶ�Ŵ���",
        .trignum		=	6,              //int     trignum;        	//#�豸��߱���������
	 .outnum		=	4,		   //		outnum;		//���������
        .com		=	2,              //int     com;            	//#�豸����������Ҫ�ڽ���������
        .quad		=	0,              //int     quad;           	//#�Ƿ��л���ָ�����1��ʾ�� 0��ʾû��
	 .osd		=	0,							//��osd
        .videonum	=	1,              //int     videonum;       	//#ϵͳ�����Ƶ������(���ƶ�����й�),��quad=0ʱvideonumӦ��Ϊ1 
        .videoencnum=	1,              //int     videoencnum;    //#��Ƶ����������
	 .hqencnum	=	1,			
	 .use_cpu_iic  =	1,		  //int	use_cpu_iic	��ʹ��cpuiic���߿�����Ƶadת��оƬ
        .ide			=	1,              //int     ide;            	//#1��ʾ��cf����Ӳ�� 0��ʾû��
        .audionum      =   1,             //int audio
        .eth_port	=	2,              //int     eth_port;       	//#������ 1��ʾһ�� 2��ʾ����
        .list_num	=	sizeof(GTVS1011_N_List)/sizeof(DevType_T*),
        .list			=	(DevType_T**)GTVS1011_N_List        //DevType_T *List[];      //���豸�б�
};

//GT1014
static DevType_T *GTVS1014_O_List[]=
{	//GT1024 ���豸�б�
	&UART0,
	&UART1,
	&QuadDev,
	&Ime6410VEnc0,
	&IDEDisk,
	&NetPort0
};
static GTSeriesDVSR     DEV_GTVS1014_O={
        .type		=	T_GTVS1014_O,      //int     type;      //#�豸�ͺ�
	 .comment	=	"һ����Ƶѹ��оƬ,������ָ���",
        .trignum		=	6,              //int     trignum;        	//#�豸��߱���������
	 .outnum		=	4,		   //		outnum;		//���������
        .com		=	2,              //int     com;            	//#�豸����������Ҫ�ڽ���������
        .quad		=	1,              //int     quad;           	//#�Ƿ��л���ָ�����1��ʾ�� 0��ʾû��
	 .osd		=	1,							//��osd
        .videonum	=	4,              //int     videonum;       	//#ϵͳ�����Ƶ������(���ƶ�����й�),��quad=0ʱvideonumӦ��Ϊ1 
        .videoencnum=	1,              //int     videoencnum;    //#��Ƶ����������
	 .hqencnum	=	1,			
	 .use_cpu_iic  =	0,		  //int	use_cpu_iic	��ʹ��cpuiic���߿�����Ƶadת��оƬ
	 .ide			=	1,              //int     ide;            	//#1��ʾ��cf����Ӳ�� 0��ʾû��
        .audionum      =   1,             //int audio
        .eth_port	=	1,              //int     eth_port;       	//#������ 1��ʾһ�� 2��ʾ����
        .list_num	=	sizeof(GTVS1014_O_List)/sizeof(DevType_T*),
        .list			=	(DevType_T**)GTVS1014_O_List        //DevType_T *List[];      //���豸�б�
};
static GTSeriesDVSR     DEV_GTVS1014_OA={
        .type		=	T_GTVS1014_OA,      //int     type;      //#�豸�ͺ�
	 .comment	=	"��GTVS1014-O��������Ƶ�Ŵ���",
	 .trignum		=	6,              //int     trignum;        	//#�豸��߱���������
	 .outnum		=	4,		   //		outnum;		//���������
        .com		=	2,              //int     com;            	//#�豸����������Ҫ�ڽ���������
        .quad		=	1,              //int     quad;           	//#�Ƿ��л���ָ�����1��ʾ�� 0��ʾû��
	 .osd		=	1,							//��osd
        .videonum	=	4,              //int     videonum;       	//#ϵͳ�����Ƶ������(���ƶ�����й�),��quad=0ʱvideonumӦ��Ϊ1 
        .videoencnum=	1,              //int     videoencnum;    //#��Ƶ����������
	 .hqencnum	=	1,			
	 .use_cpu_iic  =	0,		  //int	use_cpu_iic	��ʹ��cpuiic���߿�����Ƶadת��оƬ
        .ide			=	1,              //int     ide;            	//#1��ʾ��cf����Ӳ�� 0��ʾû��
        .audionum      =   1,             //int audio
        .eth_port	=	1,              //int     eth_port;       	//#������ 1��ʾһ�� 2��ʾ����
        .list_num	=	sizeof(GTVS1014_O_List)/sizeof(DevType_T*),
        .list			=	(DevType_T**)GTVS1014_O_List        //DevType_T *List[];      //���豸�б�
};

//GT1000
#define GT1000_List	GTVS1024_O_List
static GTSeriesDVSR     DEV_GT1000={
        .type		=	T_GT1000,  //int     type;           //#�豸�ͺ�
	 .comment	=	"���ͺŵ��豸",
        .trignum		=	6,               //int     trignum;        //#�豸��߱���������
 	 .outnum		=	4,		   //		outnum;		//���������
        .com		=	2,              //int     com;            //#�豸����������Ҫ�ڽ���������
        .quad		=	1,              //int     quad;           //#�Ƿ��л���ָ�����1��ʾ�� 0��ʾû��
	 .osd		=	1,
        .videonum	=	4,              //int     videonum;       //#ϵͳ�����Ƶ������(���ƶ�����й�),��quad=0ʱvideonumӦ��Ϊ1 
        .videoencnum=	2,              //int     videoencnum;    //#��Ƶ����������
	 .hqencnum	=	1,
	 .use_cpu_iic  =	0,		  //int	use_cpu_iic	��ʹ��cpuiic���߿�����Ƶadת��оƬ
        .ide			=	1,              //int     ide;            //#1��ʾ��cf����Ӳ�� 0��ʾû��
        .audionum      =   1,             //int audio
        .eth_port	=	1,              //int     eth_port;       //#������ 1��ʾһ�� 2��ʾ����
        .list_num	=	sizeof(GT1000_List)/sizeof(DevType_T*),
	 .list			=	(DevType_T**)GT1000_List	//DevType_T *List[];      //���豸�б�
};
//gtvs1021
static DevType_T *GTVS1021_List[]=
{	//GT1021 ���豸�б�
	&UART0,
	&UART1,
	&Ime6410VEnc0,
	&Ime6410VEnc1,
	&IDEDisk,
	&NetPort0
};

static GTSeriesDVSR     DEV_GTVS1021={
        .type		=	T_GTVS1021,      //int     type;      //#�豸�ͺ�
	 .comment	=	"������Ƶѹ��оƬ,��������ָ���",
        .trignum		=	6,              //int     trignum;        	//#�豸��߱���������
	 .outnum		=	4,		   //		outnum;		//���������
        .com		=	2,              //int     com;            	//#�豸����������Ҫ�ڽ���������
        .quad		=	0,              //int     quad;           	//#�Ƿ��л���ָ�����1��ʾ�� 0��ʾû��
	 .osd		=	1,							//��osd
        .videonum	=	1,              //int     videonum;       	//#ϵͳ�����Ƶ������(���ƶ�����й�),��quad=0ʱvideonumӦ��Ϊ1 
        .videoencnum=	2,              //int     videoencnum;    //#��Ƶ����������
	 .hqencnum	=	1,			
	 .use_cpu_iic  =	1,		  //int	use_cpu_iic	��ʹ��cpuiic���߿�����Ƶadת��оƬ
        .ide			=	1,              //int     ide;            	//#1��ʾ��cf����Ӳ�� 0��ʾû��
        .audionum      =   1,             //int audio
        .eth_port	=	1,              //int     eth_port;       	//#������ 1��ʾһ�� 2��ʾ����
        .list_num	=	sizeof(GTVS1021_List)/sizeof(DevType_T*),
        .list			=	(DevType_T**)GTVS1021_List        //DevType_T *List[];      //���豸�б�
};

//GTVM3001

static DevType_T *GTVM3001_List[]=
{       //GTVM ���豸�б�
        &NetPort0,
        &NetPort1
};

static GTSeriesDVSR     DEV_GTVM3001={
        .type           =       T_GTVM3001,      //int     type;      //#�豸�ͺ�
         .comment       =       "�������ڣ����������豸",
        .trignum                =       0,              //int     trignum;              //#�豸��߱���������
         .outnum                =       2,                 //           outnum;         //���������
        .com            =       2,              //int     com;                  //#�豸����������Ҫ�ڽ���������
        .quad           =       0,              //int     quad;                 //#�Ƿ��л���ָ�����1��ʾ�� 0��ʾû��
         .osd           =       0,                                                      //��osd
        .videonum       =       0,              //int     videonum;             //#ϵͳ�����Ƶ������(���ƶ�����й�),��quad=0ʱvideonumӦ��Ϊ1 
        .videoencnum=   0,              //int     videoencnum;    //#��Ƶ����������
         .hqencnum      =       0,
         .use_cpu_iic  =        0,                //int use_cpu_iic     ��ʹ��cpuiic���߿�����Ƶadת��оƬ
        .ide                    =       0,        //int     ide;                //#1��ʾ��cf����Ӳ�� 0��ʾû��
        .audionum     =   0,                    //int audio
        .eth_port       =       2,              //int     eth_port;             //#������ 1��ʾһ�� 2��ʾ����
        .list_num       =       sizeof(GTVM3001_List)/sizeof(DevType_T*),
        .list                   =       (DevType_T**)GTVM3001_List        //DevType_T *List[];      //���豸�б�
};

//GTVM3111
static DevType_T *GTVM3111_List[]=
{       //GTVM ���豸�б�
        &NetPort0,
        &NetPort1,
        &UART0,
		&UART1,
		&Ime6410VEnc0
};

static GTSeriesDVSR     DEV_GTVM3111={
        .type           =       T_GTVM3111,      //int     type;      //#�豸�ͺ�
         .comment       =       "�������ڣ��������ڣ�һƬ6410",
        .trignum                =       6,              //int     trignum;              //#�豸��߱���������
         .outnum                =       4,                 //           outnum;         //���������
        .com            =       2,              //int     com;                  //#�豸����������Ҫ�ڽ���������
        .quad           =       0,              //int     quad;                 //#�Ƿ��л���ָ�����1��ʾ�� 0��ʾû��
         .osd           =       0,                                                      //��osd
        .videonum       =       1,              //int     videonum;             //#ϵͳ�����Ƶ������(���ƶ�����й�),��quad=0ʱvideonumӦ��Ϊ1 
        .videoencnum=   1,              //int     videoencnum;    //#��Ƶ����������
         .hqencnum      =       0,
         .use_cpu_iic  =        1,                //int use_cpu_iic     ��ʹ��cpuiic���߿�����Ƶadת��оƬ
        .ide                    =       0,        //int     ide;                //#1��ʾ��cf����Ӳ�� 0��ʾû��
        .audionum     =   0,                    //int audio
        .eth_port       =       2,              //int     eth_port;             //#������ 1��ʾһ�� 2��ʾ����
        .list_num       =       sizeof(GTVM3111_List)/sizeof(DevType_T*),
        .list                   =       (DevType_T**)GTVM3111_List        //DevType_T *List[];      //���豸�б�
};



/***************************************************************************************************************/
#endif

