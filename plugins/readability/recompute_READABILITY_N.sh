#!/bin/bash
function sedr(){
  find ./ -type f -exec sed -i -e "$1" {} \;
} ## e.g., sedr "s/target/replacement/g"

READABILITY_N=$(wc -c ./plugins/*/readability.js | cut -d " " -f 1)
sedr "s/^#define READABILITY_N .*/#define READABILITY_N $READABILITY_N + 1/g"

