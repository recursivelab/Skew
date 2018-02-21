#include "mainwindow.h"
#include <QApplication>
#include <QImageReader>
#include <QImageWriter>
#include <iostream>
using namespace std;
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    foreach (QString name, QImageReader::supportedImageFormats()) {
        cout << name.toStdString();
        if (QImageWriter::supportedImageFormats().contains(name.toUtf8())) {
            cout << "\twriting";
        }
        cout << endl;
    }

    cout << QImageReader::supportedImageFormats().count() << "\t" << QImageWriter::supportedImageFormats().count() << endl;
    return a.exec();
}
