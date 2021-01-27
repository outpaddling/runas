# runas
Simple wrapper around su to make executing single commands cleaner.

The su command is cumbersome to use for running single commands, requiring
the -c and -m flags as well as quotes around the command, e.g.

    su -m root -c 'command with arguments'

The runas command, in contrast, minimizes requirements on the user:

    runas root command with arguments

Runas does not require any special privileges or configuration.  Users
must enter the password of the target user (not their own password as with
sudo) in order to execute a command.
