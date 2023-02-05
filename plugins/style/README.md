## Customize css style for individual websites.

- Replicates: <https://addons.mozilla.org/en-GB/firefox/addon/styl-us/>. 
- The template is similar to the readability folder.

## To enable it

In `build.sh`, uncomment this line:

```
REQS= #./plugins/*/*.c
```

In `rose.c`, uncomment:


```
// #include "plugins/style/style.h"

...

                                // Add custom style
                                /*  
                                char* style_js = malloc(STYLE_N+1);
                                read_style_js(style_js);
                                webkit_web_view_run_javascript(notebook_get_webview(notebook), 
                                            style_js, 
                                            NULL, NULL, NULL);
                                free(style_js);
                                */

```

You will also want to customize the `style.c` file.
