#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#define BUFFER_SIZE 100
#define READ_END 0
#define WRITE_END 1


int main(int argc, char *argv[]) {
	
	int num_children;
	int pid;
	int fd;
	int pipe_child[2], pipe_parent[2];
	char *filename;
	char child_msg[BUFFER_SIZE];
	char parent_msg[BUFFER_SIZE];

	// Check command line arguments
	if (argc < 3) {
		printf("Usage: %s <num_children> <filename>\n", argv[0]);
		exit(1);
	}
	
	num_children = atoi(argv[1]);
	filename = argv[2];

    if (pipe(pipe_parent) == -1) {
        printf("Error creating pipe parent.\n");
        exit(1);
    }

	// Open file for writing
	fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC);
	if (fd < 0) {
		perror("Error opening file");
		exit(1);
	}
	
	// Write parent PID
	dprintf(fd, "[PARENT] -> <%d>\n\n", getppid());
	
	for (int i = 1; i <= num_children; i++) {

		char childName[20];

		if (pipe(pipe_child) == -1) {
			printf("Error creating pipe child.\n");
			exit(1);
    	}
		
		pid = fork();

		if (pid == 0) {

			// Child process
			
			// Read message from parent

			read(pipe_parent[READ_END], parent_msg, BUFFER_SIZE);
			printf("CHILD <%d> message received: %s\n", getpid(), parent_msg);

			// Write PID and name to file

			sscanf(parent_msg, "Hello child, I am your father and I call you: %s", childName);
			dprintf(fd,"[CHILD] -> <%d>\n", getpid());
			dprintf(fd, "[CHILD <%d>] -> %s\n\n", getpid(), childName);

			// Send message to parent
			
			sprintf(child_msg, "Done");
			write(pipe_child[WRITE_END], child_msg, strlen(child_msg) + 1);
			printf("CHILD <%d> message sent: %s\n", getpid(), child_msg);
			
			// Close pipes

			close(pipe_child[WRITE_END]);
			close(pipe_child[READ_END]);
			close(pipe_parent[WRITE_END]);
			close(pipe_parent[READ_END]);
			
			// End child process

			exit(0);
			
		} else if (pid > 0) {

			// Parent process

			close(pipe_parent[READ_END]);

			// Send message to child

			sprintf(parent_msg, "Hello child, I am your father and I call you: Child_%d", i);
			write(pipe_parent[WRITE_END], parent_msg, strlen(parent_msg) + 1);
			printf("PARENT <%d> message sent: %s\n", getpid(), parent_msg);

			wait(NULL);
			
			// Read message from parent

			read(pipe_child[READ_END], child_msg, BUFFER_SIZE);
			printf("PARENT <%d> message received: %s\n\n", getpid(), child_msg);

			// Close pipes

			close(pipe_child[WRITE_END]);
			close(pipe_child[READ_END]);
			close(pipe_parent[WRITE_END]);

		} else {

			// Fork failed
			
			printf("Error in fork().\n");
			exit(1);
		}
	}

	// Close file
	close(fd);
	
	// Wait for child processes to finish
	for (int i = 0; i < num_children; i++) {
		wait(NULL);
	}
	
	return 0;
}

			































