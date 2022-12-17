#include "../libre_redirect.h"
#include <string.h>
#include <stdio.h>

int main(){
  char uri[] = "https://reddit.com/r/blah";
  
  int l = LIBRE_N + strlen(uri) + 1;
  char uri_filtered[l];
  str_init(uri_filtered, l);

  if(!libre_redirect(uri, uri_filtered)){
    printf("Filtered uri: %s\n", uri_filtered);
  }else{
    printf("Uri: %s\n", uri);
    // failure; do something with the original uri.
  }
}


