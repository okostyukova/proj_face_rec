TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

unix {
   CONFIG += link_pkgconfig
   PKGCONFIG += opencv
}

INCLUDEPATH += /home/olga/opencv/include/

LIBS += -L/home/olga/opencv/build -lopencv_core -lopencv_imgproc -lopencv_imgcodecs -lopencv_highgui -lopencv_objdetect

SOURCES += main.cpp
