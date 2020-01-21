#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTimer>
#include <QMessageBox>


#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

QImage MainWindow::getQImageFromFrame(cv::Mat frame) {
//converts the color model of the image from RGB to BGR because OpenCV uses BGR
cv::cvtColor(frame, frame, CV_RGB2BGR);
return QImage(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
}

void MainWindow::displayFrame() {
//capture a frame from the webcam
if (webcam.isOpened()) {

ui->debug->setText("Frame ready to catch");
cv::Mat frame;
webcam.grab();
webcam.read(frame);
QImage image = getQImageFromFrame(frame);

//set the image of the label to be the captured frame and resize the label appropriately

ui->label->setPixmap(QPixmap::fromImage(image));
//ui->label->resize(ui->label->pixmap()->size());
}
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect (ui->startButton, SIGNAL(released()), this, SLOT(Operate()));
}

void MainWindow::Operate() {
    if (webcam.isOpened()) {
        ui->startButton->setText("Start video capture");
        webcam.release();
        return;
    }
    if (!webcam.open(0)) {
        QMessageBox::critical(this, "Camera error", "Make sure camera is working and connected", "<br>or no accessed by anybody else");
        return;
    }
    ui->startButton->setText("Stop capture");
    webcam.set(CV_CAP_PROP_FRAME_WIDTH, 640);
    webcam.set(CV_CAP_PROP_FRAME_HEIGHT, 640);
    int fps = 1000/25;
    QTimer* qTimer = new QTimer(this);
    qTimer->setInterval(fps);
    qTimer->start();
    connect(qTimer, SIGNAL(timeout()), this, SLOT(displayFrame()));
}

MainWindow::~MainWindow()
{
    webcam.release();
    delete ui;
}
