///GTVS3000ϵ����Ƶ�������ͺŶ���
#ifndef GTVS3000_DEV_TYPE_H
#define GTVS3000_DEV_TYPE_H
//ֻ�ܱ�devtype.c�ڰ����� include <devinfo.h>�Ͷ�����dev_name_t �����
/************************������豸�ͺŵ����豸�б�*************************************************************/
/*
    �����豸�����豸�б�ʱ��ͬ���͵Ķ���豸һ��Ҫ����˳���壬��Ime6410VEnc0һ��Ҫ��Ime6410VEnc1֮ǰ
    ����һ���µ��豸�ͺ�ʱ��Ҫ�������ͺŵ�static DevType_T �ṹ�����static GTSeriesDVSR�ṹ��Դ���壬Ȼ��
    ��static GTSeriesDVSR��ָ�����DEVSupportList����
*/

//GTVS3021
/*****************************************************************
 * �豸����:    GTVS3021
 * ***************************************************************/
static DevType_T *GTVS3021_List[]=
{   //GTVS3021 ���豸�б�
    &UART0,
    &UART1,
    &IDEDisk,
    &NetPort0
};

static GTSeriesDVSR     DEV_GTVS3021={
        .type           =       T_GTVS3021,      //int     type;      //#�豸�ͺ�
        .comment        =       "˫����,����Ƶ����,��osd",
        .trignum        =       8,              //int     trignum;          //#�豸��߱���������
        .outnum         =       4,         //       outnum;     //���������
        .com            =       2,              //int     com;              //#�豸����������Ҫ�ڽ���������
        .quad           =       0,              //int     quad;             //#�Ƿ��л���ָ�����1��ʾ�� 0��ʾû��
        .osd            =       1,                          //��osd
        .videonum       =       1,              //int     videonum;         //#ϵͳ�����Ƶ������(���ƶ�����й�),��quad=0ʱvideonumӦ��Ϊ1 
        .videoencnum    =       2,              //int     videoencnum;    //#��Ƶ����������
        .hqencnum       =       1,          
        .use_cpu_iic    =       1,        //int use_cpu_iic ��ʹ��cpuiic���߿�����Ƶadת��оƬ
        .ide            =       1,              //int     ide;   //#1��ʾ��cf����Ӳ�� 0��ʾû��
        .audionum       =       1,             //int audio
        .eth_port       =       1,              //int     eth_port;         //#������ 1��ʾһ�� 2��ʾ����
        .list_num       =       sizeof(GTVS3021_List)/sizeof(DevType_T*),
        .list           =       (DevType_T**)GTVS3021_List        //DevType_T *List[];      //���豸�б�
};


/*******************************************************************
 * �豸����:    GTVS3021L
 * *****************************************************************/
static DevType_T *GTVS3021L_List[]=
{       //GTVS3021L ���豸�б�
        &UART0,
        &UART1,
        &IDEDisk,
        &NetPort0
};

static GTSeriesDVSR     DEV_GTVS3021L={
        .type           =       T_GTVS3021L,      //int     type;      //#�豸�ͺ�
        .comment        =       "˫����,����Ƶ����,��osd,�͹���",
        .trignum        =       0,              //int     trignum;              //#�豸��߱���������
        .outnum         =       0,                 //           outnum;         //���������
        .com            =       2,              //int     com;                  //#�豸����������Ҫ�ڽ���������
        .quad           =       0,              //int     quad;                 //#�Ƿ��л���ָ�����1��ʾ�� 0��ʾû��
        .osd            =       1,                                                      //��osd
        .videonum       =       1,              //int     videonum;             //#ϵͳ�����Ƶ������(���ƶ�����й�),��quad=0ʱvideonumӦ��Ϊ
        .videoencnum    =       2,              //int     videoencnum;    //#��Ƶ����������
        .hqencnum       =       1,
        .use_cpu_iic    =       1,                //int use_cpu_iic     ��ʹ��cpuiic���߿�����Ƶadת��оƬ
        .ide            =       2,              //int     ide;   //#1��ʾ��cf����Ӳ�� 0��ʾû��
        .audionum       =       1,             //int audio
        .eth_port       =       1,              //int     eth_port;             //#������ 1��ʾһ�� 2��ʾ����
        .list_num       =       sizeof(GTVS3021L_List)/sizeof(DevType_T*),
        .list           =       (DevType_T**)GTVS3021L_List        //DevType_T *List[];      //���豸�б�

};

/**************************************************************
 * �豸����:    GTMV3121
 * ***********************************************************/
static DevType_T *GTMV3121_List[]=
{       //GTVS3121 ���豸�б�
        &UART0,
        &UART1,
        &IDEDisk,
        &NetPort0
};

static GTSeriesDVSR     DEV_GTMV3121={

        .type           =       T_GTMV3121, //int     type;      //#�豸�ͺ�
        .comment        =       "˫����,����Ƶ����,��osd,�͹���",
        .trignum        =       0,              //int     trignum;              //#�豸��߱���������
        .outnum         =       0,              //           outnum;         //���������
        .com            =       2,              //int     com;                  //#�豸����������Ҫ�ڽ���������
        .quad           =       0,              //int     quad;                 //#�Ƿ��л���ָ�����1��ʾ�� 0��ʾû��
        .osd            =       1,                                                      //��osd
        .videonum       =       1,              //int     videonum;             //#ϵͳ�����Ƶ������(���ƶ�����й�),��quad=0ʱvideonumӦ��Ϊ
        .videoencnum    =       2,              //int     videoencnum;    //#��Ƶ����������
        .hqencnum       =       1,
        .use_cpu_iic    =       1,              //int use_cpu_iic     ��ʹ��cpuiic���߿�����Ƶadת��оƬ
        .ide            =       3,              //int     ide;   //#1��ʾ��cf����Ӳ�� 0��ʾû��,2-->sd,3-->tf
        .audionum       =       0,              //int audio
        .eth_port       =       1,              //int     eth_port;             //#������ 1��ʾһ�� 2��ʾ����
        .list_num       =       sizeof(GTMV3121_List)/sizeof(DevType_T*),
        .list           =       (DevType_T**)GTMV3121_List        //DevType_T *List[];      //���豸�б�
};

/************************************************************
 * �豸����:    GTVS3022
 * **********************************************************/
static DevType_T *GTVS3022_List[]=

{       //GTVS3022 ���豸�б�
        &UART0,
        &UART1,
        &IDEDisk,
        &NetPort0
};

static GTSeriesDVSR     DEV_GTVS3022={
        .type           =       T_GTVS3022,      //int     type;      //#�豸�ͺ�
        .comment        =       "��·��������˫��Ƶ����,��osd,Ŀǰֻ֧��һ������ͷ����",
        .trignum        =       8,              //int     trignum;              //#�豸��߱���������
        .outnum         =       4,              //           outnum;         //���������
        .com            =       2,              //int     com;                  //#�豸����������Ҫ�ڽ���������
        .quad           =       0,              //int     quad;                 //#�Ƿ��л���ָ�����1��ʾ�� 0��ʾû��
        .osd            =       1,                                                      //��osd
        .videonum       =       2,                    // 1,              //int     videonum;             //#ϵͳ�����Ƶ������(���ƶ�����й�),��quad=0ʱvideonumӦ��1 
        .videoencnum    =       2,              //int     videoencnum;    //#��Ƶ����������
        .hqencnum       =       2,                    //    1,
        .use_cpu_iic    =       0,              //int use_cpu_iic     ��ʹ��cpuiic���߿�����Ƶadת��оƬ
        .ide            =       1,              //int     ide;   //#1��ʾ��cf����Ӳ�� 0��ʾû��
        .audionum       =       1,              //int audio
        .eth_port       =       1,              //int     eth_port;             //#������ 1��ʾһ�� 2��ʾ����
        .list_num       =       sizeof(GTVS3022_List)/sizeof(DevType_T*),
        .list           =       (DevType_T**)GTVS3022_List        //DevType_T *List[];      //���豸�б�
};


/*************************************************
 *�豸����: GTMV3122
 *************************************************/
static DevType_T *GTMV3122_List[]=

{       //GTMV3122 ���豸�б�
        &UART0,
        &UART1,
        &IDEDisk,
        &NetPort0
};

static GTSeriesDVSR     DEV_GTMV3122={
        .type           =       T_GTMV3122,      //int     type;      //#�豸�ͺ�
        .comment        =       "��·��������˫��Ƶ����,��osd,Ŀǰֻ֧��һ������ͷ����",
        .trignum        =       0,              //int     trignum;              //#�豸��߱���������
        .outnum         =       0,                 //           outnum;         //���������
        .com            =       2,              //int     com;                  //#�豸����������Ҫ�ڽ���������
        .quad           =       0,              //int     quad;                 //#�Ƿ��л���ָ�����1��ʾ�� 0��ʾû��
        .osd            =       1,                                                      //��osd
        .videonum       =       2,                     // 1,              //int     videonum;             //#ϵͳ�����Ƶ������(���ƶ�����й�),��quad=0ʱvideonumӦ��1 
        .videoencnum    =       2,              //int     videoencnum;    //#��Ƶ����������
        .hqencnum       =       2,                    //    1,
        .use_cpu_iic    =       0,              //int use_cpu_iic     ��ʹ��cpuiic���߿�����Ƶadת��оƬ
        .ide            =       2,              //int     ide;   //#1��ʾ��cf����Ӳ�� 0��ʾû�� 2��ʾSD����3��ʾTF��
        .audionum       =       1,              //int audio
        .eth_port       =       1,              //int     eth_port;             //#������ 1��ʾһ�� 2��ʾ����
        .list_num       =       sizeof(GTMV3122_List)/sizeof(DevType_T*),
        .list           =       (DevType_T**)GTMV3122_List        //DevType_T *List[];      //���豸�б�
};

/****************************************************
 *�豸����: GTIP1004
 ****************************************************/
 static DevType_T *GTIP1004_List[]=
{
    //GTIP1004 ���豸�б�
    //lc to do  
    &IPUART0,
    &IPUART0,
    &NetPort0
};

static GTSeriesDVSR     DEV_GTIP1004={
        .type           =       T_GTIP1004,      //int     type;            //#�豸�ͺ�
        .comment        =       "������,4��Ƶ����,��osd",
        .trignum        =       8,              //int     trignum;          //#�豸��߱���������
        .outnum         =       4,      //      outnum;     //���������
        .com            =       2,              //int     com;              //#�豸����������Ҫ�ڽ���������
        .quad           =       1,              //int     quad;             //#�Ƿ��л���ָ�����1��ʾ�� 0��ʾû��
        .osd            =       1,                      //��osd
        .videonum       =       4,              //int     videonum;         //#ϵͳ�����Ƶ������(���ƶ�����й�),��quad=0ʱvideonumӦ��Ϊ1 
        .videoencnum    =       5,              //int     videoencnum;        //#��Ƶ����������
        .hqencnum       =       1,          
        .use_cpu_iic    =       0,      //int   use_cpu_iic     ��ʹ��cpuiic���߿�����Ƶadת��оƬ
        .ide            =       1,              //int     ide;          //#1��ʾ��cf����Ӳ�� 0��ʾû��
        .audionum       =       4,              //int audio
        .eth_port       =       1,              //int     eth_port;         //#������ 1��ʾһ�� 2��ʾ����
        .list_num       =       sizeof(GTIP1004_List)/sizeof(DevType_T*),
        .list           =       (DevType_T**)GTIP1004_List        //DevType_T *List[];      //���豸�б�
};

/****************************************************
 *�豸����: GTIP2004
 ****************************************************/
 static DevType_T *GTIP2004_List[]=
{
    //GTIP2004 ���豸�б�
    //lc to do  
    &IPUART0,
    &IPUART0,
    &NetPort0
};

static GTSeriesDVSR     DEV_GTIP2004={
        .type           =       T_GTIP2004,      //int     type;            //#�豸�ͺ�
        .comment        =       "������,4��Ƶ����,��osd",
        .trignum        =       16,              //int     trignum;          //#�豸��߱���������
        .outnum         =       4,      //      outnum;     //���������
        .com            =       2,              //int     com;              //#�豸����������Ҫ�ڽ���������
        .quad           =       1,              //int     quad;             //#�Ƿ��л���ָ�����1��ʾ�� 0��ʾû��
        .osd            =       1,                      //��osd
        .videonum       =       4,              //int     videonum;         //#ϵͳ�����Ƶ������(���ƶ�����й�),��quad=0ʱvideonumӦ��Ϊ1 
        .videoencnum    =       5,              //int     videoencnum;        //#��Ƶ����������
        .hqencnum       =       1,          
        .use_cpu_iic    =       0,      //int   use_cpu_iic     ��ʹ��cpuiic���߿�����Ƶadת��оƬ
        .ide            =       1,              //int     ide;          //#1��ʾ��cf����Ӳ�� 0��ʾû��
        .audionum       =       4,              //int audio
        .eth_port       =       1,              //int     eth_port;         //#������ 1��ʾһ�� 2��ʾ����
        .list_num       =       sizeof(GTIP2004_List)/sizeof(DevType_T*),
        .list           =       (DevType_T**)GTIP2004_List        //DevType_T *List[];      //���豸�б�
};


/****************************************************
 *�豸����: GTIP2000
 ****************************************************/
 static DevType_T *GTIP2000_List[]=
{
    //GTIP2000 ���豸�б�
    &NetPort0
};

static GTSeriesDVSR     DEV_GTIP2000={
        .type           =       T_GTIP2000,      //int     type;            //#�豸�ͺ�
        .comment        =       "��·ip�Խ��豸",
        .trignum        =       1,              //int     trignum;          //#�豸��߱���������
        .outnum         =       1,      //      outnum;     //���������
        .com            =       0,              //int     com;              //#�豸����������Ҫ�ڽ���������
        .quad           =       0,              //int     quad;             //#�Ƿ��л���ָ�����1��ʾ�� 0��ʾû��
        .osd            =       0,                      //��osd
        .videonum       =       0,              //int     videonum;         //#ϵͳ�����Ƶ������(���ƶ�����й�),��quad=0ʱvideonumӦ��Ϊ1 
        .videoencnum    =       0,              //int     videoencnum;        //#��Ƶ����������
        .hqencnum       =       0,          
        .use_cpu_iic    =       0,      //int   use_cpu_iic     ��ʹ��cpuiic���߿�����Ƶadת��оƬ
        .ide            =       0,              //int     ide;          //#1��ʾ��cf����Ӳ�� 0��ʾû��
        .audionum       =       1,              //int audio
        .eth_port       =       1,              //int     eth_port;         //#������ 1��ʾһ�� 2��ʾ����
        .list_num       =       sizeof(GTIP2000_List)/sizeof(DevType_T*),
        .list           =       (DevType_T**)GTIP2000_List        //DevType_T *List[];      //���豸�б�
};



//GTVS3024
/******************************************************
 * �豸����:    GTVS3024
 ******************************************************/
static DevType_T *GTVS3024_List[]=
{   //GT1024 ���豸�б�
    		&QuadDev_2835,
        &UART0,
        &UART1,
        &IDEDisk,
        &NetPort0
};

static GTSeriesDVSR     DEV_GTVS3024={
        .type           =       T_GTVS3024,      //int     type;            //#�豸�ͺ�
        .comment        =       "˫����,4��Ƶ����,��osd",
        .trignum        =       8,              //int     trignum;          //#�豸��߱���������
        .outnum         =       4,      //      outnum;     //���������
        .com            =       2,              //int     com;              //#�豸����������Ҫ�ڽ���������
        .quad           =       1,              //int     quad;             //#�Ƿ��л���ָ�����1��ʾ�� 0��ʾû��
        .osd            =       1,                      //��osd
        .videonum       =       4,              //int     videonum;         //#ϵͳ�����Ƶ������(���ƶ�����й�),��quad=0ʱvideonumӦ��Ϊ1 
        .videoencnum    =       2,              //int     videoencnum;        //#��Ƶ����������
        .hqencnum       =       1,          
        .use_cpu_iic    =       0,      //int   use_cpu_iic     ��ʹ��cpuiic���߿�����Ƶadת��оƬ
        .ide            =       1,              //int     ide;          //#1��ʾ��cf����Ӳ�� 0��ʾû��
        .audionum       =       1,              //int audio
        .eth_port       =       1,              //int     eth_port;         //#������ 1��ʾһ�� 2��ʾ����
        .list_num       =       sizeof(GTVS3024_List)/sizeof(DevType_T*),
        .list           =       (DevType_T**)GTVS3024_List        //DevType_T *List[];      //���豸�б�
};

/*******************************************************
 * �豸����:    GTVS3024L
 * *****************************************************/
static DevType_T *GTVS3024L_List[]=
{       //GTVS3024L ���豸�б�
        &QuadDev_2835,
        &NetPort0
};
static GTSeriesDVSR     DEV_GTVS3024L={
        .type           =       T_GTVS3024L,      //int     type;      //#�豸�ͺ�
        .comment        =       "˫����,����Ƶ����,��osd,�͹���",
        .trignum        =       6,              //int     trignum;              //#�豸��߱���������
        .outnum         =       4,                 //           outnum;         //���������
        .com            =       2,              //int     com;                  //#�豸����������Ҫ�ڽ���������
        .quad           =       1,              //int     quad;                 //#�Ƿ��л���ָ�����1��ʾ�� 0��ʾû��
        .osd            =       1,                                                      //��osd
        .videonum       =       4,              //int     videonum;             //#ϵͳ�����Ƶ������(���ƶ�����й�),��quad=0ʱvideonumӦ��Ϊ
        .videoencnum    =       2,              //int     videoencnum;    //#��Ƶ����������
        .hqencnum       =       1,
        .use_cpu_iic    =       0,                //int use_cpu_iic     ��ʹ��cpuiic���߿�����Ƶadת��оƬ
        .ide            =       2,              //int     ide;   //#1��ʾ��cf����Ӳ�� 0��ʾû��
        .audionum       =       0,             //int audio
        .eth_port       =       1,              //int     eth_port;             //#������ 1��ʾһ�� 2��ʾ����
        .list_num       =       sizeof(GTVS3024L_List)/sizeof(DevType_T*),
        .list           =       (DevType_T**)GTVS3024L_List        //DevType_T *List[];      //���豸�б�

};


/**************************************************************
 * �豸����:    GTVS3124
 * ************************************************************/
static DevType_T *GTVS3124_List[]=
{       //GTVS3124 ���豸�б�
    &QuadDev_2835,
    &NetPort0
};
static GTSeriesDVSR     DEV_GTVS3124={
        .type           =       T_GTVS3124,      //int     type;      //#�豸�ͺ�
        .comment        =       "˫����,����Ƶ����,��osd,�͹���",
        .trignum        =       1,              //int     trignum;              //#�豸��߱���������   lsk 2009-11-6 0->1
        .outnum         =       0,                 //           outnum;         //���������
        .com            =       0,              //int     com;                  //#�豸����������Ҫ�ڽ���������
        .quad           =       1,              //int     quad;                 //#�Ƿ��л���ָ�����1��ʾ�� 0��ʾû��
        .osd            =       1,                                                      //��osd
        .videonum       =       4,              //int     videonum;             //#ϵͳ�����Ƶ������(���ƶ�����й�),��quad=0ʱvideonumӦ��Ϊ
        .videoencnum    =       2,              //int     videoencnum;    //#��Ƶ����������
        .hqencnum       =       1,
        .use_cpu_iic    =       0,                //int use_cpu_iic     ��ʹ��cpuiic���߿�����Ƶadת��оƬ
        .ide            =       2,              //int     ide;   //#1��ʾ��cf����Ӳ�� 0��ʾû��
        .audionum       =       0,             //int audio
        .eth_port       =       1,              //int     eth_port;             //#������ 1��ʾһ�� 2��ʾ����
        .list_num       =       sizeof(GTVS3124_List)/sizeof(DevType_T*),
        .list           =       (DevType_T**)GTVS3124_List        //DevType_T *List[];      //���豸�б�

};
#endif

