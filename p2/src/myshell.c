#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

int print_prompt() {  // Prints the prompt for the user.
	printf("@> ");
	return 1;
}

int get_command(char *command) {  // Reads in the input from the user or a file.	
	if(fgets(command, 4096, stdin) != NULL) {
		int len = strlen(command);
		
		if(command[len-1] != '\n') {
			//fprintf(stderr, "command too long\n");
		}
		command[strcspn(command, "\n")] = 0;
		return 1;
	}
	else {	
		exit(0); // Exit if EOF.
	}
}

int deleteNonEmptyDirectory(const char *path) { // Recursively traverse through the path to remove a non empty directory.
	DIR *dir = opendir(path);
	size_t pathLength = strlen(path);
	int r = -1;

	if(dir) {
		struct dirent *d;
		r = 0;

		while(!r && (d = readdir(dir))) {
			int r2 = -1;
			char *buf;
			size_t length;

			if(!strcmp(d->d_name, ".") || !strcmp(d->d_name, "..")) {  // Ignore these in the directory.
				continue;
			}

			length = pathLength + strlen(d->d_name) + 2;
			buf = malloc(length);

			if(buf) {
				struct stat statbuf;

				snprintf(buf, length, "%s/%s", path, d->d_name);
				if(!stat(buf, &statbuf)) {
					if(S_ISDIR(statbuf.st_mode)) {
						r2 = deleteNonEmptyDirectory(buf);
					}
					else {
						r2 = unlink(buf);
					}
				}
				free(buf);
			}
			r = r2;
		}
		closedir(dir);
	}
	if(!r) {
		r = rmdir(path);
	}
	return r;
}

int process_command(char* command) { // Applies logic to the command read in by get_command().
	char cwd[256];
	const char delim[2] = " ";
	char* token;
	char cpy[4096];
	char cpy2[4096];
	char hist[4096];
	char histcpy[4096];

	strcpy(cpy, command);

	strcpy(cpy2, command);
	strcat(cpy2, "&");
	
	strcat(hist, cpy2);
	strcpy(histcpy, hist);
	

	token = strtok(command, delim);

	if(strcmp(command, "exit") ==0) { // If command is "exit" then exit the program.
		exit(0);
	}

	else if(strcmp(command, "author") ==0){  // If command is "author" then output the author's info.
		printf("Name:  Tyler LaBreck\n");
	}

	else if(strcmp(command, "cdir") == 0) {  // If command is "cdir" then output current directory.

		int count = 0;

		while(token != NULL) {
			if (strcmp(token, "cdir") != 0) {  
				if(chdir(token) != 0) {
					perror("cdir");
				}
			}
			token = strtok(NULL, delim);
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

	else if(strcmp(command, "create") == 0) {  // If command is "create" then create a file or directory.
		

		int count = 0;
		struct stat statbuf;

		while(token != NULL) {	
			if(strcmp(token, "-d") == 0) { 
				while(token != NULL) {
					if(strcmp(token, "-d") != 0) {
						if(mkdir(token, 0777) != 0) {
							perror("create");
						}
					}
					token = strtok(NULL, delim);
					count++;
				}
				if(count == 2) {
					fprintf(stderr, "Usage: create [-d] name\n");
				}	
			}
			token = strtok(NULL, delim);
			count++;	
		}
		if(count == 2) {
			token = strtok(cpy, delim);
			while(token != NULL) {
				if(strcmp(token, "create") != 0) {
					if(stat(token, &statbuf) == 0) {
						fprintf(stderr, "create: File exists\n");
					}
					else {
						FILE *fp;
						fp = fopen(token, "w+");
						fclose(fp);
					}					
				}
				token = strtok(NULL, delim);
			}	
		}
		if(count == 1 || count > 4) {
			fprintf(stderr, "usage: create [-d] name\n");
		}	
	}

	else if(strcmp(command, "delete") == 0) {  // If command is "delete" then delete a file or directory.
		

		int count = 0;
		struct stat statbuf;

		while(token != NULL) {
			if(strcmp(token, "-r") == 0) {			
				while(token != NULL) {
					if(strcmp(token, "-r") != 0) {
						deleteNonEmptyDirectory(token);
					}
					token = strtok(NULL, delim);
				}
			}
			else if((stat(token, &statbuf) == 0) && strcmp(token, "delete") != 0) {
				if(S_ISDIR(statbuf.st_mode) != 0) {
					if(rmdir(token) != 0) {
						perror("delete");
					}
				}
				else {
					if(remove(token) != 0) {
						perror("delete");
					}
				}				
			}
			token = strtok(NULL, delim);
			count++;
		}
		if(count == 1) {
			fprintf(stderr, "usage: delete [-r] name\n");
		}
	}

	else if(strcmp(command, "list") == 0) {  // If command is "list" then list the contents of the current file system.
		

		struct dirent *d;
		
		DIR *dir = opendir(".");

		if(dir == NULL) {
			perror("list");
			return 0;
		}

		while((d = readdir(dir)) != NULL) {
			printf("%s\n", d->d_name);
		}
		closedir(dir);
	}

	else if(strcmp(command, "hist") == 0) {  // If command is "hist" then show the history of commands executed by the shell.
		int histCount = 0;

		while(token != NULL) {
			if(strcmp(token, "-c") == 0) {
				hist[0] = '\0';
				histcpy[0] = '\0';
				histCount++;
			}
			token = strtok(NULL, delim);
		}
		
		char *token2;
		const char delim2[2] = "&";

		if(histCount == 0) {
			token2 = strtok(hist, delim2);

			while(token2 != NULL) {
				printf("%s\n", token2);
				token2 = strtok(NULL, delim2);
			}
		}
	}

	else {  // If the command is not in our list of commands then show following message.
		printf("Unrecognized command:");
		while(token != NULL) {
			printf(" %s", token);

			token = strtok(NULL, delim);
		}
		printf("\n");
	}
	return 1;
}

char command[4096];

int main(int argc, char *argv[]) {
	bool echo = false;

	if(argc >= 2 && strcmp(argv[1], "--echo") == 0) {
		echo = true;
	}

	while(1) {
		print_prompt();
		
		get_command(command);

		if(echo == true) {
			printf("%s\n", command);
		}

		process_command(command);
	}
}
