#include "../shell.h"
#include "../symtab/symtab.h"

int dump(int argc, char **argv)
{
    dumplocalsymtab();
    return 0;
}
