#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <cmath>
using namespace cv;
#define width 400
#define height 400

//ʵ��Bresenham�����㷨���ĸ������ֱ�Ϊͼ����㡢�յ����ɫ
void IntegerBresenhamline(Mat image, Point p0, Point p1, Vec3b color); 

//ʵ�����������Ļ�Բ�㷨�������ֱ�Ϊͼ��Բ�ĵ㡢�뾶����ɫ
void IntegerMidPointCircle(Mat image, Point O, int radius, Vec3b color);

//���ƶԳƵ�8���㣬ע�⴦��Խ������
void CirclePoints(Mat image, Point O, int dx, int dy, Vec3b color);

//ʵ�ִ�Kernel��Bresenham�����㷨
void IntegerBresenhamlineWithKernel(Mat image, Point p0, Point p1, Vec3b color);

//ʵ�����������������Bresenham�����㷨���ĸ������ֱ�Ϊͼ����㡢�յ����ɫ
void IntegerBresenhamlineWithArea(Mat image, Point p0, Point p1, Vec3b color); 

//ʵ�������Kernel�Ĳ�����������Bresenham�����㷨
void IntegerBresenhamlineWithKernelArea(Mat image, Point p0, Point p1, Vec3b color);

//׷��SSAA
void SSAAIntegerBresenhamlineWithKernelArea(Mat image, Point p0, Point p1, Vec3b color);

int main() {
	//��ֱ�߲���
	char line_window[] = "draw line";
	Mat line_image = Mat::zeros(height, width, CV_8UC3);
	Point p0 = Point(50,100);
	Point p1 = Point(125,350);
	Point p2 = Point(349, 51);
	IntegerBresenhamlineWithKernelArea(line_image, p1, p0, Vec3b(255,0,0));
	IntegerBresenhamlineWithKernelArea(line_image, p0, p2, Vec3b(0,255,0));
	IntegerBresenhamlineWithKernelArea(line_image, p2, p1, Vec3b(0,0,255));
	imshow(line_window, line_image);
	imwrite("line_kernelarea.bmp", line_image);

	//SSAA��ֱ�߲���
	char line_2_window[] = "draw SSAA line";
	Mat line_2_image = Mat::zeros(2*height, 2*width, CV_8UC3);
	Mat lines_image;
	Point p0s = Point(50,100);
	Point p1s = Point(125,350);
	Point p2s = Point(349, 51);
	SSAAIntegerBresenhamlineWithKernelArea(line_2_image, p1s, p0s, Vec3b(255,0,0));
	SSAAIntegerBresenhamlineWithKernelArea(line_2_image, p0s, p2s, Vec3b(0,255,0));
	SSAAIntegerBresenhamlineWithKernelArea(line_2_image, p2s, p1s, Vec3b(0,0,255));
	resize(line_2_image, lines_image, Size(height, width));
	imshow(line_2_window, lines_image);
	imwrite("line_SSAAkernelarea.bmp", lines_image);

	//��Բ������
	/*char circle_window[] = "draw circle";
	Mat circle_image = Mat::zeros(height, width, CV_8UC3);
	IntegerMidPointCircle(circle_image, Point(280, 300), 80,
		Vec3b(255,255,0)); //��һ��ȫ��ͼ���ڲ���Բ
	IntegerMidPointCircle(circle_image, p0, 90, 
		Vec3b(0,255,255)); //��һ��������ͼ���е�Բ
	imshow(circle_window, circle_image);
	imwrite("circle.bmp", circle_image); */
	waitKey(0);
	return 0;
}

//׷��SSAA
void SSAAIntegerBresenhamlineWithKernelArea(Mat image, Point p0, Point p1, Vec3b color) {
	Point p0s = Point(p0.x*2, p0.y*2);
	Point p1s = Point(p1.x*2, p1.y*2);
	IntegerBresenhamlineWithKernelArea(image, p0s, p1s, color);
}

//ʵ��Bresenham�����㷨���ĸ������ֱ�Ϊͼ����㡢�յ����ɫ
void IntegerBresenhamline(Mat image, Point p0, Point p1, Vec3b color) {
	int x = p0.x, y = p0.y; //��ʼֵ
	int dx = p1.x - x, dy = p1.y - y; //��ֵ�����֮Ϊб�ʵ��������
	int stepX = (dx >= 0 ? 1 : -1); //ÿһ��x���ı��ƶ�
	int stepY = (dy >= 0 ? 1 : -1); //ÿһ��y���ı��ƶ�
	if (dx * stepX >= dy * stepY) { //��x����л���
		int absDx = stepX * dx;
		int absDy = stepY * dy;
		int e = -absDx; //�в����ʼ��Ϊdx��һ�룬�Ӷ���֤��������		
		for (int i = 0; i <= absDx; i++) {
			image.at<Vec3b>(x, y) = color; //�������ص�
			x += stepX;
			e += 2 * absDy;
			if (e >= 0) {
				y += stepY;
				e -= 2 * absDx; //�ı�ʱҲ�����в���仯
			}
		}
	} else { //��y����л���
		int absDx = stepX * dx;
		int absDy = stepY * dy;
		int e = -absDy;
		for (int i = 0; i <= absDy; i++) {
			image.at<Vec3b>(x, y) = color;
			y += stepY;
			e += 2 * absDx;
			if (e >= 0) {
				x += stepX;
				e -= 2 * absDy;
			}
		}
	}
}

//ʵ�ִ�Kernel��Bresenham�����㷨
void IntegerBresenhamlineWithKernel(Mat image, Point p0, Point p1, Vec3b color) {
	const int k = 3; //�����зָ���
	int w[k][k] = {{1,2,1},{2,4,2},{1,2,1}}; //����С����Ȩֵ
	double splitNum[k] = {0.0, 1.0/k, 2.0/k}; //�ָ��Ĵ�С
	int wMax = 4; //��������Ȩֵ
	int x = p0.x, y = p0.y; //��ʼֵ
	int dx = p1.x - x, dy = p1.y - y; //��ֵ�����֮Ϊб�ʵ��������
	int stepX = (dx >= 0 ? 1 : -1); //ÿһ��x���ı��ƶ�
	int stepY = (dy >= 0 ? 1 : -1); //ÿһ��y���ı��ƶ�
	if (dx * stepX >= dy * stepY) { //��x����л���
		int absDx = stepX * dx;
		int absDy = stepY * dy;
		int e = -absDx; //�в����ʼ��Ϊdx��һ�룬�Ӷ���֤��������		
		for (int i = 0; i <= absDx; i++) {
			int mywMax = 0; //��¼���ڸ÷���Ĵ�С
			for (int j = 0; j < k; j++) { //��x�仯�Ķ���
				double eSplit = -(e/2.0 + j * absDy);
				int locateIndex = -1; //��������y�����index
				for (int l = 0; l < k; l++) {
					if (eSplit > absDx * splitNum[l]) {
						locateIndex = l;
					}
				}
				if (locateIndex >= 0 && w[j][locateIndex] > mywMax) {
					mywMax = w[j][locateIndex];
				}
			}
			double ita = 1.0 * mywMax / wMax;
			Vec3b myColor = Vec3b((int)(ita*color[0]),(int)(ita*color[1]),(int)(ita*color[2]));
			image.at<Vec3b>(x, y) = myColor; //�������ص�
			x += stepX;
			e += 2 * absDy;
			if (e >= 0) {
				y += stepY;
				e -= 2 * absDx; //�ı�ʱҲ�����в���仯
			}
		}
	} else { //��y����л���
		int absDx = stepX * dx;
		int absDy = stepY * dy;
		int e = -absDy;
		for (int i = 0; i <= absDy; i++) {
			int mywMax = 0; //��¼���ڸ÷���Ĵ�С
			for (int j = 0; j < k; j++) { //��x�仯�Ķ���
				double eSplit = -(e/2.0 + j * absDx);
				int locateIndex = -1; //��������x�����index
				for (int l = 0; l < k; l++) {
					if (eSplit > absDy * splitNum[l]) {
						locateIndex = l;
					}
				}
				if (locateIndex >= 0 && w[j][locateIndex] > mywMax) {
					mywMax = w[j][locateIndex];
				}
			}
			double ita = 1.0 * mywMax / wMax;
			Vec3b myColor = Vec3b((int)(ita*color[0]),(int)(ita*color[1]),(int)(ita*color[2]));
			image.at<Vec3b>(x, y) = myColor;
			y += stepY;
			e += 2 * absDx;
			if (e >= 0) {
				x += stepX;
				e -= 2 * absDy;
			}
		}
	}
}

//ʵ�����������������Bresenham�����㷨���ĸ������ֱ�Ϊͼ����㡢�յ����ɫ
void IntegerBresenhamlineWithArea(Mat image, Point p0, Point p1, Vec3b color) {
	int k = 3; //�ֿ��������������ᣩ
	int x = p0.x, y = p0.y; //��ʼֵ
	int dx = p1.x - x, dy = p1.y - y; //��ֵ�����֮Ϊб�ʵ��������
	double margin = sqrt(dx*dx + dy*dy);
	int stepX = (dx >= 0 ? 1 : -1); //ÿһ��x���ı��ƶ�
	int stepY = (dy >= 0 ? 1 : -1); //ÿһ��y���ı��ƶ�
	if (dx * stepX >= dy * stepY) { //��x����л���
		int absDx = stepX * dx;
		int absDy = stepY * dy;
		int e = -absDx; //�в����ʼ��Ϊdx��һ�룬�Ӷ���֤��������		
		for (int i = 0; i <= absDx; i++) {
			for (int j = -1; j <= 1; j++) { //�������ǿ����������������ط���
				double eJ = e - 2 * absDx * j; //���õ�ǰ��e
				int countUsed = 0; //���������Ŀ���
				for (int l = 0; l < k; l++) {
					for (int m = 0; m < k; m++) {
						double eBlock = eJ + 2.0 * l * absDy / k - 2.0 * m * absDx / k;
						if (-absDx - margin <= eBlock && eBlock <= -absDx + margin) {
							countUsed++; //���ĵ������������һ
						}
					}
				}
				double ita = 1.0 * countUsed / k / k; //��ռȨ��
				Vec3b myColor = Vec3b((int)(ita*color[0]),(int)(ita*color[1]),(int)(ita*color[2]));
				image.at<Vec3b>(x, y+j*stepY) = myColor; //�������ص�
			} 		
			x += stepX;
			e += 2 * absDy;
			if (e >= 0) {
				y += stepY;
				e -= 2 * absDx; //�ı�ʱҲ�����в���仯
			}
		}
	} else { //��y����л���
		int absDx = stepX * dx;
		int absDy = stepY * dy;
		int e = -absDy;
		for (int i = 0; i <= absDy; i++) {
			for (int j = -1; j <= 1; j++) { //�������ǿ����������������ط���
				double eJ = e - 2 * absDy * j; //���õ�ǰ��e
				int countUsed = 0; //���������Ŀ���
				for (int l = 0; l < k; l++) {
					for (int m = 0; m < k; m++) {
						double eBlock = eJ + 2.0 * l * absDx / k - 2.0 * m * absDy / k;
						if (-absDy - margin <= eBlock && eBlock <= -absDy + margin) {
							countUsed++; //���ĵ������������һ
						}
					}
				}
				double ita = 1.0 * countUsed / k / k; //��ռȨ��
				Vec3b myColor = Vec3b((int)(ita*color[0]),(int)(ita*color[1]),(int)(ita*color[2]));
				image.at<Vec3b>(x+j*stepX, y) = myColor; //�������ص�
			} 		
			y += stepY;
			e += 2 * absDx;
			if (e >= 0) {
				x += stepX;
				e -= 2 * absDy;
			}
		}
	}
}

//ʵ�������Kernel�Ĳ�����������Bresenham�����㷨
void IntegerBresenhamlineWithKernelArea(Mat image, Point p0, Point p1, Vec3b color) {
	const int k = 3; //�ֿ��������������ᣩ
	double w[k][k] = {{0.0625,0.125,0.0625},{0.125,0.25,0.125},{0.0625,0.125,0.0625}};
	int x = p0.x, y = p0.y; //��ʼֵ
	int dx = p1.x - x, dy = p1.y - y; //��ֵ�����֮Ϊб�ʵ��������
	double margin = sqrt(dx*dx + dy*dy);
	int stepX = (dx >= 0 ? 1 : -1); //ÿһ��x���ı��ƶ�
	int stepY = (dy >= 0 ? 1 : -1); //ÿһ��y���ı��ƶ�
	if (dx * stepX >= dy * stepY) { //��x����л���
		int absDx = stepX * dx;
		int absDy = stepY * dy;
		int e = -absDx; //�в����ʼ��Ϊdx��һ�룬�Ӷ���֤��������		
		for (int i = 0; i <= absDx; i++) {
			for (int j = -1; j <= 1; j++) { //�������ǿ����������������ط���
				double eJ = e - 2 * absDx * j; //���õ�ǰ��e
				double usedWeight = 0.0; //�Ը÷������õ�Ȩ��
				for (int l = 0; l < k; l++) {
					for (int m = 0; m < k; m++) {
						double eBlock = eJ + 2.0 * l * absDy / k - 2.0 * m * absDx / k;
						if (-absDx - margin <= eBlock && eBlock <= -absDx + margin) {
							usedWeight += w[l][m]; //���ĵ������������һ
						}
					}
				}
				double ita = usedWeight; //��ռȨ��
				Vec3b myColor = Vec3b((int)(ita*color[0]),(int)(ita*color[1]),(int)(ita*color[2]));
				image.at<Vec3b>(x, y+j*stepY) = myColor; //�������ص�
			} 		
			x += stepX;
			e += 2 * absDy;
			if (e >= 0) {
				y += stepY;
				e -= 2 * absDx; //�ı�ʱҲ�����в���仯
			}
		}
	} else { //��y����л���
		int absDx = stepX * dx;
		int absDy = stepY * dy;
		int e = -absDy;
		for (int i = 0; i <= absDy; i++) {
			for (int j = -1; j <= 1; j++) { //�������ǿ����������������ط���
				double eJ = e - 2 * absDy * j; //���õ�ǰ��e
				double usedWeight = 0.0; //�Ը÷������õ�Ȩ��
				for (int l = 0; l < k; l++) {
					for (int m = 0; m < k; m++) {
						double eBlock = eJ + 2.0 * l * absDx / k - 2.0 * m * absDy / k;
						if (-absDy - margin <= eBlock && eBlock <= -absDy + margin) {
							usedWeight += w[m][l];
						}
					}
				}
				double ita = usedWeight; //��ռȨ��
				Vec3b myColor = Vec3b((int)(ita*color[0]),(int)(ita*color[1]),(int)(ita*color[2]));
				image.at<Vec3b>(x+j*stepX, y) = myColor; //�������ص�
			} 		
			y += stepY;
			e += 2 * absDx;
			if (e >= 0) {
				x += stepX;
				e -= 2 * absDy;
			}
		}
	}
}

//���ƶԳƵ�8���㣬ע�⴦��Խ��������������淶������dx,dy >= 0
void CirclePoints(Mat image, Point O, int dx, int dy, Vec3b color) {
	int X[] = {O.x + dx, O.x - dx, O.x + dy, O.x - dy};
	int Y[] = {O.y + dy, O.y - dy, O.y + dx, O.y - dx};
	if (X[0] < height) {
		if (Y[0] < width) {
			image.at<Vec3b>(X[0], Y[0]) = color;
		}
		if (Y[1] >= 0) {
			image.at<Vec3b>(X[0], Y[1]) = color;
		}
	}
	if (X[1] >= 0) {
		if (Y[0] < width) {
			image.at<Vec3b>(X[1], Y[0]) = color;
		}
		if (Y[1] >= 0) {
			image.at<Vec3b>(X[1], Y[1]) = color;
		}
	}
	if (X[2] < height) {
		if (Y[2] < width) {
			image.at<Vec3b>(X[2], Y[2]) = color;
		}
		if (Y[3] >= 0) {
			image.at<Vec3b>(X[2], Y[3]) = color;
		}
	}
	if (X[3] >= 0) {
		if (Y[2] < width) {
			image.at<Vec3b>(X[3], Y[2]) = color;
		}
		if (Y[3] >= 0) {
			image.at<Vec3b>(X[3], Y[3]) = color;
		}
	}
}

//ʵ�����������Ļ�Բ�㷨�������ֱ�Ϊͼ��Բ�ĵ㡢�뾶����ɫ
void IntegerMidPointCircle(Mat image, Point O, int radius, Vec3b color) {
	int x = 0, y = radius;
	int d = 5 - 4 * radius; //��ʼֵd0
	CirclePoints(image, O, x, y, color);
	while (x <= y) {
		if (d < 0) { //����y�Ƿ���Ҫ�����仯������
			d += 8 * x + 12;
		} else {
			d += 8 * (x - y) + 20;
			y--;
		}
		x++;
		CirclePoints(image, O, x, y, color);
	}
}