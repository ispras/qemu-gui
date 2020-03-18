#include <QTextStream>
#include "GlobalConfig.h"

QTextStream out(stdout);

void usage()
{
    out << "qemu-cli usage:\n"
        "list - output configured VMs\n";
}

void vmlist()
{
    GlobalConfig cfg;
    foreach(VMConfig *vm, cfg.get_exist_vm())
    {
        out << "===============================\n";
        out << vm->get_vm_info();
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        usage();
        return 0;
    }

    if (!strcmp(argv[1], "list"))
    {
        vmlist();
    }
    else
    {
        usage();
    }    

    return 0;
}