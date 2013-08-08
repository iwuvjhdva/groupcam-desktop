#ifndef COMMON_H
#define COMMON_H

#ifdef Q_WS_WIN
#include <windows.h>
#endif

#include <TeamTalk4.h>

#if defined(Q_OS_WINCE)
#define _W(qstr) qstr.utf16()
#define _Q(wstr) QString::fromWCharArray(wstr)
#define COPY_TTSTR(wstr, qstr) wcsncpy(wstr, _W(qstr), TT_STRLEN)

#elif defined(Q_OS_WIN32)
#define _W(qstr) qstr.toStdWString().c_str()
#define _Q(wstr) QString::fromWCharArray(wstr)
#define COPY_TTSTR(wstr, qstr) wcsncpy(wstr, _W(qstr), TT_STRLEN)

#else
#define _W(qstr) qstr.toUtf8().constData()
#define _Q(wstr) QString::fromUtf8(wstr)
#define COPY_TTSTR(wstr, qstr) strncpy(wstr, _W(qstr), TT_STRLEN)
#endif

#endif // COMMON_H
