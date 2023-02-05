#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define READABILITY_N 84251 + 1

void read_readability_js(char* string){
  FILE *fp=fopen("/home/loki/Documents/core/software/fresh/C/rose-browser/rose-bud-personal/plugins/readability/readability.js", "r");
  if (!fp) { // fp is NULL, fopen failed
    fprintf(stderr, "Failed to open file\n");
    string=NULL;
    return;
  }
  int i=0;
  int c;
  while ((c = fgetc(fp)) != EOF){
     string[i++] = c;
  }
  string[i]='\0';
  fclose(fp);
}

/*
int main(){
    char* readability_js = malloc(READABILITY_N+1);
    read_readability_js(readability_js);
    printf("%s", readability_js);
    free(readability_js);
}
*/
