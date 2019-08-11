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
	putText(frame, text, pt, font, 0.7, Scalar(120, 200, 90), 2);  //�ۼ�����
}
int main()
{
	VideoCapture capture(0);
	//capture.open("../image/4x.mp4");

	//VideoCapture capture;
	//capture.open("../image/together.mp4");
	if (!capture.isOpened()) {
		cout << "ī�޶� ������� �ʾҽ��ϴ�." << endl;
		exit(1);
	}
	float radius;   //�� �ĺ��ϱ�����

	CSerial serial;


	// STEP 1. SerialPort Connect
	if (!serial.Open())
	{
		printf("connect faliled");
		return -1;
	}
	else
		printf("connect successed\n");

	// ������ ����
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


	//������ ���� 

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
	HOGDescriptor hog, hog_b;   //hog ����ڿ� ����⸦ �⺻ �Ű� ������ ����
	hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());
	hog_b.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());
	//setSVMDetector(): ���� SVM�з� �ڿ� ���� ��� ����
	//hogdescriptor::getdefaultPeopleDectector(); ��� ���������� �Ʒ� �� �з� ���� ��� ��ȯ

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


		//=======================���߱�============================//
		Scalar red(0, 0, 255);
		Point pt1(0, 300), pt2(1000, 300), pt3(0, 200), pt4(1000, 200);

		//�߰������� ��Ÿ���� ���߱�
		line(mat_frame, pt3, pt4, red, 4);
		line(mat_frame, pt1, pt2, red, 4);
		//=======================���߱�============================//


		vector<Rect> found, found_filtered, found1, found1_filtered;
		hog.detectMultiScale(mat_frame, found, 0, Size(8, 8), Size(32, 32), 1.05, 2);//��� ����
		hog_b.detectMultiScale(mat_frame, found1, 0, Size(8, 8), Size(16, 16), 0.05, 1);//���

		//defectMultiScale(): �Է��̹������� ���� �ٸ� ũ���� ��ü�� ����. ���� �� ��ü�� �簢������ ��ȯ
		//image, object, scaleFactor, minNeighbors, flags, minsize, maxsize

		size_t i;
		for (i = 0; i<found.size(); i++)
		{
			Rect r = found[i];
			Rect r1 = found[i];
			size_t j;
			//�� ū Ž�� ���ο� ���� Ž���� �߰����� ���ƾ� ��
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
			// hog������ ���� ��ü���� �ణ ū ���簢���� ��ȯ������ ���簢���� �ణ ��ҽ��� �ٻ簪�� ��´�
			//�簢���� ��ǥ���� ���ϰ� �װ��� ���� �ϳ��� ��� �� �� �� ���� ��� ��ġ �̻��̸� +ǥ�� �� ��ġ���� ��ȯ �˷���         

			r.x += cvRound(r.width*0.1);   //cvRound: �ε� �Ҽ��� ���ڸ� ���� ����� ������ �ݿø�
			r.width = cvRound(r.width*0.8);
			r.y += cvRound(r.height*0.07);
			r.height = cvRound(r.height*0.8);

			rectangle(mat_frame, r.tl(), r.br(), Scalar(255, 0, 0), 2);   //tl: ������ ��� br: ������ �ϴ�
			rectangle(mat_frame, r1.tl(), r1.br(), Scalar(0, 255, 0), 2);   //tl: ������ ��� br: ������ �ϴ�

			
			int x = r.x;
			int y = r.y + r.height;   //�������� ���� Ư�� ��ġ�� �������� �� �ð� ��������

									  //2���� �迭�� ����Ͽ� �ѻ���� �������� ��ġ ����
			int array[100][2];//100������ �ʱ�ȭ
			for (int i = 0; i < 100; i++) {
				array[i][0] = x;
				array[i][1] = y;

				//������ ��ġ ���
				put_string(mat_frame, "(x): ", Point(10, 40), array[i][0]);
				put_string(mat_frame, "(y): ", Point(10, 60), array[i][1]);

				//�ð� ī��Ʈ
				if (array[i][1] >= 200 && array[i][1] <= 300) //�߰� ��ġ�� ���
				{
					if (check) {
						//���� ���� �ش翵���� �������� ����� ��ġ ��
						end = clock();
						result = (double)(begin - end) / CLOCKS_PER_SEC;
						//cout << "����ð�(��) : " << result << endl;
						check = false;
						double first_clock = fabs(result);
						cout << "�ʹݺ� ���(��) : " << first_clock << endl;
					}
					begin = clock();

					//���������� ����� ������ ���� �ð� 
					result2 = (double)(begin - end) / CLOCKS_PER_SEC;
					cout << "����ð�(��) : " << result2 << endl;
					break;
					//result2���� �ٷ� �߰� ��ġ�� �ִ� ��, �ִ� 2�������� �ð��� �÷��ٰ�
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

				//delay �ɾ��ֱ�
			}

		}

		for (i = 0; i<found1.size(); i++)
		{
			Rect r1 = found1[i];
			size_t j;
			//�� ū Ž�� ���ο� ���� Ž���� �߰����� ���ƾ� ��
			for (j = 0; j<found1.size(); j++)
				if (j != i && (r1 & found1[j]) == r1)
					break;
			if (j == found1.size())
				found1_filtered.push_back(r1);
		}

		for (i = 0; i<found1_filtered.size(); i++)
		{
			Rect r1 = found1_filtered[i];
			// hog������ ���� ��ü���� �ణ ū ���簢���� ��ȯ������ ���簢���� �ణ ��ҽ��� �ٻ簪�� ��´�
			//�簢���� ��ǥ���� ���ϰ� �װ��� ���� �ϳ��� ��� �� �� �� ���� ��� ��ġ �̻��̸� +ǥ�� �� ��ġ���� ��ȯ �˷���         

			r1.x += cvRound(r1.width*0.1);   //cvRound: �ε� �Ҽ��� ���ڸ� ���� ����� ������ �ݿø�
			r1.width = cvRound(r1.width*0.8);
			r1.y += cvRound(r1.height*0.07);
			r1.height = cvRound(r1.height*0.8);

			rectangle(mat_frame, r1.tl(), r1.br(), Scalar(0, 255, 0), 3);   //tl: ������ ��� br: ������ �ϴ�

			int x = r1.x;
			int y = r1.y + r1.height;   //�������� ���� Ư�� ��ġ�� �������� �� �ð� ��������
									  //2���� �迭�� ����Ͽ� �ѻ���� �������� ��ġ ����
			//int array[100][2];//100������ �ʱ�ȭ
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

				//delay �ɾ��ֱ�
			}
			*/
		}
	
		/*
		//��
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