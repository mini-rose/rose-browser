## About

This code automatically redirects webpage to their open-source frontends

To enable it:

## In build.sh

In `build.sh`, uncomment this line:

```
REQS= #./plugins/*/*.c
```

or alternatively, write the full paths of `libre_redirect.c` and `str_replace_start.c`


### In rose.c

Uncomment these lines:

```
// #include "plugins/libre_redirect/libre_redirect.h"

...

/*
void redirect_if_annoying(WebKitWebView *view, const char *uri){
        int l = LIBRE_N + strlen(uri) + 1;
        char uri_filtered[l];
        str_init(uri_filtered, l);

        int check = libre_redirect(uri, uri_filtered);

        if (check == 2){
                webkit_web_view_load_uri(view, uri_filtered);
        }

}
*/

...

                case WEBKIT_LOAD_STARTED:
                                // redirect_if_annoying(self, webkit_web_view_get_uri(self));
                                break;
                case WEBKIT_LOAD_REDIRECTED:
                                // redirect_if_annoying(self, webkit_web_view_get_uri(self));
                                break;
                case WEBKIT_LOAD_COMMITTED:
                                // redirect_if_annoying(self, webkit_web_view_get_uri(self));
                                break;


```
