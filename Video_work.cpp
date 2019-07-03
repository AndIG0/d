// Video_work.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "pch.h"
#include <iostream>

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;


VideoCapture videoSource;
Mat frame; Mat srcGray; Mat Traectory;
RNG rng(12345);
#define VIDEO_PATH "ForVideoWork.mp4"

int main()
{
	//Open video
	if (!videoSource.open(VIDEO_PATH))
	{
		std::cout << "Video not found at " << VIDEO_PATH << std::endl;
		return 1;     // Exit if fail
	}
	videoSource.set(CAP_PROP_CONVERT_RGB, 1);

	int cameraWidth = videoSource.get(CAP_PROP_FRAME_WIDTH);
	int cameraHeight = videoSource.get(CAP_PROP_FRAME_HEIGHT);
	float cameraAspectRatio = cameraWidth / cameraHeight;

	std::cout << "Camera resolution: " << cameraWidth << ", " << cameraHeight << " aspect ratio: " << cameraAspectRatio << std::endl;

	

	
	while (true)
	{
		videoSource >> frame;
		if (frame.empty())
			break;
		
		cvtColor(frame, srcGray, COLOR_BGR2GRAY);
		blur(srcGray, srcGray, Size(3, 3));

		Mat srcThresh;
		double otsu;

		otsu = threshold(srcGray, srcThresh, 0, 255, THRESH_BINARY | THRESH_OTSU);

		Mat cannyOut;
		Canny(srcGray, cannyOut, otsu, otsu * 1 / 2, 3, 1);


		vector<vector<Point> > contours;
		vector<Vec4i> hierarchy;

		findContours(cannyOut, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0));


		vector<Moments> mu(contours.size());
		for (int i = 0; i < contours.size(); i++)
		{
			mu[i] = moments(contours[i], false);
		}


		vector<Point2f> mc(contours.size());
		for (int i = 0; i < contours.size(); i++)
		{
			mc[i] = Point2f(mu[i].m10 / mu[i].m00, mu[i].m01 / mu[i].m00);
		}


		Mat drawing = Mat::zeros(cannyOut.size(), CV_8UC3);
		string sObjectNumber;
		ostringstream sContourNumber;

		for (int i = 0; i < contours.size(); i++)
		{

			sContourNumber << "(" << round(mc[i].x) << ";" << round(mc[i].y) << ")";
			sObjectNumber = sContourNumber.str();
			Point pCoordinates(mc[i].x + 5, mc[i].y - 7);
			Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
			drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0, Point());
			circle(drawing, mc[i], 4, color, -1, 8, 0);
			putText(drawing, sObjectNumber, pCoordinates, FONT_HERSHEY_COMPLEX, 1, color, 1, 8);
			sContourNumber.str("");
			sContourNumber.clear();


		}


		imshow("frame", frame);
		imshow("draw", drawing);
		waitKey(3);
	}
	
	waitKey(0);
	return 0;
}



// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
