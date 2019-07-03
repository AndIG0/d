#include "pch.h"
#include <iostream>
#include <opencv2/opencv.hpp>
#define NUMBER 2

using namespace std;
using namespace cv;

Mat frame; Mat mask; Mat hsv; Mat img; Mat FragmentC;



vector<vector<Point> > contours;
vector<Vec4i> hierarchy;
vector <Mat> images(NUMBER);

int main()
{
	VideoCapture videoSource;
	if (!videoSource.open("vodeo1.mp4"))
	{
		std::cout << "Video not found" << std::endl;
		return 1;
	}

	for (int n = 0; n < NUMBER; n++) {

		string path = "";
		path += to_string(n + 1);
		path += ".png";
		img = imread(path);
		resize(img, img, Size(128, 128));
		images[n] = img;
		
	}



	Mat nothing = imread("nothing.png");



	while (1) {

		videoSource >> frame;

		if (frame.empty() || waitKey(1) == 27)
			break;

		Mat frameC = frame.clone();


		
		blur(frame, frame, Size(5, 5));

		inRange(frame, Scalar(91, 76, 74), Scalar(188, 173, 172), mask);
		
		erode(mask, mask, Mat());
		dilate(mask, mask, Mat());

		vector<Point2i> mas;
		mas.push_back(Point2i(frame.cols*(0.46), frame.rows*(0.37)));
		mas.push_back(Point2i(frame.cols*(0.6), frame.rows*(0.37)));
		mas.push_back(Point2i(frame.cols*(0.83), frame.rows*(0.66)));
		mas.push_back(Point2i(frame.cols*(0.28), frame.rows*(0.66)));

		vector<Point2i> vec;
		vec.push_back(Point2i(frame.cols*(0.483), frame.rows*(0.37)));
		vec.push_back(Point2i(frame.cols*(0.558), frame.rows*(0.37)));
		vec.push_back(Point2i(frame.cols*(0.75), frame.rows*(0.76)));
		vec.push_back(Point2i(frame.cols*(0.36), frame.rows*(0.76)));


		Point2f mas1[4];
		mas1[0] = Point2f(frame.cols*(0.483), frame.rows*(0.37));
		mas1[1] = Point2f(frame.cols*(0.558), frame.rows*(0.37));
		mas1[2] = Point2f(frame.cols*(0.81), frame.rows*(0.76));
		mas1[3] = Point2f(frame.cols*(0.3), frame.rows*(0.76));



		Point2f mas2[4];
		mas2[0] = Point2f(0, 0);
		mas2[1] = Point2f(frame.cols*(0.405), 0);
		mas2[2] = Point2f(frame.cols*(0.405), frame.rows*(0.38));
		mas2[3] = Point2f(0, frame.rows*(0.38));
		


		polylines(mask, vec, true, Scalar(255,244, 255));
		polylines(frame, mas, true, Scalar(255, 50, 255));
		polylines(frame, vec, true, Scalar(255, 50, 255));


		Mat M = getPerspectiveTransform(mas1, mas2);
		warpPerspective(mask, FragmentC, M, Size(frame.cols*(0.405), frame.rows*(0.38)));
		





		findContours(FragmentC, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_NONE, Point(0, 0));



		imshow("Fragmen", FragmentC);
		imshow("Fragmedn", mask);
		imshow("framesef", frame);


		int max[5] = { 0,0,0,0,0 };

		for (int i = 0; i < contours.size(); i++) {




			if (contourArea(contours[i]) > max[0]) {

				max[4] = max[3];
				max[3] = max[2];
				max[2] = max[1];
				max[1] = max[0];
				max[0] = i;


			}
			else if (contourArea(contours[i]) > max[1]) {

				max[4] = max[3];
				max[3] = max[2];
				max[2] = max[1];
				max[1] = i;

			}
			else if (contourArea(contours[i]) > max[2]) {


				max[4] = max[3];
				max[3] = max[2];
				max[2] = i;

			}
			else if (contourArea(contours[i]) > max[3]) {
				max[4] = max[3];
				max[3] = i;
			}
			else if (contourArea(contours[i]) > max[4]) {

				max[4] = i;
			}


		}
		int stop = 0;
		if (contours.size() > 5) {

			stop = 5;

		}
		else {

			stop = contours.size();
		}

		for (int id = 0; id < stop; id++) {
			
			if (contours.size() != 0) {
				vector<Rect> boundRect(contours.size());
				boundRect[max[id]] = boundingRect(Mat(contours[max[id]]));

				

				Mat Fragment;
				Rect r(boundRect[max[id]].x, boundRect[max[id]].y, boundRect[max[id]].width, boundRect[max[id]].height);
				//rectangle(FragmentC, boundRect[max[id]], Scalar(0, 255, 255), 2, 8, 0);

				FragmentC(r).copyTo(Fragment);



				if (Fragment.cols > 40 && Fragment.rows > 40) {
					resize(Fragment, Fragment, Size(128, 128));





					for (int n = 0; n < NUMBER; n++) {


						img = images[n];

						int  a[NUMBER];
						a[n] = 0;




						for (int m = 0; m < 128; m++) {

							for (int h = 0; h < 128; h++) {

								if (Fragment.at<uchar>(Point(m, h)) == img.at<uchar>(Point(m, h)))
									a[n]++;

								

							}


						}

						int sovp = 0;
						if (a[0] > 10000 && boundRect[max[id]].x >50 && boundRect[max[id]].x+ boundRect[max[id]].width < FragmentC.cols - 50 && a[0]>a[1]) {
							
							sovp = 1;
							imshow("Fragment", images[0]);

						}
						if (a[1] > 9000 && a[1] > a[0] && ((a[1]-9000)/9 >(a[0]-10000)/10 || a[1] > a[0])) {
							
							
							imshow("Fragment", images[1]);
							

							sovp = 1;
						}
						
						if (sovp != 1) {

							imshow("Fragment",nothing);
						}
					}

				}
			}
		}




		imshow("frame3", frameC);


		waitKey(1);




	}



}

