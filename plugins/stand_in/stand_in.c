/* Why this file is needed:
 * If the plugins are disabled, 
 * their conditionals will never resolve
 * and their functionality never comes into play
 * but the compiler still wants to know their type
*/

int libre_redirect(const char* uri, char* uri_filtered){
  return 0;
}
void str_init(char* str, int n){
  
};

int str_replace_start(const char* string, const char* target,
          const char* replacement, char* output){
  return 1;
};

void read_readability_js(char* string){
  
}

void read_style_js(char* string){

}

