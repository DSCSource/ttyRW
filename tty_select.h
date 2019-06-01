#include <stdio.h>

void send_data(const char *pbuf, const int len);

int start_tty_select(void);

void wait_for_tty_select_exit(void);
