#pragma once

#include "Client/BubbleWidget.h"
#include <QGraphicsOpacityEffect>
#include <QHBoxLayout>
#include <QLabel>
#include <QParallelAnimationGroup>// 新增并行动画组
#include <QPropertyAnimation>
#include <QWidget>
#include <cstdint>
#include <qcontainerfwd.h>
#include <qvariant.h>

/**
 * @author       : caomengxuan666
 * @brief        : 会话列表项
**/

/**
 * @struct       : 会话列表的数据存储
 * @author       : caomengxuan666
 * @brief        : 
 * @return        {*}
**/
struct SessionItemData {
    QString name;
    QString lastMessage;
    QString lastTime;
    int unreadCount;//todo
    QImage avatar;  //todo
};

/**
 * @author       : caomengxuan
 * @brief        : 会话具体的消息
 * @return        {*}
**/
struct SessionMsg {
    //消息的发送者
    QString sender;
    QString time;
    QString content;
    MessageType type;
};

/**
 * @author       : caomengxuan
 * @brief        : 会话聊天的数据
 * @return        {*}
**/
struct SessionChatData {
    //会话名称
    QString name;
    //存储消息
    QList<SessionMsg> messages;
};

class SessionItem : public QWidget {
    Q_OBJECT
public:
    explicit SessionItem(const SessionItemData &data, QWidget *parent = nullptr) noexcept
        : QWidget(parent), _sessionData(data) {
        setupUI(data);
        initAnimations();// 初始化动画系统
    }

    void updateUnreadCount(int count) {
        if (_sessionData.unreadCount == count) return;

        _sessionData.unreadCount = count;
        const int displayCount = std::min(count, 99);

        // 停止所有进行中的动画
        stopCurrentAnimations();

        if (count > 0) {
            executeFadeInAnimation(displayCount);
        } else {
            executeFadeOutAnimation();
        }
    }

    void updateLastMessage(const QString &msg, const QString &time) {
        _sessionData.lastMessage = msg;
        _sessionData.lastTime = time;

        // 限制消息长度
        QString elidedMsg = msgLabel->fontMetrics().elidedText(
                msg, Qt::ElideRight, msgLabel->width() - 20);

        msgLabel->setText(elidedMsg);
        timeLabel->setText(time);

        // 添加渐变动画
        if (_opacityAnimation) {
            _opacityAnimation->stop();
            delete _opacityAnimation;
        }
        QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect(msgLabel);
        msgLabel->setGraphicsEffect(effect);

        _opacityAnimation = new QPropertyAnimation(effect, "opacity");
        _opacityAnimation->setDuration(500);
        _opacityAnimation->setStartValue(0);
        _opacityAnimation->setEndValue(1);
        _opacityAnimation->start(QAbstractAnimation::DeleteWhenStopped);
    }

private:
    void setupUI(const SessionItemData &data) {
        QHBoxLayout *layout = new QHBoxLayout(this);
        layout->setContentsMargins(12, 8, 12, 8);

        // 头像区域
        QLabel *avatar = new QLabel(this);
        avatar->setFixedSize(48, 48);
        avatar->setStyleSheet(R"(
            background: #00B4FF;
            border-radius: 24px;
            color: white;
            font-weight: bold;
        )");
        avatar->setText(data.name.left(1));
        avatar->setAlignment(Qt::AlignCenter);

        // 未读消息标签（提升为成员变量）
        unreadLabel = new QLabel(this);
        unreadLabel->setFixedSize(20, 20);
        unreadLabel->setStyleSheet(R"(
            background-color: red;
            border-radius: 10px;
            color: white;
            font-weight: bold;
            font-size: 12px;
            qproperty-alignment: 'AlignCenter';
        )");
        unreadLabel->move(avatar->width() - unreadLabel->width() + 10, 10);
        unreadLabel->setText(data.unreadCount > 0 ? QString::number(std::min(data.unreadCount, 99)) : "");
        unreadLabel->setVisible(data.unreadCount > 0);

        // 右侧信息区域
        QWidget *infoWidget = new QWidget(this);
        QVBoxLayout *infoLayout = new QVBoxLayout(infoWidget);
        infoLayout->setContentsMargins(0, 0, 0, 0);

        // 顶部（名称 + 时间）
        QWidget *topWidget = new QWidget(infoWidget);
        QHBoxLayout *topLayout = new QHBoxLayout(topWidget);
        topLayout->setContentsMargins(0, 0, 0, 0);

        QLabel *nameLabel = new QLabel(data.name, topWidget);
        nameLabel->setStyleSheet("color: #DCDDDE; font-weight: 600;");

        timeLabel = new QLabel(data.lastTime, topWidget);// 提升为成员变量
        timeLabel->setStyleSheet("color: #7A7C80; font-size: 12px;");

        topLayout->addWidget(nameLabel);
        topLayout->addWidget(timeLabel);

        // 最后消息
        msgLabel = new QLabel(data.lastMessage, infoWidget);// 提升为成员变量
        msgLabel->setStyleSheet("color: #7A7C80; font-size: 13px;");
        msgLabel->setMaximumWidth(200);
        msgLabel->setWordWrap(true);

        infoLayout->addWidget(topWidget);
        infoLayout->addWidget(msgLabel);

        layout->addWidget(avatar);
        layout->addWidget(infoWidget, 1);
    }
    void stopCurrentAnimations() {
        if (_fadeInAnimation->state() == QAbstractAnimation::Running) {
            _fadeInAnimation->stop();
        }
        if (_fadeOutAnimation->state() == QAbstractAnimation::Running) {
            _fadeOutAnimation->stop();
        }
    }

    // 执行淡出动画
    void executeFadeOutAnimation() {
        QParallelAnimationGroup *group = new QParallelAnimationGroup(this);
        group->addAnimation(_fadeOutAnimation);

        // 位移动画（向上移动5像素）
        _positionAnimation->setStartValue(unreadLabel->pos());
        _positionAnimation->setEndValue(unreadLabel->pos() + QPoint(0, -5));
        group->addAnimation(_positionAnimation);

        connect(group, &QParallelAnimationGroup::finished, [this]() {
            unreadLabel->setVisible(false);
            unreadLabel->setText("");// 动画完成后清空
        });

        group->start(QAbstractAnimation::DeleteWhenStopped);
    }

    // 执行淡入动画
    void executeFadeInAnimation(int count) {
        unreadLabel->setText(QString::number(count));
        unreadLabel->setVisible(true);

        // 并行动画组
        QParallelAnimationGroup *group = new QParallelAnimationGroup(this);
        group->addAnimation(_fadeInAnimation);

        // 位移动画（从上方回到原位）
        _positionAnimation->setStartValue(unreadLabel->pos() + QPoint(0, -5));
        _positionAnimation->setEndValue(unreadLabel->pos());
        group->addAnimation(_positionAnimation);

        group->start(QAbstractAnimation::DeleteWhenStopped);
    }

    // 新增动画初始化
    void initAnimations() {
        _unreadEffect = new QGraphicsOpacityEffect(unreadLabel);
        unreadLabel->setGraphicsEffect(_unreadEffect);
        _unreadEffect->setOpacity(1.0);// 初始完全可见

        // 配置通用动画参数
        _fadeInAnimation = new QPropertyAnimation(_unreadEffect, "opacity", this);
        configureAnimation(_fadeInAnimation, 0.0, 1.0);

        _fadeOutAnimation = new QPropertyAnimation(_unreadEffect, "opacity", this);
        configureAnimation(_fadeOutAnimation, 1.0, 0.0);

        // 创建位移动画（Y轴向上5像素）
        _positionAnimation = new QPropertyAnimation(unreadLabel, "pos", this);
        _positionAnimation->setDuration(200);
        _positionAnimation->setEasingCurve(QEasingCurve::OutQuad);
    }

    // 配置动画参数
    void configureAnimation(QPropertyAnimation *anim, qreal start, qreal end) {
        anim->setDuration(200);                     // 统一动画时长
        anim->setEasingCurve(QEasingCurve::OutQuad);// 统一缓动曲线
        anim->setStartValue(start);
        anim->setEndValue(end);
    }

private:
    SessionItemData _sessionData;
    QLabel *unreadLabel;
    QLabel *msgLabel;
    QLabel *timeLabel;
    QPropertyAnimation *_unreadAnimation = nullptr;
    QPropertyAnimation *_opacityAnimation = nullptr;
    QGraphicsOpacityEffect *_unreadEffect = nullptr;
    QPropertyAnimation *_fadeInAnimation = nullptr;
    QPropertyAnimation *_fadeOutAnimation = nullptr;
    QPropertyAnimation *_positionAnimation = nullptr;
};