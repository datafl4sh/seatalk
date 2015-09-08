#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/select.h>
#include <signal.h>

#define BUFSZ	128

int ctrlc;

void
die(char *reason)
{
    perror(reason);
    exit(-1);
}

int
open_line(char *devname)
{
    int fd;
    
    fd = open(devname, O_RDWR | O_NOCTTY | O_NDELAY );
    if (fd == -1)
        die("open");
    
    fcntl(fd, F_SETFL, 0);
    
    return fd;
}

void
configure_port(int fd)
{
    struct termios tios;
    
    tcgetattr(fd, &tios);
    
    cfsetispeed(&tios, B4800);
    cfsetospeed(&tios, B4800);
    tios.c_cflag |= (CLOCAL | CREAD);
    tios.c_oflag = 0;
    tios.c_lflag = 0;
    tios.c_iflag = IGNBRK;
    tcsetattr(fd, TCSANOW, &tios);
}

void
ctrlc_handler(void)
{
    printf("\rDetected CTRL-C, exiting.\n");
    ctrlc = 1;
}

int
main(void)
{
    int             portfd;
    int             err;
    int             rchs;
    fd_set          readset;
    unsigned char	buffer[BUFSZ];
    
    struct sigaction	sa;
    
    ctrlc = 0;
    sa.sa_handler = ctrlc_handler;
    sa.sa_flags = 0;
    
    
    if (sigaction(SIGINT, &sa, NULL) < 0)
        die("sigaction");
    
    portfd = open_line("/dev/tty.PL2303-00002014");
    configure_port(portfd);
    
    int r;
    while (!ctrlc)
    {
        r = read(portfd, buffer, BUFSZ);
        write(1, buffer, r);
    }
    
    close(portfd);
    return 0;
}


