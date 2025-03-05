/**
 * @FilePath     : /mxChat/include/Client/SessionItem.hpp
 * @Description  :  主页面的会话界面
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-03-05 17:02:31
 * @Copyright    : PESONAL DEVELOPER CMX., Copyright (c) 2025.
**/
#include <QHBoxLayout>
#include <QLabel>
#include <QWidget>

/**
 * @author       : caomengxuan
 * @brief        : 会话列表项
**/
class SessionItem : public QWidget {
    Q_OBJECT
public:
    explicit SessionItem(const QString &name, const QString &lastMsg,
                         const QString &time, QWidget *parent = nullptr)
        : QWidget(parent) {
        setupUI(name, lastMsg, time);
    }

private:
    /**
     * @author       : caomengxuan
     * @brief        : 初始化列表项的UI
     * @param         {QString} &name:
     * @param         {QString} &lastMsg:
     * @param         {QString} &time:
     * @return        {*}
    **/
    void setupUI(const QString &name, const QString &lastMsg, const QString &time) {
        QHBoxLayout *layout = new QHBoxLayout(this);
        layout->setContentsMargins(12, 8, 12, 8);

        // 头像
        QLabel *avatar = new QLabel(this);
        avatar->setFixedSize(48, 48);
        avatar->setStyleSheet(R"(
            background: #00B4FF;
            border-radius: 24px;
            color: white;
            font-weight: bold;
        )");
        avatar->setText(name.left(1));
        avatar->setAlignment(Qt::AlignCenter);

        // 右侧信息
        QWidget *infoWidget = new QWidget(this);
        QVBoxLayout *infoLayout = new QVBoxLayout(infoWidget);
        infoLayout->setContentsMargins(0, 0, 0, 0);

        // 名称和时间
        QWidget *topWidget = new QWidget(infoWidget);
        QHBoxLayout *topLayout = new QHBoxLayout(topWidget);
        topLayout->setContentsMargins(0, 0, 0, 0);

        QLabel *nameLabel = new QLabel(name, topWidget);
        nameLabel->setStyleSheet("color: #DCDDDE; font-weight: 600;");

        QLabel *timeLabel = new QLabel(time, topWidget);
        timeLabel->setStyleSheet("color: #7A7C80; font-size: 12px;");

        topLayout->addWidget(nameLabel);
        topLayout->addWidget(timeLabel);

        // 最后消息
        QLabel *msgLabel = new QLabel(lastMsg, infoWidget);
        msgLabel->setStyleSheet("color: #7A7C80; font-size: 13px;");
        msgLabel->setMaximumWidth(200);
        msgLabel->setWordWrap(true);

        infoLayout->addWidget(topWidget);
        infoLayout->addWidget(msgLabel);

        layout->addWidget(avatar);
        layout->addWidget(infoWidget, 1);
    }
};
