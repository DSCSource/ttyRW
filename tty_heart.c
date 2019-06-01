#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "tty_select.h"
#include "tty_heart.h"

#define MAX_BUFFER_SIZE 128
static pthread_t pth_id;

static void * tty_heart_thread(void *param)
{
  char buf[MAX_BUFFER_SIZE];
  while(1)
  {
    sleep(5);
    memset(buf, 0, MAX_BUFFER_SIZE);
    strcpy(buf, "MAX_BUFFER_SIZE");
    send_data(buf, strlen(buf));
  } 
  return NULL;
}


int start_tty_heart(void)
{
  if(pthread_create(&pth_id, NULL, tty_heart_thread, NULL)<0)
  {
    printf("__DBG__ create pthread failed !\n");
    return -1;
  }
  return 0;
}

void wait_for_tty_heart_exit(void)
{
  pthread_join(pth_id, NULL);
}

