<br>
<h3 align=center>Rose Browser</h3>

<p align=center>
Small browser built on gtk and webkit. <br>
</p>

## 🌌 Showcase
![image](https://user-images.githubusercontent.com/93622468/236818028-9d90bc83-7bf9-4666-b95a-763a6a1fd3b1.png)


## ⚡️ Requirements
  - lua (5.0+)
  - webkitgtk (4.0|6.0)
  - gtk (3|4)
  - libsanitizer-devel
  
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
$ git clone https://github.com/mini-rose/rose-browser && cd rose-browser
$ (doas|sudo) GTK=(3|4) BUILDTYPE=RELEASE make install
```

## 🚀 Usage
See how to configure rose [here](https://github.com/mini-rose/rose-browser/wiki/Configuration).

## 📝 TODO
See planned changes [here](https://github.com/mini-rose/rose-browser/blob/main/todo.md).
