/***************************************************************************
 *  Description:
 *      Impersonate another user
 *      Simple wrapper around su, which is cumbersome for running
 *      single commands, requiring quotes around the command, e.g.
 *
 *      su -m root -c 'command with arguments'
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

#define CMD_MAX     1024

void    usage(char *argv[])

{
    fprintf(stderr, "Usage: %s username command\n", argv[0]);
    exit(EX_USAGE);
}

int     main(int argc,char *argv[])

{
    char    cmd[CMD_MAX + 1] = "",
	    *user;
    int     c;
    
    if ( argc < 3 )
	usage(argv);

    user = argv[1];
    for (c = 2; c < argc; ++c)
    {
	strlcat(cmd, argv[c], CMD_MAX);
	strlcat(cmd, " ", CMD_MAX);
    }
    argv[0] = "su";
    printf("%s ", user);
    fflush(stdout);
    execlp("su", "-m", user, "-c", cmd, NULL);
    return EX_OK;
}
