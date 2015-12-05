/**
 * written by smlng
 */
// standard
 #include <inttypes.h>
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
// network
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
#define PP_BUF_SIZE         (16)
#define PP_MSG_QUEUE_SIZE   (8U)
#define PP_PORT             (6414)

// function prototypes
static int ping(int argc, char **argv);
static int pong(char *addr_str);
static int pp_send(char *addr_str, char *data);
static void start_receiver(void);
static void *_receiver(void *arg);

int smartWin_network_init(void);

// array with available shell commands
static const shell_command_t shell_commands[] = {
    { "ping", "send multicast ping", ping },
    { NULL, NULL, NULL }
};
// static vars
static char pp_stack[THREAD_STACKSIZE_DEFAULT];
static char pp_buffer[PP_BUF_SIZE];
static msg_t pp_msg_queue[PP_MSG_QUEUE_SIZE];

/**
 * @brief the main programm loop
 *
 * @return non zero on error
 */
int main(void)
{
    // some initial infos
    puts("SmartUniversity - SmartWindow - UDP");
    puts("================");
    printf("You are running RIOT on a(n) %s board.\n", RIOT_BOARD);
    printf("This board features a(n) %s MCU.\n", RIOT_MCU);
    puts("================");



	if(smartWin_network_init() < 0)
	{
		puts("Error initializing network interface");
		exit(-1);
	}



	// start udp receiver
    start_receiver();

	//pp_send("ff02::1","moin");
	//pp_send("ff02::1","!");

    

//	while(1);

    // start shell
    puts("All up, running the shell now");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);



    // should be never reached
    return 0;
}

/**
 * @brief send a (multicast) ping
 *
 * @param[in] argc  unused
 * @param[in] argv  unused
 *
 * @return 0 on success, or 1 if failed
 */
static int ping(int argc, char **argv)
{
    puts(". send PING to [ff02::1]");
    int ret = pp_send("ff02::1", "PING");
    return ret;
}

/**
 * @brief send a (unicast) pong
 *
 * @param[in] addr_str  unicast destination address
 *
 * @return 0 on success, or 1 if failed
 */
static int pong(char *addr_str)
{
    printf(". send PONG to [%s].\n", addr_str);
    int ret = pp_send(addr_str, "PONG");
    return ret;
}

/*
 * @brief generic udp send method
 *
 * @param[in] addr_str  destination address
 * @param[in] data      payload to send
 *
 * @return 0 on success, or 1 if failed
 */
static int pp_send(char *addr_str, char *data)
{
    struct sockaddr_in6 src, dst;
    size_t data_len = strlen(data);
    uint16_t port;
    int s;
    src.sin6_family = AF_INET6;
    dst.sin6_family = AF_INET6;
    memset(&src.sin6_addr, 0, sizeof(src.sin6_addr));
    /* parse destination address */
    if (inet_pton(AF_INET6, addr_str, &dst.sin6_addr) != 1) {
        puts("Error: unable to parse destination address");
        return 1;
    }
    /* parse port */
    port = (uint16_t)PP_PORT;
    dst.sin6_port = htons(port);
    src.sin6_port = htons(port);
    s = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
    if (s < 0) {
        puts("error initializing socket");
        return 1;
    }
    if (sendto(s, data, data_len, 0, (struct sockaddr *)&dst, sizeof(dst)) < 0) {
        puts("error sending data");
    }
    close(s);
    return 0;
}

/**
 * @brief start udp receiver thread
 */
static void start_receiver(void)
{
    thread_create(pp_stack, sizeof(pp_stack), THREAD_PRIORITY_MAIN,
                     CREATE_STACKTEST, _receiver, NULL, "pingpong");
    printf(". started UDP server...\n");
}

/**
 * @brief udp receiver thread function
 *
 * @param[in] arg   unused
 */
static void *_receiver(void *arg)
{
    (void) arg;
    
    struct sockaddr_in6 server_addr;
    char src_addr_str[IPV6_ADDR_MAX_STR_LEN];
    uint16_t port;
    msg_init_queue(pp_msg_queue, PP_MSG_QUEUE_SIZE);
	int pp_socket = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
//    int pp_socket = socket(AF_INET6, SOCK_DGRAM, 0);
    /* parse port */
    port = (uint16_t)PP_PORT;
    if (port == 0) {
        puts("Error: invalid port specified");
        return NULL;
    }
    server_addr.sin6_family = AF_INET6;
    memset(&server_addr.sin6_addr, 0, sizeof(server_addr.sin6_addr));
    server_addr.sin6_port = htons(port);
    if (pp_socket < 0) {
        puts("error initializing socket");
        return NULL;
    }
    if (bind(pp_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        puts("error binding socket");
        return NULL;
    }
    printf("Success: started UDP server on port %" PRIu16 "\n", port);
    while (1) {
    	puts("waitMsg\n");
        int res;
        struct sockaddr_in6 src;
        socklen_t src_len = sizeof(struct sockaddr_in6);
        // blocking receive, waiting for data
        if ((res = recvfrom(pp_socket, pp_buffer, sizeof(pp_buffer), 0,
                            (struct sockaddr *)&src, &src_len)) < 0) {
            puts("Error on receive");
        }
        else if (res == 0) {
            puts("Peer did shut down");
        }
        else { // check for PING or PONG
            puts("checking for ping pong...\n");
            inet_ntop(AF_INET6, &(src.sin6_addr),
                      src_addr_str, sizeof(src_addr_str));

            if (strcmp(pp_buffer, "PING") == 0) {
                printf(". received PING from [%s].\n", src_addr_str);
                pong(src_addr_str);
            }
            else if (strcmp(pp_buffer, "PONG") == 0) {
                printf(". received PONG from [%s].\n", src_addr_str);
            }
            else {
                printf(". received data from [%s] with %d bytes.\n", src_addr_str, res);
                printf("received message\n");
            }
        }
    }
    return NULL;
}



int smartWin_network_init(void)
{
	
//set channel, pan_id
//https://github.com/smlng/nodes/blob/gnrc/plant_node/network.c

/**/
    kernel_pid_t ifs[GNRC_NETIF_NUMOF];
    uint16_t channel = 17;
    uint16_t pan_id = 0xbeef;

    if (0 >= gnrc_netif_get(ifs)) {
        puts ("[smartWin_network_init] ERROR: failed to get ifaces!");
        return -1;
    }
    if (0 > gnrc_netapi_set(ifs[0], NETOPT_CHANNEL, 0, (uint16_t *)&channel, sizeof(uint16_t))) {
        puts ("[smartWin_network_init] ERROR: failed to set channel!");
        return -1;
    }
    if (0 > gnrc_netapi_set(ifs[0], NETOPT_NID, 0, (uint16_t *)&pan_id, sizeof(uint16_t))) {
        puts ("[smartWin_network_init] ERROR: failed to set pan_id!");
        return -1;
    }
/**/
	return 0;	
}


