#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "str_replace_start.h"

#define LIBRE_N 19

/* Uncomment for debug */
/* #define DEBUG */

/* Inspired by https://libredirect.github.io/, but in C. */

void str_init(char* str, int n)
{
	for(int i = 0; i < n; i++)
		str[i] = ' ';
	str[n] = '\0';
} // could also use <https://manpages.ubuntu.com/manpages/impish/man3/strinit.3pub.html>

int libre_redirect(const char* uri, char* output)
{
	int l1 = strlen(uri);
	int l2 = strlen(output);
	int len;
	char tmp_uri[l2++];
	char tmp_output[l2++];

	if ((l2 - l1) < LIBRE_N) {
#ifdef DEBUG
	printf("Not enough memory\n");
#endif
	return 1; // not enough memory.
	} else {
	strcpy(tmp_uri, uri); // strcpy also copies the terminating '\0'
	strcpy(tmp_output, output);

	char* annoying_sites[] = {
		"https://www.youtube.com",
		"https://www.reddit.com",
		"https://medium.com",
		"https://translate.google.com",
		"https://forum.effectivealtruism.org",
		"https://www.bloomberg.com",
		"https://twitter.com"
	};

	char* alternatives[] = {
		"https://yt.artemislena.eu",
		"https://teddit.nunosempere.com",
		"https://scribe.rip",
		"https://simplytranslate.org/",
		"https://ea.greaterwrong.com",
		"https://archive.is/https://www.bloomberg.com",
		"https://nitter.net"
	};

	len = sizeof(annoying_sites) / sizeof(annoying_sites[0]);

	for (int i = 0; i < len; i++) {
		int replace_check = str_replace_start(tmp_uri, annoying_sites[i],
						      alternatives[i], output);
		if (replace_check == 2) {
#ifdef DEBUG
			printf("tmp_uri: %s\n", tmp_uri);
			printf("output: %s\n", output);
#endif
			return 2;
		} else if (replace_check == 1) {
#ifdef DEBUG
			printf("replace_check failed\n");
#endif
			return 1;
		}
		strcpy(tmp_uri, output);
		str_init(output, l2);
	}
	strcpy(output, tmp_uri);
	}
#ifdef DEBUG
	printf("No match found\n\n");
#endif
	return 0;
}
