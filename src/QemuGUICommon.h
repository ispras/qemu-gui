#ifndef QEMUGUI_COMMON_H
#define QEMUGUI_COMMON_H

#include <QWidget>
#include <QtWidgets>

#if QT_VERSION < 0x050700
template<typename... Args> struct QOverload {
    template<typename C, typename R>
    static constexpr auto of(R(C::*pmf)(Args...))->decltype(pmf) {
        return pmf;
    }
};
#endif

#define MAX_RAM_VALUE (32768 / 2)
#define MIN_RAM_VALUE 4

#endif
