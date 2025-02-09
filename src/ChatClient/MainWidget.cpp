// MainWidget.cpp
#include <Client/MainWidget.h>
#include <Client/SessionItem.hpp>
#include <QGraphicsOpacityEffect>
#include <QSequentialAnimationGroup>
#include <QThread>
#include <QTime>
#include <QTimer>
#include <qpropertyanimation.h>
#include <qtextbrowser.h>
#include <spdlog/spdlog.h>

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent),
      sessionList(nullptr),
      chatArea(nullptr),
      messageInput(nullptr),
      sendButton(nullptr),
      inputPanel(nullptr), m_client(new RpcClient) {
    setMinimumSize(900, 600);
    setupUI();// 确保在 setupStyle 之前调用 setupUI
    setupStyle();
    //设置为最大化

    // 添加测试数据
    addSessionItem("战犯101小组", "曹宇轩：打联赛", "22:30");
    addSessionItem("吴羽薇", "吴羽薇：宝宝，喜欢你...", "15:45");
    addSessionItem("程序代做接单群", "[文件] requirements.docx", "09:12");
    //connect(m_client, &Client::messageReceived, this, &MainWidget::addChatMessage);
}
MainWidget::~MainWidget() {
    delete m_client;
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
    chatArea = new QScrollArea(rightPanel);
    chatArea->setWidgetResizable(true);// 自动调整内容大小
    chatArea->setFrameShape(QFrame::NoFrame);

    QWidget *chatContent = new QWidget;// 创建一个容器用于存放聊天内容
    QVBoxLayout *chatLayout = new QVBoxLayout(chatContent);
    chatLayout->setContentsMargins(0, 0, 0, 0);
    chatLayout->setSpacing(10);
    chatLayout->addStretch();// 添加一个拉伸项，确保内容从顶部开始排列

    chatArea->setWidget(chatContent);   // 将容器设置为 QScrollArea 的内容

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
    connect(sendButton, &QPushButton::clicked, [this]() {
        QString message = messageInput->text();
        if (!message.isEmpty()) {
            //m_client->sendMessage(message.toStdString());// 发送消息
            messageInput->clear();
            addChatMessage("我", QTime::currentTime().toString("hh:mm"), message, MessageType::Self);
        }
    });


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
    QString sessionName = item->data(Qt::UserRole).toString();
    if (sessionName.isEmpty()) {
        sessionName = item->text();
    }

    QGraphicsOpacityEffect *chatEffect = new QGraphicsOpacityEffect(chatArea);
    chatArea->setGraphicsEffect(chatEffect);

    // 淡出动画
    QPropertyAnimation *fadeOut = new QPropertyAnimation(chatEffect, "opacity");
    fadeOut->setDuration(300);
    fadeOut->setStartValue(1.0);
    fadeOut->setEndValue(0.0);

    connect(fadeOut, &QPropertyAnimation::finished, this, [=]() {
        // 淡出完成后更新聊天内容
        updateChatArea(sessionName);
        spdlog::info("切换会话到：{}", sessionName.toStdString());

        // 淡入动画
        QPropertyAnimation *fadeIn = new QPropertyAnimation(chatEffect, "opacity");
        fadeIn->setDuration(300);
        fadeIn->setStartValue(0.0);
        fadeIn->setEndValue(1.0);

        connect(fadeIn, &QPropertyAnimation::finished, this, [=]() {
            chatArea->setGraphicsEffect(nullptr);
        });

        fadeIn->start(QAbstractAnimation::DeleteWhenStopped);
    });

    fadeOut->start(QAbstractAnimation::DeleteWhenStopped);
}

void MainWidget::updateChatArea(const QString &sessionName) {
    // 获取聊天区域的布局
    QVBoxLayout *chatLayout = qobject_cast<QVBoxLayout *>(chatArea->layout());
    if (chatLayout) {
        // 遍历布局中的所有小部件并删除它们
        QLayoutItem *item;
        while ((item = chatLayout->takeAt(0)) != nullptr) {
            QWidget *widget = item->widget();
            if (widget) {
                delete widget;
            }
            delete item;
        }
        // 删除布局
        delete chatLayout;
        chatArea->setLayout(nullptr);
    }

    // 这里添加根据会话名称加载聊天记录的逻辑
    // 例如，可以使用一个预设的聊天数据来模拟更新聊天记录
    addChatMessage(sessionName, "2025-1-20 10:05", "宝宝，今天怎么样？", MessageType::Other);
    addChatMessage("我", "2025-1-20 10:10", "还不错，宝宝，我还有点事，先不聊天了，待会陪你哦", MessageType::Self);
    addChatMessage("系统提示", "2025-1-20 10:15", "你的对象启动了英雄联盟", MessageType::System);
    // 根据需要更新聊天内容...
    addChatMessage(sessionName, "2025-1-20 10:05", "你居然敢玩游戏！给我爬！啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊", MessageType::Other);
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
        //chatArea->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);

        // 设置边距
        //chatArea->document()->setDocumentMargin(12);
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

    // 设置 Qt::UserRole 为会话名称
    item->setData(Qt::UserRole, name);

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
    QTimer::singleShot(300, this, &QWidget::show);// 延迟300ms显示主窗口
    connect(m_client, &RpcClient::connected, this, [this]() {
        qDebug() << "Connected to server, sending message in a thread...";
        QThread *thread = QThread::create([this]() {
            m_client->sendMessage("send", "Hello from Client!");
        });
        connect(thread, &QThread::finished, thread, &QThread::deleteLater);
        thread->start();
    });
    m_client->connectToServer("127.0.0.1", 8080);// 连接到服务器
}

void MainWidget::addChatMessage(const QString &sender,
    const QString &time,
    const QString &content,
    MessageType type) {
// 创建消息控件
MessageWidget *messageWidget = new MessageWidget(sender, time, content, type);

// 获取聊天内容的布局
QWidget *chatContent = chatArea->widget();
QVBoxLayout *chatLayout = qobject_cast<QVBoxLayout *>(chatContent->layout());

// 移除拉伸项
QLayoutItem *item = chatLayout->takeAt(chatLayout->count() - 1);
if (item) {
    if (item->spacerItem()) {
        delete item->spacerItem();
    }
}
// 添加消息控件
chatLayout->addWidget(messageWidget);

// 重新添加拉伸项
chatLayout->addStretch();

// 滚动到底部
QScrollBar *vScroll = chatArea->verticalScrollBar();
vScroll->setValue(vScroll->maximum());
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

    //chatArea->append(divider);
}

void MainWidget::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);

    // 遍历所有消息控件并发送更新信号
    QVBoxLayout *chatLayout = qobject_cast<QVBoxLayout *>(chatArea->layout());
    if (!chatLayout) return;

    for (int i = 0; i < chatLayout->count(); ++i) {
        QWidget *widget = chatLayout->itemAt(i)->widget();
        if (widget) {
            MessageWidget *messageWidget = qobject_cast<MessageWidget *>(widget);
            if (messageWidget) {
                // 发送信号更新气泡的高度
                messageWidget->updateBubbleConstraints();
                spdlog::info("更新 气泡高度 {}", i);
            }
        }
    }
}
