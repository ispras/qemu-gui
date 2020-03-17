#include "GlobalConfig.h"

int main(int argc, char *argv[])
{
    GlobalConfig cfg;
    foreach(VMConfig *vm, cfg.get_exist_vm())
    {
        qInfo() << vm->get_vm_info();
    }

    return 0;
}