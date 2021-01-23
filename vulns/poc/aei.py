#!/usr/bin/python3
# Decode (and encode) the encryption used by the function AEI_cms_encode() in libcms_core.so on the C1000a
# Example inputs:
# ZXQsbX3B -> YWRtaW4A -> admin
# dHEyd4cudnR> -> cGFzc3dvcmQ= -> password
# password -> obttxpqc (not base64 decoded)

import sys
import base64
import binascii

if len(sys.argv) < 2:
	print("Please supply a parameter (string to be encoded/decoded)")
	exit()

in_str = sys.argv[1]
in_str_len = len(in_str)
in_str_bytes = bytearray(in_str.encode('utf-8'))

for i in range(0, in_str_len):
	if (in_str_bytes[i] % 2 != 0) == 0:
		in_str_bytes[i] -= 1
	else:
		in_str_bytes[i] += 1

out_str = in_str_bytes.decode('utf-8')
print(f'Output string: {out_str}')

try:
	b64_dec = base64.b64decode(out_str, validate=True)
	# String encoded in the config file tend to have a null terminator attached, so remove it
	b64_dec_str = b64_dec.decode('utf-8').rstrip('\x00')
	print(f"Base64 decoded output string: '{b64_dec_str}'")
except binascii.Error:
	print("Output is not valid base64, meaning the input string is likely already decoded or not base64")
	pass
except UnicodeDecodeError:
	print("Output base64 decoded is not valid unicode, meaning the input string is likely already decoded or not base64")
