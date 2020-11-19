# c1000a_sec
Security details and vulnerabilities on the Actiontec C1000a router.
These are the results of my reverse engineering of the Actiontec C1000a router running firmware version 31.90L. Notes about
the reverse engineering efforts are [here](notes.md).

Vulnerabilities include:
* Command injection in telnet interface: typing `ps ; sh` will run `ps` and drop to a shell. (CWE-77)
* Stored XSS in `urlfilter.cmd`/`/advancedsetup_websiteblocking.html` by an unauthenticated user. (CWE-79)
(CVE-2018-19922). This is likely the most serious vulnerability as it allows an unauthenticated user
to inject arbitrary JavaScript code into an authenticated user's page, which could do things like change
the DNS servers, enable telnet, or get router configuration data.
* Linux kernel version 2.6.30 is old, vulnerable to towelroot priv. esc. (CVE-2014-3153)
* Hardcoded passwords for users `nobody`,`support`, and `user` in `/etc/passwd`. (CWE-259)

Other security bad practices include:
* Authentication for web administration is sent over plaintext HTTP, so it can be intercepted, viewed, and/or modified. (CWE-319)
