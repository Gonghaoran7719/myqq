#ifndef IMTOOLBOX_H
#define IMTOOLBOX_H

#include <QWidget>
class QVBoxLayout;
class QLabel;
class QMenu;
class QToolButton;
class QScrollArea;

class Item
{
public:
    virtual QString GetName() = 0;
};

class IMToolItem : public QWidget {
    Q_OBJECT
public:
    IMToolItem(const QString &title , QWidget *parent = 0);

    void addItem(QWidget *item);

    void removeItem(QWidget *item);

    void setTile(const QString &);

    int getSize();

    QString getWindowText();

    void SetWindowText(QString str);

    void SearchItem(QString str);

    QList<QWidget *> & getWidgetList()
    {
        return m_itemList;
    }

signals:
    void renameBoxSignal(const QString & title);
    void removeBoxSignal(const QString & title);

protected:
    void mousePressEvent(QMouseEvent *event);

private slots:
    void renameBox();
    void removeBox();
    void onClickRightButton();

    void on_ToolButton_Clicked();
private:
    void creatMenu();

private:
    QList<QWidget *> m_itemList;
    QVBoxLayout *m_layout;
    bool m_isVisable;
    QMenu *m_menu;
    QToolButton  *m_label;
    QPixmap *icons;
public:
    int m_Flag;
    int m_FlagWhole;
//    QLabel *m_label;
};


class IMToolBox : public QWidget
{
    Q_OBJECT
public:
    IMToolBox(QWidget *parent = 0);
    void addItem(QWidget *item);
    void removeItem(QWidget *item);
signals:

public slots:
    void mousePressEvent(QMouseEvent *event);
    void onClickRightButton();
    void creatMenu();
    void addBox();
protected:

private:
    QVBoxLayout *m_layout;
    QWidget *m_contentsWidget;
    QScrollArea *m_friendsScrollArea;
    QMenu *m_menu;
};


#endif // IMTOOLBOX_H
