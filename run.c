#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <regex.h>
#include <stdlib.h>
#include <limits.h>

const int XERR_PIPE       = 1;
const int XERR_REGCOMP    = 2;
const int XERR_REGERR     = 3;
const int XERR_EXIT       = 4;
const int XERR_REGNOMATCH = 5;
const int XERR_EXEC       = 6;
const int XERR_USAGE      = 7;
const int XERR_TMOUTNEG   = 8;

int run(const char *prog, char *argv[], const char *check, int timeout, int verbose)
{
	pid_t ch;
	int status;
	int pipefd[2];
	FILE *cfd;
	int rrv;
	regex_t *regex = malloc(0);

	if (pipe(pipefd) == -1)
	{
		if ( verbose > 0 )
			fprintf(stderr, "pipe failed: (%s)", strerror(errno));
		return XERR_PIPE;
	}

	if (rrv = regcomp(regex, check, 0)  != 0)
	{
		if ( verbose > 0 )
		{ 
			int   errsize;
			char *errstr;

			errsize = regerror( rrv, regex, NULL, REG_NOSUB);
			errstr  = malloc( errsize );
			regerror(rrv, regex, errstr, errsize);
			fprintf(stderr, "regex (%s) failed compiling: %s\n",
				check, errstr);
		}
		return XERR_PIPE;
	}

	cfd = fdopen(pipefd[0], "r");	

	if (ch = fork())
	{
		char *line = NULL;
		size_t s;
		int match = 0;

		close(pipefd[1]);
		if ( verbose > 2 )
			fprintf(stderr, "childpid = %i\n", ch);
		
		while (getline(&line, &s, cfd) > 0)
		{
			int r;
			
			if ( verbose > 4 )
				fprintf(stderr, "got line: %s", line);

			r = regexec(regex, line, 0, NULL, 0);

			if (r == 0 )
			{
				if ( verbose > 3 )
					fprintf(stderr, "line matched\n");
				match = 1;
			}
			else
			{
				if (r != REG_NOMATCH)
				{
					if ( verbose > 0 )
						fprintf(stderr, "regex error %i\n", r);

					return XERR_REGERR; 
				}
				else
				{
					if ( verbose > 4 )
						fprintf(stderr, "no match\n");
				}	
			}
		}
		
		close(pipefd[0]);
		waitpid(ch, &status,0 );

		if ( verbose > 1 )
			fprintf(stderr, "child %i exited with status %i\n",
				ch, WEXITSTATUS(status) );
		
		if (WEXITSTATUS(status) != 0)
			return XERR_EXIT;
		if (match != 1)
			return XERR_REGNOMATCH;

		return 0;
	}
	else
	{
		close(pipefd[0]);
		dup2(pipefd[1],1);
		
		execvp(prog, (char **) argv );

		if ( verbose > 0 )
			fprintf(stderr, "exec failed: '%s'\n", strerror(errno));

		return XERR_EXEC;	
	}
}

int main (int argc, const char *argv[])
{
	const char *usage      = "Usage %s [-t timeout] [-m match] [-v] cmd [ARGS]";
	const char *optstring  = "t:m:v";

	const char *defmatch   = ".";
	const char *prog   = NULL;
	const char *check  = defmatch;

 	const char **args;

	int  timeout = 0;
	int  verbose = 0;

	int  o;
	int  ac;
	int  as;
	
	while ( (o = getopt(argc, (char **) argv, optstring )  ) != -1 )
	{
		switch (o)
		{
			case 't':
				timeout = atoi(optarg);
				break;
			case 'm':
				check = optarg; 
				break;
			case 'v':
				++verbose;
				break;
			default:
				fprintf(stderr, usage, argv[0]);
				return XERR_USAGE; 
		}
	}
	
	prog = argv[optind];	
	as   = optind + 1;
	ac = argc - as;

	args = malloc(  (ac + 2) * sizeof(const char *) ); 
	args[0]  = prog;

	if (ac > 0)
		args[ac] = NULL;
	else
		args[1]  = NULL;

	if (verbose > 3) fprintf(stderr, "timeout   = %i\n", timeout);	
	if (verbose > 3) fprintf(stderr, "match     = %s\n", check);	
	if (verbose > 3) fprintf(stderr, "verbosity = %i\n", verbose);	
	if (verbose > 3) fprintf(stderr, "prog      = %s\n", prog);	
	if (verbose > 3) fprintf(stderr, "argcount  = %i\n", ac);	

	if (timeout < 0)
	{
		fprintf(stderr, "timeout < 0. int overflow? (maxint = %i)", INT_MAX);
		return XERR_TMOUTNEG;
	}

	if ( prog == NULL )
	{
		fprintf(stderr, usage, argv[0]);
		return XERR_USAGE;
	}

	if ( ( ac > 0 ) )
	{
		int i = 0;
 		if  ( verbose > 3 )
		{   
			while (i < ac)
			{
				const char *aval = argv[ as + i ]; 
				fprintf(stderr, "arg % 3i   = %s\n", i, aval);
				args[ i + 1 ] = aval;
				++i;
			}
		}
		else
		{
			while (i <  ac)
			{
				args[ i + 1] =  argv[ as + i ]; 
				++i;
			}
		}
	}

	return run(prog,( char **)args,  check, timeout, verbose);

}
 
 
