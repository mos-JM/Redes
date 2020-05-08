/*
 * Socket client-server UDP
 * @autor Agustin Jofre Millet
*/

#include <sys/types.h>
       #include <stdio.h>
       #include <stdlib.h>
       #include <unistd.h>
       #include <string.h>
       #include <sys/socket.h>
       #include <netdb.h>
 #include <time.h>


       #define BUF_SIZE 500

       int
       main(int argc, char *argv[])
       {
           struct addrinfo hints;
           struct addrinfo *result, *rp;
           int sfd, s;
           struct sockaddr_storage peer_addr;
           socklen_t peer_addr_len;
           ssize_t nread;
           char buf[BUF_SIZE];
	    time_t tiempo;
	    struct tm *info;
	    size_t buf_len;
	    
           if (argc != 2) {
               fprintf(stderr, "Usage: %s port\n", argv[0]);
               exit(EXIT_FAILURE);
           }

           memset(&hints, 0, sizeof(struct addrinfo));
           hints.ai_family = AF_INET6;    /* Allow IPv4 or IPv6 con AF_INET6 aceptamos dir ip4 y ip6   AF_UNSPEC */
           hints.ai_socktype = SOCK_DGRAM; /* Datagram socket */
           hints.ai_flags = AI_PASSIVE;    /* For wildcard IP address */
           hints.ai_protocol = 0;          /* Any protocol */
           hints.ai_canonname = NULL;
           hints.ai_addr = NULL;
           hints.ai_next = NULL;

           s = getaddrinfo(NULL, argv[1], &hints, &result);
           if (s != 0) {
               fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
               exit(EXIT_FAILURE);
           }

           /* getaddrinfo() returns a list of address structures.
              Try each address until we successfully bind(2).
              If socket(2) (or bind(2)) fails, we (close the socket
              and) try the next address. */

           for (rp = result; rp != NULL; rp = rp->ai_next) {
               sfd = socket(rp->ai_family, rp->ai_socktype,
                       rp->ai_protocol);
               if (sfd == -1)
                   continue;

               if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0)
                   break;                  /* Success */

               close(sfd);
           }

           if (rp == NULL) {               /* No address succeeded */
               fprintf(stderr, "Could not bind\n");
               exit(EXIT_FAILURE);
           }

           freeaddrinfo(result);           /* No longer needed */

           /* Read datagrams and echo them back to sender */

           for (;;) {
               peer_addr_len = sizeof(struct sockaddr_storage);
               nread = recvfrom(sfd, buf, BUF_SIZE, 0,
                       (struct sockaddr *) &peer_addr, &peer_addr_len);
               if (nread == -1)
                   continue;               /* Ignore failed request */
	      
	      switch(buf[0]){
		  case 'h': 
		    time(&tiempo);
		    info = localtime(&tiempo);
		    strftime(buf, BUF_SIZE, "%X - %I:%M%P", info);
		    break;
		  case 'd':
		    time(&tiempo);
		    info = localtime(&tiempo);
		    strftime(buf, BUF_SIZE, "%A ,%d/%B/%Y", info);
		    break;
		  case 'q':
		    
		     close(sfd);
		     exit(0);
		  
		    break;

	      }
	       
	       char host[NI_MAXHOST], service[NI_MAXSERV];

               s = getnameinfo((struct sockaddr *) &peer_addr,
                               peer_addr_len, host, NI_MAXHOST,
                               service, NI_MAXSERV, NI_NUMERICSERV | NI_NUMERICHOST);
              if (s == 0)
                   printf("Received %zd bytes from %s:%s\n",
                           nread, host, service);
               else
                   fprintf(stderr, "getnameinfo: %s\n", gai_strerror(s));

	       buf_len = strnlen(buf, BUF_SIZE);
               if (sendto(sfd, buf, buf_len, 0,
                           (struct sockaddr *) &peer_addr,
                           peer_addr_len) != buf_len)
                   fprintf(stderr, "Error sending response\n");
           }
       }



