#include "Client/MessageWidget.h"
#include <QGraphicsDropShadowEffect>
#include <QMouseEvent>
#include <QPainter>
#include <QTextDocument>
#include <QTimer>

MessageWidget::MessageWidget(const QString &sender, const QString &time,
                             const QString &content, MessageType type,
                             QWidget *parent)
    : QWidget(parent), messageBubble(nullptr), expandButton(nullptr) {
    setupUI(sender, time, content, type);
    createExpandButton();
}

int calculateContentHeight(const QString &text, int maxWidth) {
    QTextDocument doc;
    doc.setHtml(text);         // 如果需要支持富文本
    doc.setTextWidth(maxWidth);// 设置最大宽度
    return doc.size().height();// 返回实际高度
}

void MessageWidget::createExpandButton() {
    // 创建下拉指示器
    expandButton = new QLabel(this);
    expandButton->setFixedSize(32, 32);// 增大点击区域
    expandButton->setAlignment(Qt::AlignCenter);
    expandButton->setStyleSheet("background: rgba(0, 0, 0, 0.1); border-radius: 4px;");
    expandButton->setAttribute(Qt::WA_TransparentForMouseEvents, false);

    // 绘制箭头图标
    QPixmap arrow(32, 32);
    arrow.fill(Qt::transparent);
    QPainter painter(&arrow);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(Qt::white, 2));
    painter.drawLine(10, 14, 22, 14);// 水平线
    painter.drawLine(16, 18, 22, 14);// 下斜线
    painter.drawLine(16, 10, 22, 14);// 上斜线
    expandButton->setPixmap(arrow);

    expandButton->raise();
    expandButton->hide();
}

void MessageWidget::setupUI(const QString &sender, const QString &time,
                            const QString &content, MessageType type) {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QHBoxLayout *messageLayout = new QHBoxLayout();

    // 头像
    QLabel *avatar = new QLabel(this);
    avatar->setFixedSize(40, 40);
    avatar->setStyleSheet("background: #00B4FF; border-radius: 20px;");
    avatar->setAlignment(Qt::AlignCenter);

    // 消息气泡
    messageBubble = new QLabel(content, this);
    messageBubble->setWordWrap(true);
    messageBubble->setTextInteractionFlags(Qt::TextSelectableByMouse);
    messageBubble->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
    messageBubble->setMaximumWidth(600);// 绝对最大宽度
    messageBubble->setAlignment(Qt::AlignTop);

    // 时间戳
    QLabel *timestamp = new QLabel(time, this);
    timestamp->setStyleSheet("font-size: 12px; color: #999;");

    // 添加阴影效果
    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setBlurRadius(8);
    shadowEffect->setOffset(0, 2);
    shadowEffect->setColor(QColor(0, 0, 0, 50));
    messageBubble->setGraphicsEffect(shadowEffect);

    // 根据消息类型设置不同的样式
    switch (type) {
        case MessageType::Self:
            messageBubble->setStyleSheet("background: #00B4FF; color: white; padding: 10px; border-radius: 12px 12px 0 12px;");
            messageLayout->addWidget(messageBubble);
            messageLayout->addWidget(avatar);
            messageLayout->setAlignment(Qt::AlignRight);
            break;
        case MessageType::Other:
            messageBubble->setStyleSheet("background: rgba(51, 224, 103, 0.95); color: #333; padding: 10px; border-radius: 12px 12px 0 12px;");
            messageLayout->addWidget(avatar);
            messageLayout->addWidget(messageBubble);
            messageLayout->addStretch(1);
            messageLayout->setAlignment(Qt::AlignLeft);
            break;
        case MessageType::System:
            messageBubble->setStyleSheet("background: #FFFFFF; color: #333; padding: 10px; border-radius: 12px 12px 12px 0;");
            messageLayout->addWidget(messageBubble);
            messageLayout->setAlignment(Qt::AlignCenter);
            break;
        case MessageType::File:
            messageBubble->setStyleSheet("background: #FFFFFF; color: #333; padding: 10px; border-radius: 12px 12px 12px 0;");
            messageLayout->addWidget(messageBubble);
            messageLayout->setAlignment(Qt::AlignCenter);
            break;
        default:
            break;
    }

    // 添加到主布局
    mainLayout->addLayout(messageLayout);
    mainLayout->addWidget(timestamp, 0, Qt::AlignRight);
    mainLayout->setContentsMargins(12, 6, 12, 6);

    // 初始化约束
    updateBubbleConstraints();

    // 添加消息气泡的鼠标追踪
    messageBubble->setAttribute(Qt::WA_Hover);
    messageBubble->installEventFilter(this);

    // 初始化动画
    heightAnimation = new QPropertyAnimation(messageBubble, "maximumHeight");
    heightAnimation->setDuration(200);
    heightAnimation->setEasingCurve(QEasingCurve::InOutCubic);
}

void MessageWidget::updateBubbleConstraints() {
    // 计算内容实际高度
    int contentHeight = calculateContentHeight(messageBubble->text(),
                                               messageBubble->maximumWidth());

    // 判断是否需要显示展开按钮
    canExpand = (contentHeight > collapsedHeight);
    updateExpandVisibility();

    // 设置默认高度限制
    expandedHeight = contentHeight + 20;// 加20px留白
    messageBubble->setMaximumHeight(isExpanded ? expandedHeight : collapsedHeight);

    // 更新按钮位置
    if (expandButton) {
        QPoint btnPos = messageBubble->geometry().bottomRight() -
                        QPoint(expandButton->width() + 8, expandButton->height() + 8);
        expandButton->move(btnPos);
    }
}

void MessageWidget::updateExpandVisibility() {
    if (!expandButton) return;

    const bool shouldShow = canExpand &&
                            (messageBubble->underMouse() || isExpanded);

    if (shouldShow) {
        expandButton->setVisible(true);
    } else {
        // 使用 QTimer 延迟隐藏
        QTimer::singleShot(500, this, [this]() {
            if (!isExpanded && !messageBubble->underMouse()) {
                expandButton->setVisible(false);
            }
        });
    }

    // 更新按钮位置
    if (expandButton->isVisible()) {
        QPoint btnPos = messageBubble->geometry().bottomRight() -
                        QPoint(expandButton->width() + 8, expandButton->height() + 8);
        expandButton->move(btnPos);
    }
}

void MessageWidget::toggleExpand(bool expanded) {
    if (!canExpand) return;

    isExpanded = expanded;
    heightAnimation->stop();
    heightAnimation->setStartValue(messageBubble->maximumHeight());
    heightAnimation->setEndValue(expanded ? expandedHeight : collapsedHeight);
    heightAnimation->start();

    // 调整整个控件高度
    this->setMinimumHeight(messageBubble->height() + 40);// 40=时间戳+间距
    updateExpandVisibility();
}

void MessageWidget::mousePressEvent(QMouseEvent *event) {
    if (expandButton->isVisible() &&
        expandButton->geometry().contains(event->pos())) {
        toggleExpand(!isExpanded);
    }
    QWidget::mousePressEvent(event);
}

void MessageWidget::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    updateBubbleConstraints();
}

bool MessageWidget::eventFilter(QObject *obj, QEvent *event) {
    // 鼠标悬停时显示/隐藏展开按钮
    if (obj == messageBubble) {
        if (event->type() == QEvent::HoverEnter ||
            event->type() == QEvent::HoverLeave) {
            updateExpandVisibility();
        }
    }
    return QWidget::eventFilter(obj, event);
}