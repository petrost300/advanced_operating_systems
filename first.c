#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char *argv[]){
	
	int num_children;
	int fd;
	char *filename;
	
	//elegxos parametrwn
	if (argc < 3) {
		printf("Usage: %s <num_children> <filename>\n", argv[0]);
		exit(1);
	}
	
	num_children = atoi(argv[1]);
	filename = argv[2];
	
	//open file for writing
	fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC);
	if (fd < 0) {
		perror("Error opening file");
		exit(1);
	}
	
	//write parent pid
	dprintf(fd,"[PARENT] -> <%d>\n", getppid());
	
	for (int i = 0; i < num_children; i++){
		int pid = fork();
		if (pid ==0){
		//write children pid
			dprintf(fd,"[CHILD] -> <%d>\n", getpid());
			exit(0);
		}else {
		//wait for all children to finish
			wait(NULL);
		}
	}
	
	//close file
	close(fd);
	return 0;
	

}