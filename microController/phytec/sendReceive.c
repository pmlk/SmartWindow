#include "sendReceive.h"

int pp_send(char *addr_str, char *data)
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
		printf("Error: unable to parse destination address %s", addr_str);
        //puts("Error: unable to parse destination address");
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

int sw_network_init(void)
{
	//set channel, pan_id
	//https://github.com/smlng/nodes/blob/gnrc/plant_node/network.c

    kernel_pid_t ifs[GNRC_NETIF_NUMOF];
    uint16_t channel = SW_CHANNEL;
    uint16_t pan_id = SW_PAN_ID;
    
    //gnrc_netif_init();

    if (0 == gnrc_netif_get(ifs)) {
        puts ("[smartWin_network_init] ERROR: failed to get ifaces!");
        
        return -1;
    }
    printf("ifs: %d\n", ifs[0]);
    //printf("num: %d",num);
    if (0 > gnrc_netapi_set(ifs[0], NETOPT_CHANNEL, 0, (uint16_t *)&channel, sizeof(uint16_t))) {
        puts ("[smartWin_network_init] ERROR: failed to set channel!");
        return -1;
    }
    if (0 > gnrc_netapi_set(ifs[0], NETOPT_NID, 0, (uint16_t *)&pan_id, sizeof(uint16_t))) {
        puts ("[smartWin_network_init] ERROR: failed to set pan_id!");
        return -1;
    }
    
    printf("initialized network channel: %i pan_id: %x\n", channel, pan_id);
    
	return 0;	
}


