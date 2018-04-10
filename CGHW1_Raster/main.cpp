#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
using namespace cv;
#define width 400
#define height 400

//ʵ��Bresenham�����㷨���ĸ������ֱ�Ϊͼ����㡢�յ����ɫ
void IntegerBresenhamline(Mat image, Point p0, Point p1, Vec3b color); 

//ʵ�����������Ļ�Բ�㷨�������ֱ�Ϊͼ��Բ�ĵ㡢�뾶����ɫ
void IntegerMidPointCircle(Mat image, Point O, int radius, Vec3b color);

//���ƶԳƵ�8���㣬ע�⴦��Խ������
void CirclePoints(Mat image, Point O, int dx, int dy, Vec3b color);


int main() {
	char line_window[] = "draw line";
	Mat line_image = Mat::zeros(height, width, CV_8UC3 );
	Point p0 = Point(50,100);
	Point p1 = Point(125,350);
	Point p2 = Point(349, 51);
	IntegerBresenhamline(line_image, p1, p0, Vec3b(255,0,0));
	IntegerBresenhamline(line_image, p0, p2, Vec3b(0,255,0));
	IntegerBresenhamline(line_image, p2, p1, Vec3b(0,0,255));
	imshow(line_window, line_image);
	imwrite("line.bmp", line_image);
	waitKey(0);
	return 0;
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

}