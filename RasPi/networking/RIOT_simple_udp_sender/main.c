/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Example application for demonstrating the RIOT network stack
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "shell.h"
#include "msg.h"

#include "net/ipv6/addr.h"
#include "net/gnrc/ipv6/netif.h"
#include "net/gnrc/netif.h"
#include "net/gnrc/netapi.h"
#include "net/netopt.h"

#define MAIN_QUEUE_SIZE     (8)
//static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

extern int udp_cmd(int argc, char **argv);
static const shell_command_t shell_commands[] = {
    { "udp", "send data over UDP and listen on UDP ports", udp_cmd },
    { NULL, NULL, NULL }
};

extern void send(char *addr_str, char *port_str, char *data, unsigned int num,
                 unsigned int delay);

int smartWin_network_init(void);

int main(void)
{
    /* we need a message queue for the thread running the shell in order to
     * receive potentially fast incoming networking packets */
    //msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
    puts("RIOT network stack example application");

	
	char addr[] = "ff02::1";
	char port[] = "9998";
	//char data[] = "moinsen";
	
	char* msg[] = {"hi", "my", "name", "is", "Patrick", "whats", "your", "fabulous", "first", "and", "last", "name"};
	
	smartWin_network_init();
	int i = 0;
	for(i = 0; i < sizeof(msg)/sizeof(char*); i++)
	{
		smartWin_network_init();
		send(addr, port, msg[i], 1, 1000);
	}
	
    /* start shell */

	/*
    puts("All up, running the shell now");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
	*/

    /* should be never reached */
    return 0;
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
        puts ("[watr_li_network_init] ERROR: failed to get ifaces!");
        return -1;
    }
    if (0 > gnrc_netapi_set(ifs[0], NETOPT_CHANNEL, 0, (uint16_t *)&channel, sizeof(uint16_t))) {
        puts ("[watr_li_network_init] ERROR: failed to set channel!");
        return -1;
    }
    if (0 > gnrc_netapi_set(ifs[0], NETOPT_NID, 0, (uint16_t *)&pan_id, sizeof(uint16_t))) {
        puts ("[watr_li_network_init] ERROR: failed to set pan_id!");
        return -1;
    }
/**/
	return 0;	
}










