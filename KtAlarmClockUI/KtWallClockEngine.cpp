/**
 * @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2022
 * @license     LGPL 3.0
 * @author      Kuntai
 * @file        KtWallClockEngine.cpp
 */
#include "KtWallClockEngine.h"

#include <QDateTime>
#include <algorithm>

//------------------------------------------------------
KtWallClockEngine::KtWallClockEngine()
    : state_(0)            // 1
    , counter_(0)          // 2
    , phaseDurationSec_(0) // 3
    , phaseStartMs_(0)     // 4
    , running_(false)      // 5
{
}
//------------------------------------------------------
KtWallClockEngine::~KtWallClockEngine() {
    // 无指针成员；下列序号仅与 .h 成员顺序对齐核对
    // state_            // 1
    // counter_          // 2
    // phaseDurationSec_ // 3
    // phaseStartMs_     // 4
    // running_          // 5
}
//------------------------------------------------------
void KtWallClockEngine::pause() {
    // 暂停：先同步剩余时间，再清除墙钟起点

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
}
//------------------------------------------------------
void KtWallClockEngine::start(int state, int counterSec) {
    // 启动或恢复：以当前 counter_ 为新阶段，重建墙钟起点

    state_   = state;
    counter_ = counterSec;

    // 剩余秒数无效时不启动，避免下一 tick 立即判到期
    if (counter_ <= 0) {
        running_ = false;
        return;
    }

    phaseDurationSec_ = counter_;                            // 本阶段总秒数
    phaseStartMs_     = QDateTime::currentMSecsSinceEpoch(); // 记录墙钟起点
    update_remaining_from_wall_clock();                      // 对齐 counter_ 与墙钟
    running_ = true;                                         // 标记运行中
}
//------------------------------------------------------
void KtWallClockEngine::sync_from_wall_clock() {
    if (!running_) return;              // 暂停态不更新
    update_remaining_from_wall_clock(); // 外部每秒 tick 调用
}
//------------------------------------------------------
void KtWallClockEngine::update_remaining_from_wall_clock() {
    // 公式：counter_ = max(0, phaseDurationSec_ - (now - phaseStartMs_) / 1000)

    // 无效阶段或未 start（phaseStartMs_==0）时不推导
    if (state_ == 0 || phaseDurationSec_ <= 0 || phaseStartMs_ <= 0) return;

    const qint64 elapsedMs  = QDateTime::currentMSecsSinceEpoch() - phaseStartMs_; // 已过毫秒
    const int    elapsedSec = static_cast<int>(elapsedMs / 1000);                  // 折算整秒
    counter_ = std::max(0, phaseDurationSec_ - elapsedSec); // 剩余秒，截断到 0
}
