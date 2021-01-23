# c1000a_sec
Security details and vulnerabilities on the Actiontec C1000a router.
These are the results of my reverse engineering of the Actiontec C1000a router running firmware version 31.90L.95. Notes about
the reverse engineering efforts are [here](notes.md).

Vulnerabilities include:
* [Command injection in telnet interface](vulns/cmd_injection.md): 
typing `ps ; sh` will run `ps` and drop to a shell. (CWE-77).
* [Stored XSS](https://github.com/logern5/c1000a_xss/blob/master/README.md) in 
`urlfilter.cmd`/`/advancedsetup_websiteblocking.html` by an authenticated user or an unauthenticated
user that bypasses authentication. (CWE-79) (CVE-2018-19922).
* [Authentication bypass](vulns/auth_bypass.md), if an authenticated user is logged on to the web interface, an 
unauthenticated user can perform authenticated actions, as the web server does not check for cookies. These 
authenticated actions include enabling telnet, changing the telnet password (thus gaining root access with the
previous command injection vulnerability), and changing the admin password. (CWE-306).
* Linux kernel version 2.6.30 is old, vulnerable to towelroot priv. esc. (CVE-2014-3153)
* Hardcoded passwords for users `nobody`,`support`, and `user` in `/etc/passwd`. (CWE-259)

Other security bad practices include:
* Authentication for web administration is sent over plaintext HTTP, so it can be intercepted, viewed, and/or
modified. (CWE-319)
* All users in `/etc/passwd` have uid 0, even 'nobody' (CWE-250)
* The router uses telnet and not SSH for a remote shell, allowing passwords to be intercepted, viewed, and/or
modified (CWE-319)
* Plaintext passwords for the WLAN and for the admin user are stored as plaintext in the NVRAM (CWE-256)
* [Weak 'encryption'](vulns/poc/AEI.c) (really just an obscure encoding algorithm, as no key is used) of passwords 
in the router's configuration XML (`/etc/default.cfg`). (CWE-261)
