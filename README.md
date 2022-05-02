# KTQAlarmClock

#### 介绍
KT闹钟程序

#### 软件架构
纯QT程序，QT5，QT Widget， 信号槽等


#### 安装教程

1.  绿色软件，直接运行，需要Qt5的相关dll支持。

#### 使用说明

1. 设定总时间、休息时间，工作时间，
1. 到时机后会锁定屏幕几分钟，无法解锁，
1. 解锁结束后，可以继续操作电脑
1. 可以用于PPT演讲的计时。
详细请查看wiki使用说明

### 待完成工作
1.换肤功能
2.UI美化

### pro配置
#### SUBDIRS
- 主目录的pro，配置为 subdirs
- 子目录的顺序就是编译的顺序。要把底层动态库放到前面。
```
SUBDIRS += \
    KTQAlarmClockUI \
    KTQAlarmClock
```
#### include path

区分系统见KT_BASE_INCLUDE的配置。

#### 参与贡献

1.  kevin
2.  Jane
