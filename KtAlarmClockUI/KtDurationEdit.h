/**
 * @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2022
 * @license     LGPL 3.0
 * @author      Kuntai
 * @file        KtDurationEdit.h
 * @brief       m:ss 时长输入框（内部始终为秒）
 */
#ifndef KtDurationEdit_H
#define KtDurationEdit_H

#include <QLineEdit>

class QKeyEvent;

/**
 * @brief 以分:秒格式编辑时长；对外以秒读写
 */
class KtDurationEdit : public QLineEdit {
    Q_OBJECT
    Q_PROPERTY(int seconds READ get_seconds WRITE set_seconds NOTIFY seconds_changed)

public:
    explicit KtDurationEdit(QWidget* parent = nullptr);
    ~KtDurationEdit() override;

public:
    /** @brief 当前秒数 */
    int get_seconds() const {
        return seconds_;
    }

    /** @brief 合法范围下限（秒） */
    int get_minimum() const {
        return minSeconds_;
    }

    /** @brief 合法范围上限（秒） */
    int get_maximum() const {
        return maxSeconds_;
    }

    /** @brief 设置秒数并刷新显示（不向外发射信号） */
    void set_seconds(int seconds);

    /** @brief 设置合法范围（秒） */
    void set_range(int minSeconds, int maxSeconds);

public slots:
    /** @brief 解析当前文本、校正格式并发射 seconds_changed */
    void commit_text();

signals:
    /** @brief 用户确认输入后的秒数 */
    void seconds_changed(int seconds);

protected:
    void keyPressEvent(QKeyEvent* event) override;

private:
    /** @brief 秒数格式化为 m:ss */
    static QString format_seconds(int seconds);

    /** @brief 解析 m:ss、Ns（秒）或纯分钟文本为秒 */
    static bool parse_text(const QString& text, int* outSeconds);

private:
    int seconds_;    ///< 1. 当前秒数
    int minSeconds_; ///< 2. 下限
    int maxSeconds_; ///< 3. 上限
};

#endif // KtDurationEdit_H
