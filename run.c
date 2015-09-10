#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>


int main (int argc, const char *argv[])
{
	int rv = 1;
	pid_t ch;
	int status;
	int pipefd[2];
	FILE *cfd;

	const char *prog = "uname";
	 
	if (pipe(pipefd) == -1)
	{
		printf("pipe failed: (%s)", strerror(errno));
		return 1;
	}

	cfd = fdopen(pipefd[0], "r");	

	if (ch = fork())
	{
		char *buf[64];
		int s;
		close(pipefd[1]);
		printf("hello from parent, child = %i\n", ch);
		
		while (s = fread(buf, 1, 1, cfd) > 0)
		{
				s = write(1, buf, s);
		}

		close(pipefd[0]);
		waitpid(ch, &status,0 );
		printf("child %i exited with status %i\n", ch, WEXITSTATUS(status) );
		
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
