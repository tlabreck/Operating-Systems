#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

int print_prompt() {
	printf("@> ");
	return 1;
}

int get_command(char command[4096]) {
	if(fgets(command, 4096, stdin) != NULL) {
	int len = strlen(command);

	if(command[len-1] != '\n') {
		printf("command too long\n");
	}

	command[strcspn(command, "\n")] = 0;
	return 1;
	}
	else {
		exit(0);
	}
}

int process_command(char* command) {
	char cwd[256];
	const char s[2] = " ";
	char* token;

	token = strtok(command, s);

	if(strcmp(command, "exit") ==0) {
		exit(0);
	}

	else if(strcmp(command, "author") ==0){
		printf("Name:  Tyler LaBreck\n");
	}

	else if(strcmp(command, "cdir") == 0) {
		int count = 0;
		while(token != NULL) {
			if (strcmp(token, "cdir") != 0) {
				if(chdir(token) != 0) {
					perror("cdir");
				}	
			}
			token = strtok(NULL, s);
			count++;
		}
		if (count == 1) {
			if(getcwd(cwd, sizeof(cwd)) == NULL) {
				perror("cdir: No such file or directory");		
			}
			else {
				printf("%s\n", cwd);
			}
		}
	}
	else {
		printf("Unrecognized command:  %s\n", command);
	}
	return 1;
}

int main(int argc, char *argv[]) {
	char command[4096];
	bool echo = false;

	if(argc >= 2 && strcmp(argv[1], "--echo") == 0) {
		echo = true;
	}
	if (argc >= 2) {
		
	}

	while(1) {

		print_prompt();
		get_command(&command);
		if(echo == true) {
			printf("%s\n", command);
		}
		process_command(command);
	}
}


