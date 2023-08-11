#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/can.h>
#include <linux/can/raw.h>

int soc;
int read_can_port;

int open_port(const char *port)
{
    struct ifreq ifr;
    struct sockaddr_can addr;

    /* open socket */
    soc = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (soc < 0) {
	    printf("error!");
        return (-1);
    }
    addr.can_family = AF_CAN;
    strcpy(ifr.ifr_name, port);
    if (ioctl(soc, SIOCGIFINDEX, &ifr) < 0) {
	    printf("error!");
        return (-1);
    }
    addr.can_ifindex = ifr.ifr_ifindex;
    fcntl(soc, F_SETFL, O_NONBLOCK);
    if (bind(soc, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
	    printf("binding error!");
        return (-1);
    }

    return 0;
}

void read_port()
{
    struct can_frame frame;
    frame.can_dlc = 2;
    int recvbytes = 0;
    read_can_port = 1;
    
    while(read_can_port)
    {
        usleep(100000);
        struct timeval timeout = {1, 0};
        fd_set readSet;
        FD_ZERO(&readSet);
        FD_SET(soc, &readSet);
        if (select((soc + 1), &readSet, NULL, NULL, &timeout) >= 0) {
            if (!read_can_port) {
		        printf("error!");
                break;
            }
            if (FD_ISSET(soc, &readSet)) {
                recvbytes = read(soc, &frame, sizeof(struct can_frame));
                if(recvbytes) { 
                   /* ==================== Receive Data ====================*/
                   int disk_rpm = (frame.data[0] << 8) + frame.data[1];

                   int rpm = (int)((float)disk_rpm / 2.6);
                   printf("Car RPM: %d\t", rpm);

                   float speed = (float)rpm * 0.0034;
                   printf("Car Speed: %.3f m/s\n", speed);
                }
            }
        }
    }
}

int close_port()
{
    close(soc);
    return 0;
}


int main(void)
{
    open_port("can0");
    read_port();
   
    return 0;
}