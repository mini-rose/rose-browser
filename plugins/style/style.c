#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define STYLE_N 794 + 1

void read_style_js(char* string){
  FILE *fp=fopen("/home/loki/Documents/core/software/fresh/C/rose-browser/rose-bud-personal/plugins/style/style.js", "r");
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
    char* readability_js = malloc(STYLE_N+1);
    read_readability_js(readability_js);
    printf("%s", readability_js);
    free(readability_js);
}
*/
