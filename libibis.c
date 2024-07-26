#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#define DISPLAY_BUF_LEN 48
void sendstring(char *string, void *tty) {

  int i;
  unsigned char parity;
  parity = 0x7F;
  for (i = 0; string[i]; i++) {
    putc(string[i], tty);
    parity ^= string[i];
  }
  putc(0x0D, tty);
  parity ^= 0x0D;
  putc(parity & 0x7F, tty);
}
extern void setdisplay(char *text, void *tty) {
  char cmdbuf[DISPLAY_BUF_LEN];
  int i;
  snprintf(cmdbuf, DISPLAY_BUF_LEN - 1, "zI%d%s%s", (strlen(text) + 3) / 4,
           text, "   " + ((strlen(text) + 3) % 4));
  sendstring(cmdbuf, tty);
  fflush(tty);
}

// void laufschrift(char *string, int num) {
//   int i;
//   char outbuf[DISPLAY_BUF_LEN];
//   while (1) {
//     for (i = 0; i < strlen(string) - num + 1; i++) {
//       strncpy(outbuf, string + i, num);
//       outbuf[num] = 0;
//       setdisplay(outbuf);
//       fflush(stdout);
//       //         putchar('\n');
//       //         sleep (10);
//       system("sleep 1");
//       //         break;
//     }
//     //      break;
//   }
// }

extern void *prepareDevice(void *device) {
  FILE *tty = fopen(device, "w");
  if (!tty) {
    fprintf(stderr, "Failed to open /dev/ttyUSB \n");
  }
  int modemstate;
  struct termios options;
  ioctl(fileno(tty), TIOCMGET, &modemstate);
  tcgetattr(fileno(tty), &options);
  cfsetispeed(&options, B1200);
  cfsetispeed(&options, B1200);
  options.c_cflag &= ~(CSIZE | CRTSCTS | PARODD | HUPCL);
  options.c_cflag |= (CS7 | PARENB | CSTOPB);
  //   options.c_lflag&=~(ICANON | ECHO | ECHOE | ISIG);
  options.c_oflag &= ~OPOST;
  tcsetattr(fileno(tty), TCSANOW, &options);
  return tty;
}