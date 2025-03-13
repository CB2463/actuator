#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pigpio.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define IN1_PIN 23
#define IN2_PIN 24
#define PWM_PIN 18

void init_gpio();

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUFFER_SIZE];
    socklen_t addr_len = sizeof(client_addr);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
	    perror("FAil MAKING SOCKET");
	    exit(1);
    }
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
	    perror("FAIL BINDING");
	    exit(1);
    }

    if (listen(server_fd, 5) == -1) {
	    perror("FAIL LISTENING");
	    exit(1);
    }

    client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
    if (client_fd == -1) {
	    perror("FAIL client connection");
	    exit(1);
    }

    printf("CLIENT connected: %s\n", inet_ntoa(client_addr.sin_addr));

    init_gpio();
    while(1){    
	    memset(buffer, 0, BUFFER_SIZE);
	    read(client_fd, buffer, BUFFER_SIZE);
    	    printf("RCV MESSAGE: %s\n", buffer);
	   
	    if(strncmp(buffer, "open", 4) == 0){
	    	gpioWrite(IN1_PIN, 1);
		gpioWrite(IN2_PIN, 0);
		gpioPWM(PWM_PIN, 128);
	    }


	    if(strncmp(buffer,"close", 5) == 0){    
		gpioWrite(IN1_PIN, 0);
		gpioWrite(IN2_PIN, 1);
		gpioPWM(PWM_PIN, 128); 
    	    }

	    if(strncmp(buffer,"off", 3) == 0){
		close(client_fd);
	    	break;
	    }

    }
    
    close(server_fd);

    return 0;
}

void init_gpio(){
	if(gpioInitialise() < 0){
		printf("Error");
	}

	gpioSetMode(IN1_PIN, PI_OUTPUT);
	gpioSetMode(IN2_PIN, PI_OUTPUT);
	gpioSetMode(PWM_PIN, PI_OUTPUT);
	
}
