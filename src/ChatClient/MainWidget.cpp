#include "Client/BubbleWidget.h"
#include <Client/LeftNavButtonFactory.hpp>
#include <Client/MainWidget.h>
#include <Client/SessionItem.hpp>
#include <QDockWidget>
#include <QGraphicsOpacityEffect>
#include <QRandomGenerator>
#include <QSequentialAnimationGroup>
#include <QShortcut>
#include <QSplitter>
#include <QThread>
#include <QTime>
#include <QTimer>
#include <QToolButton>
#include <qmainwindow.h>
#include <qnamespace.h>
#include <qpainterpath.h>
#include <qpropertyanimation.h>
#include <qtextbrowser.h>
#include <qtoolbutton.h>
#include <spdlog/spdlog.h>

MainWidget::MainWidget(QMainWindow *parent)
    : QMainWindow(parent),
      sessionManager(nullptr),
      chatArea(nullptr),
      messageInput(nullptr),
      sendButton(nullptr),
      inputPanel(nullptr),
      cachedChatPage(nullptr),
      cachedContactsPage(nullptr),
      cachedSearchBar(nullptr),
      cachedLeftContent(nullptr) {// 初始化缓存聊天区域的成员变量
    setMinimumSize(900, 600);
    setupUI();// 确保在 setupStyle 之前调用 setupUI
    setupStyle();
    //设置为最大化

    insertTestCase();

    // 安装事件过滤器
    sessionListWidget = sessionManager->getSessionListWidget();
    if (sessionListWidget) {
        sessionListWidget->installEventFilter(this);
    }
    this->installEventFilter(this);// 安装事件过滤器到 MainWidget
}

MainWidget::~MainWidget() {
}

bool MainWidget::eventFilter(QObject *watched, QEvent *event) {
    if (watched == sessionListWidget) {
        if (event->type() == QEvent::Enter) {
            // 鼠标进入时显示滚动条
            showScrollBars(true);
            return true;
        } else if (event->type() == QEvent::Leave) {
            // 鼠标离开时隐藏滚动条
            showScrollBars(false);
            return true;
        }
    }
    return QWidget::eventFilter(watched, event);
}

void MainWidget::showScrollBars(bool show) {
    if (sessionListWidget) {
        sessionListWidget->setVerticalScrollBarPolicy(
                show ? Qt::ScrollBarAsNeeded : Qt::ScrollBarAlwaysOff);
        sessionListWidget->setHorizontalScrollBarPolicy(
                show ? Qt::ScrollBarAsNeeded : Qt::ScrollBarAlwaysOff);
    }
}


void MainWidget::setupUI() {
    centralWidget = new QWidget(this);
    rootLayout = new QHBoxLayout(centralWidget);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    // 左侧导航栏
    leftPanel = new QWidget(this);
    leftPanel->setFixedWidth(340);// 修改宽度以容纳导航栏和会话内容
    leftPanel->setStyleSheet("background: #23262E;");

    leftPanelLayout = new QHBoxLayout(leftPanel);
    leftPanelLayout->setContentsMargins(0, 0, 0, 0);
    leftPanelLayout->setSpacing(0);

    navBar = new QWidget(this);
    navBar->setFixedWidth(60);
    navBar->setStyleSheet("background: #23262E;");

    QVBoxLayout *navLayout = new QVBoxLayout(navBar);
    navLayout->setContentsMargins(0, 10, 0, 10);
    navLayout->setSpacing(10);
    navLayout->addStretch();

    // 创建导航按钮
    LeftNavButtonCreator exportFactory;
    auto [avt, chatBtn, contactsBtn, filesBtn, settingsBtn,infoBtn] = exportFactory.getLeftNavButtons();

    navLayout->addWidget(avt);
    navLayout->addWidget(chatBtn);
    navLayout->addWidget(contactsBtn);
    navLayout->addWidget(filesBtn);
    navLayout->addWidget(settingsBtn);
    navLayout->addWidget(infoBtn);
    navLayout->addStretch();

    // 绑定每一个按钮的点击事件
    connect(chatBtn, &QToolButton::clicked, this, &MainWidget::showChatPage);
    connect(contactsBtn, &QToolButton::clicked, this, &MainWidget::showContactsPage);
    connect(filesBtn, &QToolButton::clicked, this, &MainWidget::showFilesPage);
    connect(settingsBtn, &QToolButton::clicked, this, &MainWidget::showSettingsPage);
    connect(avt, &QToolButton::clicked, this, &MainWidget::showUserHomePage);

    // 将 navBar 添加到 leftPanel
    leftPanelLayout->addWidget(navBar);

    // 将 leftPanel 添加到 rootLayout
    rootLayout->addWidget(leftPanel);

    // 设置 centralWidget 为 QMainWindow 的中心部件

    setCentralWidget(centralWidget);
    showChatPage();
}

void MainWidget::showContactsPage() {
    // 显示新增好友界面
    spdlog::info("显示新增好友界面");

    // 如果已经有了其他的主页面元素，隐藏起来
    if (cachedChatPage) {
        rootLayout->removeWidget(cachedChatPage);
        cachedChatPage->hide();
    }

    // 隐藏 cachedLeftContent
    if (cachedLeftContent) {
        leftPanelLayout->removeWidget(cachedLeftContent);
        cachedLeftContent->hide();
    }

    // 临时将 leftPanel 的宽度设置为Nav bar的宽度
    leftPanel->setFixedWidth(navBar->width());

    // 检查缓存的好友页面是否存在
    if (cachedContactsPage) {
        // 如果缓存存在，直接使用缓存的好友页面
        rootLayout->addWidget(cachedContactsPage, 1);
        cachedContactsPage->show();
        return;
    }

    // 创建好友页面
    QWidget *contactsPage = new QWidget(this);
    contactsPage->setStyleSheet("background: #F5F6F7;");

    QVBoxLayout *contactsLayout = new QVBoxLayout(contactsPage);
    contactsLayout->setContentsMargins(0, 0, 0, 0);
    contactsLayout->setSpacing(0);

    // 创建添加好友的搜索框
    QLineEdit *contactsSearchBar = new QLineEdit(contactsPage);
    contactsSearchBar->setPlaceholderText("添加新好友...");
    contactsSearchBar->setStyleSheet(R"(
            QLineEdit {
                background: #FFFFFF;
                border: 2px solid #E0E0E0;
                border-radius: 6px;
                padding: 10px;
                font-size: 14px;
            }
            QLineEdit:focus {
                border-color: #00B4FF;
            }
        )");

    // 在搜索框右侧添加一个[+]按钮,并且设置样式
    QHBoxLayout *searchLayout = new QHBoxLayout;
    searchLayout->setContentsMargins(0, 0, 0, 0);
    searchLayout->setSpacing(0);
    searchLayout->addWidget(contactsSearchBar);

    QPushButton *addButton = new QPushButton("+", contactsPage);
    addButton->setFixedSize(40, 40);
    addButton->setStyleSheet(R"(
            QPushButton {
                background: #00B4FF;
                color: white;
                border: none;
                border-radius: 20px;
                font-weight: bold;
            }
            QPushButton:hover {
                background: #0099D6;
            }
            QPushButton:pressed {
                background: #0080B3;
            }
        )");
    searchLayout->addWidget(addButton);

    contactsLayout->addLayout(searchLayout);

    // 新增的好友列表
    QListWidget *contactsListWidget = new QListWidget(contactsPage);
    contactsListWidget->setFrameShape(QFrame::NoFrame);
    contactsListWidget->setVerticalScrollMode(QListWidget::ScrollPerPixel);

    contactsLayout->addWidget(contactsListWidget);

    // 将 contactsPage 添加到 rootLayout
    rootLayout->addWidget(contactsPage, 1);

    // 缓存联系人页面
    cachedContactsPage = contactsPage;

    // 显示会话列表
    sessionListWidget->setVisible(true);
}

void MainWidget::showChatPage() {
    // 显示聊天主页面
    spdlog::info("显示聊天主页面");

    // 如果已经有了其他的主页面元素，隐藏起来
    if (cachedContactsPage) {
        rootLayout->removeWidget(cachedContactsPage);
        cachedContactsPage->hide();
    }

    // 恢复 leftPanel 的宽度
    leftPanel->setFixedWidth(340);

    // 检查缓存的聊天页面是否存在
    if (cachedChatPage) {
        // 如果缓存存在，直接使用缓存的聊天页面
        rootLayout->addWidget(cachedChatPage, 1);
        cachedChatPage->show();

        // 确保 cachedLeftContent 存在并显示
        if (cachedLeftContent) {
            leftPanelLayout->addWidget(cachedLeftContent);
            cachedLeftContent->show();
        }
        return;
    }

    // 初始化 cachedLeftContent
    cachedLeftContent = new QWidget(this);
    cachedLeftContent->setFixedWidth(280);
    cachedLeftContent->setStyleSheet("background: #2E323B;");

    QVBoxLayout *leftLayout = new QVBoxLayout(cachedLeftContent);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(0);

    // 用户信息栏
    QWidget *userBar = new QWidget(cachedLeftContent);
    userBar->setFixedHeight(60);
    userBar->setStyleSheet("background: #23262E; border-bottom: 1px solid #3A3D45;");

    QHBoxLayout *userLayout = new QHBoxLayout(userBar);
    userLayout->setContentsMargins(12, 0, 12, 0);

    QLabel *searchLogo = new QLabel(userBar);
    searchLogo->setFixedSize(40, 40);
    searchLogo->setStyleSheet(R"(
        background: #00B4FF;
        border-radius: 20px;
        color: white;
        font-weight: bold;
    )");
    searchLogo->setText("Q");
    searchLogo->setAlignment(Qt::AlignCenter);

    // 创建新的 searchBar
    cachedSearchBar = new QLineEdit(userBar);
    cachedSearchBar->setPlaceholderText("搜索联系人...");
    cachedSearchBar->setStyleSheet(R"(
        QLineEdit {
            background: #FFFFFF;
            border: 2px solid #E0E0E0;
            border-radius: 6px;
            padding: 10px;
            font-size: 14px;
        }
        QLineEdit:focus {
            border-color: #00B4FF;
        }
    )");
    userLayout->addWidget(searchLogo);
    userLayout->addWidget(cachedSearchBar);

    // 会话列表
    sessionListWidget = new QListWidget(cachedLeftContent);
    sessionListWidget->setFrameShape(QFrame::NoFrame);
    sessionListWidget->setVerticalScrollMode(QListWidget::ScrollPerPixel);

    connect(sessionListWidget, &QListWidget::itemClicked, this, &MainWidget::onSessionItemClicked);

    leftLayout->addWidget(userBar);
    leftLayout->addWidget(sessionListWidget);

    // 将 cachedLeftContent 添加到 leftPanel
    leftPanelLayout->addWidget(cachedLeftContent);

    // 右侧主区域
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
    inputPanel->setMinimumHeight(80); // 设置最小高度
    inputPanel->setMaximumHeight(300);// 设置最大高度
    // 初始化 inputPanel 的初始高度
    QList<int> splitterSizes;
    splitterSizes << 430 << 120;// 聊天区 430px，输入区 120px
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
            addMessage("wyw", "我", QTime::currentTime().toString("hh:mm"), message, MessageType::Self);
        }
    });

    inputLayout->addWidget(messageInput, 1);
    inputLayout->addLayout(btnLayout);

    splitter->addWidget(chatArea);
    splitter->addWidget(inputPanel);
    splitter->setStretchFactor(0, 3);// 聊天区域占更多空间
    splitter->setStretchFactor(1, 1);// 输入区域占较少空间

    rightLayout->addWidget(splitter);

    // 将 rightPanel 添加到 rootLayout
    rootLayout->addWidget(rightPanel, 1);

    // 缓存聊天页面
    cachedChatPage = rightPanel;

    // 绑定搜索框的回调
    connect(cachedSearchBar, &QLineEdit::textChanged, this, &MainWidget::querySearchBar);

    // 初始化 SessionManager
    sessionManager = new SessionManager(sessionListWidget);

    // 显示会话列表
    sessionListWidget->setVisible(true);
}

void MainWidget::onSessionItemClicked(QListWidgetItem *item) {
    QString sessionName = item->data(Qt::UserRole).toString();
    if (sessionName.isEmpty()) {
        sessionName = item->text();
    }

    //如果此时当前主页面为contactsPage，则切换回chatPage
    if (cachedChatPage) {
        showChatPage();
    }
    // 更新会话
    _currentSessionName = sessionName;

    // 将所有的未读消息置为已读
    sessionManager->clearUnreadedCount(sessionName);


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
            addMessage("wyw", "我", QTime::currentTime().toString("hh:mm"), message, MessageType::Self);
        }
    });
}

void MainWidget::updateChatArea(const QString &sessionName) {
    // 清空当前聊天区域
    QWidget *chatContent = chatArea->widget();
    QVBoxLayout *chatLayout = qobject_cast<QVBoxLayout *>(chatContent->layout());
    QLayoutItem *item;
    while ((item = chatLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    // 加载新的聊天记录
    SessionChatData chatData = sessionManager->getMessagesForSession(sessionName);

    // 添加调试信息
    spdlog::info("Loading messages for session: {}", sessionName.toStdString());
    spdlog::info("Number of messages: {}", chatData.messages.size());

    // 遍历消息列表并添加到聊天区域
    for (const auto &msg: chatData.messages) {
        // 检查消息是否已经存在（防止重复加载）
        bool exists = false;
        for (int i = 0; i < chatLayout->count(); ++i) {
            QWidget *widget = chatLayout->itemAt(i)->widget();
            if (widget && widget->property("messageContent") == msg.content &&
                widget->property("messageSender") == msg.sender &&
                widget->property("messageTime") == msg.time) {
                exists = true;
                break;
            }
        }
        if (!exists) {
            BubbleWidget *bubble = new BubbleWidget(msg.sender, msg.time, msg.content, msg.type);
            bubble->setMinimumHeight(60);
            bubble->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            bubble->setProperty("messageContent", msg.content);
            bubble->setProperty("messageSender", msg.sender);
            bubble->setProperty("messageTime", msg.time);
            chatLayout->addWidget(bubble);
        }
    }

    // 重新添加拉伸项
    chatLayout->addStretch();

    // 滚动到底部
    QScrollBar *vScroll = chatArea->verticalScrollBar();
    vScroll->setValue(vScroll->maximum());
    QTimer::singleShot(50, [this]() {
        chatArea->verticalScrollBar()->setValue(chatArea->verticalScrollBar()->maximum());
    });
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
    if (sessionManager) {
        QListWidget *sessionListWidget = sessionManager->getSessionListWidget();
        if (sessionListWidget) {
            sessionListWidget->setStyleSheet(R"(
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

void MainWidget::addSessionItem(const SessionItemData &data) {
    sessionManager->addSessionItem(data);
}

void MainWidget::onLoginSuccess() {
    QTimer::singleShot(300, this, &QWidget::show);// 延迟300ms显示主窗口
}

void MainWidget::addMessage(const QString &session_name, const QString &sender, const QString &time,
                            const QString &content, MessageType type) {
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

    // 创建 SessionMsg 对象并添加到 QList<SessionMsg>
    SessionMsg msg{.sender = sender, .time = time, .content = wrappedContent, .type = type};
    QList<SessionMsg> messages;
    messages.append(msg);

    // 将消息添加到会话管理器
    sessionManager->addSessionData({.name = session_name, .messages = messages});

    // 滚动到底部
    QScrollBar *vScroll = chatArea->verticalScrollBar();
    vScroll->setValue(vScroll->maximum());
    QTimer::singleShot(50, [this]() {
        chatArea->verticalScrollBar()->setValue(chatArea->verticalScrollBar()->maximum());
    });
}

void MainWidget::querySearchBar() {
    QString searchText = cachedSearchBar->text();
    if (searchText.isEmpty()) {
        // 如果搜索文本为空，显示所有会话
        for (QListWidgetItem *item: sessionListWidget->findItems("", Qt::MatchContains)) {
            item->setHidden(false);
        }
    } else {
        // 隐藏所有会话
        for (QListWidgetItem *item: sessionListWidget->findItems("", Qt::MatchContains)) {
            item->setHidden(true);
        }
        // 显示匹配的会话
        for (int i = 0; i < sessionListWidget->count(); ++i) {
            QListWidgetItem *item = sessionListWidget->item(i);
            QString sessionName = item->data(Qt::UserRole).toString();
            if (sessionName.contains(searchText, Qt::CaseInsensitive)) {
                item->setHidden(false);
            }
        }
    }
}

const QString MainWidget::getCurrentSessionName() const {
    return _currentSessionName;
}

void MainWidget::showFilesPage() {
    // 显示文件管理界面
    spdlog::info("显示文件管理界面");

    // 创建文件管理界面的 QWidget
    QWidget *filesPage = new QWidget(this);
    QVBoxLayout *filesLayout = new QVBoxLayout(filesPage);
    filesLayout->addWidget(new QLabel("文件管理界面内容", this));
    filesPage->setLayout(filesLayout);

    // 创建新的窗口
    QMainWindow *filesWindow = new QMainWindow(this);
    filesWindow->setCentralWidget(filesPage);
    filesWindow->setWindowTitle("文件管理");
    filesWindow->show();
}

void MainWidget::showSettingsPage() {
    // 显示设置界面
    spdlog::info("显示设置界面");

    // 创建设置界面的 QWidget
    QWidget *settingsPage = new QWidget(this);
    QVBoxLayout *settingsLayout = new QVBoxLayout(settingsPage);
    settingsLayout->addWidget(new QLabel("设置界面内容", this));
    settingsPage->setLayout(settingsLayout);

    // 创建新的窗口
    QMainWindow *settingsWindow = new QMainWindow(this);
    settingsWindow->setCentralWidget(settingsPage);
    settingsWindow->setWindowTitle("设置");
    settingsWindow->show();
}

void MainWidget::showUserHomePage() {
    // 显示用户主页
    spdlog::info("显示用户主页");
    // 创建用户主页的 QWidget
    QWidget *userHomePage = new QWidget(this);
    QVBoxLayout *userHomeLayout = new QVBoxLayout(userHomePage);
    userHomeLayout->addWidget(new QLabel("用户主页内容", this));
    userHomePage->setLayout(userHomeLayout);

    // 创建新的窗口
    QMainWindow *userHomeWindow = new QMainWindow(this);
    userHomeWindow->setCentralWidget(userHomePage);
    userHomeWindow->setWindowTitle("用户主页");
    userHomeWindow->show();
}


//---------------------------------测试区域-------------------------------------------------//
inline static QString getRandomTime() {
    int hour = QRandomGenerator::global()->bounded(0, 24);
    int minute = QRandomGenerator::global()->bounded(0, 60);
    return QString("%1:%2").arg(hour, 2, 10, QLatin1Char('0')).arg(minute, 2, 10, QLatin1Char('0'));
}

inline static int getRandomUnreadCount() {
    return QRandomGenerator::global()->bounded(0, 100);
}

void MainWidget::startTestMessagePush() {
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, [this]() {
        // 获取当前激活的会话名称
        QString sessionName = getCurrentSessionName();
        if (sessionName.isEmpty()) {
            qWarning() << "No active session selected!";
            return;
        }

        // 随机生成发送者和内容
        QString sender = QRandomGenerator::global()->bounded(0, 2) ? "我" : "系统通知";
        QString content = QString("动态消息%1").arg(QRandomGenerator::global()->bounded(1, 100));
        QString time = getRandomTime();

        // 添加消息到当前会话
        addMessage(sessionName, sender, time, content,
                   sender == "系统通知" ? MessageType::System : MessageType::Other);

        // 更新聊天区域
        updateChatArea(sessionName);
    });
    timer->start(5000);// 每5秒推送一条消息
}

void MainWidget::insertTestCase() {
    // 添加测试会话
    QStringList names = {"wyw", "现代C++卢瑟群", "Alice", "Bob", "Charlie", "David", "Eve", "Frank", "Grace", "Hannah"};
    QStringList lastMessages = {"宝宝你是一个宝宝", "感觉不如Rust", "你好，世界", "明天会更好", "一起吃饭吧",
                                "周末一起玩吧", "学习新东西", "看电影", "旅行计划", "生日快乐"};

    for (int i = 0; i < names.size(); ++i) {
        addSessionItem({.name = names[i], .lastMessage = lastMessages[i], .lastTime = getRandomTime(), .unreadCount = getRandomUnreadCount()});
    }

    // 添加测试消息
    QMap<QString, QList<QPair<QString, QString>>> testMessages = {
            {"wyw", {{"系统通知", "聊天系统初始化完成"}, {"wyw", "宝宝在吗"}, {"我", "宝宝我爱你哦！"}}},
            {"现代C++卢瑟群", {{"国王", "我觉得Rust还是太loser了"}, {"群友1", "国王说得对"}}}};

    for (auto it = testMessages.begin(); it != testMessages.end(); ++it) {
        const QString &sessionName = it.key();
        for (const auto &[sender, content]: it.value()) {
            addMessage(sessionName, sender, getRandomTime(), content,
                       sender == "系统通知" ? MessageType::System : MessageType::Other);
        }
    }

    // 启动动态消息推送
    startTestMessagePush();
}
//---------------------------------测试区域-------------------------------------------------//