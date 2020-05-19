#include <QApplication>
#include "GUI/imgSRWidget.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    imgSRWidget w;
    w.show();
    return a.exec();
}
