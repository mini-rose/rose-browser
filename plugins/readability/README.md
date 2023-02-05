## Readability

Taken from <https://raw.githubusercontent.com/ushnisha/readability-reader-webextensions/master/content_scripts/tranquilize.js>


## To enable it

In rose.c uncomment:

```

// #include "plugins/readability/readability.h"

/*
  	case prettify:
    {

      char* readability_js = malloc(READABILITY_N+1);
      read_readability_js(readability_js);
      webkit_web_view_run_javascript(notebook_get_webview(notebook), 
                  readability_js, 
                  NULL, NULL, NULL);
      free(readability_js);
                  
      break;
	  }
*/

```

In config.h, uncomment:

```

typedef enum {
	goback,
	goforward,
	refresh,
	refresh_force,
	back_to_home,
	toggle_fullscreen,
	zoomin,
	zoomout,
	zoom_reset,
	next_tab,
	prev_tab,
	close_tab,
	show_searchbar,
	show_finder,
	finder_next,
	finder_prev,
	newtab,
  /*prettify,*/
	hidebar
} func;

...

    //     { CTRL,        KEY(p),      prettify            },


```