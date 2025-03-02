/**
 * @FilePath     : /mxChat/include/Client/BubbleWidget.h
 * @Description  :  cmx手绘的气泡类，整个聊天界面的气泡都是在这里绘制的
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-03-02 11:26:39
 * @Copyright    : PESONAL DEVELOPER CMX., Copyright (c) 2025.
**/
#pragma once

#include <QPainter>
#include <QWidget>

enum class MessageType {
    Self,
    Other,
    System,
    File
};

class BubbleWidget : public QWidget {
    Q_OBJECT
public:
    explicit BubbleWidget(const QString &sender,
                          const QString &time,
                          const QString &content,
                          MessageType type,
                          QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;


private:
    struct LayoutMetrics {
        QRect bubbleRect; // 气泡区域
        QRect avatarRect; // 头像区域
        QRect textRect;   // 文本区域
        QRect timeRect;   // 时间戳区域
        int contentHeight;// 动态计算高度
        QRect fileInfoRect;
    };

    void calculateLayout();
    void drawBubbleBackground(QPainter &painter);
    void drawAvatar(QPainter &painter);
    void drawTextContent(QPainter &painter);
    void drawTimestamp(QPainter &painter);
    void drawFileIcon(QPainter &painter, const QRect &rect);
    void applyStyle();

    QString sender;
    QString time;
    QString content;
    MessageType type;
    LayoutMetrics layout;
    QFont contentFont{"Microsoft YaHei", 12};// 内容字体
    QFont timeFont{"Arial", 9};              // 时间戳字体
    const int kMinHeight = 60;               // 最小高度
};
