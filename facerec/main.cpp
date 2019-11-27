#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>

using namespace std;
using namespace cv;

int main()
{
 Mat image;
 image = imread("/home/olga/facerec/lena.jpg", CV_LOAD_IMAGE_COLOR);
 namedWindow("window1", 1); imshow("window1", image);

// Load Face cascade (.xml file)
 CascadeClassifier face_cascade;
 face_cascade.load("/home/olga/facerec/haarcascade_frontalface_alt2.xml");

// Detect faces
 std::vector<Rect> faces;
//Detects objects of different sizes in the input image.
//The detected objects are returned as a list of rectangles.
 face_cascade.detectMultiScale(image, faces, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, Size(30, 30));

// Draw circles on the detected faces
 for (int i = 0; i < faces.size(); i++)
 {
 Point center(faces[i].x + faces[i].width*0.5, faces[i].y + faces[i].height*0.5);
 ellipse(image, center, Size(faces[i].width*0.5, faces[i].height*0.5), 0, 0, 360, Scalar(255, 17, 0), 4, 8, 0);
 }
 imshow("Detected Face", image);

 waitKey(0);
 return 0;
}
