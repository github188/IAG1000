#include "ipmain.h"
#include "ipmain_para.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "mainnetproc.h"
#include "watch_board.h"
#include "gate_connect.h"
#include <sys/types.h>
#include <signal.h>
#include "netcmdproc.h"
#include "gate_connect.h"
#include "devstat.h"
#include <commonlib.h>
//#include "gtvs_io_api.h"
#include "mod_socket.h"
#include "ipmain_para.h"


void *watch_process_thread(void)
{
	//static int regist_cnt=0;
	static int net_refresh_cnt=0;
	struct ipmain_para_struct *ipmain_para;
	
    printf("watch_process_thread thread running...\n");
	gtloginfo("start watch_process_thread...\n");

	sleep(10);

	ipmain_para=get_mainpara();
 	
	while(1)
	{		
		netcmd_second_proc();

		gate_connect_second_proc();
		
		if(++net_refresh_cnt>10)
		{
			net_refresh_cnt=0;
			refresh_netinfo();
		}
		sleep(1);
	}
	
	return NULL;
}

