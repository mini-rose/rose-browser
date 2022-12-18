#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#define DEBUG false

/*
See also: 
* <https://web.archive.org/web/20160201212501/coding.debuntu.org/c-implementing-str_replace-replace-all-occurrences-substring>
* https://github.com/irl/la-cucina/blob/master/str_replace.c
*/

int str_replace_start(const char* string, const char* target, const char* replacement, char* output){
  
  int l1 = strlen(string);
  int l2 = strlen(target);
  int l3 = strlen(replacement);
  int l4 = strlen(output);
  if(DEBUG) printf("%d,%d,%d,%d\n", l1, l2, l3, l4);
  // if(DEBUG) printf("%s,%s,%s,%s\n", string, target, replacement, output);
  
  if((l4 < (l1 - l2 + l3)) || l4 < l1 ){
    // Not enough memory in output string.
    if(DEBUG) printf("String not long enough.\n");
    return 1; 
  } 
  /* else if(l1 < l2){
    // Not even possible that there is a match.
    if(DEBUG) printf("Target larger than string.\n");
    strcpy(output, string);
  } */
  else {
    if(DEBUG) printf("Looking for a match for %s in %s.\n", target, string);
    int match = true;
    for(int i=0; i<l2; i++){
      if(string[i] != target[i]){
	match = false;
	break;
      }
    }
    if(match){
      if(DEBUG) printf("Found match.\n");
      for(int i=0; i<l3; i++){
	output[i] = replacement[i];
      }
      int counter = l3;
      for(int i=l2; i<l1; i++){
        output[counter] = string[i];
        counter++;
      }
      output[counter] = '\0';
      return 2; // success
    }
    else {
      if(DEBUG) printf("Did not find match.\n");
      strcpy(output, string);
    }
  }

  return 0;
}
