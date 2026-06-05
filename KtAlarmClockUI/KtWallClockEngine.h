/**
 * @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2022
 * @license     LGPL 3.0
 * @author      Kuntai
 * @file        KtWallClockEngine.h
 * @brief       墙钟倒计时引擎（MyClock.qml 核心逻辑的 C++ 实现）
 */
#ifndef KtWallClockEngine_H
#define KtWallClockEngine_H

#include <QtGlobal>

/**
 * @brief 基于系统时钟的倒计时引擎
 *
 * 以 phaseStartMs_ + phaseDurationSec_ 为基准计算剩余秒数，
 * 支持暂停时冻结剩余时间、继续时重新建立墙钟起点。
 */
class KtWallClockEngine {
public:
    KtWallClockEngine();  // @nodoc
    ~KtWallClockEngine(); // @nodoc

public:
    /** @brief 剩余秒数 */
    int get_counter() const {
        return counter_;
    }

    /** @brief 是否正在倒计时 */
    bool get_running() const {
        return running_;
    }

    /** @brief 当前工作阶段 */
    int get_state() const {
        return state_;
    }

    /** @brief 运行中且剩余秒数 ≤ 0 */
    bool is_expired() const {
        return running_ && counter_ <= 0;
    }

    /** @brief 暂停计时并保存当前剩余秒数 */
    void pause();

    /**
     * @brief 重置阶段状态，停止计时
     * @param state 工作阶段枚举（见 KtAlarmClock）
     * @param counterSec 剩余秒数
     */
    void reset(int state, int counterSec);

    /**
     * @brief 启动或恢复倒计时
     * @param state 工作阶段枚举
     * @param counterSec 剩余秒数；≤0 时不启动
     */
    void start(int state, int counterSec);

    /** @brief 从墙钟同步剩余秒数（运行中每秒调用） */
    void sync_from_wall_clock();

private:
    /** @brief 根据墙钟更新 counter_ */
    void update_remaining_from_wall_clock();

private:
    int    state_;            ///< 1. 工作阶段（KtAlarmClock::WorkTime 等）
    int    counter_;          ///< 2. 剩余秒数
    int    phaseDurationSec_; ///< 3. 当前阶段总时长(秒)
    qint64 phaseStartMs_;     ///< 4. 阶段墙钟起点(ms)；0 表示未启动
    bool   running_;          ///< 5. 是否正在倒计时
};

#endif // KtWallClockEngine_H
