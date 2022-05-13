#include "keyconf.h"

OPTIONS {
	[CACHE]    = DEFAULT,
	[HOMEPAGE] = DEFAULT,
};

APPERANCE {
	[HEIGHT]   = DEFAULT,
	[WIDTH]    = DEFAULT,
	[DARKMODE] = TRUE
};

KEYBINDS {
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
	{ MODSFT, GDK_KEY_H,   history     }
};
