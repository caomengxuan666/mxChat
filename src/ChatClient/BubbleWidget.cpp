/**
 * @FilePath     : /mxChat/src/ChatClient/BubbleWidget.cpp
 * @Description  : 
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-03-02 12:14:24
 * @Copyright    : PESONAL DEVELOPER CMX., Copyright (c) 2025.
**/
#include "Client/BubbleWidget.h"
#include <QFileInfo>
#include <QFontMetrics>
#include <QPainterPath>
#include <QContextMenuEvent>
#include <QMenu>
#include <QClipboard>
#include <QApplication>

BubbleWidget::BubbleWidget(const QString &sender,
                           const QString &time,
                           const QString &content,
                           MessageType type,
                           QWidget *parent)
    : QWidget(parent),
      sender(sender),
      time(time),
      content(content),
      type(type) {
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    calculateLayout();
}

void BubbleWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    drawBubbleBackground(painter);
    drawAvatar(painter);
    drawTextContent(painter);
    drawTimestamp(painter);
}

void BubbleWidget::resizeEvent(QResizeEvent *event) {
    calculateLayout();
    QWidget::resizeEvent(event);
}

void BubbleWidget::calculateLayout() {
    const int kAvatarSize = 40;// 头像尺寸
    const int kPadding = 12;   // 基础边距
    const int kTextPadding = 8;// 文本内边距

    // 计算文本实际高度
    QFontMetrics fm(contentFont);
    int textWidth = width() - 2 * kPadding - kAvatarSize - 20;// 可用宽度
    textWidth = qBound(100, textWidth, 600);                  // 限制最小100px，最大600px

    // 计算文本高度（包含换行）
    QRect textBounding = fm.boundingRect(
            0, 0, textWidth, 1000,
            Qt::TextWordWrap | Qt::AlignTop,
            content);

    // 计算文本的实际宽度（不包括换行）
    int actualTextWidth = fm.horizontalAdvance(content);

    // 如果文本只有一行，则根据实际宽度调整气泡宽度
    if (textBounding.height() <= fm.height()) {
        textWidth = qBound(100, actualTextWidth + 2 * kTextPadding, 600);
    }

    // 修改后的高度计算（移除了最大高度限制）
    int contentHeight = textBounding.height() + 2 * kPadding + 30;
    layout.contentHeight = qMax(kMinHeight, contentHeight);
    setFixedHeight(layout.contentHeight);

    // 根据消息类型布局
    if (type == MessageType::Self) {
        layout.avatarRect = QRect(
                width() - kAvatarSize - kPadding,
                kPadding,
                kAvatarSize,
                kAvatarSize);
        // 修改后的气泡区域计算
        layout.bubbleRect = QRect(width() - textWidth - kAvatarSize - 3 * kPadding, kPadding, textWidth + 2 * kPadding, layout.contentHeight - 2 * kPadding);
    } else if (type == MessageType::Other) {
        layout.avatarRect = QRect(kPadding, kPadding, kAvatarSize, kAvatarSize);
        // 修改后的气泡区域计算
        layout.bubbleRect = QRect(kAvatarSize + 2 * kPadding, kPadding, textWidth + 2 * kPadding, layout.contentHeight - 2 * kPadding);
    } else if (type == MessageType::System) {
        // 系统消息居中显示
        layout.avatarRect = QRect();// 系统消息不需要头像
        layout.bubbleRect = QRect((width() - textWidth - 2 * kPadding) / 2, kPadding, textWidth + 2 * kPadding, layout.contentHeight - 2 * kPadding);
    } else if (type == MessageType::File) {
        // 文件消息居中显示
        layout.avatarRect = QRect();// 文件消息不需要头像
        layout.bubbleRect = QRect((width() - textWidth - 2 * kPadding) / 2, kPadding, textWidth + 2 * kPadding, layout.contentHeight - 2 * kPadding);
        // 初始化 fileInfoRect
        layout.fileInfoRect = QRect(layout.bubbleRect.left() + kPadding, layout.bubbleRect.top() + kPadding + 40, textWidth, 40);
    }

    layout.textRect = layout.bubbleRect.adjusted(
            kTextPadding,
            kTextPadding,
            -kTextPadding,
            -kTextPadding);
    if (type == MessageType::System) {
        layout.textRect.moveCenter(layout.bubbleRect.center());
    }
    if (type == MessageType::Self || type == MessageType::Other) {
        layout.timeRect = QRect(
                layout.bubbleRect.right() - kPadding - fm.horizontalAdvance(time),
                layout.bubbleRect.bottom() - 28,
                fm.horizontalAdvance(time) + 2 * kPadding,
                20);
    } else {
        layout.timeRect = QRect(
                layout.bubbleRect.left() + kPadding,
                layout.bubbleRect.bottom() - 28,
                layout.bubbleRect.width() - 2 * kPadding,
                20);
    }
}

void BubbleWidget::drawAvatar(QPainter &painter) {
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor("#00B4FF"));
    painter.drawEllipse(layout.avatarRect);

    // 绘制首字母
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 16, QFont::Bold));
    painter.drawText(layout.avatarRect, Qt::AlignCenter, sender.left(1).toUpper());
}

void BubbleWidget::drawTextContent(QPainter &painter) {
    painter.setFont(contentFont);
    // 确保文本颜色设置正确
    painter.setPen(type == MessageType::Self ? Qt::white : Qt::black);

    // 带自动换行的文本绘制
    QTextOption textOption;
    textOption.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    painter.drawText(layout.textRect, content, textOption);
}

void BubbleWidget::drawTimestamp(QPainter &painter) {
    painter.setFont(timeFont);
    // 修改时间戳颜色为白色，以便在蓝色气泡上更明显
    painter.setPen(type == MessageType::Self ? Qt::white : QColor(150, 150, 150));
    // 时间戳居中显示
    painter.drawText(layout.timeRect, Qt::AlignCenter, time);
}

void BubbleWidget::drawBubbleBackground(QPainter &painter) {
    QPainterPath path;
    const int kCornerRadius = 12;
    const int kArrowWidth = 10;
    QColor bubbleColor;
    QColor borderColor;

    // 公共路径定义
    QRect bubbleRect = layout.bubbleRect;
    const int bubbleTop = bubbleRect.top();
    const int bubbleBottom = bubbleRect.bottom();
    const int bubbleLeft = bubbleRect.left();
    const int bubbleRight = bubbleRect.right();

    switch (type) {
        case MessageType::Self: {
            bubbleColor = QColor("#1688FF");
            path.addRoundedRect(bubbleRect, kCornerRadius, kCornerRadius);
            path.moveTo(bubbleRight, bubbleTop + kCornerRadius);
            path.lineTo(bubbleRight + kArrowWidth, bubbleTop + kCornerRadius - kArrowWidth / 2.0);
            path.lineTo(bubbleRight + kArrowWidth, bubbleTop + kCornerRadius + kArrowWidth / 2.0);
            break;
        }

        case MessageType::Other: {
            bubbleColor = QColor("#FFFFFF");
            path.addRoundedRect(bubbleRect, kCornerRadius, kCornerRadius);
            path.moveTo(bubbleLeft, bubbleTop + kCornerRadius);
            path.lineTo(bubbleLeft - kArrowWidth, bubbleTop + kCornerRadius - kArrowWidth / 2.0);
            path.lineTo(bubbleLeft - kArrowWidth, bubbleTop + kCornerRadius + kArrowWidth / 2.0);
            break;
        }

        case MessageType::System: {
            bubbleColor = QColor("#F8F9FA");
            borderColor = QColor("#E9ECEF");
            path.addRoundedRect(bubbleRect, kCornerRadius, kCornerRadius);
            QRect iconRect(bubbleLeft + 8, bubbleTop + 8, 20, 20);
            painter.setPen(Qt::NoPen);
            painter.setBrush(QColor("#ADB5BD"));
            painter.drawEllipse(iconRect);
            break;
        }

        case MessageType::File: {
            bubbleColor = QColor("#E7F5FF");
            path.addRoundedRect(bubbleRect, kCornerRadius, kCornerRadius);

            // 文件图标绘制在气泡顶部
            QRect fileIconRect(bubbleLeft + 12, bubbleTop + 12, 32, 40);
            drawFileIcon(painter, fileIconRect);

            // 文件信息绘制在图标下方
            QRect fileInfoRect = QRect(bubbleLeft + 12, bubbleTop + 55, bubbleRect.width() - 24, 40);// 下移文字位置
            painter.setFont(QFont("Microsoft YaHei", 10));
            painter.setPen(QColor("#495057"));
            painter.drawText(fileInfoRect, Qt::AlignLeft, "requirements.docx");

            QFileInfo info("requirements.docx");
            QString sizeText = QString("%1 KB").arg(info.size() / 1024);
            painter.drawText(fileInfoRect, Qt::AlignRight, sizeText);
            break;
        }
    }

    path.closeSubpath();
    painter.fillPath(path, bubbleColor);

    if (type == MessageType::System) {
        painter.setPen(QPen(borderColor, 1));
        painter.drawPath(path);
    }
}

void BubbleWidget::contextMenuEvent(QContextMenuEvent *event) {
    QMenu menu(this);
    QAction *copyAction = menu.addAction("Copy");
    connect(copyAction, &QAction::triggered, this, &BubbleWidget::copyContent);

    // 设置菜单样式表
    menu.setStyleSheet(
        "QMenu {"
        "   background-color: #FFFFFF;"
        "   border: 1px solid #D3D3D3;"
        "   border-radius: 5px;"
        "   padding: 5px;"
        "}"
        "QMenu::item {"
        "   padding: 5px 15px;"
        "   border: none;"
        "}"
        "QMenu::item:selected {"
        "   background-color: #E0E0E0;"
        "}"
    );

    menu.exec(event->globalPos());
}

void BubbleWidget::copyContent() {
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(content);
}

// 完整的文件图标绘制
void BubbleWidget::drawFileIcon(QPainter &painter, const QRect &rect) {
    painter.save();

    // 文件主体
    QPainterPath filePath;
    filePath.moveTo(rect.left(), rect.top() + 8);
    filePath.lineTo(rect.left(), rect.bottom());
    filePath.lineTo(rect.right() - 4, rect.bottom());
    filePath.lineTo(rect.right(), rect.bottom() - 4);
    filePath.lineTo(rect.right(), rect.top());
    filePath.closeSubpath();

    // 折叠角
    QPainterPath foldPath;
    foldPath.moveTo(rect.right() - 12, rect.top());
    foldPath.lineTo(rect.right(), rect.top());
    foldPath.lineTo(rect.right(), rect.top() + 12);
    foldPath.closeSubpath();

    // 填充颜色
    painter.setPen(QPen(QColor("#1C7CD5"), 1));
    painter.setBrush(QColor("#B3D7FF"));
    painter.drawPath(filePath);
    painter.drawPath(foldPath);

    // 文件类型图标
    QFont font = painter.font();
    font.setPixelSize(14);
    painter.setFont(font);
    painter.setPen(QColor("#1C7CD5"));
    painter.drawText(rect.adjusted(4, 4, -4, -4), Qt::AlignCenter, "📁");

    painter.restore();
}