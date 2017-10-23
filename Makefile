top_DIR  = $(shell pwd)
#EMBED = 0 

EXEC = ipcall
SRC  = ipmain.c mainnetproc.c ipmain_para.c watch_process.c gate_connect.c netcmdproc.c devstat.c alarm_process.c watch_board.c
SRC += ./devinfo/devinfo.c ./devinfo/devinfo_virdev.c ./devinfo/devres.c ./devinfo/devtype.c 
SRC += ./iniparser/src/dictionary.c ./iniparser/src/iniparser.c ./iniparser/src/strlib.c
SRC += ./gtlog/gtlog.c
SRC += ./gtvs_io_api.c
#SRC += ../../libsrc/update/update.c
SRC += ./gtsocket/src/gtsocket.c
SRC += ./modcom/mod_com.c
SRC += ./netinfo/netinfo.c
SRC += ./modsocket/mod_socket.c
SRC += ./gtthread/gtthread.c
SRC += ./confparser/confparser.c
SRC += ./gtcomapi_ssl/gt_com_api.c ./gtcomapi_ssl/tab_crc32.c
SRC += ./common/commonlib.c
SRC += ./xmlparser/src/attr.c ./xmlparser/src/document.c ./xmlparser/src/element.c ./xmlparser/src/ixml.c ./xmlparser/src/ixmlmembuf.c ./xmlparser/src/ixmlparser.c ./xmlparser/src/namedNodeMap.c ./xmlparser/src/node.c ./xmlparser/src/nodeList.c
SRC += remote_file.c
SRC += rtimg_para2.c avserver.c net_avstream.c net_aplay.c
SRC += ./imencoder/AVIEncoder.c  ./gtipcs/mshm.c 

OBJS = $(SRC:.c=.o)

ifdef EMBED
	#CC=arm-hisiv100nptl-linux-gcc
	CC=mipsel-linux-gcc
	CFLAGS +=  -DEMBEDED=1  
#	INCDIR = -I/usr/local/arm/2.95.3/arm-linux/include
#	LIBDIR = -L/usr/local/arm/2.95.3/arm-linux/lib
	LIBDIR += -L ../../lib
else
	CC = gcc
	CFLAGS += -m32 -g -DEMBEDED=0
	LIBDIR += -L../pclib
endif
INCDIR = -I ./include -I./ -I./gtcomapi_ssl -I./gtsocket/src -I./iniparser/src -I./gtlog/
INCDIR += -I./confparser/ -I./modcom/ -I./modsocket/ -I./netinfo -I./gtthread -I./devinfo
INCDIR += -I./gtcomapi_ssl -I./common/ -I./xmlparser/src/ -I./xmlparser/inc/
INCDIR += -I./media_api -I./gtipcs
CFLAGS += $(INCDIR) 
CFLAGS += -Wall
LDLIBS += $(LIBDIR)
LDLIBS += -lpthread    
#LDLIBS += -lcsvparser 
all: $(EXEC)


$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $(SRC) -o $(EXEC) $(LDLIBS) 
clean:
	-rm -f $(EXEC) $(OBJS) *.elf *.gdb *.o
cp :
	cp -frv $(EXEC) /nfs/$(shell whoami)
test:
	chmod 777 $(EXEC)
	cp $(EXEC) /home/temp/nfs/vs3test/conf/gt1000
symlink:
