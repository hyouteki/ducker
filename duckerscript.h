#ifndef DUCKER_SCRIPT_H_
#define DUCKER_SCRIPT_H_

#include "helper.h"

#define Line_Capacity 1024

typedef struct DuckerScript_Entry {
	char *key;
	char *val;
} DuckerScript_Entry;

typedef struct DuckerScript_Table {
	DuckerScript_Entry *entry;
	struct DuckerScript_Table *next;
} DuckerScript_Table;

void DuckerScript_TableAdd(DuckerScript_Table **, DuckerScript_Entry *);
void DuckerScript_TableFree(DuckerScript_Table *);
void DuckerScript_TablePrint(DuckerScript_Table *);
char *DuckerScript_TableFind(DuckerScript_Table *, char *);
void DuckerScript_ParseFile(char *, DuckerScript_Table **);

void DuckerScript_TableAdd(DuckerScript_Table **table, DuckerScript_Entry *entry) {
	DuckerScript_Table *node = (DuckerScript_Table *)malloc(sizeof(DuckerScript_Table));
	node->entry = entry;
	node->next = *table;
	*table = node;
}

void DuckerScript_TableFree(DuckerScript_Table *table) {
	while (table) {
		free(table->entry->key);
		free(table->entry->val);
		DuckerScript_Table *next = table->next;
		free(table);
		table = next;
	}
}

void DuckerScript_TablePrint(DuckerScript_Table *table) {
	DuckerScript_Table *itr = table;
	while (itr) {
		printf("%s: %s\n", itr->entry->key, itr->entry->val);
		itr = itr->next;
	}
}

char *DuckerScript_TableFind(DuckerScript_Table *table, char *key) {
	DuckerScript_Table *itr = table;
	while (itr) {
		if (strcmp(itr->entry->key, key) == 0) return strdup(itr->entry->val);
		itr = itr->next;
	}
	return NULL;
}

void DuckerScript_ParseFile(char *filename, DuckerScript_Table **table) {
    FILE *file = fopen(filename, "r");
    if (!file) Error("duckerscript", "could not open file `%s`", filename);
	char *line = (char *)malloc(sizeof(char)*Line_Capacity);
	*table = NULL;
    while (fgets(line, Line_Capacity, file)) {
		DuckerScript_Entry *entry = (DuckerScript_Entry *)malloc(sizeof(DuckerScript_Entry));
        char *ptr = line;
		Ducker_Trim(&ptr);
		entry->key = Ducker_FetchUntil(&ptr, '=');
		Ducker_Trim(&entry->key);
		Ducker_RemoveSpecial(&ptr);
		Ducker_Trim(&ptr);
		entry->val = strdup(ptr);
		DuckerScript_TableAdd(table, entry);
	}
}

#endif // DUCKER_SCRIPT_H_
