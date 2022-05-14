#!/bin/sh

# FIXME: Sometimes when new bookmark added cant open it

BOOKMARKS_PATH=~/.cache/rose/bookmarks

search()
{
	[ -f $BOOKMARKS_PATH ] || touch $BOOKMARKS_PATH
	BOOKMARKS=$(cat ~/.cache/rose/bookmarks)
	CURRENT_ID=$(xdotool getactivewindow)
	SEARCH=$(printf "Add\nRemove\n$(printf "$BOOKMARKS" | cut -f1 -d" ")" | dmenu -p SEARCH:)

	[ "$SEARCH" = Add ] && {
		printf "" | dmenu -p Url: | sed 's/\./ /g' >> $BOOKMARKS_PATH
		$0 & exit
	}

	[ "$SEARCH" = Remove ] && {
		printf "$(grep -v "$(printf "$BOOKMARKS" | dmenu -p Select:)" $BOOKMARKS_PATH)" > $BOOKMARKS_PATH
		$0 & exit
	}

	[ -z "$SEARCH" ] && exit

	ROSE_GO=$(xprop -id "$CURRENT_ID" | grep rose)

	[ -z "$1" ] || ROSE_GO=''

	[ -z "$ROSE_GO" ] || {
		[ -z "$(printf "$BOOKMARKS" | grep -w "$SEARCH")" ] && {
				xprop -id "$CURRENT_ID" -f "_ROSE_GO" 8u -set "_ROSE_GO" https://duckduckgo.com/?q="$SEARCH"
				exit
		} || {
			FULL_URL=$(printf "$BOOKMARKS" | grep -w "$SEARCH" | sed 's/ /\./g')
			xprop -id "$CURRENT_ID" -f "_ROSE_GO" 8u -set "_ROSE_GO" https://$FULL_URL
			exit
		}
	}

	[ -z "$(printf "$BOOKMARKS" | grep -w "$SEARCH")" ] && {
		rose https://duckduckgo.com/?q="$SEARCH"
	} || {
		FULL_URL="$(printf "$BOOKMARKS" | grep "$SEARCH" | sed 's/ /\./g')"
		rose https://"$FULL_URL"
	}
}

find()
{
	FIND=$(printf "" | dmenu -p Find:)
	CURRENT_ID=$(xdotool getactivewindow)
	ROSE_FIND=$(xprop -id "$CURRENT_ID" | grep rose)

	[ -z "$ROSE_FIND" ] || xprop -id "$CURRENT_ID" -f "_ROSE_FIND" 8u -set "_ROSE_FIND" "$FIND"
}

history()
{
	CHOOSE=$(tac ~/.cache/rose/history | dmenu -p History:);

	CURRENT_ID=$(xdotool getactivewindow)
	ROSE_GO=$(xprop -id "$CURRENT_ID" | grep rose)

	[ -z "$ROSE_GO" ] || xprop -id "$CURRENT_ID" -f "_ROSE_GO" 8u -set "_ROSE_GO" "$CHOOSE"
}

[ -z "$1" ] && search || {
	INSTALL_PATH=/usr/local/bin/dmenu_rose
	case "$1" in
		install) (set -x; cp $0 $INSTALL_PATH);;
		uninstall) (set -x; rm $INSTALL_PATH);;
		add_bookmark) echo "$2" >> $BOOKMARKS_PATH;;
		new_window) search new;;
		find) find;;
		history) history;
	esac
}
