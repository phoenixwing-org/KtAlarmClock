/**
 * @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2022
 * @license     Apache-2.0
 * @author      Kuntai
 * @file        KtDurationEdit.cpp
 */

#include "KtDurationEdit.h"

#include <QKeyEvent>
#include <QSignalBlocker>

//------------------------------------------------------
KtDurationEdit::KtDurationEdit(QWidget* parent)
    : QLineEdit(parent)
    , seconds_(0)     // 1
    , minSeconds_(0)  // 2
    , maxSeconds_(0) { // 3
    setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    setPlaceholderText(QStringLiteral("分:秒"));
    setToolTip(QStringLiteral("分:秒如 45:00；10s 表示 10 秒；纯数字视为分钟"));
    setClearButtonEnabled(false);

    connect(this, &QLineEdit::editingFinished, this, &KtDurationEdit::commit_text);

    set_seconds(0);
}
//------------------------------------------------------
KtDurationEdit::~KtDurationEdit() {
    // seconds_     // 1
    // minSeconds_  // 2
    // maxSeconds_  // 3
}
//------------------------------------------------------
void KtDurationEdit::commit_text() {
    int parsed = 0;
    if (!parse_text(text(), &parsed)) {
        set_seconds(seconds_);
        return;
    }

    const int clamped = qBound(minSeconds_, parsed, maxSeconds_);
    if (clamped == seconds_) {
        set_seconds(seconds_);
        return;
    }

    set_seconds(clamped);
    emit seconds_changed(clamped);
}
//------------------------------------------------------
QString KtDurationEdit::format_seconds(int seconds) {
    if (seconds <= 0) return QStringLiteral("0:00");
    const int minutePart = seconds / 60;
    const int secondPart = seconds % 60;
    return QStringLiteral("%1:%2").arg(minutePart).arg(secondPart, 2, 10, QChar('0'));
}
//------------------------------------------------------
void KtDurationEdit::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        commit_text();
        clearFocus();
        event->accept();
        return;
    }

    QLineEdit::keyPressEvent(event);
}
//------------------------------------------------------
bool KtDurationEdit::parse_text(const QString& text, int* outSeconds) {
    if (!outSeconds) return false;

    const QString trimmed = text.trimmed();
    if (trimmed.isEmpty()) return false;

    QString body = trimmed;
    if (body.endsWith(QLatin1Char('s'), Qt::CaseInsensitive)) {
        body = body.left(body.length() - 1).trimmed();
        bool okSecond = false;
        const int secondsOnly = body.toInt(&okSecond);
        if (!okSecond || secondsOnly < 0) return false;

        *outSeconds = secondsOnly;
        return true;
    }

    int minutePart = 0;
    int secondPart = 0;

    if (body.contains(QLatin1Char(':'))) {
        const QStringList parts = body.split(QLatin1Char(':'));
        if (parts.size() != 2) return false;

        bool okMinute = false;
        bool okSecond = false;
        minutePart    = parts[0].toInt(&okMinute);
        secondPart    = parts[1].toInt(&okSecond);
        if (!okMinute || !okSecond || minutePart < 0 || secondPart < 0 || secondPart > 59)
            return false;
    }
    else {
        bool okMinute = false;
        minutePart    = body.toInt(&okMinute);
        if (!okMinute || minutePart < 0) return false;
        secondPart = 0;
    }

    *outSeconds = minutePart * 60 + secondPart;
    return true;
}
//------------------------------------------------------
void KtDurationEdit::set_range(int minSeconds, int maxSeconds) {
    minSeconds_ = qMin(minSeconds, maxSeconds);
    maxSeconds_ = qMax(minSeconds, maxSeconds);
    set_seconds(qBound(minSeconds_, seconds_, maxSeconds_));
}
//------------------------------------------------------
void KtDurationEdit::set_seconds(int seconds) {
    seconds_ = qBound(minSeconds_, seconds, maxSeconds_);

    const QSignalBlocker blocker(this);
    setText(format_seconds(seconds_));
}
