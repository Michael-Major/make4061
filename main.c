#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h> 
#include <unistd.h>

#include "util.h"

int n_flag = 0;
int b_flag = 0;

void show_error_message(char * lpszFileName){

	fprintf(stderr, "Usage: %s [options] [target] : only single target is allowed.\n", lpszFileName);
	fprintf(stderr, "-f FILE\t\tRead FILE as a makefile.\n");
	fprintf(stderr, "-h\t\tPrint this message and exit.\n");
	fprintf(stderr, "-n\t\tDon't actually execute commands, just print them.\n");
	fprintf(stderr, "-B\t\tDon't check files timestamps.\n");
	exit(0);

}

/* 
*  build_target does 3 things
*  1. visits each target in the targets array
*  2. calls itself recursively for each of the current target's dependencies
*  3. then performs the fork(), execvp(), and wait() process, making sure to take flags into
*     account 
*/
int build_target(target_t* targets , int nTargetCount , int index){
	
	targets[index].nStatus = RUNNING;

	int i;
	int dependency;
	int needs_update = 0;
	int doPrint = 0;
	
	//loop through each target and call build_target for each of its dependencies if that dependency exists
	//else dependency is an object file, and it has not been made yet, throw the "No rule to make target" error
	for(i = 0 ; i < targets[index].nDependencyCount ; i++){
		
		dependency = find_target(targets[index].szDependencies[i],targets,nTargetCount);
		
		if(dependency != -1){			
			if(targets[dependency].nStatus == READY){
				build_target(targets,nTargetCount,dependency);
			}
		}else{
			if(does_file_exist(targets[index].szDependencies[i]) == -1){	
				printf("make4061: *** No rule to make target '%s', needed by '%s'. Stop.\n",targets[index].szDependencies	   [i],targets[index].szTarget);
				exit(-1);
			}
		}
	}
	
	//if the -B and the -n flags have been set, just print the command for the target
	if(b_flag && n_flag){
		printf("%s\n",targets[index].szCommand);

	//otherwise, if only the -B flag has been set, run commands without checking for update times
	}else if(b_flag){
	
		pid_t pid;
		pid = fork();
		
		if(pid == -1 ){
			perror("Failed to fork!");
			exit(-1);
		}else if(pid == 0){ //child process
			printf("%s\n",targets[index].szCommand);
			execvp(targets[index].prog_args[0],targets[index].prog_args);
			printf("execvp() failed with error: %s \n", strerror(errno));
		}else{ //parent process
			
			int status;
			wait(&status);		
			
			if(WEXITSTATUS(status) != 0){	
				printf("child exited with error code = %d\n", WEXITSTATUS(status));
				exit(-1);
			}
		}

	//otherwise, if only the -n flag has been set, check for file update times and only print the commands instead of running them
	}else if(n_flag){
		
		for(i = 0 ; i < targets[index].nDependencyCount ; i++){
	
			needs_update = (compare_modification_time(targets[index].szTarget,targets[index].szDependencies[i]));
			
			//if there is no dependency file
			if(needs_update == -1){
				
				dependency = find_target(targets[index].szDependencies[i],targets,nTargetCount);
				
				if(dependency == -1){
					perror("is_file_exists returned -1 during time checking");
					exit(-1);	
				}else{
					needs_update = 2;
				}
			}else if(needs_update == 2){
				break;
			}
		}
		
		//target needs to be rebuilt
		if(needs_update == 2 || targets[index].nDependencyCount == 0){
			printf("%s\n",targets[index].szCommand);
			
		//target is up to date, don't execute any commands yet
		}else{
			doPrint = 1;
		}

	//otherwise, neither the -B nor the -n flags have been set, so check for update times and run the commands as needed
	}else{	
		
		for(i = 0 ; i < targets[index].nDependencyCount ; i++){
			
			needs_update = (compare_modification_time(targets[index].szTarget,targets[index].szDependencies[i]));
			
			//if there is no dependency file
			if(needs_update == -1){
				
				dependency = find_target(targets[index].szDependencies[i],targets,nTargetCount);
				
				if(dependency == -1){
					perror("is_file_exists returned -1 during time checking");
					exit(-1);	
				}else{
					needs_update = 2;
				}

			}else if(needs_update == 2){
				break;
			}
		}
		
		//target needs to be rebuilt
		if(needs_update == 2 || targets[index].nDependencyCount == 0){
			
			pid_t pid;
			pid = fork();
			
			if(pid == -1 ){
				perror("Failed to fork!");
				exit(-1);
			}else if(pid == 0){ //child process
				printf("%s\n",targets[index].szCommand);
				execvp(targets[index].prog_args[0],targets[index].prog_args);
				printf("execvp() failed with error: %s \n", strerror(errno));
			}else{ //parent process

				int status;
				wait(&status);
				
				if(WEXITSTATUS(status) != 0){
					printf("child exited with error code = %d\n", WEXITSTATUS(status));
					exit(-1);
				}
			}
		
		//target is up to date, don't execute any commands yet
		}else{
			doPrint = 1;
		}

	}

	targets[index].nStatus = FINISHED;
	
	return doPrint;
}

int main(int argc, char **argv) {
	target_t targets[MAX_NODES]; //List of all the targets. Check structure target_t in util.h to understand what each target will contain.
	int nTargetCount = 0;

	// Declarations for getopt
	extern int optind;
	extern char * optarg;
	int ch;
	char * format = "f:hnB";
	
	// Variables you'll want to use
	char szMakefile[64] = "Makefile";
	char szTarget[64] = "";
	int i=0;
	
	//init Targets 
	for(i=0;i<MAX_NODES;i++){
		targets[i].pid=0 ;
		targets[i].nDependencyCount = 0;
		strcpy(targets[i].szTarget, "");
		strcpy(targets[i].szCommand, "");
		targets[i].nStatus = READY;
	}

	while((ch = getopt(argc, argv, format)) != -1) {
		switch(ch) {
			case 'f':
				strcpy(szMakefile, strdup(optarg));
				break;
			case 'n':
				n_flag = 1;
				break;
			case 'B':
				b_flag = 1;
				break;
			case 'h':
			default:
				show_error_message(argv[0]);
				exit(1);
		}
	}

	argc -= optind;
	argv += optind;

	if(argc > 1){
		show_error_message(argv[0]);
		return EXIT_FAILURE;
	}

	/* Parse graph file or die */
	if((nTargetCount = parse(szMakefile, targets)) == -1) {
		return EXIT_FAILURE;
	}

	//Setting Targetname
	//if target is not set, set it to default (first target from makefile)
	if(argc == 1){
		strcpy(szTarget, argv[0]);
	}else{
		strcpy(szTarget, targets[0].szTarget);
	}

	//show_targets(targets, nTargetCount);

	//if the specified target is not a target in the makefile
	if(find_target(szTarget,targets,nTargetCount) == -1){
		printf("make4061: *** No rule to make target '%s', Stop.\n", szTarget);
		exit(-1);
	}
	
	//call build_target() with our targets array, the number of targets, and the index of the specified target
	if(build_target(targets,nTargetCount,find_target(szTarget,targets,nTargetCount))){
		printf("make4061: '%s' is up to date. \n", targets[find_target(szTarget,targets,nTargetCount)].szTarget);
	}

	return EXIT_SUCCESS;
}

