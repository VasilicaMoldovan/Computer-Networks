#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdlib.h>
#include <string.h>

int main(){
	// s - file descriptor for the initial server socket
	// c - file descriptor for the socket we use specifically with a certain client
	// clientLength - size of the client structure (we need the pointer to this later, in a function call)
	// planes - vector we use to mark the positions of the planes (one dimensional array to make our life easier)
	int s, c, clientLength, planes[10];

	// These are the structures we use for communicating over the network
	struct sockaddr_in server, client;
	
	// INITIALISATION
	// Planes: [0,0,1,0,0,1,0,0,1,0]
	for (int pos=0; pos<10; pos++)
		if(pos%3==2)
			planes[pos] = 1;
		else
			planes[pos] = 0;


	//Creating new TCP/IP socket
	s = socket(AF_INET, SOCK_STREAM, 0);
	
	//Check if the socket was successfully created
	if (s < 0){
		printf("Socket error\n");
		return 1;
	}

	memset(&server, 0, sizeof(server)); // Empty the entire space
	server.sin_port = htons(5555);      // Set the port
	server.sin_family = AF_INET;	    // Set the IP protocol
	server.sin_addr.s_addr = INADDR_ANY;// Set the IP (accepts any IP in this case)


	//Check if the bind is successful
	if(bind(s, (struct sockaddr*) &server, sizeof(server))<0) {
		printf("Bind error\n");
		return 1;
	}

	// Our queue can wait for 5 clients whew
	listen(s, 5);
	clientLength = sizeof(client); // We need a pointer to this
	
	memset(&client, 0, sizeof(client)); // Store the client info here, we leave the memory nice and empty
	
	// Looooooop
	while (1) {
		printf ("Listening...\n");

		// c is basically kind of a new socket, a channel we use specifically with a certain client 
		c = accept(s, (struct sockaddr*) &client, &clientLength);

		// Check if it's all good, if the client successfully connected
		if (c < 0) {
			printf("Accept error\n");
			// DON'T FORGET THE EXIT
			exit(1);
		}

		// AND NOW IT'S THE SHITTY FUN PART - we do the fork()
		// The following part only happens for the CHILD PROCESS, because of the lovely if condition
		if (fork()==0) 
		{
			// We close the previous socket, it's kind of a duplicate, we don't need it
			close(s);
			
			// This is a print to see who we're handling right now
			printf("Playing with: %s\n", inet_ntoa(client.sin_addr));
			
			// uint16_t is a standard short int format, so that we don't fuck up the data over the network
			uint16_t plane, win, hit = 0;

			while(hit == 0)
			{
				// Here we receive the guessed position
				recv(c, &plane, sizeof(plane), MSG_WAITALL);
				plane = ntohs(plane);
			
				printf("Player %s chose %hu\n", inet_ntoa(client.sin_addr), plane);
			
				char messageWin[] = "You won!";
				char messageLose[] = "Try again!";
				
				// Here we check the guessed position and send the appropriate message
				if (planes[plane] == 1)
				{
					hit = 1;
					win = htons(hit);
					send(c, &win, sizeof(win), 0);
					send(c, &messageWin, sizeof(messageWin), 0);
					printf("Player %s won!!!\n", inet_ntoa(client.sin_addr));
				}

				else
				{
					win = htons(hit);
					send(c, &win, sizeof(win), 0);
					send(c, &messageLose, sizeof(messageLose), 0);
				}
		
			}

			// We close our socket, so that we don't leave it hanging
			close(c);
			// We're in the child, let's exit here. Unholy stuff happened when I forgot this
			exit(0);
		}
	}

	return 0;
}
	

