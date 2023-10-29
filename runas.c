/***************************************************************************
 *  Description:
 *      Impersonate another user
 *      Simple wrapper around su, which is cumbersome for running
 *      single commands, requiring quotes around the command, e.g.
 *
 *      su -l root -c 'command with arguments'
 *
 *  History: 
 *  Date        Name        Modification
 *  2021-01-26  J Bacon     Begin
 ***************************************************************************/

#include <stdio.h>
#include <sysexits.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

#define ARGV_MAX 1024

#ifdef __linux__
#define ARG_MAX 2097152 // getconf ARG_MAX CentOS 7
#endif

// Remove usage() from libbacon?
void    usage(char *argv[])

{
    if ( strstr(argv[0], "asroot") != NULL )
	fprintf(stderr, "Usage: %s [su flags] command\n", argv[0]);
    else
	fprintf(stderr, "Usage: %s username [su flags] command\n", argv[0]);
    exit(EX_USAGE);
}

#ifdef __sun
#define LOGIN_ENV   "-"
#else
#define LOGIN_ENV   "-l"
#endif

// Invoke using absolute pathname to thwart Trojan su
#define SU          "/usr/bin/su"

// Could use libxtend, but avoiding a dependency just for one function
#ifdef __linux__
size_t  strlcat(char *dest,const char *src,size_t maxlen)

{
    char    *dp,*sp;
    
    /* Find end of first string */
    for (dp=dest; (*dp != '\0') && --maxlen; ++dp)
	;
    
    /* Concetanate second string */
    for (sp=(char *)src; (*sp != '\0') && --maxlen; )
	*dp++ = *sp++;
    
    /* Null terminate */
    *dp = '\0';
    return dp-dest;
}
#endif

void    check_argv_max(char *cmd, int argc)

{
    if ( argc >= ARGV_MAX )
    {
	fprintf(stderr, "%s: Too many command arguments.  Max = %d.\n", cmd, ARGV_MAX);
	fprintf(stderr, "Increase ARGV_MAX and recompile if this is really necessary.\n");
	exit(EX_DATAERR);
    }
}


int     main(int argc,char *argv[])

{
    char    *exec_argv[ARGV_MAX],
	    *user,
	    cmd[ARG_MAX];
    int     c,
	    min_args,
	    first_arg,
	    exec_argc;

    if ( strstr(argv[0], "asroot") != NULL )
    {
	min_args = 2;
	first_arg = 1;
	user = "root";
    }
    else
    {
	min_args = 3;
	first_arg = 2;
	user = argv[1];
    }
    
    if ( argc < min_args )
	usage(argv);

    exec_argv[0] = SU;
    
    // FIXME: Warn if executable is not owned by root or is writable
    // by non-root users.  Shouldn't be a problem if properly installed
    // via a package manager, but do everything possible for security,
    // since users may be entering the root password.
    
    // Add su flags
    for (c = first_arg, exec_argc = 1; *argv[c] == '-'; ++c, ++exec_argc)
    {
	check_argv_max(argv[0], exec_argc);
	exec_argv[exec_argc] = argv[c];
    }
    
    // Username and -c to precede command
    check_argv_max(argv[0], exec_argc);
    exec_argv[exec_argc++] = user;
    
    check_argv_max(argv[0], exec_argc);
    exec_argv[exec_argc++] = "-c";
    
    // Command must be one argument to execvp()
    *cmd = '\0';
    while ( c < argc )
    {
	strlcat(cmd, argv[c++], ARG_MAX);
	if ( c < argc )
	    strlcat(cmd, " ", ARG_MAX);
    }
    
    if ( strlen(cmd) == ARG_MAX )
    {
	fprintf(stderr, "%s: Command too long.\n", argv[0]);
	exit(EX_DATAERR);
    }
    
    check_argv_max(argv[0], exec_argc);
    exec_argv[exec_argc++] = cmd;
    
    // Terminate argv array
    exec_argv[exec_argc] = NULL;
    
    //for (c = 0; exec_argv[c] != NULL; ++c)
    //    puts(exec_argv[c]);
    
    /* Indicate which user's password must be entered */
    if ( geteuid() != 0 )
    {
	printf("%s ", user);
	fflush(stdout);
    }
    execv(SU, exec_argv);
    return EX_OK;
}
