#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <regex.h>
#include <stdlib.h>

const int XERR_PIPE       = 1;
const int XERR_REGCOMP    = 2;
const int XERR_REGERR     = 3;
const int XERR_EXIT       = 4;
const int XERR_REGNOMATCH = 5;
const int XERR_EXEC       = 6;

	int run(const char *prog, char *argv[], const char *check)
 
{
	pid_t ch;
	int status;
	int pipefd[2];
	FILE *cfd;
	int rrv;
	regex_t *regex = malloc(0);

	if (pipe(pipefd) == -1)
	{
		printf("pipe failed: (%s)", strerror(errno));
		return XERR_PIPE;
	}

	if (rrv = regcomp(regex, check, 0)  != 0)
	{
		int   errsize;
		char *errstr;
		  
		errsize = regerror( rrv, regex, NULL, REG_NOSUB);
		errstr  = malloc( errsize );
		regerror(rrv, regex, errstr, errsize);
		printf("regex (%s) failed compiling: %s\n", check, errstr);
		return XERR_PIPE;
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
					return XERR_REGERR; 
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
			return XERR_EXIT;
		if (match != 1)
			return XERR_REGNOMATCH;

		return 0;
	}
	else
	{
		const char *aaar[24];	
		aaar[0] = prog;
		aaar[1] = NULL;	
	
		close(pipefd[0]);
		dup2(pipefd[1],1);
		

		execvp(prog, (char **) aaar );
		printf("exec failed: '%s'\n", strerror(errno));
		return XERR_EXEC;	
	}
}

int main (int argc, const char *argv[])
{

	char *prog  = (char *) argv[1];
	char *check = "";
	char *args[128];
	args[0] = prog;
	args[1] = NULL;

	return run(prog,( char **)args,  check);

}
 
