#Actiontec C1000A Plaintext WPA2 and Administrator password storage

##High level overview:
The CenturyLink-branded Actiontec C1000A DSL modem and router running firmware version CAC004-31.30L.95 stores credentials for WPA2 and the HTTP GUI administration 
console in plaintext in the NVRAM. An attacker with physical access to the router or root access to the device could dump the passwords from NVRAM, potentially 
maliciously changing settings on the router such as the DNS server.

##Root cause analysis:
The cause of this is using plaintext and not hashed passwords. A fix to this would to store hashed passwords instead of plaintext passwords in the NVRAM. 
That way, the router GUI HTTP server could check against the hash instead of the plaintext password.

##Proof of Concept:
On the device's telnet root shell, the nvram can be dumped with cat from /dev/mtd3 like so:
`cat /dev/mtd3 > dump.bin`
To confirm the password is present:
`grep $password dump.bin`
NVRAM variables can also be shown with the command 'nvram show', but this shows only the WLAN password and not the administrator password.

##Software download link:
The vulnerable firmware image can be downloaded [here](http://qwest.centurylink.com/internethelp/modems/c1000a/firmware/CAC004-31.30L.95.img).
