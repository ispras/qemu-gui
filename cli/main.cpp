#include <QTextStream>
#include "GlobalConfig.h"
#include "CommandLineParameters.h"
#include "QemuList.h"

QTextStream out(stdout);

void usage()
{
    out << "qemu-cli usage:\n"
        "list - output configured VMs\n"
        "qemulist - output configured QEMU installations\n"
        "cmdline <vm> [record | replay]"
        " - output command line for running specified VM\n";
}

int vmlist()
{
    foreach(VMConfig *vm, GlobalConfig::get_exist_vm())
    {
        out << "===============================\n";
        out << vm->get_vm_info();
    }
    return 0;
}

int qemulist()
{
    QemuList::Items q = QemuList::getAllQemuInstallations();
    foreach(QString name, q.keys())
    {
        out << name << " : " << q.value(name) << "\n";
    }
    return 0;
}

int vmcmdline(const char *name, LaunchMode mode)
{
    VMConfig *vm = GlobalConfig::get_vm_by_name(name);
    if (!vm)
    {
        out << "VM " << name << " does not exist\n";
        return 1;
    }
    CommandLineParameters params(mode);
    params.setOverlayEnabled(false);
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
    else if (!strcmp(argv[1], "qemulist"))
    {
        return qemulist();
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
            LaunchMode mode = LaunchMode::NORMAL;
            char **arg = argv + 3;
            while (*arg)
            {
                if (!strcmp(*arg, "record"))
                {
                    mode = LaunchMode::RECORD;
                }
                else if (!strcmp(*arg, "replay"))
                {
                    mode = LaunchMode::REPLAY;
                }
                else
                {
                    usage();
                    return 1;
                }
                ++arg;
            }
            return vmcmdline(argv[2], mode);
        }
    }
    else
    {
        usage();
        return 1;
    }    

    return 0;
}