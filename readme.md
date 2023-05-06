<br>
<h3 align=center>Rose Browser</h3>

<p align=center>
Small browser built on gtk and webkit. <br>
</p>

## ⚡️ Requirements
  - webkitgtk (4.0|6.0)
  - gtk (3|4)
  
  - Video and audio:
  	- gst-plugins-bad
	- gst-plugins-base
	- gst-plugins-good
	- gst-libav
	- gstreamer-vaapi (gpu acceleration)
	
## ✨ Features
  - supports the lastes webkit and gtk features
  - configuration via lua modules
  
## 📦 Installation
```sh
$ git clone https://github.com/mini-rose/rose && cd rose
# GTK=(3|4) BUILDTYPE=RELEASE make install
```

## 🚀 Usage
Configure rose in ~/.config/rose/init.lua:
```lua
rose.startpage = "https://duckduckgo.com"

rose.webkit = {
	gestures = true
}
```

## Work in progress

For now i recommend to use old rose (at "old" branch).

Moved to main becouse old branch will not be maintained.
