#include "duckerscript.h"

int main() {
	DuckerScript_Table *table = NULL;
	DuckerScript_ParseFile("duckerfile", &table);
	DuckerScript_TablePrint(table);
	printf("root = '%s'\n", DuckerScript_TableFind(table, "root"));
	DuckerScript_TableFree(table);
	return 0;
}
