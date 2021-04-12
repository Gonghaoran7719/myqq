#include "myqq.h"
#include <QApplication>
#include<QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QFile file(":/qss/default.css");
    file.open(QIODevice::ReadOnly);
    qApp->setStyleSheet( file.readAll());

    MyQQ w;
    w.hide();
 //   w.show();

    return a.exec();
}
