#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <regex.h>
#include <stdlib.h>

int main (int argc, const char *argv[])
{
	pid_t ch;
	int status;
	int pipefd[2];
	FILE *cfd;
	int rrv;
	regex_t *regex = malloc(4);

	const char *prog  = "uname";
	const char *check = "Debian";
	 
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
			printf("got line: %s", line);
			
			if ( regexec(regex, line, 0, NULL, 0) == 0 )
			{
				printf("line matched\n");
				match = 1;
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
		execlp("uname", "", "-a", (char *) NULL);
		printf("exec failed: '%s'\n", strerror(errno));
	}

	return rv;
}
