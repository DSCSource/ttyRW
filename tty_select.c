#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "tty_select.h"
#include "tty_init.h"
#include "spinlock.h"

static int tty_fd=0;
struct tty_pthread {
  pthread_t pth_id;
  struct spinlock lock;
};
static struct tty_pthread tty_pth;
#define MAX_PACKET_SIZE 128

static int init_tty(void)
{
  tty_fd = openDev("/dev/ttyS0");  //打开的串口设备
  if(tty_fd > 0)   //
  {
    if(fcntl(tty_fd, F_SETFL, 0)<0)
    {
      printf("fcntl failed!\n");
    }

    if(set_opt(tty_fd,115200,8,1,'N')<0)    //波特率--4800 ，数据位--8， 校验位--1，停止位--‘N’
    {
      printf(" set_opt error \n");
    }
    struct termios options;
    tcgetattr(tty_fd, &options);
    options.c_cflag |= (CLOCAL | CREAD);
    options.c_lflag &= ~(ICANON | ECHO | ECHOE);
    options.c_oflag &= ~OPOST;
    options.c_cc[VMIN] = 0;
    options.c_cc[VTIME] = 100;
    options.c_iflag &= ~(INLCR | ICRNL | IGNCR);
    options.c_oflag &= ~(ONLCR | OCRNL);
    tcsetattr(tty_fd, TCSANOW, &options);
  } 
}

static void * tty_select_thread(void *param)
{
  printf("__DBG__ tty_select_thread !\n");
  int *ttyfd=(int*)param;
  int ret=0;
  char t_tmpbuf[MAX_PACKET_SIZE];
  fd_set tty_fds;
  struct timeval tv;
  printf("__DBG__ init pthread end !\n");
  while(1){
    FD_ZERO(&tty_fds);
    FD_SET(*ttyfd, &tty_fds);
    FD_SET(STDIN_FILENO, &tty_fds);
    
    tv.tv_sec=3;
    tv.tv_usec=0;
    ret = select(FD_SETSIZE, &tty_fds, NULL, NULL, &tv);
    if(ret == -1) {
      sleep(2);
      continue;
    }
    else if(ret==0) { // timeout
      //printf("__DBG__ time out !\n");
      continue;
    }
    if(FD_ISSET(*ttyfd,&tty_fds))       //接受串口发送过来的数据
    {
      memset(t_tmpbuf, '\0',MAX_PACKET_SIZE);
      ret = read(*ttyfd, t_tmpbuf, MAX_PACKET_SIZE);
      if(ret < 0){
        printf("errno == EAGAIN \n");
        if (ret == EAGAIN) {
          continue;
        }
      }
      printf("read size == %d \n",ret);   //输出接受到的字节大小。
    }
    if(FD_ISSET(STDIN_FILENO,&tty_fds))   //往串口回写数据
    {
      memset(t_tmpbuf, '\0', MAX_PACKET_SIZE);
      fgets(t_tmpbuf,MAX_PACKET_SIZE, stdin);
      if(!strncasecmp(t_tmpbuf, "exit", 4))
      {
        printf("exit test! \n");
      }
      printf("get input %s \n",t_tmpbuf);
      //int slen = str_to_hex(t_tmpbuf);
      send_data(t_tmpbuf, strlen(t_tmpbuf));  //发送数据帧
    }
  }
  return NULL;
}

void send_data(const char *pbuf, const int len)
{
  SPIN_LOCK(&tty_pth)
  write(tty_fd, pbuf, len);
  SPIN_UNLOCK(&tty_pth)
}

int start_tty_select(void)
{
  init_tty();
  SPIN_INIT(&tty_pth);
  printf("__DBG__ init tty  pthread !\n");
  if(pthread_create(&tty_pth.pth_id, NULL, tty_select_thread, &tty_fd)<0)
  {
    printf("__DBG__ create pthread failed !\n");
    return -1;
  }
  return 0;
}

void wait_for_tty_select_exit(void)
{
  pthread_join(tty_pth.pth_id, NULL);
}
