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
#include <QFont>

#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>


#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <opencv2/objdetect/objdetect.hpp>


QImage MainWindow::getQImageFromFrame(cv::Mat frame) {
//converts the color model of the image from RGB to BGR because OpenCV uses BGR
cv::cvtColor(frame, frame, CV_RGB2BGR);
return QImage(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
}

void MainWindow::faceDetect(cv::Mat frame) {
std::vector<cv::Rect> faces;
this->counter++;
ui->Timer->setText(QString::number(secs_counter));
face_cascade.detectMultiScale(frame, faces, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, cv::Size(30, 30));
int act_size = int(faces.size());
cv::Mat curr_face;
QImage face;
// this line corresponds to the extraction of the frame number. It gets Unix timestamp since 1970/01/01 in msecs
// and exctracts last 5 numbers as a timestamp (or a consecutive part of a name) of an image capture.
// this allows to have consecutively marked photos during 1 min of observation
double offset = (double)(((long long int) QDateTime::currentMSecsSinceEpoch()) % 100000);
QString im_name = "D:/c++/testOpenCV/data/image " + QString::number(offset) + ".jpg";
for (int i = 0; i < act_size; i++)
 {
 //cv::Point center(faces[i].x + faces[i].width*0.5, faces[i].y + faces[i].height*0.5); // for Ellipse drawing
 //cv::ellipse(frame, center, cv::Size(faces[i].width*0.5, faces[i].height*0.5), 0, 0, 360, cv::Scalar(255, 17, 0), 4, 8, 0); // for Ellipse drawing
 cv::rectangle(frame, faces[i], cv::Scalar(255,17,0));
 if (i==0) {
    curr_face = frame(faces[i]);
    face = getQImageFromFrame(curr_face);
    ui->face->setPixmap(QPixmap::fromImage(face));
    cv::cvtColor(curr_face, curr_face, CV_RGB2BGR);
    //call for cnn
    if (this->counter==25) {
        // adding saving to file
        // change PATH in im_name up to the word "/data" to operate on your PC.
        // You are able to choose the capture folder here
    this->counter = 0;
    cv::cvtColor(curr_face, curr_face, CV_RGB2GRAY);
    cv::imwrite(cv::String(im_name.toStdString()), curr_face);
    cmd->start(program,QStringList()<< script << im_name);
    cmd->waitForFinished();
    qDebug() << cmd->readAllStandardOutput();
    }
 }
 }
}

void MainWindow::BCIData() {
    std::string fileType = "*.csv";
    //Change this path for the one with the CSV file
    std::string pathFile = "D://c++/testOpenCV/";
    std::string fileName;
    WIN32_FIND_DATAA file;
    HANDLE search_handle = FindFirstFileA((pathFile+fileType).c_str(),&file);
    if (search_handle) {
        do {
        fileName = file.cFileName;
        } while (FindNextFileA(search_handle, &file));
        FindClose(search_handle);
    }
    //The last file will be extracted.
    //Parse .csv file for data.
    std::vector<std::vector<double>> csvData;
    std::ifstream csvfile(pathFile+fileName);
    std::string line;
    int l=0;
    while(getline(csvfile,line)) {
        if (!getline(csvfile, line)) break;
        std::istringstream ss(line);
        std::vector<double> record;
        while(ss) {
           std::string elems;
           if(!getline(ss,elems,',')) break;
           double value;
           try {
           value = std::stof(elems);
           }
           catch (const std::invalid_argument e) {}
           if (value < 1000) record.push_back(value);
        }
        if (!record.empty() && record.size()==31) {
        csvData.push_back(record);
        l++;
        }
        //filtered empty lines
        if (!record.empty() && record.size()==30) {
          double inv = record[record.size()-1];
          record.push_back(inv);
          csvData.push_back(record);
          l++;
        }
    }
    if (!csvfile.eof()) {
        std::cerr << "file not found" << std::endl;
    }

     //Build graph
     QVector <double> x(l), ltexc(l), eng(l), exc(l), stre(l), rel(l), inter(l), foc(l);

     //Performance Metrics appear every 10 seconds. We will exploit this fact.
     //building x axis and extracting y axis for LT Excitement
     //also finding out min and max of performance metrics
     double max = csvData[0][10], min = csvData[0][10];
     //Metrics for showing:
     // Engagement Scaled
     // Excitement Scaled
     // Long-term Excitement
     // Stress Scaled
     // Relaxation Scaled
     // Interest Scaled
     // Focus Scaled
     for (int i=0; i<l;i++) {
         x[i] = (i+1)*10;
         eng[i] = csvData[i][1];
         if (eng[i]>max) max = eng[i];
         if (eng[i]<min) min = eng[i];
         exc[i] = csvData[i][6];
         if (exc[i]>max) max = exc[i];
         if (exc[i]<min) min = exc[i];
         ltexc[i] = csvData[i][10];
         if (ltexc[i]>max) max = ltexc[i];
         if (ltexc[i]<min) min = ltexc[i];
         stre[i] = csvData[i][12];
         if (stre[i]>max) max = stre[i];
         if (stre[i]<min) min = stre[i];
         rel[i] = csvData[i][17];
         if (rel[i]>max) max = rel[i];
         if (rel[i]<min) min = rel[i];
         inter[i] = csvData[i][22];
         if (inter[i]>max) max = inter[i];
         if (inter[i]<min) min = inter[i];
         foc[i] = csvData[i][27];
         if (foc[i]>max) max = foc[i];
         if (foc[i]<min) min = foc[i];
     }
    // now build the graph
     ui->Graph->clearGraphs();
     ui->Graph->legend->setVisible(true);
     ui->Graph->legend->setFont(QFont("Times New Roman",14));
     ui->Graph->addGraph();
     ui->Graph->addGraph();
     ui->Graph->addGraph();
     ui->Graph->addGraph();
     ui->Graph->addGraph();
     ui->Graph->addGraph();
     ui->Graph->addGraph();
     ui->Graph->graph(0)->setData(x,eng);
     ui->Graph->graph(0)->setPen(QPen(Qt::red));
     ui->Graph->graph(0)->setName("Engagement Scaled");
     ui->Graph->graph(1)->setData(x,exc);
     ui->Graph->graph(1)->setPen(QPen(Qt::magenta));
     ui->Graph->graph(1)->setName("Excitement Scaled");
     ui->Graph->graph(2)->setData(x,ltexc);
     ui->Graph->graph(2)->setPen(QPen(Qt::darkMagenta));
     ui->Graph->graph(2)->setName("Long-term Excitement");
     ui->Graph->graph(3)->setData(x,stre);
     ui->Graph->graph(3)->setPen(QPen(Qt::blue));
     ui->Graph->graph(3)->setName("Stress Scaled");
     ui->Graph->graph(4)->setData(x,rel);
     ui->Graph->graph(4)->setPen(QPen(Qt::green));
     ui->Graph->graph(4)->setName("Relaxation Scaled");
     ui->Graph->graph(5)->setData(x,inter);
     ui->Graph->graph(5)->setPen(QPen(Qt::darkYellow));
     ui->Graph->graph(5)->setName("Interest Scaled");
     ui->Graph->graph(6)->setData(x,foc);
     ui->Graph->graph(6)->setPen(QPen(Qt::darkBlue));
     ui->Graph->graph(6)->setName("Focus Scaled");
     QFont serifFont("Times",10);
     ui->Graph->xAxis->setLabel("sec");
     ui->Graph->xAxis->setLabelFont(serifFont);
     ui->Graph->yAxis->setLabel("PerformanceMetrics");
     ui->Graph->yAxis->setLabelFont(serifFont);
     ui->Graph->xAxis->setRange(0,l*10);
     ui->Graph->yAxis->setRange(min, max);

     ui->Graph->replot();

}

void MainWindow::displayFrame() {
//capture a frame from the webcam
if (webcam.isOpened()) {

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

void MainWindow::changeTiming() {
    this->secs_counter++;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    face_cascade.load("D:/c++/testOpenCV/haarcascade_frontalface_alt2.xml"); // change this PATH to operate on your PC.
    connect (ui->startButton, SIGNAL(released()), this, SLOT(Operate()));
    connect (ui->displayButton, SIGNAL(released()), this, SLOT(BCIData()));
    this -> cmd = new QProcess();
    this -> program = "python";
    this -> script = "D:/c++/testOpenCV/a.py";
    this -> counter = 0;
    this -> secs_counter = 0;
    ui->debug->setText("Face detected (1st entry)");
}

void MainWindow::Operate() {
    if (webcam.isOpened()) {
        ui->startButton->setText("Start video capture");
        ui->displayButton->setEnabled(true);
        webcam.release();
        return;
    }
    if (!webcam.open(0)) {
        QMessageBox::critical(this, "Camera error", "Make sure camera is working and connected", "<br>or no accessed by anybody else");
        return;
    }
    ui->startButton->setText("Stop capture");
    ui->displayButton->setEnabled(false);
    webcam.set(CV_CAP_PROP_FRAME_WIDTH, 640);
    webcam.set(CV_CAP_PROP_FRAME_HEIGHT, 640);
    int fps = 1000/25;
    QTimer* qTimer = new QTimer(this);
    QTimer* timerTimer = new QTimer(this);
    qTimer->setInterval(fps);
    timerTimer->setInterval(1000);
    qTimer->start();
    timerTimer->start();
    connect(qTimer, SIGNAL(timeout()), this, SLOT(displayFrame()));
    connect(timerTimer, SIGNAL(timeout()), this, SLOT(changeTiming()));
}

MainWindow::~MainWindow()
{
    webcam.release();
    delete ui;
}
