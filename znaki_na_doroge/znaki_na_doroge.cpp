#include "pch.h"
#include <iostream>
#include <opencv2/opencv.hpp>
#define NUMBER 2

using namespace std;
using namespace cv;

vector <Mat> images(NUMBER);
Mat nothing;
Mat perehod;

//********************************************ПОЛУЧАЕМ МАССИВ ЭТАЛОННЫХ ИЗОБРАЖЕНИЙ С КОТОРЫМИ ПОТОМ БУДЕМ СРАВНИВАТЬ*********************
void get_etalone_images() {

	for (int n = 0; n < NUMBER; n++) {

		string path = "";
		path += to_string(n + 1);
		path += ".png";
		Mat img = imread(path);
		resize(img, img, Size(128, 128));
		images[n] = img;

	}

	nothing = imread("nothing.png");
	perehod = imread("perehod.png");

}




//***********************************************ВЫВОДИМ ФРЕЙМ**************************************************

void show_frame(Mat frame_c) {

	
		imshow("Video", frame_c);

		waitKey(0);
	

}



//************************************************ПОЛУЧАЕМ МАСКУ***************************

Mat get_mask(Mat frame) {


	Mat mask;

	blur(frame, frame, Size(5, 5));

	inRange(frame, Scalar(91, 76, 74), Scalar(188, 173, 172), mask);

	erode(mask, mask, Mat());
	dilate(mask, mask, Mat());


	return mask;
}


//**************************************РАСПОЗНАЕМ КАКОЙ ЖЕ ВСЕ ТАКИ ЗНАК ЕСЛИ ЭТО ЗНАК**************************************

void describe_znak(Mat Fragment,double min_x,double max_x, double max_val) {

	for (int n = 0; n < NUMBER; n++) {
		
		Mat img = images[n];

		int  a[NUMBER];
		a[n] = 0;
	   

		for (int m = 0; m < 128; m++) {

			for (int h = 0; h < 128; h++) {

				if (Fragment.at<uchar>(Point(m, h)) == img.at<uchar>(Point(m, h)))
					a[n]++;

			}


		}

		int sovp = 0;
		if (a[0] > 10000 && min_x > 50 && max_x < max_val && a[0]>a[1]) {

			sovp = 1;
			imshow("Fragment", images[0]);

		}

		if (a[1] > 9000 && a[1] > a[0] && ((a[1] - 9000) / 9 > (a[0] - 10000) / 10 || a[1] > a[0])) {


			imshow("Fragment", images[1]);


			sovp = 1;
		}

		if (sovp != 1) {

			imshow("Fragment", nothing);
		}
	}
}

//**************************ИЩЕМ ЧТО_ТО ПОХОЖЕЕ НА ЗНАК В 5ТИ САМЫХ БОЛЬШИХ КОНТУРАХ*************************************

void find_znak(vector<vector<Point>> contours, Mat  FragmentC, int * max) {

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
			FragmentC(r).copyTo(Fragment);



			if (Fragment.cols > 40 && Fragment.rows > 40) {
				
				resize(Fragment, Fragment, Size(128, 128));
					   
				describe_znak(Fragment, boundRect[max[id]].x, boundRect[max[id]].x + boundRect[max[id]].width, FragmentC.cols - 50);
							   
			}
		}
	}

}


//********************************************НАХОДИМ ТЕ САМЫЕ 5 САМЫХ БОЛЬШИХ КОНТУРОВ***************************************

void find_five_max_contours(vector < vector<Point>> contours, Mat FragmentC) {

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

	find_znak(contours, FragmentC, max);

}

void find_perehod(Mat FragmentC) {

	
	Mat yr; 
	int a = 0;

	Rect r(0, FragmentC.rows*(0.79), FragmentC.cols, FragmentC.rows*(0.2));
	FragmentC(r).copyTo(yr);

	
	
	resize(yr, yr, Size(256, 128));

	for (int m = 0; m < yr.cols; m++) {

		for (int h = 0; h < yr.rows; h++) {

			if (yr.at<uchar>(Point(m, h)) == perehod.at<uchar>(Point(m, h)))
				a++;

		}


	}
		

	if (a > 11500 && a < 12600) {


		imshow("Fragment2", perehod);
		
		
	}
	else {

		imshow("Fragment2", nothing);
	}
	cout << a <<" ";
}

//***************ИСПОЛЬЗУЕМ ВСЕ ЧТО ВЫШЕ***************************

void obrabotka(Mat frame) {




		Mat FragmentC;
		vector<vector<Point> > contours;
		vector<Vec4i> hierarchy;


		Point2f mas[2][4];
		mas[0][0] = Point2f(frame.cols*(0.483), frame.rows*(0.37));
		mas[0][1] = Point2f(frame.cols*(0.558), frame.rows*(0.37));
		mas[0][2] = Point2f(frame.cols*(0.81), frame.rows*(0.76));
		mas[0][3] = Point2f(frame.cols*(0.3), frame.rows*(0.76));

		mas[1][0] = Point2f(0, 0);
		mas[1][1] = Point2f(frame.cols*(0.405), 0);
		mas[1][2] = Point2f(frame.cols*(0.405), frame.rows*(0.38));
		mas[1][3] = Point2f(0, frame.rows*(0.38));


		Mat frameC = frame.clone();


		Mat mask = get_mask(frame);


		Mat M = getPerspectiveTransform(mas[0], mas[1]);
		warpPerspective(mask, FragmentC, M, Size(frame.cols*(0.405), frame.rows*(0.38)));
		findContours(FragmentC, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_NONE, Point(0, 0));


		Rect r(0, FragmentC.rows*(0.79), FragmentC.cols, FragmentC.rows*(0.2));
		rectangle(FragmentC, r, Scalar(255, 255, 255), 2, 8, 0);

		imshow("sdgs", FragmentC);

		
		find_five_max_contours(contours, FragmentC);
		find_perehod(FragmentC);


	
}
int main()
{

	VideoCapture videoSource;

	VideoCapture videoSource2 = videoSource;

	string v_name;

	cout << "Input video's name" << endl;
	cin >> v_name;

	if (!videoSource.open(v_name))
	{
		std::cout << "Video not found" << std::endl;

	}

	while (1) {

		Mat frame;

		
		videoSource >> frame;


		if (frame.empty() || waitKey(1) == 27)
			break;


		Mat fc = frame.clone();


		get_etalone_images();


		obrabotka(frame);
		show_frame(fc);


	}
}

