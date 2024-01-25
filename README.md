```
 ____  _          _ _   _
/ ___|| |__   ___| | | / |
\___ \| '_ \ / _ \ | | | |
 ___) | | | |  __/ | | | |
|____/|_| |_|\___|_|_| |_|
```
One of the foundations of my shell project revolved around parsing and populating the token and argv array at the same time, while running parallel operations upon them so that argv does not get populated with redirection tokens and segmenting file names. In terms of delegating and creating helper functions, I relied on these for ease of use when going through and explaining my code not to myself an others. For example I have two versions of identifying redirection tokens. One function <is_redirect> is used for simply identifying if a redirection token is in token is in the 0th position of the tokens array. The alternate version <is_redirection> uses a for loop that checks for every other position outside the 0th position to find wherever a redirection token is within the list of tokens that came from parse.

I use several helper functions for more readability too and this includes implementing functions for executing external commands, built in commands, generating an onscreen prompt for when we run the PROMPT micro and more. In my main function we run these helper functions and decide if we want to execute different commands from the parent process or the child process depending on if we are using a external command or built in command based on what is inputed into the shell terminal.

Known Bugs: None

