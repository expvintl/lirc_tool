#include <iostream>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include "lirc.h"
using namespace std;
int fd;
// PULSE or MODE2
int mode=LIRC_MODE_PULSE;
int mode2=LIRC_SET_SEND_MODE;
char *code="1";
// ioctl
int io = -1;
//用来判断是否用了-d
int reset=0;
void usage(int argc, char *argv[])
{
	int item;
	while ((item = getopt(argc, argv, "rs:hvd:")) != -1)
	{
		switch (item)
		{
		case 'r':
			printf("Listening....\n");
			mode2 = LIRC_SET_REC_MODE;
			mode = LIRC_MODE_MODE2;
			break;
		case 's':
			// 设置为发送模式
			mode2 = LIRC_SET_SEND_MODE;
			mode = LIRC_MODE_PULSE;
			code = optarg;
			printf("已发送:%s\n", code);
			break;
			case 'd':
			fd = open(optarg, O_RDWR);
			reset++;
			break;
			case 'v':
			printf("Version 1.0\n\033[31m qq1198\033[0m | \033[31m 2018.11.29 \033[0m \n");
			break;
		case 'h':
		default:
			printf("Usage:%s [arguments] [code]\n", argv[0]);
			printf("红外遥控代码发送工具\n");
			printf("-r                           接收模式\n");
			printf("-s                           发送模式\n");
			printf("-d                           设备路径\n");
			printf("-h                           帮助\n");
			printf("-v                           版本信息\n");
			break;
		}
	}
}

int main(int argc, char *argv[])
{
	if(getuid()!=0){
		printf("Not Root,\033[31m Permission denied!\033[0m\n");
		return 0;
		}
	usage(argc, argv);
	if(reset==0){
	// 以读写模式打开设备
	fd = open("/dev/lirc0", O_RDWR);
	}
	if (fd < 0)
	{
		perror("ioctl");
		return 0;
	}
	io = ioctl(fd, mode2, &mode);
	if (io < 0)
	{
		printf("ioctl:Return %d\n", io);
		return 0;
	}
	if (mode2 == LIRC_SET_SEND_MODE)
	{
		// 发送代码
		write(fd, code, strlen(code));
		// 关闭
		close(fd);
	}
	if (mode2 == LIRC_SET_REC_MODE)
	{
		//键
		int keyid;
		// 状态 先读一下防止eof
		int status = read(fd, &keyid, sizeof(keyid));
		// 读取
		while (status != -1)
		{
			status = read(fd, &keyid, sizeof(keyid));
			printf("Key:%d\n", keyid);
		}
			// 关闭
			close(fd);
		}
	return 0;
}