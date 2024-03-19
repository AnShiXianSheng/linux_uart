#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/select.h>
#include <termios.h>    //Linux下串口驱动头文件

int UART_Recv(int fd, char *rcv_buf, int data_len, int timeout)
{
    int iCount;
    int fs_sel;
    fd_set fs_read;
    struct timeval time;

    time.tv_sec = timeout/1000;
    time.tv_usec = timeout%1000*1000;

    FD_ZERO(&fs_read);      //清空串口接收端口集
    FD_SET(fd, &fs_read);   //设置串口接收端口集
    fs_sel = select(fd+1, &fs_read, NULL, NULL, &time);
    if(fs_sel){
        iCount = read(fd, rcv_buf, 100);
        return iCount;
    }
}

int UART_Send(int fd, char *send_buf, int data_len)
{
    ssize_t ret = 0;
    
    ret = write(fd, send_buf, data_len);    //str的长度是12
    if(ret==data_len){
        printf("send data is %s\n", send_buf);
    }else{
        tcflush(fd, TCOFLUSH);
        return -1;
    }
}

int main(int argc, char *argv[])
{
    int fd;
    int len;
    char buff[101];
    buff[100] = '\0';
    
    fd = open("/dev/ttyS2", O_RDWR | O_NOCTTY | O_NDELAY);
    if(fcntl(fd, F_SETFL, 0) < 0){
        //阻塞--即使前面在open串口设备时设置的是非阻塞的，这里设为阻塞后，以此为准
        printf("fcntl failed\n");
    }else{
        printf("fcntl=%d\n", fcntl(fd, F_SETFL, 0));
    }
    fcntl(fd, F_SETFL, 0);      //串口阻塞

    while(1){
        tcflush(fd, TCIOFLUSH); //清掉串口缓存
        len = UART_Recv(fd, buff, 99, 10000);
        if(len>0){
            printf("I'v read %d bytes from serral, as below:\n", len);
            printf("%s\n", buff);
            UART_Send(fd, buff, len);
            len = 0;
        }
    }
    close(fd);
    return 0;
}