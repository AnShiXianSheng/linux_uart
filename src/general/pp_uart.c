/**
 * @file pp_uart.c
 * @brief 
 * @author simon.xiaoapeng (simon.xiaoapeng@gmail.com)
 * @version 1.0
 * @date 2022-10-18
 * 
 * @copyright Copyright (c) 2022  simon.xiaoapeng@gmail.com
 * 
 * @par 修改日志:
 *     2022/10/18 14:49 +8 创建文件
 *
 */
 
#include <stdio.h>      /*标准输入输出定义*/
#include <stdlib.h>     /*标准函数库定义*/
#include <stdint.h>	
#include <string.h>
#include <termios.h>
#include <unistd.h>     /*Unix标准函数定义*/
#include <netdb.h>
#include <poll.h>
#include <sys/types.h>  /**/
#include <sys/stat.h>   /**/
#include <fcntl.h>      /*文件控制定义*/
#include <termios.h>    /*PPSIX终端控制定义*/
#include <errno.h>      /*错误号定义*/
#include <sys/time.h>

static int baudRate_tab[] = {
	1200,2400,4800,9600, 19200, 38400, 
	57600,	115200,230400,576000,921600,1000000
};
#define SPEED_CNT	(sizeof(baudRate_tab)/sizeof(baudRate_tab[0]))

int speed_arr[SPEED_CNT] = {
	B1200,B2400,B4800,B9600, B19200, B38400, 
	B57600,	B115200,B230400,B576000,B921600,B1000000
};

static int _set_speed(int fd, int speed)
{
	uint32_t i;
	int status;
	struct termios opt;
	
	tcgetattr(fd, &opt);

	for (i= 0; i<SPEED_CNT; i++)
	{
		if (speed == baudRate_tab[i])
		{
			tcflush(fd, TCIOFLUSH);
			/*  设置串口的波特率 */
			cfsetispeed(&opt, speed_arr[i]);
			cfsetospeed(&opt, speed_arr[i]);
			status = tcsetattr(fd, TCSANOW, &opt);

			if (status != 0)
			{
				perror("tcsetattr set_speed");
				return -1;
			}
			
			return 0;
     	}
		/*清空所有正在发生的IO数据*/
		tcflush(fd, TCIOFLUSH);
   }
   
	fprintf(stderr,"Cannot find suitable speed\n");
	return -1;
}

static int _set_parity(int fd, int databits, int stopbits, int parity)
{

	struct termios options;
	if (tcgetattr(fd, &options) != 0)
	{
		perror("tcgetattr");
		return -1;
	}

	options.c_cflag &= ~CSIZE;
	switch (databits) /*设置数据位数*/
	{
		case 5:
			options.c_cflag |= CS5;
			break;
		case 6:
			options.c_cflag |= CS6;
			break;
		case 7:
			options.c_cflag |= CS7;
			break;
		case 8:
			options.c_cflag |= CS8;
			break;
		default:
			return -1;
	}
	
	switch (parity)
  	{
		case 'n':
		case 'N':
			options.c_cflag &= ~PARENB;   /* Clear parity enable */
			options.c_iflag &= ~INPCK;    /* Disnable parity checking */
			break;
		case 'o':
		case 'O':
			options.c_cflag |= (PARODD | PARENB);  /* 设置为奇效验*/ 
			options.c_iflag |= INPCK;     /* Enable parity checking */
			break;
		case 'e':
		case 'E':
			options.c_cflag |= PARENB;     /* Enable parity */
			options.c_cflag &= ~PARODD;   /* 转换为偶效验*/  
			options.c_iflag |= INPCK;       /* Disnable parity checking */
			break;
		default:
			return -1;
	}

	/* 设置停止位*/   
	switch (stopbits)
  	{
		case 1:
			options.c_cflag &= ~CSTOPB;
			break;
		case 2:
			options.c_cflag |= CSTOPB;
			break;
		default:
			fprintf(stderr,"Unsupported stop bits\n");
			return -1;
	}
	
	/* 若以O_NONBLOCK 方式open，这两个设置没有作用，等同于都为0 */
	/* 若非O_NONBLOCK 方式open，具体作用可参考其他博客，关键词linux VTIME */
    //options.c_cc[VTIME] = 1; // 100ms
    //options.c_cc[VMIN] = 0xff;  ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
	//							  ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON)

	/* 清空正读的数据，且不会读出 */
	tcflush(fd,TCIFLUSH); 
	
	/*采用原始模式通讯*/
	options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	options.c_oflag &= ~(OPOST | ONLCR | OCRNL | ONOCR | ONLRET);
	options.c_iflag &= ~( IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IXON | IXOFF | IXANY | ICRNL | IGNCR);
	
	//options.c_iflag &= ~(BRKINT | ICRNL | ISTRIP | IXON);
	/*解决发送0x0A的问题*/
	//options.c_iflag &= ~(INLCR | ICRNL | IGNCR);
	//options.c_oflag &= ~(ONLCR | OCRNL | ONOCR | ONLRET);
	//options.c_iflag &= ~(ICRNL | IXON);

	/* Update the options and do it NOW */
	if (tcsetattr(fd, TCSANOW, &options) != 0)
  	{
  		perror("SetupSerial 3");
		return -1;
	}

	return 0;
}

/**
 * @brief   打开串口
 * @param  uart_name        串口名称
 * @param  speed            波特率 1200,2400,4800,9600,19200, 38400,57600,115200,230400
 * @param  databits         数据位
 * @param  stopbits         停止位
 * @param  parity           校验位
 * @return int 
 */
int uart_Open(const char *uart_name, int speed, int databits, int stopbits, int parity)
{
    int uart_fd;

    /* 设置波特率 */
    uart_fd = open(uart_name, O_RDWR|O_NOCTTY|O_NONBLOCK|O_CLOEXEC);
	if(uart_fd < 0)
	{
		return -1;
	}
    
    if( _set_speed(uart_fd, speed) ||
        _set_parity(uart_fd, databits, stopbits, parity) )
    {
        close(uart_fd);
        return -1;
    }
    return uart_fd;
}

/**
 * @brief 关闭串口
 * @param fd   文件描述符
 */
void uart_Close(int fd)
{
    close(fd);
}

/**
 * @brief 串口写
 * @param  fd               文件描述符
 * @param  data             数据缓冲区指针
 * @param  data_len         数据长度
 * @return ssize_t 
 */
ssize_t uart_Write(int fd, const void *data, size_t data_len)
{
	return write(fd, data, data_len);
}

/**
 * @brief 	读串口
 * @param  fd               文件描述符
 * @param  data_buf         数据缓冲区指针
 * @param  buf_size         数据缓冲大小
 * @param  timeout          当缓冲区未满，且timeout毫秒无数据时就返回
 * @return ssize_t 			成功返回读到的字节数，超时返回0，错误返回负数
 */
ssize_t uart_Read(int fd,void *data_buf, size_t buf_size, int timeout)
{
	struct pollfd fdset;
	int ret;
	ssize_t rn;
	char *read_p = (char *)data_buf;
	char *end_p = read_p + buf_size;

	fdset.fd = fd;
	fdset.events = POLLIN;
	while(end_p - read_p > 0){
		ret = poll(&fdset, 1, timeout);
		if (ret < 0)
			return ret;
		if(ret == 0)
			break;
		if(fdset.revents & POLLIN){
			rn = read(fd, read_p, end_p-read_p);
			if(rn < 0)
				return rn;
			if( rn == 0)
				break ;
			read_p += rn;
		}
	}
	return  read_p - (char*)data_buf;
}

/**
 * @brief 清空输入缓冲区的数据
 * @param  fd               文件描述符
 */
void uart_InClean(int fd){
	tcflush(fd, TCIFLUSH);
}

/**
 * @brief 清空输出缓冲区的数据
 * @param  fd               文件描述符
 */
void uart_OutClean(int fd){
	tcflush(fd, TCOFLUSH);
}



