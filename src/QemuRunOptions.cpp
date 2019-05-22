#include "QemuRunOptions.h"

QemuRunOptions::QemuRunOptions() :
    logfileName(""), logOptionList({}), additionalCmdLine(""),
    isDebugEnable(false), isSnapshotEnable(false)
{

}

