#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
using namespace cv;
#define width 400
#define height 400

//实现Bresenham画线算法，四个参数分别为图像、起点、终点和颜色
void IntegerBresenhamline(Mat image, Point p0, Point p1, Vec3b color); 

//实现整数的中心画圆算法，参数分别为图像、圆心点、半径、颜色
void IntegerMidPointCircle(Mat image, Point O, int radius, Vec3b color);

//绘制对称的8个点，注意处理越界的情况
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

//实现Bresenham画线算法，四个参数分别为图像、起点、终点和颜色
void IntegerBresenhamline(Mat image, Point p0, Point p1, Vec3b color) {
	int x = p0.x, y = p0.y; //起始值
	int dx = p1.x - x, dy = p1.y - y; //差值，或称之为斜率的整数拆分
	int stepX = (dx >= 0 ? 1 : -1); //每一步x向哪边移动
	int stepY = (dy >= 0 ? 1 : -1); //每一步y向哪边移动
	if (dx * stepX >= dy * stepY) { //沿x轴进行绘制
		int absDx = stepX * dx;
		int absDy = stepY * dy;
		int e = -absDx; //残差项，初始化为dx的一半，从而保证四舍五入		
		for (int i = 0; i <= absDx; i++) {
			image.at<Vec3b>(x, y) = color; //绘制像素点
			x += stepX;
			e += 2 * absDy;
			if (e >= 0) {
				y += stepY;
				e -= 2 * absDx; //改变时也带动残差发生变化
			}
		}
	} else { //沿y轴进行绘制
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

//绘制对称的8个点，注意处理越界的情况，在这里，规范化处理，dx,dy >= 0
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

//实现整数的中心画圆算法，参数分别为图像、圆心点、半径、颜色
void IntegerMidPointCircle(Mat image, Point O, int radius, Vec3b color) {

}