#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <ftw.h>

int print_prompt() {
	printf("@> ");
	return 1;
}

int get_command(char *command) {	

	if(fgets(command, 4096, stdin) != NULL) {
		int len = strlen(command);
		
		if(command[len-1] != '\n') {
			//fprintf(stderr, "command too long\n");

		}

		command[strcspn(command, "\n")] = 0;
		return 1;
	}
	else {
		exit(0);
	}
}

int deleteNonEmptyDirectory(const char *path) {
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

			if(!strcmp(d->d_name, ".") || !strcmp(d->d_name, "..")) {
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

int process_command(char* command) {
	char cwd[256];
	const char delim[2] = " ";
	char* token;
	char cpy[4096];

	strcpy(cpy, command);

	token = strtok(command, delim);

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

	else if(strcmp(command, "create") == 0) {
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

	else if(strcmp(command, "delete") == 0) {
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

	else if(strcmp(command, "list") == 0) {
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

	else {
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


