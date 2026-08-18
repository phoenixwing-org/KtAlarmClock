/**
 * @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2022
 * @license     Apache-2.0
 * @author      Kuntai
 * @file        KtWallClockEngine.h
 * @brief       墙钟倒计时引擎
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
    /** @brief 系统进入休眠：冻结剩余秒数，休眠期间不计时 */
    void enter_system_sleep();

    /**
     * @brief 按最后一次有效 tick 时刻冻结（用于未收到 Suspended 时的补检）
     * @param wallMs 冻结参照时刻（通常为末次 tick 的墙钟 ms）
     */
    void freeze_for_system_sleep_at(qint64 wallMs);

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

    /** @brief 是否因系统休眠而冻结（唤醒后可自动续计） */
    bool is_sleep_suspended() const {
        return sleepSuspended_;
    }

    /** @brief 暂停计时并保存当前剩余秒数 */
    void pause();

    /**
     * @brief 墙钟起点落在系统时间之后时对齐到当前时刻
     *
     * 休眠唤醒或 NTP 回拨后 elapsed 恒为 0，显示会长时间不走秒。
     */
    void realign_phase_start_if_ahead();

    /**
     * @brief 重置阶段状态，停止计时
     * @param state 工作阶段枚举（见 KtAlarmClock）
     * @param counterSec 剩余秒数
     */
    void reset(int state, int counterSec);

    /** @brief 休眠唤醒后从冻结点继续；若已到期返回 false */
    bool resume_after_system_sleep();

    /**
     * @brief 启动或恢复倒计时
     * @param state 工作阶段枚举
     * @param counterSec 剩余秒数；≤0 时不启动
     * @param leanFirstSecond 为 true 时首秒立即计入（仅播放/继续）；Next、±60s 应为 false
     */
    void start(int state, int counterSec, bool leanFirstSecond = false);

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
    bool   sleepSuspended_;   ///< 6. 工作时因系统休眠冻结，唤醒后续计
};

#endif // KtWallClockEngine_H
