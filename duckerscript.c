#include "duckerscript.h"

int main() {
	DuckerScript_Table *table = NULL;
	DuckerScript_ParseFile("duckerfile", &table);
	printf("Table\n\n");
	DuckerScript_TablePrint(table);
	printf("\n\nroot = '%s'\n", DuckerScript_TableFind(table, "root"));
	DuckerScript_TableFree(table);
	return 0;
}
