
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

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>

// riot
//#include "board.h"
//#include "periph/gpio.h"
//#include "shell.h"
//#include "thread.h"

//#include "UDPConnection.h"

#define PORT 9998
#define BUFSIZE 2048

#define MAXBUF 65536

int main()
{
   int sock;
   int status;
   struct sockaddr_in6 sin6;
   unsigned int sin6len;
   char buffer[MAXBUF];

   sock = socket(PF_INET6, SOCK_DGRAM,0);

   sin6len = sizeof(struct sockaddr_in6);

   memset(&sin6, 0, sin6len);

   /* just use the first address returned in the structure */

   sin6.sin6_port = htons(PORT);
   sin6.sin6_family = AF_INET6;
   sin6.sin6_addr = in6addr_any;

   status = bind(sock, (struct sockaddr *)&sin6, sin6len);
   if(-1 == status)
     perror("bind"), exit(1);

   status = getsockname(sock, (struct sockaddr *)&sin6, &sin6len);

   printf("%d\n", ntohs(sin6.sin6_port));

   while(status >= 0)
   {
	   status = recvfrom(sock, (void*)buffer, MAXBUF, 0,
		     (struct sockaddr *)&sin6, &sin6len);
	   printf("status: %i buffer : %s\n",status, buffer);

	   for(int i = 0; i < status; i++)
	   {
		   buffer[i] = 0;
	   }

   }

   shutdown(sock, 2);
   close(sock);
   return 0;
}



