<h1 align="center">Rose Browser</h1>
<p align="center">Simple browser based on webkit2-5.0/GTK4.</p>

## Showcase

<img src="https://github.com/mini-rose/rose/blob/master/screenshots/homepage.png?raw=true">
<img src="https://github.com/mini-rose/rose/blob/master/screenshots/youtube.png?raw=true">

## Requirements
In order to build rose you need gtk4 and webkit2gtk-5.0.

In order to use the functionalities:
  - video support: gst-libav, gst-plugins-good
  - searchbar/find/history: dmenu
  - download: aria2c

## Installation

Run following command to build and install
(if necessary as root):

```$ make clean install```

## Usage

To set default homepage change config.h

Opening new window with specific website:

```$ rose https://duckduckgo.com```

## TODO
- [x] Add tabs option in config
- [ ] Improve mass weight image loading time 
- [ ] Add support for xdg-open and opening files
