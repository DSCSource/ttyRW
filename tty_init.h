#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>


int openDev(const char *Dev);
void setSpeed(int fd, int speed);
