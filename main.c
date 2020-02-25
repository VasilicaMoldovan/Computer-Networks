#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

struct sockaddr_in server, client;
int s;

void tratare() {
    uint16_t n, count;
    int clientLength = sizeof(client);

    n = 0;
    // Receive the length of the string to be sent; also store the client address and stuff in &client
    recvfrom(s, &n, sizeof(n), MSG_WAITALL, (struct sockaddr *) &client, &clientLength);

    // Convert the little shit
    n = ntohs(n);

    // Memory allocation!!!
    char *message = (char *) malloc(sizeof(char) * n);


    // Receive the longer shit
    recvfrom(s, message, sizeof(message[0]) * n, MSG_WAITALL, (struct sockaddr *) &client, &clientLength);

    // Initialize the counter
    count = 0;

    // Parse the longer shit and count the spaces
    for (int i = 0; i < n; i++) {
        if (message[i] == ' ')
            count = count + 1;
    }

    // Convert the counter to network standard blah blah
    count = htons(count);

    // Send the counter back to the client, boom
    sendto(s, &count, sizeof(count), 0, (struct sockaddr *) &client, clientLength);


    free(message);

    exit(0);
    // Terminam procesul fiu - foarte important!!! altfel numarul de procese creste exponential.
    // Fiul se termina dupa ce deserveste clientul.
}


int main() {


    // Create socket - DGRAM = DATAGRAM - specific to UDP connections
    s = socket(AF_INET, SOCK_DGRAM, 0);

    // Check the creation of the socket
    if (s < 0) {
        printf("Socket error\n");
        return 1;
    }

    // Do the address structure usual shit
    memset(&server, 0, sizeof(server));
    server.sin_port = htons(1234);
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;


    // Bind the server to the socket
    if (bind(s, (struct sockaddr*) &server, sizeof(server)) < 0) {
        printf("Bind error\n");
        return 1;
    }
    //memset(&client, 0, sizeof(client));

    while(1) {
        memset(&client, 0, sizeof(client));
        if (fork() == 0) // server concurent, conexiunea va fi tratata de catre un proces fiu separat
            tratare();
    }
    close(s);


}
