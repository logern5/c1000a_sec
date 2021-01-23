#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

void AEI_cms_encode(char *param_1,char param_2)

{
  size_t sVar1;
  uint local_18;
  
  if (param_2 == '\0') {
    local_18 = 0;
    while( 1 ) {
      sVar1 = strlen(param_1);
      if (sVar1 <= local_18) break;
      if ((param_1[local_18] & 1U) == 0) {
	printf("Bitwise equals 0: %d\n",param_1[local_18]);
        param_1[local_18] = param_1[local_18] + -1;
      }
      else {
	printf("Bitwise does not equal 0: %d\n",param_1[local_18]);
        param_1[local_18] = param_1[local_18] + '\x01';
      }
      local_18 = local_18 + 1;
    }
    //log_log(4,"AEI_cms_encode",0xbf9,"@@@@@@aei decryption:%s\n",param_1);
    printf("AEI_cms_encode decryption: %s\n", param_1);
  }
  else {
    local_18 = 0;
    while( 1 ) {
      sVar1 = strlen(param_1);
      if (sVar1 <= local_18) break;
      if ((param_1[local_18] & 1U) == 0) {
        param_1[local_18] = param_1[local_18] + -1;
      }
      else {
        param_1[local_18] = param_1[local_18] + '\x01';
      }
      local_18 = local_18 + 1;
    }
    //log_log(4,"AEI_cms_encode",0xbed,"@@@@@@aei encryption:%s\n",param_1);
    printf("AEI_cms_encode encryption :%s\n",param_1);
  }
  return;
}

int main(int argc, char **argv){
	if(argc < 2){
		printf("Please supply a parameter.\n");
		return -1;
	}
	printf("Decrypting string: %s\n",argv[1]);
	AEI_cms_encode(argv[1],'\0');
	return 0;
}
