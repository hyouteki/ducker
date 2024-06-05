#ifndef DUCKER_HELPER_H_
#define DUCKER_HELPER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define Error(module, ...)												\
	({printf("[ERR] %s: %s\n", module, __VA_ARGS__); exit(EXIT_FAILURE);})

static int Ducker_IsBlankChar(char);
static int Ducker_IsSpecialChar(char);

void Ducker_Trim(char **);
char *Ducker_FetchUntil(char **, char);
void Ducker_RemoveSpecial(char **);
int Ducker_StartsWith(char *, char *);

static int Ducker_IsBlankChar(char ch) {
	switch (ch) {
	case '\t':
	case '\n':
	case '\r':
	case ' ':
		return 1;
	default:
		return 0;
	}
}

static int Ducker_IsSpecialChar(char ch) {
	if (Ducker_IsBlankChar(ch)) return 1;
	switch (ch) {
	case '=':
		return 1;
	default:
		return 0;
	}
}

void Ducker_Trim(char **text) {
	char *start = *text;
	while (Ducker_IsBlankChar(*start)) start++;
	char *end = *text + strlen(*text)-1;
    while (end > *text && Ducker_IsBlankChar(*end)) end--;
    *(end+1) = 0;
	*text = start;
}

char *Ducker_FetchUntil(char **text, char delim) {
	char *start = *text;
	int i = 0;
	while (*start != delim && i < strlen(*text)) {
		start++;
		i++;
	}
	char *res = strndup(*text, i);
	*text = start;
	return res;
}

void Ducker_RemoveSpecial(char **text) {
	char *start = *text;
	while (Ducker_IsSpecialChar(*start)) start++;
	*text = start;
}

int Ducker_StartsWith(char *text, char *prefix) {
	if (strlen(text) < strlen(prefix)) return 0;
	for (int i = 0; i < strlen(prefix); ++i)
		if (text[i] != prefix[i]) return 0;
	return 1;
}

#endif // DUCKER_HELPER_H_
