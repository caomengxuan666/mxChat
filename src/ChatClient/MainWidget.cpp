// MainWidget.cpp
#include <Client/MainWidget.h>
#include <Client/SessionItem.hpp>
#include <QSequentialAnimationGroup>
#include <QGraphicsOpacityEffect>
#include <QTimer>
#include <qpropertyanimation.h>

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent),
      sessionList(nullptr),
      chatArea(nullptr),
      messageInput(nullptr),
      sendButton(nullptr),
      inputPanel(nullptr), windowOpacityAnimation(new QPropertyAnimation(this, "windowOpacity")) {
    setMinimumSize(900, 600);
    setupUI();// 确保在 setupStyle 之前调用 setupUI
    setupStyle();
    startAnimation();

    // 添加测试数据
    addSessionItem("战犯101小组", "曹宇轩：打联赛", "22:30");
    addSessionItem("吴羽薇", "吴羽薇：宝宝，喜欢你...", "15:45");
    addSessionItem("程序代做接单群", "[文件] requirements.docx", "09:12");
}
void MainWidget::setupUI() {
    QHBoxLayout *rootLayout = new QHBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    // 左侧导航
    QWidget *leftPanel = new QWidget(this);
    leftPanel->setFixedWidth(280);
    leftPanel->setStyleSheet("background: #2E323B;");

    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(0);

    // 用户信息栏
    QWidget *userBar = new QWidget(leftPanel);
    userBar->setFixedHeight(60);
    userBar->setStyleSheet("background: #23262E; border-bottom: 1px solid #3A3D45;");

    QHBoxLayout *userLayout = new QHBoxLayout(userBar);
    userLayout->setContentsMargins(12, 0, 12, 0);

    QLabel *avatar = new QLabel(userBar);
    avatar->setFixedSize(40, 40);
    avatar->setStyleSheet(R"(
        background: #00B4FF;
        border-radius: 20px;
        color: white;
        font-weight: bold;
    )");
    avatar->setText("Q");
    avatar->setAlignment(Qt::AlignCenter);

    userLayout->addWidget(avatar);
    userLayout->addStretch();

    // 会话列表
    sessionList = new QListWidget(leftPanel);
    sessionList->setFrameShape(QFrame::NoFrame);
    sessionList->setVerticalScrollMode(QListWidget::ScrollPerPixel);

    connect(sessionList, &QListWidget::itemClicked, this, &MainWidget::onSessionItemClicked);// 监听点击事件

    leftLayout->addWidget(userBar);
    leftLayout->addWidget(sessionList);

    // 右侧主区域
    QWidget *rightPanel = new QWidget(this);
    rightPanel->setStyleSheet("background: #F5F6F7;");

    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(0);

    // 聊天区域
    chatArea = new QTextEdit(rightPanel);
    chatArea->setReadOnly(true);

    // 输入区域
    inputPanel = new QWidget(rightPanel);
    inputPanel->setFixedHeight(120);

    QVBoxLayout *inputLayout = new QVBoxLayout(inputPanel);
    inputLayout->setContentsMargins(12, 12, 12, 12);

    messageInput = new QLineEdit(inputPanel);
    messageInput->setPlaceholderText("输入消息...");

    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->addStretch();

    sendButton = new QPushButton("发送", inputPanel);
    sendButton->setFixedSize(80, 32);

    btnLayout->addWidget(sendButton);

    inputLayout->addWidget(messageInput, 1);
    inputLayout->addLayout(btnLayout);

    rightLayout->addWidget(chatArea, 1);
    rightLayout->addWidget(inputPanel);

    rootLayout->addWidget(leftPanel);
    rootLayout->addWidget(rightPanel, 1);

    connect(sendButton, &QPushButton::clicked, [this]() {
    // 按钮按压动画
    QPropertyAnimation *pressAnim = new QPropertyAnimation(sendButton, "geometry");
    pressAnim->setDuration(150);
    QRect origRect = sendButton->geometry();
    pressAnim->setKeyValueAt(0, origRect);
    pressAnim->setKeyValueAt(0.5, origRect.adjusted(2, 2, -2, -2));
    pressAnim->setKeyValueAt(1, origRect);
    pressAnim->start(QAbstractAnimation::DeleteWhenStopped);
});
}

void MainWidget::onSessionItemClicked(QListWidgetItem *item) {
    // 创建淡入淡出效果组
    QGraphicsOpacityEffect *chatEffect = new QGraphicsOpacityEffect(chatArea);
    chatArea->setGraphicsEffect(chatEffect);
    
    QSequentialAnimationGroup *seqGroup = new QSequentialAnimationGroup(this);
    
    // 淡出动画
    QPropertyAnimation *fadeOut = new QPropertyAnimation(chatEffect, "opacity");
    fadeOut->setDuration(300);
    fadeOut->setStartValue(1.0);
    fadeOut->setEndValue(0.0);
    
    // 淡入动画
    QPropertyAnimation *fadeIn = new QPropertyAnimation(chatEffect, "opacity");
    fadeIn->setDuration(300);
    fadeIn->setStartValue(0.0);
    fadeIn->setEndValue(1.0);
    
    // 动画链
    seqGroup->addAnimation(fadeOut);
    seqGroup->addAnimation(fadeIn);
    
    connect(seqGroup, &QSequentialAnimationGroup::finished, this, [=]() {
        chatArea->setGraphicsEffect(nullptr); // 移除效果
        updateChatArea(item->text());
    });
    
    seqGroup->start(QAbstractAnimation::DeleteWhenStopped);
}

void MainWidget::updateChatArea(const QString &sessionName) {
    chatArea->clear();// 清空当前聊天区域

    // 这里添加根据会话名称加载聊天记录的逻辑
    // 例如，可以使用一个预设的聊天数据来模拟更新聊天记录
    addChatMessage(sessionName, "2025-1-20 10:05", "宝宝，今天怎么样？", MessageType::Other);
    addChatMessage("我", "2025-1-20 10:10", "还不错，就是这个傻逼项目老段错误吐核！", MessageType::Self);
    addChatMessage("系统提示", "2025-1-20 10:15", "你的对象启动了英雄联盟", MessageType::System);
    // 根据需要更新聊天内容...
}
void MainWidget::setupStyle() {
    // 设置全局滚动条样式
    QString scrollbarStyle = R"(
        QScrollBar:vertical {
            background: #2E323B;
            width: 8px;
        }
        QScrollBar::handle:vertical {
            background: #4A4E58;
            min-height: 30px;
            border-radius: 4px;
        }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0px;
        }
    )";
    this->setStyleSheet(scrollbarStyle);

    // 设置会话列表样式
    if (sessionList) {
        sessionList->setStyleSheet(R"(
            QListWidget {
                background: transparent;
                border: none;
                outline: none;
            }
            QListWidget::Item {
                border-bottom: 1px solid #3A3D45;
            }
            QListWidget::Item:hover {
                background: #373B44;
            }
            QListWidget::Item:selected {
                background: #4A4E58;
                border-left: 4px solid #00B4FF;
            }
        )");
    }

    // 更新 chatArea 样式
    if (chatArea) {
        chatArea->setStyleSheet(R"(
        QTextEdit {
            background: #F5F6F7;
            border: none;
            font-family: 'Microsoft YaHei';
            font-size: 14px;
            color: #333333;
        }
        a {
            color: #00B4FF;
            text-decoration: none;
        }
        a:hover {
            text-decoration: underline;
        }
    )");

        // 启用自动换行
        chatArea->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);

        // 设置边距
        chatArea->document()->setDocumentMargin(12);
    }

    // 设置输入区域样式
    if (inputPanel) {
        inputPanel->setStyleSheet("background: white; border-top: 1px solid #E0E0E0;");
    }

    // 设置消息输入框样式
    if (messageInput) {
        messageInput->setStyleSheet(R"(
            QLineEdit {
                border: 2px solid #E0E0E0;
                border-radius: 6px;
                padding: 10px;
                font-size: 14px;
            }
            QLineEdit:focus {
                border-color: #00B4FF;
            }
        )");
    }

    // 设置发送按钮样式
    if (sendButton) {
        sendButton->setStyleSheet(R"(
            QPushButton {
                background: #00B4FF;
                color: white;
                border: none;
                border-radius: 6px;
                font-weight: 500;
            }
            QPushButton:hover {
                background: #0099D6;
            }
            QPushButton:pressed {
                background: #0080B3;
            }
        )");
    }
}

void MainWidget::addSessionItem(const QString &name, const QString &msg, const QString &time) {
    QListWidgetItem *item = new QListWidgetItem;
    item->setSizeHint(QSize(280, 70));
    sessionList->addItem(item);
    
    SessionItem *sessionWidget = new SessionItem(name, msg, time);
    sessionList->setItemWidget(item, sessionWidget);

    // 会话项渐显动画
    QGraphicsOpacityEffect *itemEffect = new QGraphicsOpacityEffect(sessionWidget);
    sessionWidget->setGraphicsEffect(itemEffect);
    
    QPropertyAnimation *itemAnim = new QPropertyAnimation(itemEffect, "opacity");
    itemAnim->setDuration(600);
    itemAnim->setStartValue(0.0);
    itemAnim->setEndValue(1.0);
    itemAnim->setEasingCurve(QEasingCurve::OutQuad);
    
    // 分时启动动画
    QTimer::singleShot(sessionList->count() * 100, [=]() {
        itemAnim->start(QAbstractAnimation::DeleteWhenStopped);
    });
}

void MainWidget::onLoginSuccess() {
    this->show();// Display the main window after login
}

void MainWidget::addChatMessage(const QString &sender,
                                const QString &time,
                                const QString &content,// 修改为 const QString &
                                MessageType type) {
    // 生成消息HTML模板
    QString messageHtml = QStringLiteral(
            "<div style='margin: 8px 12px;'>"
            "<div style='%1'>"
            "<div style='display: flex; %2'>"
            "%3"// 头像占位
            "<div style='max-width: 70%%; %4'>"
            "<div style='font-size: 12px; color: #666; margin-bottom: 4px;'>"
            "%5 · %6"// 发送者 + 时间
            "</div>"
            "<div style='%7'>"
            "%8"// 消息内容
            "</div>"
            "</div>"
            "</div>"
            "</div>"
            "</div>");

    // 根据消息类型设置样式
    QString bubbleStyle, timeStyle, contentStyle, alignment;
    QString avatar;
    QString formattedContent = content;// 新增变量用于存储格式化后的内容

    switch (type) {
        case MessageType::Self:// 用户自己消息（右对齐）
            alignment = "flex-direction: row-reverse;";
            bubbleStyle = "background: #00B4FF; color: white; border-radius: 12px 12px 0 12px;";
            avatar = "<div style='width:32px; height:32px; background:#4CAF50; border-radius:16px; margin-left:8px;'></div>";
            break;
        case MessageType::Image:// 图片消息
            contentStyle = "padding:8px; background:#F0F2F5; border-radius:8px;";
            formattedContent = QString("<img src=':/icons/image.png' width='120' style='border-radius:6px;'>"
                                       "<div style='font-size:12px; color:#666; margin-top:4px;'>%1</div>")
                                       .arg(content);
            break;
        case MessageType::File:// 文件消息
            contentStyle = "padding:8px; background:#F0F2F5; border-radius:8px; display:flex; align-items:center;";
            formattedContent = QString("<img src=':/icons/file.png' width='24' style='margin-right:8px;'>"
                                       "<div>"
                                       "<div style='font-weight:500;'>%1</div>"
                                       "<div style='font-size:12px; color:#666;'>12.5 MB</div>"
                                       "</div>")
                                       .arg(content);
            break;
        case MessageType::System:// 系统消息
            alignment = "justify-content: center;";
            timeStyle = "display:none;";
            bubbleStyle = "color: #666; font-size: 12px; text-align: center;";
            break;
        default:// 其他人消息（左对齐）
            bubbleStyle = "background: #FFFFFF; color: #333; border-radius: 12px 12px 12px 0; box-shadow: 0 2px 4px rgba(0,0,0,0.05);";
            avatar = "<div style='width:32px; height:32px; background:#FF9800; border-radius:16px; margin-right:8px;'></div>";
    }

    // 构建完整HTML
    QString finalHtml = messageHtml
                                .arg(alignment)
                                .arg(bubbleStyle)
                                .arg(avatar)
                                .arg(contentStyle)
                                .arg(sender)
                                .arg(time)
                                .arg(timeStyle)
                                .arg(formattedContent);

    // 添加HTML到聊天区域
    chatArea->append(finalHtml);
        // 消息滚动动画
    QScrollBar *vScroll = chatArea->verticalScrollBar();
    QPropertyAnimation *scrollAnim = new QPropertyAnimation(vScroll, "value");
    scrollAnim->setDuration(400);
    scrollAnim->setStartValue(vScroll->value());
    scrollAnim->setEndValue(vScroll->maximum());
    scrollAnim->setEasingCurve(QEasingCurve::OutExpo);
    scrollAnim->start(QAbstractAnimation::DeleteWhenStopped);
}

// 添加时间分割线
void MainWidget::addTimeDivider(const QString &timeText) {
    QString divider = QStringLiteral(
                              "<div style='margin: 20px auto; text-align: center;'>"
                              "<div style='display: inline-block; padding: 4px 12px; background: #E0E0E0; border-radius: 12px;"
                              "color: #666; font-size: 12px;'>"
                              "%1"
                              "</div>"
                              "</div>")
                              .arg(timeText);

    chatArea->append(divider);
}

void MainWidget::startAnimation() {
    // 窗口淡入动画
    windowOpacityAnimation->setDuration(800);
    windowOpacityAnimation->setStartValue(0.0);
    windowOpacityAnimation->setEndValue(1.0);
    windowOpacityAnimation->setEasingCurve(QEasingCurve::OutCubic);
    windowOpacityAnimation->start();
}