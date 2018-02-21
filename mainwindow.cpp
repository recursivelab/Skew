#include <QFile>
#include <QFileDialog>
#include <QImageReader>
#include <QPainter>
#include <QPaintEvent>
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    counter = 0;
    ui->setupUi(this);
    mouseX = -1;
    mouseY = -1;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_action_Open_triggered()
{
    QString filename = QFileDialog::getOpenFileName(0, tr("Open"));

    if (filename.isEmpty()) {
        return;
    }

    QImageReader reader(filename);

    if (reader.canRead()) {
        const_cast<QImage&>(image) = reader.read();
        double imageRatio = (double)(image.height())/image.width();
        double widgetRatio = (double)height()/width();
        double zoom;

        if (imageRatio > widgetRatio) {
            zoom = (double)height()/image.height();
        } else {
            zoom = imageRatio/widgetRatio;
        }

        trans = Transform();
        trans(2, 2) = zoom;
        cache = QImage();
        update();
    }
}

void draw(QPainter &painter, QPoint pos, int index, QColor color)
{
    painter.setPen(color);
    painter.drawEllipse(pos, 5, 5);
    painter.drawText(QRect(pos.x() - 5, pos.y() - 5, 10, 10), QString("%1").arg(index));
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    if (image.isNull() == false) {
        if (cache.isNull())
        {
            cache = QImage(size(), image.format());

            for (double y = 0; y < cache.height(); ++y) {
                for (double x = 0; x < cache.width(); ++x) {
                    Point p;

                    p(0) = x;
                    p(1) = y;
                    p(2) = 1;
                    p = trans*p;

                    int a = p(0)/p(2) + 0.5;
                    int b = p(1)/p(2) + 0.5;
                    uint color = 0;

                    if (a >= 0 && b >= 0 && a < image.width() && b < image.height()) {
                        color = image.pixel(a, b);
                    }

                    cache.setPixel(x, y, color);
                }
            }
        }

        painter.drawImage(QRect(QPoint(0, 0), QSize(cache.width(), cache.height())), cache);

        int whatPoints = counter;
        int wherePoints = 0;

        if (whatPoints > 4) {
            wherePoints = counter - 4;
            whatPoints = 4;
        }

        for (int i = 0; i < whatPoints; ++i) {
            draw(painter, what[i], i + 1, qRgb(255, 0, 0));
        }

        for (int i = 0; i < wherePoints; ++i) {
            draw(painter, where[i], i + 1, qRgb(0, 0, 255));
        }
    }

    painter.setPen(qRgb(255, 255, 255));
    painter.drawLine(0, mouseY, width() - 1, mouseY);
    painter.drawLine(mouseX, 0, mouseX, height() - 1);
    event->accept();
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    cache = QImage();
    event->accept();
    update();
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if (counter == 8) {
        counter = 0;
    }

    if (counter < 8) {
        if (counter < 4) {
            what[counter] = event->pos();
        } else {
            where[counter - 4] = event->pos();
        }

        ++counter;
    }

    if (counter == 8) {
        Point wherePoints[4];
        Point whatPoints[4];

        for (int i = 0; i < 4; ++i) {
            wherePoints[i](0) = where[i].x();
            wherePoints[i](1) = where[i].y();
            wherePoints[i](2) = 1;
            whatPoints[i](0) = what[i].x();
            whatPoints[i](1) = what[i].y();
            whatPoints[i](2) = 1;
            whatPoints[i] = trans*whatPoints[i];
        }

        trans = transform(wherePoints, whatPoints);
        cache = QImage();
    }

    update();
    event->accept();
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    mouseX = event->x();
    mouseY = event->y();
    update();
    event->accept();
}
