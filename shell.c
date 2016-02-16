#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>

void sig_handler(int signal) {
  int status;
  int result = wait(&status);

  printf("Wait returned %d\n", result);
}
void sig_handler2(int signal) {
    printf("child pid:%d\n",getpid());
    printf("parent pid:%d\n",getppid());

    
            printf("killing process %d\n",getpid());
    exit(0);

            
    
}



void execute(char **argv, int background)
{
	pid_t pid;
	int status;
	int i=0;
	int output=0;
	int input=0;
	int flag1 = 0;
	int flag2 = 0;
	FILE *fp1, *fp2;
	char *file1;
	char *file2;
	char *argd[8];

	//Loop through the arguments and check for either a > or a <

	 while(argv[i] != NULL)
     { 
            //flag1 signifies if a > was found
		    if(flag1 == 1)
			{
				//The next element is the file name
				file1 = (char*)malloc(sizeof(char) * 15);
				strcpy(file1, argv[i]);
				flag1=0;
				
			
			}

			//flag2 signifies if a < was found
			if(flag2 == 1)
			{
				file2 =(char*)malloc(sizeof(char) * 15);
				
				strcpy(file2, argv[i]);
				flag2=0;
			}
			    

            if((strcmp(">", argv[i])) ==0)
			{
				output = 1;
				flag1 = 1;
				printf("HI\n");
				
				
			}
           
            else if((strcmp("<", argv[i])) ==0)
			{
                input = 1;
				flag2 = 1;
			}

			++i;
	 }
	 i=0;

	 //If both a > < were found, then get both the file names

	 if(output == 1 && input == 1)
	 {
		 while((strcmp(">", argv[i])) != 0 && (strcmp("<", argv[i])) != 0)
		 {
			 argd[i] = (char*)malloc(sizeof(char) * 15);
			 strcpy(argd[i], argv[i]);
			 ++i;
			 printf("Enter\n");
		 }
         argd[i] = NULL;
		 
         strcat(file1, ".txt");
		 strcat(file2, ".txt");

		 printf("test:%s\n", file1);
		 printf("test:%s\n", file2);

		 //redirect stdout to file1 and stdin to file2

         fp1 = freopen(file1, "w", stdout);
		 fp2 = freopen(file2, "r", stdin);

	 }



    else if(output == 1)
	{

    
	while((strcmp(">", argv[i])) != 0)
	{
		printf("HIs\n");
		argd[i] = (char*)malloc(sizeof(char) * 15);
		strcpy(argd[i], argv[i]);
		printf("COPY\n");
		printf("test:%s\n", argd[i]);
	    ++i;
	}
	argd[i] = NULL;
	printf("%s\n", argv[2]);

	strcat(file1, ".txt");

	printf("%s\n", file1);

	fp1 = freopen(file1, "w", stdout);
	   
	}

	else if(input == 1)
	{

    
	while((strcmp("<", argv[i])) != 0)
	{
		argd[i] = (char*)malloc(sizeof(char) * 15);
		strcpy(argd[i], argv[i]);	
	    ++i;
	}
	argd[i] = NULL;
	strcat(file2, ".txt");
	fp2 = freopen(file2, "r", stdin);
	}
    pid = fork();

	if(pid == -1)
	{
		
		perror("fork");
	    exit(1);
	}

	else if(pid == 0)
	{
		signal(SIGINT, sig_handler2);
	   
	   if(output == 0 && input == 0)
	   {
		if(execvp(*argv, argv)==-1)
		{
			printf("Invalid command\n");
			exit(1);
		}
	   }
	   else
	   {
		   printf("other\n");
		   if(execvp(*argd, argd)==-1)
		   {
			printf("Invalid command\n");
			exit(1);
		   }
	   }	 
	}
	else if(background == 0)
	{
		
		while (wait(&status) != pid)
		{
			printf("HIll\n");
		}

	}

	if(output == 1)
	{
		printf("Touch\n");
		fclose(fp1);
		freopen("/dev/tty", "a", stdout);			
		
	}
	if(input == 1)
	{		
		fclose(fp2);
		freopen("/dev/tty", "r", stdin);		
	}

}
void pipeit(char **argv)
{
	int pipefd[2];
	pid_t pid1, pid2;
	char *argv1[8];
	char *argv2[8];
	int i=0;
	int j=0;
	int status;
	char *file;
	FILE *fp;
	int output = 0;

	while((strcmp("|", argv[i])) != 0)
	{
        argv1[i] = (char*)malloc(sizeof(char) * 15);
		strcpy(argv1[i], argv[i]);
		++i;
	}
	argv1[i] = NULL;

	++i;

	while(argv[i] != NULL)
	{
		if(strcmp(argv[i], ">") == 0) 
		{
			file = (char*)malloc(sizeof(char) * 15);
		    strcpy(file, argv[i+1]);
  			
         	strcat(file, ".txt");

         	printf("%s\n", file);
			
		    fp = freopen(file, "w", stdout);
			output = 1;
			break;
		}
		argv2[j] = (char*)malloc(sizeof(char) * 15);
		strcpy(argv2[j], argv[i]);
		++j;
	    ++i;
	}
	argv2[j] = NULL;

    printf("%s\n", argv1[0]);

	pipe(pipefd);

	pid1 = fork();

	if(pid1 == -1)
	{
		
		perror("fork");
	    exit(1);
	}


	else if(pid1 == 0)
	{
	  dup2(pipefd[1], STDOUT_FILENO);
      close(pipefd[0]);
	  
      if(execvp(*argv1, argv1)==-1)
	   {
			printf("Invalid command\n");
			exit(1);
		}
	}
	

	pid2 = fork();

	if(pid2 == -1)
	{
		
		perror("fork");
	    exit(1);
	}

    if (pid2 == 0) {
      // Hook stdin up to the read end of the pipe and close the write end of
      // the pipe which is no longer needed by this process.
      dup2(pipefd[0], STDIN_FILENO);
      close(pipefd[1]);
      // Similarly, exec `wc -l`.
      if(execvp(*argv2, argv2)==-1)
	   {
			printf("Invalid command\n");
			exit(1);
		}
	}
      close(pipefd[0]);
      close(pipefd[1]);
	  if(output == 1)
	  {
		  fclose(fp);
	      freopen("/dev/tty", "a", stdout);
	  }
      // Wait for everything to finish and exit.
      waitpid(pid1, &status, 0);
      waitpid(pid2, &status, 0);
}


void printPrompt()
{
	printf("$ ");
}

int parseCommand(char* line, char **argv)
{
	int i=0;	
	int background = 0;

	while (*line != '\0')
	{      
          while (*line == ' ' || *line == '\n')
			  *line++ = '\0';     
          argv[i] = line; 
		  
		  ++i;
		  
          while (*line != '\0' && *line != ' ' && *line != '\n') 
               line++;             
     }
     if (strcmp(argv[i-1], "&") == 0)
	 {
	     background = 1;
	     argv[i-1] = NULL;
		 
	 }
	 else
        argv[i] = '\0';  

	 i=0;

	 while(argv[i] != NULL)
	 {
		 printf("%s\n", argv[i]);
	     ++i;
	 }

	 return background;
	 
}
void main ()
{
    sigset(SIGCHLD, sig_handler);
	sigset(SIGINT, sig_handler2);

	while (1)
	{
		int childPid;
		char line[2014];
		char *argv[8];
		int background;
		int i=0;
		int flag=0;

	    printPrompt();
	    
	    gets(line);   
		
		background = parseCommand(line, argv);

		 if (strcmp(argv[0], "quit") == 0)    // exit if the user enters bye
         {
			 kill(-getpid(),SIGINT);
			 exit(0);
		 }  

         while(argv[i] != NULL)
		 {
			 
            if((strcmp("|", argv[i])) ==0)
			{
				printf("Hi\n");
				pipeit(argv);
				flag =1;
			}

			++i;
		 }

         if(flag == 0)
		 execute(argv, background);

	}

}