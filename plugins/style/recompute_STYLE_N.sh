#!/bin/bash
function sedr(){
  find ./ -type f -exec sed -i -e "$1" {} \;
} ## e.g., sedr "s/target/replacement/g"

STYLE_N=$(wc -c style.js | cut -d " " -f 1)
sedr "s/^#define STYLE_N .*/#define STYLE_N $STYLE_N + 1/g"

