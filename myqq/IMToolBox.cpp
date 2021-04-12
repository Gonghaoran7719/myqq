#include "IMToolBox.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QMenu>
#include <QDebug>
#include<QToolButton>
#include <QScrollArea>
#include<QMessageBox>


IMToolItem::IMToolItem(const QString &title  , QWidget *parent) :
    QWidget(parent), m_layout(new QVBoxLayout)
{
    m_Flag = 1;m_FlagWhole = 1;
    m_isVisable = /*false*/true;
    m_layout = new QVBoxLayout;

    m_label = new QToolButton ;
    m_label->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    m_label->setFont(QFont( "微软雅黑" , 10 /*,  QFont::Bold*/) );
    m_label->setStyleSheet("color:#0000C6");

    m_label->setText(title);
    m_label->setIcon(QIcon(":/images/down.png")) ;


//    label->setStyleSheet("background-color: #1f2021;");
    m_layout->addWidget(m_label);

    m_layout->setContentsMargins(0,0,0,0);
    m_layout->setSpacing(0);
    setLayout(m_layout);

    creatMenu();

    connect(m_label,&QToolButton::clicked , this, &IMToolItem::on_ToolButton_Clicked);
}

// 添加
void IMToolItem::addItem(QWidget *item)
{
    m_itemList.append(item);
    m_layout->addWidget(item, 0 , Qt::AlignTop);
//    m_layout->addStretch(1);
    if (0 == m_itemList.size())
        item->setVisible(false);
    else
    {
        item->setVisible(m_isVisable);//m_itemList[0]->isVisible());
    }

    QString str = this->getWindowText();
    str = str.left(  str.lastIndexOf('(') ) +QString("(%1)").arg(this->getWidgetList().size());
    this->setTile(str);

    m_layout->addSpacing(5);
    this->repaint();
}

QString IMToolItem::getWindowText()
{
    return m_label->text();
}
void IMToolItem::SetWindowText(QString str)
{
    m_label->setText(str);
}

//删除
void IMToolItem::removeItem(QWidget *item)
{
    item->setVisible(false);
    m_layout->removeWidget(item);
    m_itemList.removeAt(m_itemList.indexOf(item));

    QString str = this->getWindowText();
    str = str.left(  str.indexOf('(') ) +QString("(%1)").arg(this->getWidgetList().size());

    this->setTile(str);
    this->repaint();
}


// 设置标题
void IMToolItem::setTile(const QString & title)
{
    m_label->setText(title);
}

// 获取成员数量
int IMToolItem::getSize()
{
    return m_itemList.size();
}
void IMToolItem::on_ToolButton_Clicked()
{
    if( !m_FlagWhole ) return;
    bool isVisable = !m_isVisable;
    for (int i=0; i<m_itemList.size(); ++i)
    {
        m_itemList[i]->setVisible(isVisable);
    }

    if (m_itemList.size() > 0)
        m_isVisable = m_itemList[0]->isVisible();
    if( m_isVisable )
    {
        m_label->setIcon( QIcon(":/images/down.png")) ;
    }else
    {
        m_label->setIcon(QIcon(":/images/right.png")) ;
    }
    return;
}

// 鼠标点击事件
void IMToolItem:: mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton)
    {
        if( !m_Flag ) {

            return;
        }
        onClickRightButton();

    }

}
void IMToolItem::SearchItem(QString str)
{
    for (int i=0; i<m_itemList.size(); ++i)
    {
        if(  ((Item*)m_itemList[i] )->GetName().contains( str) )
            m_itemList[i]->setVisible(true);
        else
            m_itemList[i]->setVisible(false);
    }
}

// mark private slots:--------------------------------------------------
/*************************************************
Function Name： renameBox()
Description:  重命名
*************************************************/
void IMToolItem::renameBox()
{
    QString str = m_label->text().left( m_label->text().indexOf('(') );
    //qDebug() << str;
    if(str == "未分组好友" ||str == "自己"||str == "黑名单" )
    {
        QMessageBox::information(0,"提示:","不能重命名该组!", QMessageBox::Ok);
    }
        else
    {
       // qDebug() << "chose rename";
        emit renameBoxSignal(m_label->text());
    }
}

/*************************************************
Function Name： removeBox()
Description:  删除整个
*************************************************/
void IMToolItem::removeBox()
{
    if(this->m_label->text() == "未分组好友" ||this->m_label->text() == "自己"||this->m_label->text() == "黑名单" )
    {
        QMessageBox::information(0,"提示:","不能删除该组!", QMessageBox::Ok);
    }
        else
    {
        //qDebug() << "chose remove";
        emit removeBoxSignal(m_label->text());
    }
}


/*************************************************
Function Name： onClickRightButton()
Description: 右键点击按键
*************************************************/
void IMToolItem::onClickRightButton()
{
    return;
    if (m_menu == NULL)
    {
        //创建右键菜单
        creatMenu();
    }
    m_menu->exec(QCursor::pos());
}

/*************************************************
Function Name： creatMenu
Description: 创建右键菜单
*************************************************/
void IMToolItem:: creatMenu()
{
    m_menu = new QMenu(this);
    //m_menu->setStyleSheet("QMenu::item:selected{background-color:#0000C6;}");
    m_menu->setStyleSheet(
                "\
                QMenu {\
                background-color:rgb(255,255,255); /*整个背景*/\
                border: 3px solid rgb(60,60,60);/*整个菜单边缘*/\
                border-image: url(:/images/border.png);\
                }\
            QMenu::item {\
                font-size: 9pt; \
                color: rgb(0,0,0);  /*字体颜色*/\
                border: 1px solid rgb(60,60,60);    /*item选框*/\
                background-color:rgb(255,255,255);\
                 }\
            QMenu::item:selected { \
                background-color:rgb(108,182,255);/*选中的样式*/\
                }\
            QMenu::item:pressed {/*菜单项按下效果*/\
                                      border: 1px solid rgb(60,60,61); \
                                      background-color: rgb(108,182,255); \
                                  }");
    QAction *rename = new QAction(tr("重命名"), m_menu);
    QAction *remove = new QAction(tr("删除"), m_menu);


    connect(rename, SIGNAL(triggered()),
            this, SLOT(renameBox()));
    connect(remove, SIGNAL(triggered()),
            this, SLOT(removeBox()));

    m_menu->addAction(rename);
    m_menu->addAction(remove);
}





IMToolBox::IMToolBox(QWidget *parent) :
    QWidget(parent), m_layout(new QVBoxLayout)
{

    QVBoxLayout *mainLayout=new QVBoxLayout();
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(0);

    m_contentsWidget = new QWidget;
    m_layout->setContentsMargins(0,0,0,0);
    m_layout->setSpacing(0);
    m_contentsWidget->setLayout(m_layout);
    m_contentsWidget->setStyleSheet("QWidget{border: 0;}");

    m_friendsScrollArea = new QScrollArea(this);
    m_friendsScrollArea->setWidgetResizable(true);
    m_friendsScrollArea->setAlignment(Qt::AlignLeft);
    m_friendsScrollArea->setWidget(m_contentsWidget);

    mainLayout->addWidget(m_friendsScrollArea);
    setLayout(mainLayout);
    m_menu = 0;

    setStyleSheet("QWidget{border: 0;}");
}


/*************************************************
Function Name： addItem()
Description:  添加
*************************************************/
void IMToolBox::addItem(QWidget *item)
{
    // Remove last spacer item if present.
    int count = m_layout->count();
    if (count > 1)
    {
        m_layout->removeItem(m_layout->itemAt(count - 1));
    }

    // Add item and make sure it stretches the remaining space.
    m_layout->addWidget(item);
    m_layout->addStretch();
}
void IMToolBox::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton)
    {
        onClickRightButton();
    }
    QWidget::mousePressEvent(event);
}

//删除
void IMToolBox::removeItem(QWidget *item)
{
    m_layout->removeWidget(item);
}
void IMToolBox::onClickRightButton()
{
    return;
    if (m_menu == NULL)
    {
        //创建右键菜单
        creatMenu();
    }
    m_menu->exec(QCursor::pos());
}

void IMToolBox::creatMenu()
{
    m_menu = new QMenu(this);

    m_menu->setStyleSheet(
                "\
                QMenu {\
                background-color:rgb(255,255,255); /*整个背景*/\
                border: 3px solid rgb(60,60,60);/*整个菜单边缘*/\
                border-image: url(:/images/border.png);\
                }\
            QMenu::item {\
                font-size: 9pt; \
                color: rgb(0,0,0);  /*字体颜色*/\
                border: 1px solid rgb(60,60,60);    /*item选框*/\
                background-color:rgb(255,255,255);\
                 }\
            QMenu::item:selected { \
                background-color:rgb(108,182,255);/*选中的样式*/\
                }\
            QMenu::item:pressed {/*菜单项按下效果*/\
                                      border: 1px solid rgb(60,60,61); \
                                      background-color: rgb(108,182,255); \
                                  }");
    QAction *addGp = new QAction(tr("添加分组"), m_menu);

    connect(addGp, SIGNAL(triggered()),
            this, SLOT(addBox()));

    m_menu->addAction(addGp);
}

void IMToolBox::addBox()
{

}
