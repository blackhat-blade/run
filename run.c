#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <regex.h>
#include <stdlib.h>


int run(const char *prog, int argc, const char *argv[], const char *check)
{
	return 0;
}

int main (int argc, const char *argv[])
{
	pid_t ch;
	int status;
	int pipefd[2];
	FILE *cfd;
	int rrv;
	regex_t *regex = malloc(0);

	const char *prog  = "dmesg";
	const char *check = "wuseldusl \\d+";
	 
	if (pipe(pipefd) == -1)
	{
		printf("pipe failed: (%s)", strerror(errno));
		return 1;
	}

	if (rrv = regcomp(regex, check, 0)  != 0)
	{
		int   errsize;
		char *errstr;
		  
		errsize = regerror( rrv, regex, NULL, REG_NOSUB);
		errstr  = malloc( errsize );
		regerror(rrv, regex, errstr, errsize);
		printf("regex (%s) failed compiling: %s\n", check, errstr);
		return 2;
	}

	cfd = fdopen(pipefd[0], "r");	

	if (ch = fork())
	{
		char *line = NULL;
		size_t s;
		int match = 0;

		close(pipefd[1]);
		printf("hello from parent, child = %i\n", ch);
		
		while (getline(&line, &s, cfd) > 0)
		{
			int r;
			printf("got line: %s", line);

			r = regexec(regex, line, 0, NULL, 0);

			if (r == 0 )
			{
				printf("line matched\n");
				match = 1;
			}
			else
			{
				if (r != REG_NOMATCH)
				{
					printf("regex error %i\n", r);
					return 6; 
				}
				else
				{
					printf("no match\n");
				}	
			}
		}
		
		close(pipefd[0]);
		waitpid(ch, &status,0 );
		printf("child %i exited with status %i\n", ch, WEXITSTATUS(status) );
		
		if (WEXITSTATUS(status) != 0)
			return 3;
		if (match != 1)
			return 4;

		return 0;
	}
	else
	{
		close(pipefd[0]);
		dup2(pipefd[1],1);
		execlp(prog, "",  (char *) NULL);
		printf("exec failed: '%s'\n", strerror(errno));
		return 5;	
	}

}
