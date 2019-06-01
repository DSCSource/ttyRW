#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>
#include "tty_select.h"
#include "tty_heart.h"

#define APPLICATION_VERSION 0   //当前程序版本
#define SERVICE_NUM         0   //设备号
#define TTY_TIMEOUT         10  //超时设置

#define MAX_DATA_LENGTH   1024  //接收的缓存数据大小
#define MAX_PACKET_SIZE   256   //解析一个接收到的帧的最大大小
#define CHECK_DATA_LENGTH 16    //CHECK/STARTUP发送数据缓存大小
#define MIN_DATA_LENGTH   8     //接收到的最小数据包长度
#define HEAD_DATA_LENGTH  4     //数据包头长度


#define SYNC_DATA         0x5A  //数据帧同步数据
#define TTY_INIT_CHECK    0x01  //从设备检测命令
#define TTY_INIT_RESPONSE 0x02  //从设备返回检测命令
#define TTY_STARTUP       0x03
#define TTY_READY_STATUS  0x04
#define TTY_READ_REQUEST  0x05
#define TTY_READ_ACK      0x06
#define TTY_WRITE_REQUEST 0x07
#define TTY_WTITE_ACK     0x08
#define TTY_STATUS_REPORT 0x09  //

int main(int argc, char *argv[])
{
  printf("__DBG__ init tty and select pthread !\n");
  start_tty_select();

  printf("__DBG__ init heart pthread !\n");
  start_tty_heart();

  printf("__DBG__ wait exit !\n");
  wait_for_tty_heart_exit();

  wait_for_tty_select_exit();
  return 0;
}
