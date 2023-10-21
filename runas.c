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

#ifdef __linux__
#define ARG_MAX 2097152 // getconf ARG_MAX CentOS 7
#endif

// Remove usage() from libbacon?
void    usage(char *argv[])

{
    fprintf(stderr, "Usage: %s username command\n", argv[0]);
    exit(EX_USAGE);
}

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

int     main(int argc,char *argv[])

{
    char    cmd[ARG_MAX + 1] = "",
	    *user;
    int     c,
	    min_args,
	    first_arg;

    if ( strcmp(argv[0], "asroot") == 0 )
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

    // FIXME: Warn if executable is not owned by root or is writable
    // by non-root users
    
    for (c = first_arg; c < argc; ++c)
    {
	strlcat(cmd, argv[c], ARG_MAX);
	strlcat(cmd, " ", ARG_MAX);
    }
    /* Indicate which user's password must be entered */
    if ( geteuid() != 0 )
    {
	printf("%s ", user);
	fflush(stdout);
    }
    argv[0] = "su";
    execlp("su", "su", "-l", user, "-c", cmd, NULL);
    return EX_OK;
}
