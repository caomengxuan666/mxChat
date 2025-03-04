// MainWidget.cpp

#include <Client/MainWidget.h>
#include <Client/SessionItem.hpp>
#include <QGraphicsOpacityEffect>
#include <QSequentialAnimationGroup>
#include <QShortcut>
#include <QSplitter>
#include <QThread>
#include <QTime>
#include <QTimer>
#include <QToolButton>
#include <qnamespace.h>
#include <qpainterpath.h>
#include <qpropertyanimation.h>
#include <qtextbrowser.h>
#include <qtoolbutton.h>
#include <spdlog/spdlog.h>

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent),
      sessionList(nullptr),
      chatArea(nullptr),
      messageInput(nullptr),
      sendButton(nullptr),
      inputPanel(nullptr) {
    setMinimumSize(900, 600);
    setupUI();// 确保在 setupStyle 之前调用 setupUI
    setupStyle();
    //设置为最大化

    // 添加测试数据
    addSessionItem("战犯101小组", "曹宇轩：打联赛", "22:30");
    addSessionItem("吴羽薇", "吴羽薇：宝宝，喜欢你...", "15:45");
    addSessionItem("程序代做接单群", "[文件] requirements.docx", "09:12");

    // 添加测试消息
    addMessasge("系统通知", "09:00:00", "聊天系统初始化完成", MessageType::System);
    addMessasge("我", "09:00:05", "大家好！测试开始", MessageType::Self);
    addMessasge("客服助手", "09:00:10", "您好，请问有什么可以帮助您？", MessageType::Other);
}
MainWidget::~MainWidget() {
}
// 新增导航按钮创建函数
QToolButton *MainWidget::createNavButton(const QString &iconName,bool isAvatar) {
    QToolButton *btn = new QToolButton;
    btn->setIconSize(QSize(24, 24));
    btn->setToolButtonStyle(Qt::ToolButtonIconOnly); // 只显示图标，不显示文字
    btn->setFixedSize(60, 60);
    btn->setStyleSheet(R"(
        QToolButton {
            color: #A0A4AD;
            border: none;
            padding: 5px;
        }
        QToolButton:hover {
            background: #373B44;
        }
        QToolButton[active="true"] {
            color: #00B4FF;
            background: #373B44;
        }
    )");

    if (isAvatar) {
        // 如果是用户头像，加载图片并设置为圆形
        QPixmap avatar(":/images/avatar.png");
        QPixmap circularAvatar(24, 24); // 创建一个正方形画布
        circularAvatar.fill(Qt::transparent); // 设置透明背景

        QPainter painter(&circularAvatar);
        painter.setRenderHint(QPainter::Antialiasing);

        // 绘制圆形裁剪区域
        QPainterPath path;
        path.addEllipse(0, 0, 24, 24);
        painter.setClipPath(path);

        // 将头像图片绘制到圆形区域
        painter.drawPixmap(0, 0, avatar.scaled(24, 24, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        painter.end();

        btn->setIcon(QIcon(circularAvatar)); // 设置圆形头像图标
    } else {
        // 使用 QPainter 绘制其他图标
        QPixmap icon(24, 24);
        icon.fill(Qt::transparent); // 设置透明背景
        QPainter painter(&icon);
        painter.setRenderHint(QPainter::Antialiasing);

        if (iconName == "chat") {
            // 绘制“聊”图标（圆形尖角气泡）
            painter.setPen(QPen(QColor("#FFFFFF"), 2)); // 白色粗轮廓线
            QPainterPath path;
            path.addRoundedRect(QRectF(4, 4, 16, 16), 8, 8); // 圆角矩形
            path.moveTo(20, 12); // 尖角起点
            path.lineTo(24, 16); // 尖角顶点
            path.lineTo(20, 20); // 尖角终点
            painter.drawPath(path); // 绘制路径
        } else if (iconName == "contacts") {
            // 绘制“友”图标（用户头像）
            painter.setPen(QPen(QColor("#FFFFFF"), 2)); // 白色粗轮廓线
            painter.setBrush(Qt::transparent); // 透明填充
            painter.drawEllipse(4, 4, 16, 16); // 外部圆形
            painter.drawLine(12, 8, 12, 16); // 垂直线表示头部
            painter.drawLine(8, 12, 16, 12); // 水平线表示肩膀
        } else if (iconName == "files") {
            // 绘制“文”图标（文件形状）
            painter.setPen(QPen(QColor("#FFFFFF"), 2)); // 白色粗轮廓线
            QPainterPath path;
            path.moveTo(4, 4); // 左上角
            path.lineTo(20, 4); // 右上角
            path.lineTo(20, 20); // 右下角
            path.lineTo(4, 20); // 左下角
            path.lineTo(4, 4); // 回到左上角
            path.moveTo(20, 4); // 文件折角起点
            path.lineTo(16, 8); // 折角中间
            path.lineTo(16, 4); // 折角终点
            painter.drawPath(path); // 绘制路径
        } else if (iconName == "settings") {
            // 绘制“设”图标（齿轮）
            painter.setPen(QPen(QColor("#FFFFFF"), 2)); // 白色粗轮廓线
            painter.setBrush(Qt::transparent); // 透明填充
            painter.drawEllipse(4, 4, 16, 16); // 齿轮主体
            for (int i = 0; i < 8; ++i) {
                qreal angle = i * M_PI / 4; // 每隔 45 度绘制一个齿
                qreal x1 = 12 + 6 * cos(angle);
                qreal y1 = 12 + 6 * sin(angle);
                qreal x2 = 12 + 8 * cos(angle);
                qreal y2 = 12 + 8 * sin(angle);
                painter.drawLine(x1, y1, x2, y2); // 绘制齿
            }
        }

        painter.end();
        btn->setIcon(QIcon(icon));
    }

    return btn;
}
void MainWidget::setupUI() {
    QHBoxLayout *rootLayout = new QHBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    // 左侧导航栏
    QWidget *navBar = new QWidget(this);
    navBar->setFixedWidth(60);
    navBar->setStyleSheet("background: #23262E;");

    QVBoxLayout *navLayout = new QVBoxLayout(navBar);
    navLayout->setContentsMargins(0, 10, 0, 10);
    navLayout->setSpacing(10);
    navLayout->addStretch();

    // 创建导航按钮
    QToolButton *avt=createNavButton("avt",true);
    QToolButton *chatBtn = createNavButton("chat");
    QToolButton *contactsBtn = createNavButton("contacts");
    QToolButton *filesBtn = createNavButton("files");
    QToolButton *settingsBtn = createNavButton("settings");

    navLayout->addWidget(avt);
    navLayout->addWidget(chatBtn);
    navLayout->addWidget(contactsBtn);
    navLayout->addWidget(filesBtn);
    navLayout->addWidget(settingsBtn);
    navLayout->addStretch();

    // 左侧内容区域（包含用户信息和会话列表）
    QWidget *leftContent = new QWidget(this);
    leftContent->setFixedWidth(280);
    leftContent->setStyleSheet("background: #2E323B;");

    QVBoxLayout *leftLayout = new QVBoxLayout(leftContent);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(0);

    // 用户信息栏（保持原有代码）
    QWidget *userBar = new QWidget(leftContent);
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

    // 会话列表（保持原有代码）
    sessionList = new QListWidget(leftContent);
    sessionList->setFrameShape(QFrame::NoFrame);
    sessionList->setVerticalScrollMode(QListWidget::ScrollPerPixel);

    connect(sessionList, &QListWidget::itemClicked, this, &MainWidget::onSessionItemClicked);

    leftLayout->addWidget(userBar);
    leftLayout->addWidget(sessionList);

    // 将导航栏和内容区域添加到主布局
    rootLayout->addWidget(navBar);
    rootLayout->addWidget(leftContent);

    // 右侧主区域（保持原有代码）
    QWidget *rightPanel = new QWidget(this);
    rightPanel->setStyleSheet("background: #F5F6F7;");

    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(0);

    // 使用 QSplitter 实现拖拽功能
    QSplitter *splitter = new QSplitter(Qt::Vertical, rightPanel);

    // 聊天区域
    chatArea = new QScrollArea(splitter);
    chatArea->setWidgetResizable(true);
    chatArea->setFrameShape(QFrame::NoFrame);

    QWidget *chatContent = new QWidget;
    QVBoxLayout *chatLayout = new QVBoxLayout(chatContent);
    chatLayout->setContentsMargins(0, 0, 0, 0);
    chatLayout->setSpacing(10);
    chatLayout->addStretch(1);

    chatArea->setWidget(chatContent);

    // 输入区域
    inputPanel = new QWidget(splitter);
    inputPanel->setMinimumHeight(120);// 设置最小高度
    inputPanel->setMaximumHeight(300);// 设置最大高度

    // 初始化inputpannel的初始高度
    QList<int> splitterSizes;
    splitterSizes << 430 << 120;// 聊天区430px，输入区120px
    splitter->setSizes(splitterSizes);

    QVBoxLayout *inputLayout = new QVBoxLayout(inputPanel);
    inputLayout->setContentsMargins(12, 12, 12, 12);

    messageInput = new QTextEdit(inputPanel);
    messageInput->setPlaceholderText("输入消息...");
    messageInput->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    messageInput->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    messageInput->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->addStretch();

    sendButton = new QPushButton("发送", inputPanel);
    sendButton->setFixedSize(80, 32);

    btnLayout->addWidget(sendButton);
    connect(sendButton, &QPushButton::clicked, [this]() {
        QString message = messageInput->toPlainText();
        if (!message.isEmpty()) {
            messageInput->clear();
            addMessasge("我", QTime::currentTime().toString("hh:mm"), message, MessageType::Self);
        }
    });

    inputLayout->addWidget(messageInput, 1);
    inputLayout->addLayout(btnLayout);

    splitter->addWidget(chatArea);
    splitter->addWidget(inputPanel);
    splitter->setStretchFactor(0, 3);// 聊天区域占更多空间
    splitter->setStretchFactor(1, 1);// 输入区域占较少空间

    rightLayout->addWidget(splitter);

    rootLayout->addWidget(rightPanel, 1);

    // 按钮按压动画
    connect(sendButton, &QPushButton::clicked, [this]() {
        QPropertyAnimation *pressAnim = new QPropertyAnimation(sendButton, "geometry");
        pressAnim->setDuration(150);
        QRect origRect = sendButton->geometry();
        pressAnim->setKeyValueAt(0, origRect);
        pressAnim->setKeyValueAt(0.5, origRect.adjusted(2, 2, -2, -2));
        pressAnim->setKeyValueAt(1, origRect);
        pressAnim->start(QAbstractAnimation::DeleteWhenStopped);
    });

    // 绑定 CTRL+Enter 键到发送消息
    QShortcut *enterShortcut = new QShortcut(QKeySequence(Qt::Key_Return, Qt::ControlModifier), this);
    connect(enterShortcut, &QShortcut::activated, [this]() {
        QString message = messageInput->toPlainText();
        if (!message.isEmpty()) {
            messageInput->clear();
            addMessasge("我", QTime::currentTime().toString("hh:mm"), message, MessageType::Self);
        }
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

    connect(fadeOut, &QPropertyAnimation::finished, this, [this, sessionName, chatEffect]() {
        // 淡出完成后更新聊天内容
        updateChatArea(sessionName);
        spdlog::info("切换会话到：{}", sessionName.toStdString());

        // 淡入动画
        QPropertyAnimation *fadeIn = new QPropertyAnimation(chatEffect, "opacity");
        fadeIn->setDuration(300);
        fadeIn->setStartValue(0.0);
        fadeIn->setEndValue(1.0);

        connect(fadeIn, &QPropertyAnimation::finished, this, [this]() {
            chatArea->setGraphicsEffect(nullptr);
        });

        fadeIn->start(QAbstractAnimation::DeleteWhenStopped);
    });

    fadeOut->start(QAbstractAnimation::DeleteWhenStopped);

    // 绑定Enter键到addTestMessage
    QShortcut *enterShortcut = new QShortcut(QKeySequence(Qt::Key_Return), this);


    connect(enterShortcut, &QShortcut::activated, [this]() {
        QString message = messageInput->toPlainText();
        if (!message.isEmpty()) {
            messageInput->clear();
            addMessasge("我", QTime::currentTime().toString("hh:mm"), message, MessageType::Self);
        }
    });
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

    // 设置输入区域样式
    if (inputPanel) {
        inputPanel->setStyleSheet("background: white; border-top: 1px solid #E0E0E0;");
    }

    // 设置消息输入框样式
    if (messageInput) {
        messageInput->setStyleSheet(R"(
            QTextEdit {
                border: 2px solid #E0E0E0;
                border-radius: 6px;
                padding: 10px;
                font-size: 14px;
            }
            QTextEdit:focus {
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
}

void MainWidget::addMessasge(const QString &sender, const QString &time, const QString &content, MessageType type) {
    // 自动换行处理
    QFontMetrics fm(QFont("Microsoft YaHei", 12));
    int maxWidth = 600;// 最大宽度限制
    QString wrappedContent = "";
    QStringList words = content.split(' ');
    QString line = "";

    for (const QString &word: words) {
        if (fm.horizontalAdvance(line + word) > maxWidth) {
            wrappedContent += line.trimmed() + "\n";
            line = word + " ";
        } else {
            line += word + " ";
        }
    }
    wrappedContent += line.trimmed();

    // 创建消息控件
    BubbleWidget *bubble = new BubbleWidget(sender, time, wrappedContent, type);
    bubble->setMinimumHeight(60);
    bubble->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    // 获取聊天内容的布局
    QWidget *chatContent = chatArea->widget();
    QVBoxLayout *chatLayout = qobject_cast<QVBoxLayout *>(chatContent->layout());

    // 添加消息控件
    chatLayout->addWidget(bubble);

    // 重新添加拉伸项
    chatLayout->addStretch();

    // 滚动到底部
    QScrollBar *vScroll = chatArea->verticalScrollBar();
    vScroll->setValue(vScroll->maximum());
    QTimer::singleShot(50, [this]() {
        chatArea->verticalScrollBar()->setValue(chatArea->verticalScrollBar()->maximum());
    });
}


// 添加时间分割线

[[maybe_unused]]
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
