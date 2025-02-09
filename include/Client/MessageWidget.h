#pragma once

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QPropertyAnimation>

enum class MessageType { Self, Other, System, File };

class MessageWidget : public QWidget {
    Q_OBJECT
public:
    explicit MessageWidget(const QString &sender, const QString &time,
                           const QString &content, MessageType type,
                           QWidget *parent = nullptr);

    void toggleExpand(bool expanded); // 切换展开状态
    void updateBubbleConstraints();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    void setupUI(const QString &sender, const QString &time,
                 const QString &content, MessageType type);
    void createExpandButton();
    void updateExpandVisibility();

    QLabel *messageBubble;
    QLabel *expandButton;
    QPropertyAnimation *heightAnimation;
    int collapsedHeight = 120;       // 收起时默认高度
    int expandedHeight = 0;          // 展开后的高度
    bool isExpanded = false;
    bool canExpand = false;
};