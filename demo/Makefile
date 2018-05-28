all:
	mipsel-openwrt-linux-g++ -fpermissive -g test.cpp librtmp_send264.cpp -I ../include/ -L ../lib/ -lxmnetsdk -liconv -lrtmp -o main -ldl -lpthread 
scp:
	scp main root@192.168.31.105:/overlay
cp:
	cp main /mnt/
