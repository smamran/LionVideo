#include "TimeFormatter.h"
#include <algorithm>

namespace utils {

QString TimeFormatter::formatTime(qint64 milliseconds, bool forceHours) {
    if (milliseconds < 0) {
        milliseconds = 0;
    }

    qint64 totalSeconds = milliseconds / 1000;
    qint64 hours = totalSeconds / 3600;
    qint64 minutes = (totalSeconds % 3600) / 60;
    qint64 seconds = totalSeconds % 60;

    if (hours > 0 || forceHours) {
        return QString("%1:%2:%3")
            .arg(hours, 2, 10, QChar('0'))
            .arg(minutes, 2, 10, QChar('0'))
            .arg(seconds, 2, 10, QChar('0'));
    }
    return QString("%1:%2")
        .arg(minutes, 2, 10, QChar('0'))
        .arg(seconds, 2, 10, QChar('0'));
}

QString TimeFormatter::formatTimeWithTotal(qint64 currentMs, qint64 totalMs) {
    bool hasHours = totalMs >= 3600000;
    return QString("%1 / %2")
        .arg(formatTime(currentMs, hasHours))
        .arg(formatTime(totalMs, hasHours));
}

} // namespace utils
