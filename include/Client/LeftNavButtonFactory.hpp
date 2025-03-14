#ifndef LEFTNAVBUTTONFACTORY_H
#define LEFTNAVBUTTONFACTORY_H

#include <QList>
#include <QPainter>
#include <QPainterPath>
#include <QPixmap>
#include <QToolButton>
#include <cstdint>

class RoundPoint {
public:
    RoundPoint(QToolButton *button) : _button(button), _infonum(0) {}

    void drawRoundPoint(QSize size, QPainter &painter) {
        if (_infonum > 0) {
            if (_infonum >= 99) {
                // 绘制99+时，增加圆点大小
                painter.setBrush(QColor("#00B4FF"));
                painter.drawEllipse(size.width() - 24, 0, 24, 24);// 增加圆点大小

                // 绘制 "99+"
                painter.setPen(Qt::red);
                painter.setFont(QFont("Arial", 10, QFont::Bold));
                painter.drawText(QRect(size.width() - 24, 0, 24, 24), Qt::AlignCenter, "99+");
            } else {
                // 绘制圆点和数字
                painter.setBrush(QColor("#00B4FF"));
                painter.drawEllipse(size.width() - 20, 0, 16, 16);// 圆点大小

                // 绘制数字
                painter.setPen(Qt::white);
                painter.setFont(QFont("Arial", 10, QFont::Bold));
                painter.drawText(QRect(size.width() - 20, 0, 16, 16), Qt::AlignCenter, QString::number(_infonum));
            }
        }
    }

    void update() {
        if (_infonum == 0) {
            hideRoundPoint();
        } else {
            _button->update();// 强制按钮重绘
        }
    }

    void setInfoNum(std::uint8_t num) {
        _infonum = num;
        update();
    }

    void addInfoNum() {
        ++_infonum;
        update();
    }

    void hideRoundPoint() {
        // 隐藏圆点可以通过设置 _infonum 为 0 并调用 update 来实现
        _infonum = 0;
        _button->update();// 强制按钮重绘
    }

    const auto getInfoNum() const {
        return _infonum;
    }

private:
    QToolButton *_button;
    std::uint8_t _infonum;
};

class ButtonDecorator : public QToolButton {
    Q_OBJECT
public:
    ButtonDecorator(QToolButton *button, std::uint8_t infonum = 0)
        : QToolButton(button->parentWidget()), _roundPoint(this) {
        setIcon(button->icon());
        setIconSize(button->iconSize());
        setToolButtonStyle(button->toolButtonStyle());
        setFixedSize(button->size());
        setStyleSheet(button->styleSheet());
        _roundPoint.setInfoNum(infonum);
        regist_listen_events();
    }

    /**
     * @author       : caomengxuan
     * @brief        : 红点装饰器的监视事件,比如收到消息之后直接触发这个事件让他+1
     * @return        {*}
    **/    
    void regist_listen_events() {
        connect(this, &QToolButton::clicked, [this]() {
            _roundPoint.setInfoNum(_roundPoint.getInfoNum() + 1);// 示例：每次点击增加通知数
        });
        // todo 这里需要接收到通知之后更新，然后点击之后把通知去掉
    }

    void paintEvent(QPaintEvent *event) override {
        QToolButton::paintEvent(event);
        QPainter painter(this);
        _roundPoint.drawRoundPoint(size(), painter);
    }

    std::uint8_t getInfoNum() const {
        return _roundPoint.getInfoNum();
    }

private:
    RoundPoint _roundPoint;
};

template<class ButtonType>
class LeftNavButtonFactory {
public:
    QToolButton *createButton() const {
        return ButtonType::createButtonImpl();
    }

    static QToolButton *createBaseButton() {
        QToolButton *btn = new QToolButton;
        btn->setIconSize(QSize(24, 24));
        btn->setToolButtonStyle(Qt::ToolButtonIconOnly);
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
        return btn;
    }
};

class AvatarButtonFactory : public LeftNavButtonFactory<AvatarButtonFactory> {
public:
    static QToolButton *createButtonImpl() {
        QToolButton *btn = createBaseButton();

        QPixmap avatar(":/images/avatar.png");
        QPixmap circularAvatar(24, 24);
        circularAvatar.fill(Qt::transparent);

        QPainter painter(&circularAvatar);
        painter.setRenderHint(QPainter::Antialiasing);

        QPainterPath path;
        path.addEllipse(0, 0, 24, 24);
        painter.setClipPath(path);

        painter.drawPixmap(0, 0, avatar.scaled(24, 24, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        painter.end();

        btn->setIcon(QIcon(circularAvatar));

        return btn;
    }
};

class ChatButtonFactory : public LeftNavButtonFactory<ChatButtonFactory> {
public:
    static QToolButton *createButtonImpl() {
        QToolButton *btn = createBaseButton();

        QPixmap icon(24, 24);
        icon.fill(Qt::transparent);
        QPainter painter(&icon);
        painter.setRenderHint(QPainter::Antialiasing);

        painter.setPen(QPen(QColor("#FFFFFF"), 2));
        QPainterPath path;
        path.addRoundedRect(QRectF(4, 4, 16, 16), 8, 8);
        path.moveTo(20, 12);
        path.lineTo(24, 16);
        path.lineTo(20, 20);
        painter.drawPath(path);

        painter.end();
        btn->setIcon(QIcon(icon));

        // 使用 ButtonDecorator 包装 baseButton
        return new ButtonDecorator(btn, 0);
    }
};

class ContactsButtonFactory : public LeftNavButtonFactory<ContactsButtonFactory> {
public:
    static QToolButton *createButtonImpl() {
        QToolButton *btn = createBaseButton();

        QPixmap icon(24, 24);
        icon.fill(Qt::transparent);
        QPainter painter(&icon);
        painter.setRenderHint(QPainter::Antialiasing);

        painter.setPen(QPen(QColor("#FFFFFF"), 2));
        painter.drawEllipse(4, 4, 16, 16);
        painter.drawLine(12, 8, 12, 16);
        painter.drawLine(8, 12, 16, 12);

        painter.end();
        btn->setIcon(QIcon(icon));

        return btn;
    }
};

class FilesButtonFactory : public LeftNavButtonFactory<FilesButtonFactory> {
public:
    static QToolButton *createButtonImpl() {
        QToolButton *btn = createBaseButton();

        QPixmap icon(24, 24);
        icon.fill(Qt::transparent);
        QPainter painter(&icon);
        painter.setRenderHint(QPainter::Antialiasing);

        painter.setPen(QPen(QColor("#FFFFFF"), 2));
        QPainterPath path;
        path.lineTo(20, 4);
        path.moveTo(4, 4);
        path.lineTo(20, 20);
        path.lineTo(4, 20);
        path.lineTo(4, 4);
        path.moveTo(20, 4);
        path.lineTo(16, 8);
        path.lineTo(16, 4);
        painter.drawPath(path);

        painter.end();
        btn->setIcon(QIcon(icon));

        return btn;
    }
};

class SettingsButtonFactory : public LeftNavButtonFactory<SettingsButtonFactory> {
public:
    static QToolButton *createButtonImpl() {
        QToolButton *btn = createBaseButton();

        QPixmap icon(24, 24);
        icon.fill(Qt::transparent);
        QPainter painter(&icon);
        painter.setRenderHint(QPainter::Antialiasing);

        painter.setPen(QPen(QColor("#FFFFFF"), 2));
        painter.drawEllipse(4, 4, 16, 16);
        for (int i = 0; i < 8; ++i) {
            qreal angle = i * M_PI / 4;
            qreal x1 = 12 + 6 * cos(angle);
            qreal y1 = 12 + 6 * sin(angle);
            qreal x2 = 12 + 8 * cos(angle);
            qreal y2 = 12 + 8 * sin(angle);
            painter.drawLine(x1, y1, x2, y2);
        }

        painter.end();
        btn->setIcon(QIcon(icon));

        return btn;
    }
};

class InfoButtonFactory : public LeftNavButtonFactory<InfoButtonFactory> {
public:
    static QToolButton *createButtonImpl() {
        QToolButton *btn = createBaseButton();

        QPixmap icon(24, 24);
        icon.fill(Qt::transparent);
        QPainter painter(&icon);
        painter.setRenderHint(QPainter::Antialiasing);

        painter.setPen(QPen(QColor("#FFFFFF"), 2));
        QPainterPath path;

        // 绘制信封形状
        path.moveTo(4, 12);
        path.lineTo(12, 4);
        path.lineTo(20, 12);
        path.lineTo(4, 12);
        path.moveTo(4, 12);
        path.lineTo(4, 20);
        path.lineTo(20, 20);
        path.lineTo(20, 12);
        painter.drawPath(path);

        painter.end();
        btn->setIcon(QIcon(icon));

        // 使用 ButtonDecorator 包装 baseButton
        return new ButtonDecorator(btn, 0);// 示例：初始通知数为 2
    }
};

class LeftNavButtonCreator {
public:
    auto getLeftNavButtons() const {
        auto avatarBtn = std::make_unique<AvatarButtonFactory>()->createButton();
        auto chatBtn = std::make_unique<ChatButtonFactory>()->createButton();
        auto contactsBtn = std::make_unique<ContactsButtonFactory>()->createButton();
        auto filesBtn = std::make_unique<FilesButtonFactory>()->createButton();
        auto settingsBtn = std::make_unique<SettingsButtonFactory>()->createButton();
        auto infoBtn = std::make_unique<InfoButtonFactory>()->createButton();
        return std::make_tuple(std::move(avatarBtn), std::move(chatBtn), std::move(contactsBtn), std::move(filesBtn), std::move(settingsBtn), std::move(infoBtn));
    }
};

#endif// LEFTNAVBUTTONFACTORY_H