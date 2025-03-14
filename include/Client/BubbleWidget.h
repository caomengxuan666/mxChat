/**
 * @FilePath     : /mxChat/include/Client/BubbleWidget.h
 * @Description  :  cmx手绘的气泡类，整个聊天界面的气泡都是在这里绘制的
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-03-12 12:39:12
 * @Copyright    : PESONAL DEVELOPER CMX., Copyright (c) 2025.
**/
#pragma once

#include <QPainter>
#include <QWidget>

/**
 * @author       : caomengxuan666
 * @brief        : 具体的消息类型，分为[自我，他人，系统，文件]
**/
enum class MessageType {
    Self,
    Other,
    System,
    File
};

/**
 * @author       : caomengxuan666
 * @brief        : 用于创建聊天气泡
**/
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
    void contextMenuEvent(QContextMenuEvent *event)override;
    void copyContent();

private:
    struct LayoutMetrics {
        QRect bubbleRect; // 气泡区域
        QRect avatarRect; // 头像区域
        QRect textRect;   // 文本区域
        QRect timeRect;   // 时间戳区域
        int contentHeight;// 动态计算高度
        QRect fileInfoRect;
    };

    /**
     * @author       : caomengxuan666
     * @brief        : 用于计算气泡的布局
     * @return        {*}
    **/    
    void calculateLayout();
    /**
     * @author       : caomengxuan666
     * @brief        : 绘制气泡的背景
     * @param         {QPainter} &painter:
     * @return        {*}
    **/    
    void drawBubbleBackground(QPainter &painter);
    /**
     * @author       : caomengxuan666
     * @brief        : 绘制气泡的头像
     * @param         {QPainter} &painter:
     * @return        {*}
    **/    
    void drawAvatar(QPainter &painter);
    /**
     * @author       : caomengxuan666
     * @brief        : 绘制气泡的内容
     * @param         {QPainter} &painter:
     * @return        {*}
    **/    
    void drawTextContent(QPainter &painter);
    /**
     * @author       : caomengxuan666
     * @brief        : 绘制气泡的时间戳
     * @param         {QPainter} &painter:
     * @return        {*}
    **/    
    void drawTimestamp(QPainter &painter);
    /**
     * @author       : caomengxuan666
     * @brief        : 绘制文件图标
     * @param         {QPainter} &painter:
     * @param         {QRect} &rect:
     * @return        {*}
    **/    
    void drawFileIcon(QPainter &painter, const QRect &rect);
    //void applyStyle();

    QString sender;
    QString time;
    QString content;
    MessageType type;
    LayoutMetrics layout;
    QFont contentFont{"Microsoft YaHei", 12};// 内容字体
    QFont timeFont{"Arial", 9};              // 时间戳字体
    const int kMinHeight = 60;               // 最小高度
};
