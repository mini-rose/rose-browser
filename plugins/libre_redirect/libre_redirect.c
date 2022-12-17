#include "str_replace_start.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#define LIBRE_N 19
#define DEBUG false

/* Inspired by https://libredirect.github.io/, but in C. */

void str_init(char* str, int n){
  for(int i=0; i<n; i++){
    str[i] = ' ';
  }
  str[n] = '\0';
} // could also use <https://manpages.ubuntu.com/manpages/impish/man3/strinit.3pub.html>

int libre_redirect(const char* uri, char* output){
  int l1 = strlen(uri);
  int l2 = strlen(output);
  
  if((l2 - l1) < LIBRE_N){ 
    if(DEBUG) printf("Not enough memory\n");
    return 1; // not enough memory. 
  }else{
    char tmp_uri[l2++];
    char tmp_output[l2++];
    strcpy(tmp_uri, uri); // strcpy also copies the terminating '\0'
    strcpy(tmp_output, output);
  
    char* annoying_sites[] = { 
      "https://www.youtube.com", 
      "https://www.reddit.com", 
      "https://medium.com", 
      "https://translate.google.com",
      "https://forum.effectivealtruism.org",
      "https://www.bloomberg.com",
      "https://twitter.com"
    };
    char* alternatives[] = { 
      "https://yt.artemislena.eu", 
      "https://teddit.nunosempere.com", 
      "https://scribe.rip", 
      "https://simplytranslate.org/",
      "https://ea.greaterwrong.com",
      "https://archive.is/https://www.bloomberg.com",
      "https://nitter.net"
    };
    int n = sizeof(annoying_sites)/sizeof(annoying_sites[0]);
    for(int i=0; i<n ; i++){
      int replace_check = str_replace_start(tmp_uri, annoying_sites[i], alternatives[i], output);
      if(replace_check == 2){
	if(DEBUG) printf("tmp_uri: %s\n", tmp_uri);
	if(DEBUG) printf("output: %s\n", output);
        // strcpy(output, tmp_uri);
        // break;
	return 2;
      }else if(replace_check == 1){
        if(DEBUG) printf("replace_check failed\n");
        return 1;
      }
      strcpy(tmp_uri, output);
      str_init(output, l2);
    }
    strcpy(output, tmp_uri);
  }
  if(DEBUG) printf("No match found\n\n");
  return 0;
  
}
