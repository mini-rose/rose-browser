#include "keyconf.h"

static const char *options[] = {
	[CACHE]       = DEFAULT,
	[HOMEPAGE]    = DEFAULT,
};

static int appearance[] = {
	[HEIGHT]       = DEFAULT,
	[WIDTH]        = DEFAULT,
	[DARKMODE]     = TRUE,
	[SMOOTHSCROLL] = FALSE,
	[ANIMATIONS]   = TRUE
};

static const Key keys[] = {
	{ MODKEY, GDK_KEY_h,   goback      },
	{ MODKEY, GDK_KEY_l,   goforward   },
	{ MODKEY, GDK_KEY_y,   copy_url    },
	{ MODKEY, GDK_KEY_p,   paste_url   },
	{ MODKEY, GDK_KEY_e,   search      },
	{ MODSFT, GDK_KEY_K,   zoomin      },
	{ MODSFT, GDK_KEY_J,   zoomout     },
	{ MODSFT, GDK_KEY_Q,   zoomreset   },
	{ MODKEY, GDK_KEY_i,   inspector   },
	{ MODKEY, GDK_KEY_j,   down        },
	{ MODKEY, GDK_KEY_k,   up          },
	{ MODKEY, GDK_KEY_f,   find        },
	{ MODKEY, GDK_KEY_n,   findnext    },
	{ MODSFT, GDK_KEY_N,   findprev    },
	{ MODKEY, GDK_KEY_r,   reload      },
	{ MODSFT, GDK_KEY_R,   reloadforce },
	{ NOMODK, GDK_KEY_F11, fullscreen  },
	{ MODSFT, GDK_KEY_H,   history     },
	{ MODKEY, GDK_KEY_g,   gotop       },
	{ MODSFT, GDK_KEY_G,   gobottom    },
	{ MODKEY, GDK_KEY_b,   tabshow     },
	{ ALTKEY, GDK_KEY_h,   tabprev     },
	{ ALTKEY, GDK_KEY_l,   tabnext     },
	{ ALTKEY, GDK_KEY_w,   tabclose    },
	{ ALTKEY, GDK_KEY_1,   tabsel      },
	{ ALTKEY, GDK_KEY_2,   tabsel      },
	{ ALTKEY, GDK_KEY_3,   tabsel      },
	{ ALTKEY, GDK_KEY_4,   tabsel      },
	{ ALTKEY, GDK_KEY_5,   tabsel      },
	{ ALTKEY, GDK_KEY_6,   tabsel      },
	{ ALTKEY, GDK_KEY_7,   tabsel      },
	{ ALTKEY, GDK_KEY_8,   tabsel      },
	{ ALTKEY, GDK_KEY_9,   tabsel      }
};
