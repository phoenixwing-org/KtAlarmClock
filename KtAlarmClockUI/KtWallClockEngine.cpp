/**
 * @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2022
 * @license     LGPL 3.0
 * @author      Kuntai
 * @file        KtWallClockEngine.cpp
 */
#include "KtWallClockEngine.h"

#include <QDateTime>
#include <algorithm>

namespace {
/** @brief 已过秒数；墙钟回拨（休眠唤醒、NTP 校时）时不得为负，否则剩余会反常增大 */
int elapsed_seconds_clamped(qint64 phaseStartMs, qint64 wallMs) {
    if (phaseStartMs <= 0)
        return 0;
    const qint64 elapsedMs = qMax<qint64>(0, wallMs - phaseStartMs);
    return static_cast<int>(elapsedMs / 1000);
}
} // namespace

//------------------------------------------------------
KtWallClockEngine::KtWallClockEngine()
    : state_(0)            // 1
    , counter_(0)          // 2
    , phaseDurationSec_(0) // 3
    , phaseStartMs_(0)     // 4
    , running_(false)      // 5
    , sleepSuspended_(false) { // 6
}
//------------------------------------------------------
KtWallClockEngine::~KtWallClockEngine() {
    // 无指针成员；下列序号仅与 .h 成员顺序对齐核对
    // state_            // 1
    // counter_          // 2
    // phaseDurationSec_ // 3
    // phaseStartMs_     // 4
    // running_          // 5
    // sleepSuspended_   // 6
}
//------------------------------------------------------
void KtWallClockEngine::enter_system_sleep() {
    if (!running_) return;

    update_remaining_from_wall_clock();

    running_         = false;
    phaseStartMs_    = 0;
    sleepSuspended_  = true;
    if (counter_ > 0) phaseDurationSec_ = counter_;
}
//------------------------------------------------------
void KtWallClockEngine::freeze_for_system_sleep_at(qint64 wallMs) {
    if (!running_ || phaseStartMs_ <= 0 || phaseDurationSec_ <= 0) return;

    const int elapsedSec = elapsed_seconds_clamped(phaseStartMs_, wallMs);
    counter_             = std::max(0, phaseDurationSec_ - elapsedSec);

    running_        = false;
    phaseStartMs_   = 0;
    sleepSuspended_ = true;
    if (counter_ > 0) phaseDurationSec_ = counter_;
}
//------------------------------------------------------
bool KtWallClockEngine::resume_after_system_sleep() {
    if (!sleepSuspended_) return false;

    sleepSuspended_ = false;
    if (counter_ <= 0) return false;

    start(state_, counter_);
    return running_;
}
//------------------------------------------------------
void KtWallClockEngine::pause() {
    // 暂停：先同步剩余时间，再清除墙钟起点

    sleepSuspended_ = false;

    if (running_) update_remaining_from_wall_clock(); // 冻结当前剩余秒数到 counter_

    running_      = false;                          // 停止 tick 驱动
    phaseStartMs_ = 0;                              // 清除起点，resume 时会重建
    if (counter_ > 0) phaseDurationSec_ = counter_; // 保留剩余时长供 resume 使用
}
//------------------------------------------------------
void KtWallClockEngine::reset(int state, int counterSec) {
    // 停止计时并写入新的阶段与剩余秒数

    state_            = state;      // 如 WorkTime / WorkBreak
    counter_          = counterSec; // 显示用剩余秒
    phaseDurationSec_ = counterSec; // 若随后 start，以此为阶段总长
    phaseStartMs_     = 0;          // 清除墙钟，尚未运行
    running_          = false;      // 标记停止
    sleepSuspended_   = false;
}
//------------------------------------------------------
void KtWallClockEngine::start(int state, int counterSec, bool leanFirstSecond) {
    // 启动或恢复：以当前 counter_ 为新阶段，重建墙钟起点

    state_   = state;
    counter_ = counterSec;

    // 剩余秒数无效时不启动，避免下一 tick 立即判到期
    if (counter_ <= 0) {
        running_ = false;
        return;
    }

    const qint64 nowMs = QDateTime::currentMSecsSinceEpoch();
    phaseDurationSec_ = counter_; // 本阶段总秒数
    phaseStartMs_     = leanFirstSecond ? (nowMs - 1000) : nowMs;
    update_remaining_from_wall_clock();
    running_         = true;                                 // 标记运行中
    sleepSuspended_  = false;
}
//------------------------------------------------------
void KtWallClockEngine::sync_from_wall_clock() {
    if (!running_) return;              // 暂停态不更新
    realign_phase_start_if_ahead();
    update_remaining_from_wall_clock(); // 外部每秒 tick 调用
}
//------------------------------------------------------
void KtWallClockEngine::realign_phase_start_if_ahead() {
    if (!running_ || phaseStartMs_ <= 0)
        return;

    const qint64 now = QDateTime::currentMSecsSinceEpoch();
    if (now < phaseStartMs_)
        phaseStartMs_ = now; // 从当前墙钟重新计 elapsed，保持 counter_ 不变
}
//------------------------------------------------------
void KtWallClockEngine::update_remaining_from_wall_clock() {
    // 公式：counter_ = max(0, phaseDurationSec_ - (now - phaseStartMs_) / 1000)

    // 无效阶段或未 start（phaseStartMs_==0）时不推导
    if (state_ == 0 || phaseDurationSec_ <= 0 || phaseStartMs_ <= 0) return;

    const int elapsedSec =
        elapsed_seconds_clamped(phaseStartMs_, QDateTime::currentMSecsSinceEpoch());
    counter_ = std::max(0, phaseDurationSec_ - elapsedSec); // 剩余秒，截断到 0
}
