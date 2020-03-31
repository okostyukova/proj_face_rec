#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTimer>
#include <QMessageBox>
#include <QString>
#include <QDateTime>
#include <QProcess>
#include <QStringList>
#include <QDebug>
#include <QByteArray>
#include <QtCore/QtCore>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>

QImage MainWindow::getQImageFromFrame(cv::Mat frame) {
//converts the color model of the image from RGB to BGR because OpenCV uses BGR
cv::cvtColor(frame, frame, CV_RGB2BGR);
return QImage(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
}

void MainWindow::faceDetect(cv::Mat frame) {
// face detection algorithm by @okostyukova added
std::vector<cv::Rect> faces;
face_cascade.detectMultiScale(frame, faces, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, cv::Size(30, 30));
int act_size = int(faces.size());
cv::Mat curr_face;
QImage face;
for (int i = 0, j = 0; i < act_size; i++, j++)
{ cv::rectangle(frame, faces[i], cv::Scalar(255,17,0));
  if (i == 0) {
    // face object can later be propagated to the NN and used there. Make a return from the current function and make it not void, but QImage.
    // also possible to be done with a vector to operate on
    // we show only face 0
    curr_face = frame(faces[i]);
    face = getQImageFromFrame(curr_face);
    ui->face->setPixmap(QPixmap::fromImage(face));
    //double conversion of the face back
    cv::cvtColor(curr_face, curr_face, CV_RGB2BGR);
    // adding saving to file
    // change PATH in im_name up to the word "/data" to operate on your PC.
    // You are able to choose the capture folder here
    cv::imwrite(cv::String(im_name.toStdString()), curr_face);

    //call for cnn
    cmd->start(program,QStringList()<< script << im_name);
    cmd->waitForFinished();
    qDebug() << cmd->readAllStandardOutput();

    //save image for cnn
    QString filename = "image" + j;
    face.save(filename, "JPG", -1);
 }
 }
}

void MainWindow::displayFrame() {
//capture a frame from the webcam and display it into the GUI
if (webcam.isOpened()) {

ui->debug->setText("Face detected (1st entry)");
cv::Mat frame;
webcam.grab();
webcam.read(frame);
faceDetect(frame);
QImage image = getQImageFromFrame(frame);

//set the image of the label to be the captured frame and resize the label appropriately

ui->label->setPixmap(QPixmap::fromImage(image));
//ui->label->resize(ui->label->pixmap()->size());
}
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
    // init of the main GUI
{
    ui->setupUi(this);
    // face detection algorithm initialized
    face_cascade.load("/home/olga/opencv/data/haarcascades/haarcascade_frontalface_alt2.xml"); // change this PATH to operate on your PC.
    connect (ui->startButton, SIGNAL(released()), this, SLOT(Operate()));
    this -> cmd = new QProcess();
    this -> program = "python";
    this -> script = "D:/c++/testOpenCV/a.py";
}

void MainWindow::Operate() {
    // On-button-pressed actions. Closing webcam if open and opening it for capture.
    // a timing method by FPS rate is used to trigger face detection and video grab
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
    int fps = 1000/1;
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
