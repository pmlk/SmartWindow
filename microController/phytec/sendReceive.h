#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// networking
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
 
#include "net/gnrc.h"
#include "net/gnrc/ipv6.h"
#include "net/gnrc/udp.h"
#include "net/gnrc/pktdump.h"

// riot
#include "board.h"
#include "periph/gpio.h"
#include "shell.h"
#include "thread.h"

// parameters
#define PP_BUF_SIZE         (64)
#define PP_MSG_QUEUE_SIZE   (8U)
//#ifdef IN
	#define PP_PORT             (6414)
//#else
	//#define PP_PORT 			(6415)
//#endif
// smartwindow

#define SW_CHANNEL			26
#define SW_PAN_ID			0x23

// function prototypes
int pp_send(char *addr_str, char *data);
//static void start_receiver(void);
//static void *_receiver(void *arg);

// smartWindow functions
//static int sw_send(int argc, char **argv);
int sw_network_init(void);

