static char *options[] = {
	[CACHE]    = NULL,
	[HOMEPAGE] = "https://duckduckgo.com",
};

static int appearance[] = {
	[HEIGHT]       = DEFAULT,
	[WIDTH]        = DEFAULT,
	[DARKMODE]     = 1,
	[SMOOTHSCROLL] = 0,
	[ANIMATIONS]   = 1
};

static int privacy[] = {
	[COOKIES] = 1,
	[HISTORY] = 1,
	[CACHING] = 1
};

static Key keys[] = {
	{ MODKEY, GDK_KEY_h,      goback      },
	{ MODKEY, GDK_KEY_l,      goforward   },
	{ MODKEY, GDK_KEY_y,      copy_url    },
	{ MODKEY, GDK_KEY_p,      paste_url   },
	{ MODKEY, GDK_KEY_e,      search      },
	{ MODSFT, GDK_KEY_K,      zoomin      },
	{ MODSFT, GDK_KEY_J,      zoomout     },
	{ MODSFT, GDK_KEY_Q,      zoomreset   },
	{ MODSFT, GDK_KEY_I,      inspector   },
	{ MODKEY, GDK_KEY_j,      down        },
	{ MODKEY, GDK_KEY_k,      up          },
	{ MODKEY, GDK_KEY_f,      find        },
	{ MODKEY, GDK_KEY_n,      findnext    },
	{ MODSFT, GDK_KEY_N,      findprev    },
	{ MODKEY, GDK_KEY_r,      reload      },
	{ 0     , GDK_KEY_Escape, hidebar     },
	{ MODSFT, GDK_KEY_R,      reloadforce },
	{ 0,      GDK_KEY_F11,    fullscreen  },
	{ MODSFT, GDK_KEY_H,      history     },
	{ MODKEY, GDK_KEY_g,      gotop       },
	{ MODSFT, GDK_KEY_G,      gobottom    },
	{ MODKEY, GDK_KEY_b,      tabshow     },
	{ MODKEY, GDK_KEY_w,      tabclose    },
	{ ALTKEY, GDK_KEY_h,      tabprev     },
	{ ALTKEY, GDK_KEY_l,      tabnext     },
	{ ALTKEY, GDK_KEY_1,      tabsel      },
	{ ALTKEY, GDK_KEY_2,      tabsel      },
	{ ALTKEY, GDK_KEY_3,      tabsel      },
	{ ALTKEY, GDK_KEY_4,      tabsel      },
	{ ALTKEY, GDK_KEY_5,      tabsel      },
	{ ALTKEY, GDK_KEY_6,      tabsel      },
	{ ALTKEY, GDK_KEY_7,      tabsel      },
	{ ALTKEY, GDK_KEY_8,      tabsel      },
	{ ALTKEY, GDK_KEY_9,      tabsel      }
};
