#include <stdio.h>
#include "Serial.h"
#include <Windows.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <time.h>
#include <opencv2\highgui\highgui.hpp>
#include<opencv2\core\core.hpp>
#include<opencv2\imgproc\imgproc.hpp>

using namespace cv;
using namespace std;

Mat mat_frame;
void put_string(Mat &frame, string text, Point pt, int value) {
	text += to_string(value);
	Point shade = pt + Point(2, 2);
	int font = FONT_HERSHEY_SIMPLEX;
	putText(frame, text, pt, font, 0.7, Scalar(120, 200, 90), 2);  //작성문자
}
int main()
{
	VideoCapture capture(0);
	//capture.open("../image/4x.mp4");

	//VideoCapture capture;
	//capture.open("../image/together.mp4");
	if (!capture.isOpened()) {
		cout << "카메라가 연결되지 않았습니다." << endl;
		exit(1);
	}
	float radius;   //원 식별하기위함

	CSerial serial;


	// STEP 1. SerialPort Connect
	if (!serial.Open())
	{
		printf("connect faliled");
		return -1;
	}
	else
		printf("connect successed\n");

	// 데이터 전송
	char *at = "AT+CONA810871B7E0E";

	if (!serial.SendData(at, 18)) {
		printf("send fail\n");
		return -1;
	}
	else
		printf("send successed\n");

	/*char *at3 = "A";

	if (!serial.SendData(at3, 1)) {
	printf("send fail\n");
	return -1;
	}
	else if (serial.SendData(at3, 1)) {
	printf("ok\n");
	}
	else
	printf("send successed2\n");
	*/
	/*
	char *at2 =NULL;
	if (serial.ReadData(at2,2)) {
	printf("receive");
	}*/

	//(!serial.SendData(at2, 1))
	serial.ReadDataWaiting();
	Sleep(1000);


	//데이터 리드 

	void *buffer = 0;
	int curT = 0, oldT = 0; //, dispType =0;
							//while (1)
							//{
	curT = GetTickCount();

	if (curT - oldT > 10)
	{
		char buffer[256];
		int nBytesRead = serial.ReadData(buffer, sizeof(buffer));

		if (nBytesRead > 0)
		{
			for (int i = 0; i < nBytesRead; ++i)
			{
				printf("%c", buffer[i]);
				/*
				switch (dispType)
				{
				case 0: printf("%c", buffer[i]); break;
				case 1: printf("%d", buffer[i]); break;
				case 2: printf("%x", buffer[i]); break;
				}
				*/
			}
			printf("\n");
		}

		oldT = curT;
	}
	char *at3 = "Q\n";

	if (!serial.SendData(at3, 2)) {
		printf("send fail\n");
		return -1;
	}
	else
		printf("send successed2\n");

	bool check = true;
	HOGDescriptor hog, hog_b;   //hog 기술자와 검출기를 기본 매개 변수로 만듦
	hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());
	hog_b.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());
	//setSVMDetector(): 선형 SVM분류 자에 대한 계수 설정
	//hogdescriptor::getdefaultPeopleDectector(); 사람 감지용으로 훈련 된 분류 자의 계수 반환

	clock_t begin = clock();
	clock_t end;

	int ju = 0;
	double result = 0;
	double result2 = 0;
	double final;

	for (;;) {
		capture >> mat_frame;
		if (mat_frame.empty())
			continue;


		//=======================선긋기============================//
		Scalar red(0, 0, 255);
		Point pt1(0, 300), pt2(1000, 300), pt3(0, 200), pt4(1000, 200);

		//중간지점을 나타내는 선긋기
		line(mat_frame, pt3, pt4, red, 4);
		line(mat_frame, pt1, pt2, red, 4);
		//=======================선긋기============================//


		vector<Rect> found, found_filtered, found1, found1_filtered;
		hog.detectMultiScale(mat_frame, found, 0, Size(8, 8), Size(32, 32), 1.05, 2);//사람 추출
		hog_b.detectMultiScale(mat_frame, found1, 0, Size(8, 8), Size(16, 16), 0.05, 1);//사람

		//defectMultiScale(): 입력이미지에서 서로 다른 크기의 객체를 감지. 감지 된 객체는 사각형으로 반환
		//image, object, scaleFactor, minNeighbors, flags, minsize, maxsize

		size_t i;
		for (i = 0; i<found.size(); i++)
		{
			Rect r = found[i];
			Rect r1 = found[i];
			size_t j;
			//더 큰 탐지 내부에 작은 탐지를 추가하지 말아야 함
			for (j = 0; j<found.size(); j++)
				if (j != i && (r & found[j]) == r)
					break;
			if (j == found.size())
				found_filtered.push_back(r);
		}

		for (i = 0; i<found_filtered.size(); i++)
		{
			Rect r = found_filtered[i];
			Rect r1 = found_filtered[i];
			// hog검출기는 실제 객체보다 약간 큰 직사각형을 반환하지만 직사각형을 약간 축소시켜 근사값을 얻는다
			//사각형의 좌표값을 구하고 그것의 중점 하나를 계산 그 후 그 점이 어느 위치 이상이면 +표시 및 위치정보 변환 알려줌         

			r.x += cvRound(r.width*0.1);   //cvRound: 부동 소수점 숫자를 가장 가까운 정수로 반올림
			r.width = cvRound(r.width*0.8);
			r.y += cvRound(r.height*0.07);
			r.height = cvRound(r.height*0.8);

			rectangle(mat_frame, r.tl(), r.br(), Scalar(255, 0, 0), 2);   //tl: 오른쪽 상단 br: 오른쪽 하단
			rectangle(mat_frame, r1.tl(), r1.br(), Scalar(0, 255, 0), 2);   //tl: 오른쪽 상단 br: 오른쪽 하단

			
			int x = r.x;
			int y = r.y + r.height;   //보행자의 발이 특정 위치에 도달했을 때 시간 측정시작

									  //2차원 배열을 사용하여 한사람씩 보행자의 위치 저장
			int array[100][2];//100명으로 초기화
			for (int i = 0; i < 100; i++) {
				array[i][0] = x;
				array[i][1] = y;

				//보행자 위치 출력
				put_string(mat_frame, "(x): ", Point(10, 40), array[i][0]);
				put_string(mat_frame, "(y): ", Point(10, 60), array[i][1]);

				//시간 카운트
				if (array[i][1] >= 200 && array[i][1] <= 300) //중간 위치일 경우
				{
					if (check) {
						//가장 먼저 해당영역에 도착했을 경우의 위치 값
						end = clock();
						result = (double)(begin - end) / CLOCKS_PER_SEC;
						//cout << "수행시간(초) : " << result << endl;
						check = false;
						double first_clock = fabs(result);
						cout << "초반부 경과(초) : " << first_clock << endl;
					}
					begin = clock();

					//최종적으로 사람이 나갔을 때의 시간 
					result2 = (double)(begin - end) / CLOCKS_PER_SEC;
					cout << "수행시간(초) : " << result2 << endl;
					break;
					//result2값이 바로 중간 위치에 있는 초, 최대 2번까지만 시간을 늘려줄것
				}
				begin = clock();
				end = clock();
			}

			if (result2 > 3 && ju <50) {
				cout << ju << endl;
				char *at2 = "F\n";
				//char *at0 = "\n";

				ju++;
				if (ju == 2) {
					if (!serial.SendData(at2, 2)) {
						printf("send fail\n");
						return -1;
					}
					else {
						printf("send successed(1)\n");
					}

					serial.ReadDataWaiting();
					//Sleep(8000);
					break;

				}
				else if (ju == 100) {
					char *at5 = "F\n";
					if (!serial.SendData(at5, 2)) {
						printf("send fail\n");
						return -1;
					}
					else {
						printf("send successed(2)\n");
					}

					serial.ReadDataWaiting();
					//Sleep(8000);
					break;
				}

				//delay 걸어주기
			}

		}

		for (i = 0; i<found1.size(); i++)
		{
			Rect r1 = found1[i];
			size_t j;
			//더 큰 탐지 내부에 작은 탐지를 추가하지 말아야 함
			for (j = 0; j<found1.size(); j++)
				if (j != i && (r1 & found1[j]) == r1)
					break;
			if (j == found1.size())
				found1_filtered.push_back(r1);
		}

		for (i = 0; i<found1_filtered.size(); i++)
		{
			Rect r1 = found1_filtered[i];
			// hog검출기는 실제 객체보다 약간 큰 직사각형을 반환하지만 직사각형을 약간 축소시켜 근사값을 얻는다
			//사각형의 좌표값을 구하고 그것의 중점 하나를 계산 그 후 그 점이 어느 위치 이상이면 +표시 및 위치정보 변환 알려줌         

			r1.x += cvRound(r1.width*0.1);   //cvRound: 부동 소수점 숫자를 가장 가까운 정수로 반올림
			r1.width = cvRound(r1.width*0.8);
			r1.y += cvRound(r1.height*0.07);
			r1.height = cvRound(r1.height*0.8);

			rectangle(mat_frame, r1.tl(), r1.br(), Scalar(0, 255, 0), 3);   //tl: 오른쪽 상단 br: 오른쪽 하단

			int x = r1.x;
			int y = r1.y + r1.height;   //보행자의 발이 특정 위치에 도달했을 때 시간 측정시작
									  //2차원 배열을 사용하여 한사람씩 보행자의 위치 저장
			//int array[100][2];//100명으로 초기화
			/*
			for (int i = 0; i < 100; i++) {
				array[i][0] = x;
				array[i][1] = y;
			}*/
			/*
			if (result2 > 3 && ju <50) {
				cout << ju << endl;
				char *at2 = "F\n";
				//char *at0 = "\n";

				ju++;
				if (ju == 2) {
					if (!serial.SendData(at2, 2)) {
						printf("send fail\n");
						return -1;
					}
					else {
						printf("send successed(1)\n");
					}

					serial.ReadDataWaiting();
					//Sleep(8000);
					break;

				}
				else if (ju == 100) {
					char *at5 = "F\n";
					if (!serial.SendData(at5, 2)) {
						printf("send fail\n");
						return -1;
					}
					else {
						printf("send successed(2)\n");
					}

					serial.ReadDataWaiting();
					//Sleep(8000);
					break;
				}

				//delay 걸어주기
			}
			*/
		}
	
		/*
		//원
		Mat gray_image;
		cvtColor(mat_frame, gray_image, COLOR_BGR2GRAY, 0);

		Mat binary_image;
		threshold(gray_image, binary_image, 200, 255, THRESH_BINARY_INV | THRESH_OTSU);

		Mat canny_image;
		blur(gray_image, canny_image, Size(3, 3));
		Canny(canny_image, canny_image, 125, 125 * 3, 3);

		vector<Vec3f> circles;
		HoughCircles(canny_image, circles, CV_HOUGH_GRADIENT, 2, canny_image.rows / 4, 50, 150);

		Mat circle_image = mat_frame.clone();
		for (size_t i = 0; i < circles.size(); i++) {
			Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
			int radius = cvRound(circles[i][2]);
			circle(circle_image, center, 3, Scalar(0, 0), -1, 8, 0);
			circle(circle_image, center, radius, Scalar(0, 0), -1, 8, 0);
		}*/

		//imshow("Circle", circle_image);
		imshow("source", mat_frame);

		if (waitKey(30) >= 0) {
			break;
		}
	}

	return 0;

}