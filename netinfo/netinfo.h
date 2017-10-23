#ifndef NET_INFO_H
#define NET_INFO_H
#include <gtsocket.h>
//#include <netinet/in.h>
//#include <arpa/inet.h>


/**********************************************************************************************
 * 函数名	:get_net_dev_ip()
 * 功能	:获取本机指定接口的ip地址
 * 输入	:dev:本机的网络接口名,如"eth0" "ppp0"...
 * 返回值	:ip地址
 **********************************************************************************************/
in_addr_t get_net_dev_ip(char *dev);

/**********************************************************************************************
 * 函数名	:get_net_dev_mask()
 * 功能	:获取本机指定接口的ip地址子网掩码
 * 输入	:dev:本机的网络接口名,如"eth0" "ppp0"...
 * 返回值	:子网掩码
 **********************************************************************************************/
in_addr_t get_net_dev_mask(char *dev);

/**********************************************************************************************
 * 函数名	:get_net_dev_mac()
 * 功能	:获取本机指定接口的mac地址
 * 输入	:dev:本机的网络接口名,如"eth0" "ppp0"...
 * 输出 	 buf:要被填充的mac地址缓冲区[0][1][2][3][4][5]
 * 返回值	:0表示成功负值表示失败
 **********************************************************************************************/
int get_net_dev_mac(char *dev,unsigned char *buf);

//检测网线的连接状态 0表示没有连接 1表示连接上了 负值表示出错
int get_link_stat( char * dev);
#endif


