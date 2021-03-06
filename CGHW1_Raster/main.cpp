#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <cmath>
#include <stack>
using namespace cv;
#define width 400
#define height 400

//在某个像素点追加上其他的颜色,并将结果写入aim中
void addWithColor(Vec3b& aim, Vec3b& color);

//实现Bresenham画线算法，四个参数分别为图像、起点、终点和颜色
void IntegerBresenhamline(Mat image, Point p0, Point p1, Vec3b color); 

//实现整数的中心画圆算法，参数分别为图像、圆心点、半径、颜色
void IntegerMidPointCircle(Mat image, Point O, int radius, Vec3b color);

//绘制对称的8个点，注意处理越界的情况
void CirclePoints(Mat image, Point O, int dx, int dy, Vec3b color);

//实现带Kernel的Bresenham画线算法
void IntegerBresenhamlineWithKernel(Mat image, Point p0, Point p1, Vec3b color);

//实现区域采样反走样的Bresenham画线算法，四个参数分别为图像、起点、终点和颜色
void IntegerBresenhamlineWithArea(Mat image, Point p0, Point p1, Vec3b color); 

//实现区域带Kernel的采样反走样的Bresenham画线算法
void IntegerBresenhamlineWithKernelArea(Mat image, Point p0, Point p1, Vec3b color);

//追加SSAA
void SSAAIntegerBresenhamlineWithKernelArea(Mat image, Point p0, Point p1, Vec3b color);

//内点表示的区域填充算法，其中p0表示初始点,image为需要处理的图像
void ScanLineFill4(Mat image, Point p0, Vec3b oldcolor, Vec3b newcolor);

int main() {
	//画直线部分
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

	//SSAA画直线部分
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

	//相交线反走样部分
	char line_3_window[] = "draw intersection line";
	Mat line_3_image = Mat::zeros(height, width, CV_8UC3);
	IntegerBresenhamlineWithKernelArea(line_3_image, Point(50, 48), Point(260, 309), Vec3b(220,40,40));
	IntegerBresenhamlineWithKernelArea(line_3_image, Point(270, 35), Point(40, 366), Vec3b(35,200,20));
	IntegerBresenhamlineWithKernelArea(line_3_image, Point(100, 50), Point(150, 300), Vec3b(50,10,180));
	imshow(line_3_window, line_3_image);
	imwrite("line_intersection.bmp", line_3_image);


	//画圆弧部分
	char circle_window[] = "draw circle";
	Mat circle_image = Mat::zeros(height, width, CV_8UC3);
	IntegerMidPointCircle(circle_image, Point(280, 300), 80,
		Vec3b(255,255,0)); //画一个全在图像内部的圆
	IntegerMidPointCircle(circle_image, p0, 90, 
		Vec3b(0,255,255)); //画一个部分在图像中的圆
	imshow(circle_window, circle_image);
	imwrite("circle.bmp", circle_image); 

	//区域填充部分
	char area_window[] = "area fill";
	Mat area_image = Mat::zeros(height, width, CV_8UC3);
	IntegerBresenhamline(area_image, Point(160,50), Point(160,160), Vec3b(200,200,200));
	IntegerBresenhamline(area_image, Point(160,160), Point(40,200), Vec3b(200,200,200));
	IntegerBresenhamline(area_image, Point(40,200), Point(160,240), Vec3b(200,200,200));
	IntegerBresenhamline(area_image, Point(160,240), Point(160,350), Vec3b(200,200,200));
	IntegerBresenhamline(area_image, Point(160,350), Point(240,270), Vec3b(200,200,200));
	IntegerBresenhamline(area_image, Point(240,270), Point(340,300), Vec3b(200,200,200));
	IntegerBresenhamline(area_image, Point(340,300), Point(300,200), Vec3b(200,200,200));
	IntegerBresenhamline(area_image, Point(300,200), Point(340,100), Vec3b(200,200,200));
	IntegerBresenhamline(area_image, Point(340,100), Point(240,130), Vec3b(200,200,200));
	IntegerBresenhamline(area_image, Point(240,130), Point(160,50), Vec3b(200,200,200));
	ScanLineFill4(area_image, Point(200, 200), Vec3b(0, 0, 0), Vec3b(240,160, 80));
	imshow(area_window, area_image);
	imwrite("area_fill.bmp", area_image);

	waitKey(0);
	return 0;
}

//在某个像素点追加上其他的颜色,并将结果写入aim中
void addWithColor(Vec3b& aim, Vec3b& color) {
	for (int i = 0; i < 3; i++) {
		aim[i] = min(aim[i]+color[i], 255);
	}
}

//追加SSAA
void SSAAIntegerBresenhamlineWithKernelArea(Mat image, Point p0, Point p1, Vec3b color) {
	Point p0s = Point(p0.x*2, p0.y*2);
	Point p1s = Point(p1.x*2, p1.y*2);
	IntegerBresenhamlineWithKernelArea(image, p0s, p1s, color);
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
			addWithColor(image.at<Vec3b>(x, y), color); //绘制像素点
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
			addWithColor(image.at<Vec3b>(x, y), color); //绘制像素点
			y += stepY;
			e += 2 * absDx;
			if (e >= 0) {
				x += stepX;
				e -= 2 * absDy;
			}
		}
	}
}

//实现带Kernel的Bresenham画线算法
void IntegerBresenhamlineWithKernel(Mat image, Point p0, Point p1, Vec3b color) {
	const int k = 3; //设置切分个数
	int w[k][k] = {{1,2,1},{2,4,2},{1,2,1}}; //各个小格点的权值
	double splitNum[k] = {0.0, 1.0/k, 2.0/k}; //分隔的大小
	int wMax = 4; //其中最大的权值
	int x = p0.x, y = p0.y; //起始值
	int dx = p1.x - x, dy = p1.y - y; //差值，或称之为斜率的整数拆分
	int stepX = (dx >= 0 ? 1 : -1); //每一步x向哪边移动
	int stepY = (dy >= 0 ? 1 : -1); //每一步y向哪边移动
	if (dx * stepX >= dy * stepY) { //沿x轴进行绘制
		int absDx = stepX * dx;
		int absDy = stepY * dy;
		int e = -absDx; //残差项，初始化为dx的一半，从而保证四舍五入		
		for (int i = 0; i <= absDx; i++) {
			int mywMax = 0; //记录属于该方格的大小
			for (int j = 0; j < k; j++) { //随x变化的多少
				double eSplit = -(e/2.0 + j * absDy);
				int locateIndex = -1; //它所属的y方向的index
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
			addWithColor(image.at<Vec3b>(x, y), myColor); //绘制像素点
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
			int mywMax = 0; //记录属于该方格的大小
			for (int j = 0; j < k; j++) { //随x变化的多少
				double eSplit = -(e/2.0 + j * absDx);
				int locateIndex = -1; //它所属的x方向的index
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
			addWithColor(image.at<Vec3b>(x, y), myColor); //绘制像素点
			y += stepY;
			e += 2 * absDx;
			if (e >= 0) {
				x += stepX;
				e -= 2 * absDy;
			}
		}
	}
}

//实现区域采样反走样的Bresenham画线算法，四个参数分别为图像、起点、终点和颜色
void IntegerBresenhamlineWithArea(Mat image, Point p0, Point p1, Vec3b color) {
	int k = 3; //分块的区域个数（单轴）
	int x = p0.x, y = p0.y; //起始值
	int dx = p1.x - x, dy = p1.y - y; //差值，或称之为斜率的整数拆分
	double margin = sqrt(dx*dx + dy*dy);
	int stepX = (dx >= 0 ? 1 : -1); //每一步x向哪边移动
	int stepY = (dy >= 0 ? 1 : -1); //每一步y向哪边移动
	if (dx * stepX >= dy * stepY) { //沿x轴进行绘制
		int absDx = stepX * dx;
		int absDy = stepY * dy;
		int e = -absDx; //残差项，初始化为dx的一半，从而保证四舍五入		
		for (int i = 0; i <= absDx; i++) {
			for (int j = -1; j <= 1; j++) { //这里我们考虑这上下三个像素方格
				double eJ = e - 2 * absDx * j; //设置当前的e
				int countUsed = 0; //计数经过的块数
				for (int l = 0; l < k; l++) {
					for (int m = 0; m < k; m++) {
						double eBlock = eJ + 2.0 * l * absDy / k - 2.0 * m * absDx / k;
						if (-absDx - margin <= eBlock && eBlock <= -absDx + margin) {
							countUsed++; //中心点在区域内则加一
						}
					}
				}
				double ita = 1.0 * countUsed / k / k; //所占权重
				Vec3b myColor = Vec3b((int)(ita*color[0]),(int)(ita*color[1]),(int)(ita*color[2]));
				addWithColor(image.at<Vec3b>(x, y+j*stepY), myColor); //绘制像素点
			} 		
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
			for (int j = -1; j <= 1; j++) { //这里我们考虑这上下三个像素方格
				double eJ = e - 2 * absDy * j; //设置当前的e
				int countUsed = 0; //计数经过的块数
				for (int l = 0; l < k; l++) {
					for (int m = 0; m < k; m++) {
						double eBlock = eJ + 2.0 * l * absDx / k - 2.0 * m * absDy / k;
						if (-absDy - margin <= eBlock && eBlock <= -absDy + margin) {
							countUsed++; //中心点在区域内则加一
						}
					}
				}
				double ita = 1.0 * countUsed / k / k; //所占权重
				Vec3b myColor = Vec3b((int)(ita*color[0]),(int)(ita*color[1]),(int)(ita*color[2]));
				addWithColor(image.at<Vec3b>(x+j*stepX, y), myColor); //绘制像素点
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

//实现区域带Kernel的采样反走样的Bresenham画线算法
void IntegerBresenhamlineWithKernelArea(Mat image, Point p0, Point p1, Vec3b color) {
	const int k = 3; //分块的区域个数（单轴）
	double w[k][k] = {{0.0625,0.125,0.0625},{0.125,0.25,0.125},{0.0625,0.125,0.0625}};
	int x = p0.x, y = p0.y; //起始值
	int dx = p1.x - x, dy = p1.y - y; //差值，或称之为斜率的整数拆分
	double margin = sqrt(dx*dx + dy*dy);
	int stepX = (dx >= 0 ? 1 : -1); //每一步x向哪边移动
	int stepY = (dy >= 0 ? 1 : -1); //每一步y向哪边移动
	if (dx * stepX >= dy * stepY) { //沿x轴进行绘制
		int absDx = stepX * dx;
		int absDy = stepY * dy;
		int e = -absDx; //残差项，初始化为dx的一半，从而保证四舍五入		
		for (int i = 0; i <= absDx; i++) {
			for (int j = -1; j <= 1; j++) { //这里我们考虑这上下三个像素方格
				double eJ = e - 2 * absDx * j; //设置当前的e
				double usedWeight = 0.0; //对该方格有用的权重
				for (int l = 0; l < k; l++) {
					for (int m = 0; m < k; m++) {
						double eBlock = eJ + 2.0 * l * absDy / k - 2.0 * m * absDx / k;
						if (-absDx - margin <= eBlock && eBlock <= -absDx + margin) {
							usedWeight += w[l][m]; //中心点在区域内则加一
						}
					}
				}
				double ita = usedWeight; //所占权重
				Vec3b myColor = Vec3b((int)(ita*color[0]),(int)(ita*color[1]),(int)(ita*color[2]));
				addWithColor(image.at<Vec3b>(x, y+j*stepY), myColor); //绘制像素点
			} 		
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
			for (int j = -1; j <= 1; j++) { //这里我们考虑这上下三个像素方格
				double eJ = e - 2 * absDy * j; //设置当前的e
				double usedWeight = 0.0; //对该方格有用的权重
				for (int l = 0; l < k; l++) {
					for (int m = 0; m < k; m++) {
						double eBlock = eJ + 2.0 * l * absDx / k - 2.0 * m * absDy / k;
						if (-absDy - margin <= eBlock && eBlock <= -absDy + margin) {
							usedWeight += w[m][l];
						}
					}
				}
				double ita = usedWeight; //所占权重
				Vec3b myColor = Vec3b((int)(ita*color[0]),(int)(ita*color[1]),(int)(ita*color[2]));
				addWithColor(image.at<Vec3b>(x+j*stepX, y), myColor); //绘制像素点
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

//绘制对称的8个点，注意处理越界的情况，在这里，规范化处理，dx,dy >= 0
void CirclePoints(Mat image, Point O, int dx, int dy, Vec3b color) {
	int X[] = {O.x + dx, O.x - dx, O.x + dy, O.x - dy};
	int Y[] = {O.y + dy, O.y - dy, O.y + dx, O.y - dx};
	if (X[0] < height) {
		if (Y[0] < width) {
			addWithColor(image.at<Vec3b>(X[0], Y[0]), color);
		}
		if (Y[1] >= 0) {
			addWithColor(image.at<Vec3b>(X[0], Y[1]), color);
		}
	}
	if (X[1] >= 0) {
		if (Y[0] < width) {
			addWithColor(image.at<Vec3b>(X[1], Y[0]), color);
		}
		if (Y[1] >= 0) {
			addWithColor(image.at<Vec3b>(X[1], Y[1]), color);
		}
	}
	if (X[2] < height) {
		if (Y[2] < width) {
			addWithColor(image.at<Vec3b>(X[2], Y[2]), color);
		}
		if (Y[3] >= 0) {
			addWithColor(image.at<Vec3b>(X[2], Y[3]), color);
		}
	}
	if (X[3] >= 0) {
		if (Y[2] < width) {
			addWithColor(image.at<Vec3b>(X[3], Y[2]), color);
		}
		if (Y[3] >= 0) {
			addWithColor(image.at<Vec3b>(X[3], Y[3]), color);
		}
	}
}

//实现整数的中心画圆算法，参数分别为图像、圆心点、半径、颜色
void IntegerMidPointCircle(Mat image, Point O, int radius, Vec3b color) {
	int x = 0, y = radius;
	int d = 5 - 4 * radius; //初始值d0
	CirclePoints(image, O, x, y, color);
	while (x <= y) {
		if (d < 0) { //根据y是否需要发生变化来分类
			d += 8 * x + 12;
		} else {
			d += 8 * (x - y) + 20;
			y--;
		}
		x++;
		CirclePoints(image, O, x, y, color);
	}
}

//内点表示的区域填充算法，其中p0表示初始点,image为需要处理的图像
void ScanLineFill4(Mat image, Point p0, Vec3b oldcolor, Vec3b newcolor) {
	int xleft = 0, xright = 0; //左右边界点
	int x = 0, y = 0; //所需的临时变量
	bool spanNeedFill = false; //扩展的部分是否需要绘制
	std::stack<Point> seeds; //种子栈
	seeds.push(p0); //将初始点压入栈中
	while(!seeds.empty()) { //栈非空时继续循环
		Point pt = seeds.top(); //取出栈顶元素
		seeds.pop();
		x = pt.x;
		y = pt.y;
		while (image.at<Vec3b>(x, y) == oldcolor) { //不断向右填充
			addWithColor(image.at<Vec3b>(x, y), newcolor);
			x++;
		}
		xright = x - 1; //右边界为最后一个着色点
		x = pt.x - 1;
		while (image.at<Vec3b>(x, y) == oldcolor) { //不断向左填充
			addWithColor(image.at<Vec3b>(x, y), newcolor);
			x--;
		}
		xleft = x + 1;

		//处理上面一条扫描线，从左向右扫描，每段各找出一个代表点
		x = xleft;
		y = y + 1;
		while (x <= xright) {
			spanNeedFill = false; //初始化不需要填充
			while (image.at<Vec3b>(x,y) == oldcolor) { //找到最靠右的一个代表元
				spanNeedFill = true;
				x++;
			}
			if (spanNeedFill) {
				Point newPoint(x-1, y);
				seeds.push(newPoint);
			}
			while (image.at<Vec3b>(x, y) != oldcolor && x <= xright) x++; //跳过中间边界的点
		}

		//同理处理下面一条扫描线
		x = xleft;
		y -= 2;
		while (x <= xright) {
			spanNeedFill = false; //初始化不需要填充
			while (image.at<Vec3b>(x,y) == oldcolor) { //找到最靠右的一个代表元
				spanNeedFill = true;
				x++;
			}
			if (spanNeedFill) {
				Point newPoint(x-1, y);
				seeds.push(newPoint);
			}
			while (image.at<Vec3b>(x, y) != oldcolor && x <= xright) x++; //跳过中间边界的点
		}
	}
}