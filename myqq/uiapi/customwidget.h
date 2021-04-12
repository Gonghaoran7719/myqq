#ifndef CUSTOMWIDGET_H
#define CUSTOMWIDGET_H

#include <QWidget>
#include<QMouseEvent>
#include<QDialog>


class CustomMoveWidget : public QWidget {
    Q_OBJECT
public:
    explicit CustomMoveWidget(QWidget *parent = 0);
    ~CustomMoveWidget();

protected:
    QPoint mousePoint;
    bool m_mousePressed;

    void mouseMoveEvent(QMouseEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *);
};

class CustomMoveDialog : public QDialog {
    Q_OBJECT
public:
    explicit CustomMoveDialog(QWidget *parent = 0);
    ~CustomMoveDialog();

protected:
    QPoint mousePoint;
    bool m_mousePressed;
    bool max;
    QRect location;


    void mouseMoveEvent(QMouseEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *);
public slots:
    void slot_showMax();
    void slot_showMin();
    void slot_close();

};

#endif // CUSTOMWIDGET_H
