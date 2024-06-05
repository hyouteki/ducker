#ifndef DUCKER_SCRIPT_H_
#define DUCKER_SCRIPT_H_

#include "helper.h"

#define Line_Capacity 1024

typedef struct DuckerScript_Entry_KeyVal {
	char *key;
	char *val;
} DuckerScript_Entry_KeyVal;

typedef struct DuckerScript_Entry_Cmd {
	char *cmd;
} DuckerScript_Entry_Cmd;

typedef enum {
	DuckerScript_EntryType_KeyVal,
	DuckerScript_EntryType_Cmd
} DuckerScript_EntryType;

#define DuckerScript_EntryType_Default DuckerScript_EntryType_KeyVal

typedef struct DuckerScript_Entry {
    DuckerScript_EntryType type;
	union {
		DuckerScript_Entry_KeyVal *keyval;
		DuckerScript_Entry_Cmd *cmd;
	};
} DuckerScript_Entry;

typedef struct DuckerScript_Table {
	DuckerScript_Entry *entry;
	struct DuckerScript_Table *next;
} DuckerScript_Table;

void DuckerScript_TableAdd(DuckerScript_Table **, DuckerScript_Entry *);
void DuckerScript_TableFree(DuckerScript_Table *);
void DuckerScript_TablePrint(DuckerScript_Table *);
char *DuckerScript_TableFind(DuckerScript_Table *, char *);
void DuckerScript_TableCmdExecute(DuckerScript_Table *);
void DuckerScript_ParseFile(char *, DuckerScript_Table **);

void DuckerScript_TableAdd(DuckerScript_Table **table, DuckerScript_Entry *entry) {
	DuckerScript_Table *node = (DuckerScript_Table *)malloc(sizeof(DuckerScript_Table));
	node->entry = entry;
	node->next = *table;
	*table = node;
}

void DuckerScript_TableFree(DuckerScript_Table *table) {
	while (table) {
		switch (table->entry->type) {
		case DuckerScript_EntryType_KeyVal:
			free(table->entry->keyval->key);
			free(table->entry->keyval->val);
			free(table->entry->keyval);
			break;
		case DuckerScript_EntryType_Cmd:
			free(table->entry->cmd->cmd);
			free(table->entry->cmd);
			break;
		default:
		}
		DuckerScript_Table *next = table->next;
		free(table);
		table = next;
	}
}

void DuckerScript_TablePrint(DuckerScript_Table *table) {
	DuckerScript_Table *itr = table;
	while (itr) {
		switch (itr->entry->type) {
		case DuckerScript_EntryType_KeyVal:
			printf("%s: %s\n", itr->entry->keyval->key, itr->entry->keyval->val);
			break;
		case DuckerScript_EntryType_Cmd:
			printf("%s\n", itr->entry->cmd->cmd);
			break;
		default:
		}
		itr = itr->next;
	}
}

char *DuckerScript_TableFind(DuckerScript_Table *table, char *key) {
	DuckerScript_Table *itr = table;
	while (itr) {
		if (itr->entry->type == DuckerScript_EntryType_KeyVal &&
			strcmp(itr->entry->keyval->key, key) == 0) return strdup(itr->entry->keyval->val);
		itr = itr->next;
	}
	return NULL;
}

void DuckerScript_TableCmdExecute(DuckerScript_Table *table) {
	DuckerScript_Table *itr = table;
	while (itr) {
		if (itr->entry->type == DuckerScript_EntryType_Cmd) system(itr->entry->cmd->cmd);
		itr = itr->next;
	}
}

void DuckerScript_ParseFile(char *filename, DuckerScript_Table **table) {
    FILE *file = fopen(filename, "r");
    if (!file) Error("duckerscript", "could not open file `%s`", filename);
	char *line = (char *)malloc(sizeof(char)*Line_Capacity);
	*table = NULL;
	DuckerScript_EntryType type = DuckerScript_EntryType_Default;
    while (fgets(line, Line_Capacity, file)) {
		char *ptr = line;
		Ducker_Trim(&ptr);
		if (strlen(ptr) == 0) continue;
		if (Ducker_StartsWith(ptr, "section")) {
			ptr += strlen("section");
			Ducker_Trim(&ptr);
			if (strcmp(ptr, "config") == 0) type = DuckerScript_EntryType_KeyVal;
			else if (strcmp(ptr, "cmd") == 0) type = DuckerScript_EntryType_Cmd;
			else Error("duckerscript", "invalid section name '%s'", ptr);
			continue;
		}
		DuckerScript_Entry *entry = (DuckerScript_Entry *)malloc(sizeof(DuckerScript_Entry));
		entry->type = type;
		switch (type) {
		case DuckerScript_EntryType_KeyVal:
			entry->keyval = (DuckerScript_Entry_KeyVal *)malloc(sizeof(DuckerScript_Entry_KeyVal));
			entry->keyval->key = Ducker_FetchUntil(&ptr, '=');
			Ducker_Trim(&entry->keyval->key);
			Ducker_RemoveSpecial(&ptr);
			Ducker_Trim(&ptr);
			entry->keyval->val = strdup(ptr);
			break;
		case DuckerScript_EntryType_Cmd:
			entry->cmd = (DuckerScript_Entry_Cmd *)malloc(sizeof(DuckerScript_Entry_Cmd));
			entry->cmd->cmd = strdup(ptr);
			break;
		}
		DuckerScript_TableAdd(table, entry);
	}
}

#endif // DUCKER_SCRIPT_H_
