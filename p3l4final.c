#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <pthread.h>

typedef int SOCKET;

SOCKET sock;
struct sockaddr_in server;
int length = sizeof(server);

char* getTime() {
    time_t rTime;
    struct tm* timeInfo;

    time(&rTime);
    timeInfo = localtime(&rTime);

    char t[9];
    strncpy(t, asctime(timeInfo) + 11, 8);
    strncpy(t + 8, "", 1);

    return t;
}

char* getDate() {
    time_t rTime;
    struct tm* timeInfo;

    time(&rTime);
    timeInfo = localtime(&rTime);

    char d[20];
    int mon = timeInfo->tm_mon;
    mon += 1;
    char buf[5];
    sprintf(buf, "%d", timeInfo->tm_mday);
    strcpy(d, buf);
    strcat(d, ":");
    sprintf(buf, "%d", mon);
    strcat(d, buf);
    strcat(d, ":");
    int year = timeInfo->tm_year;
    year += 1900;
    sprintf(buf, "%d", year);
    strcat(d, buf);
    strcat(d, "");

    return d;
}

void* thread1(void *param){
   while(1){
      	char query[] = "TIMEQUERY";
    sendto(sock, &query, sizeof(query),0, (struct sockaddr*)&server, sizeof(struct sockaddr_in));
   printf("Thread 1\n");
    sleep(3);
   }
}

void* thread2(void* param){
    while(1){
	char query[] = "DATEQUERY";
    sendto(sock, &query, sizeof(query), 0, (struct sockaddr*)&server, sizeof(struct sockaddr_in) );
   printf("Thread 2\n");
    sleep(10);
    }
}

void* thread3(void* param){
	char buff2[64]; 
	while (1) {

	printf("Thread 3\n");
	int fromlen = sizeof(struct sockaddr_in);
        recvfrom(sock, &buff2, 1024, 0, (struct sockaddr*)&server, &fromlen);
        printf("O primit\n");
	if (strcmp(buff2, "TIMEQUERY")){
           printf("1\n"); 
	    char aux[32];
            strcpy(aux, getTime());
            printf("%s\n", aux);
            sendto(sock, aux, strlen(aux), 0, (struct sockaddr*)&server, sizeof(struct sockaddr_in));
        }
        else{
	printf("2\n");
            char aux[32];
            strcpy(aux, getDate());
            printf("%s\n", aux);
            sendto(sock, aux, strlen(aux), 0, (struct sockaddr*)&server, sizeof(struct sockaddr_in));
        }
}
}

void error(char *msg){
    perror(msg);
    exit(0);
}

int main() {
    pthread_t thr[3];

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) error("Opening socket");
    int broadcast = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast)) < 0){
        error("Error broadcast option");
    }

    bzero(&server,length);
    server.sin_family=AF_INET;
    server.sin_addr.s_addr=INADDR_BROADCAST;
    server.sin_port=htons(9009);

    /*
    if (bind(sock,(struct sockaddr *)&server,length)<0)
        error("binding");
````*/

    pthread_create(&thr[0], NULL, thread1, NULL);
    pthread_create(&thr[1], NULL, thread2, NULL);
    pthread_create(&thr[2], NULL, thread3, NULL);	

    pthread_join(&thr[0], NULL);
    pthread_join(&thr[1], NULL);
    pthread_join(&thr[2], NULL);

    return 0;
}
