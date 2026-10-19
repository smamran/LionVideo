#pragma once

#include <QString>
#include <QtGlobal>

namespace utils {

class TimeFormatter {
public:
    static QString formatTime(qint64 milliseconds, bool forceHours = false);
    static QString formatTimeWithTotal(qint64 currentMs, qint64 totalMs);
};

} // namespace utils
