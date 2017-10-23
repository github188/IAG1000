#ifndef FILE_DEF_H
#define FILE_DEF_H
#include <typedefine.h>
#include <stdio.h>
#include <string.h>
#include <gtlog.h>

//电路板型号定义

#define		ALARMIN_MASK			0xffff	// from 0xff lc //from 0x3f,wsy		//报警输入淹码
#define		TOTAL_ALARMOUT_PORT  		4			//最大报警输出通道数
#define		ALARMOUT_MASK			0xff 		//fixme,输出端子改变时

#define		TOTAL_TRIG_IN			6			//触发输入数
#define 	MAX_TRIG_IN				20 		////最大触发输入数目,这个数只做分配空间用,需要设备的具体值时调用get_trigin_num()

#define 		MAX_TRIG_EVENT		10			//单路触发最大触发事件个数

#define 		MAX_FILE_LEN		30*1024			//配置文件最大长度 远程设定参数时用到

#if 0

#ifndef _WIN32
#include <syslog.h>

//打开日志记录功能
//name表示日志信息中的名字
#define gtopenlog(name) openlog(name,LOG_CONS|LOG_NDELAY|LOG_PID,LOG_LOCAL0 );//LOG_USER);

//#define gtlog  syslog		//系统日志信息记录
#define gtlog syslog
//一般性信息
#define gtloginfo(args...) syslog(LOG_INFO,##args)	//记录一般信息
//严重的错误信息
#define gtlogfault(args...) syslog(LOG_CRIT,##args)	//
//错误信息
#define gtlogerr(args...) syslog(LOG_ERR,##args)	//
//警告信息
#define gtlogwarn(args...) syslog(LOG_WARNING,##args)

#define gtlogdebug(args...) syslog(LOG_DEBUG,##args)
#endif //_WIN32
#endif


/*
	vsmain define
*/

#define		IPMAIN_TMP_DIR			"/tmp"							//ipmain用于存放临时文件的地方
#ifndef FOR_PC_MUTI_TEST
#define	 	DEVINFO_PARA_FILE		"/conf/ipcall.ini"					//存放设备固定信息的文件，这些信息一般不会修改
#define	 	IPMAIN_PARA_FILE		"/conf/ipcall.ini"	
#define	 	IPMAIN_PARA_GATE_BAK		"/conf/ipcall_gate.ini" 			//用于指示变化的配置文件备份

#endif

/*
	rtimage define
*/


#ifndef FOR_PC_MUTI_TEST
#define	 	RTIMAGE_PARA_FILE		"/conf/ipcall.ini"//"/conf/rtimage.ini"
#endif

#endif
