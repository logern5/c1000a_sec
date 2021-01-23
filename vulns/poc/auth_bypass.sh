#!/bin/sh
# This script requires curl
# Enables telnet and sets the telnet password to 'admin' on the Actiontec C1000a router
server=192.168.0.1
echo "Setting telnet password on" $server
printf '\n\n'
key=$(curl http://$server/login.html | grep "var sessionKey" | cut -d "'" -f2)
out=$(curl -d 'serCtlTelnet=3&remTelUser=admin&remTelTimeout=300&remTelPassChanged=1&nothankyou=0&remTelPass=YWRtaW4%3D&sessionKey='$key \
http://$server/advancedsetup_remotetelnet.cgi)
echo $out | head -c 500
printf '\n\n'
echo "If 'Invalid session key' prints, it means the session key failed."
echo "If 'Enter the administrator username' prints, it means that there is no active session on the router."
echo "If 'Saving Settings' prints, it means that the telnet password was successfully changed."
printf '\n\n'
echo $out | grep "Invalid"
echo $out | grep -o "Enter the administrator username"
echo $out | grep -o "Saving Settings"
