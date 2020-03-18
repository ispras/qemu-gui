#include <QTextStream>
#include "GlobalConfig.h"
#include "CommandLineParameters.h"

QTextStream out(stdout);

void usage()
{
    out << "qemu-cli usage:\n"
        "list - output configured VMs\n"
        "cmdline <vm>"
        " - output command line for running specified VM\n";
}

int vmlist()
{
    GlobalConfig cfg;
    foreach(VMConfig *vm, cfg.get_exist_vm())
    {
        out << "===============================\n";
        out << vm->get_vm_info();
    }
    return 0;
}

int vmcmdline(const char *name)
{
    GlobalConfig cfg;
    VMConfig *vm = cfg.get_vm_by_name(name);
    if (!vm)
    {
        out << "VM " << name << " does not exist\n";
        return 1;
    }
    CommandLineParameters params;
    out << vm->getCommandLine(params) << "\n";
    return 0;
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
        return vmlist();
    }
    else if (!strcmp(argv[1], "cmdline"))
    {
        if (argc < 3)
        {
            usage();
            return 1;
        }
        else
        {
            return vmcmdline(argv[2]);
        }
    }
    else
    {
        usage();
        return 1;
    }    

    return 0;
}