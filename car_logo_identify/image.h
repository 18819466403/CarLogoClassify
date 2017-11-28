#pragma once
#include<opencv2/opencv.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#define PI 3.1415926535
#define _radian(angle) ((angle)*PI/180)
#define _angle(radian) ((radian)*180/PI)
typedef int PIXEL_VALUE_TYPE;
class Image
{
private:
	cv::Mat srcImage;
	cv::String fileName;
public:
	Image(cv::String fileName);
	Image(int ** array,int row,int column);
	void loadImage(int type);
	void showImage(cv::String title);
	void saveImage(cv::String filename);
	cv::Mat& getImage();
    static void imrotate(cv::Mat& img, cv::Mat& result, double degree, int x_y_z =0);
	static void toSqure(cv::Mat& img, cv::Mat& result, cv::Scalar scalar);
	static void shear(cv::Mat& img, cv::Mat& result, double x_degree,double y_degree = 0);
};

void Image::shear(cv::Mat& source, cv::Mat& result, double x_degree,double y_degree) {
	int width = source.cols;
	int height = source.rows;
	int width_rotate = width*cos(_radian(abs(x_degree)));
	int height_rotate = height+ width*sin(_radian(abs(x_degree)));
	cv::Mat warp_mat(2, 3, CV_32FC1);
	cv::Point2f srcTri[3];
	cv::Point2f dstTri[3];
	srcTri[0] = cv::Point2f(0, 0);
	srcTri[1] = cv::Point2f(source.cols - 1, 0);
	srcTri[2] = cv::Point2f(0, source.rows - 1);
	if(x_degree<=0)
	{
		dstTri[0] = cv::Point2f(0, 0);
		dstTri[1] = cv::Point2f((source.cols)*cos(_radian(abs(x_degree))) -1, width*sin(_radian(abs(x_degree)))-1);
		dstTri[2] = cv::Point2f(0 , source.rows - 1);

	}
	else {
		dstTri[0] = cv::Point2f(0, width*sin(_radian(abs(x_degree))) - 1);
		dstTri[1] = cv::Point2f((source.cols)*cos(_radian(abs(x_degree))) - 1, 0);
		dstTri[2] = cv::Point2f(0, source.rows - 1+ width*sin(_radian(abs(x_degree))) - 1);

	}

	warp_mat = getAffineTransform(srcTri, dstTri);
	cv::warpAffine(source, result, warp_mat, cv::Size(width_rotate,height_rotate), 1, 0, cv::Scalar(255, 255, 255));
}
void Image::toSqure(cv::Mat& img, cv::Mat& result, cv::Scalar scalar) {
	

	long oRows = img.rows; 
	long oCols = img.cols*img.channels();

	long NdR = img.rows > img.cols ? img.rows : img.cols;

	cv::Mat New(NdR, NdR, CV_8UC3, scalar);

	long nNR = NdR * 3; //New 矩阵的宽  
	long nmidd = (img.rows - img.cols) / 2 * 3; //新图空白的部分  
											//两个矩阵的指针  
	uchar *pL = img.data;
	uchar *pN = New.data;

	if (img.isContinuous()) //判断原图的存储方式  
	{
		if (img.rows > img.cols)
		{
			for (long i = 0; i < NdR; i++)
			{
				for (long j = nmidd; j <= oCols + nmidd; j++)
				{
					//将原图的相应位置的像素赋值给新图  
					*(pN + (i*nNR + j)) = *(pL + (i*img.cols*img.channels() + j - nmidd));
				}
			}
		}
		else if (img.rows < img.cols)
		{
			for (long i = ((img.cols - img.rows) / 2); i < (img.rows + (img.cols - img.rows) / 2); i++)
			{
				for (long j = 0; j <= oCols; j++)
				{
					//将原图的相应位置的像素赋值给新图  
					*(pN + (i*nNR + j)) = *(pL + ((i - (img.cols - img.rows) / 2)*img.cols*img.channels() + j));
				}
			}
		}
		else
		{
			New = img;
		}
	}
	result = New;
}

/**@state:
     x_y_z==0: z axis rotation  //default situation
	 x_y_z==1: y axis rotation
	 x_y_z==2: x axis rotation
*/
void Image::imrotate(cv::Mat& source, cv::Mat& result, double degree, int x_y_z) {

	int width = source.cols;
	int height = source.rows;
	cv::Mat warp_mat(2, 3, CV_32FC1);
	cv::Point2f srcTri[3];
	cv::Point2f dstTri[3];
	srcTri[0] = cv::Point2f(0, 0);
	srcTri[1] = cv::Point2f(source.cols - 1, 0);
	srcTri[2] = cv::Point2f(0, source.rows - 1);

	if (0 == x_y_z) {
		if (((int)(degree)) % 90 != 0)
		{
			int width_rotate = source.cols*cos(_radian(90 - ((int)(degree)) % 90)) + source.rows*sin(_radian(90 - ((int)(degree)) % 90));
			int height_rotate = source.rows*cos(_radian(90 - ((int)(degree)) % 90)) + source.cols*sin(_radian(90 - ((int)(degree)) % 90));
			cv::Mat warp_dst = cv::Mat::zeros(height_rotate, width_rotate, source.type());
			dstTri[0] = cv::Point2f((width_rotate - width) / 2, (height_rotate - height) / 2);
			dstTri[1] = cv::Point2f((width_rotate - width) / 2 + source.cols - 1, (height_rotate - height) / 2);
			dstTri[2] = cv::Point2f((width_rotate - width) / 2, (height_rotate - height) / 2 + source.rows - 1);
			warp_mat = getAffineTransform(srcTri, dstTri);
			cv::warpAffine(source, warp_dst, warp_mat, warp_dst.size(), CV_INTER_LINEAR | CV_WARP_FILL_OUTLIERS, 0, cv::Scalar(255, 255, 255));
			result = warp_dst;
			cv::Point2f center(width_rotate / 2, height_rotate / 2);
			cv::Mat rot_mat = cv::getRotationMatrix2D(center, degree, 1.0);

			cv::warpAffine(warp_dst, result, rot_mat,
				cv::Size(source.cols*cos(_radian(abs(degree))) + source.rows*sin(_radian(abs(degree))), source.rows*cos(_radian(abs(degree))) + source.cols*sin(_radian(abs(degree)))),
				CV_INTER_LINEAR | CV_WARP_FILL_OUTLIERS, 0, cv::Scalar(255, 255, 255));
		}
		else
		{
			if (((int)(degree)) % 180 == 0)
			{ 
				cv::Point2f center(width / 2, height / 2);
				cv::Mat rot_mat = cv::getRotationMatrix2D(center, degree, 1.0);
				cv::warpAffine(source, result, rot_mat,
					cv::Size(source.cols, source.rows),
					CV_INTER_LINEAR | CV_WARP_FILL_OUTLIERS, 0, cv::Scalar(255, 255, 255));
			}
			else {
				int width_rotate = source.cols*cos(_radian(90- ((int)(degree)) %90)) + source.rows*sin(_radian(90 - ((int)(degree)) % 90));
				int height_rotate = source.rows*cos(_radian(90 - ((int)(degree)) % 90)) + source.cols*sin(_radian(90 - ((int)(degree)) % 90));
				int size = width_rotate > height_rotate ? width_rotate : height_rotate;
				cv::Point2f center(size / 2, size / 2);
				cv::Mat rot_mat = cv::getRotationMatrix2D(center, degree, 1.0);
				cv::Mat warp_dst = cv::Mat::zeros(size, size, source.type());
				dstTri[0] = cv::Point2f((size - width) / 2, (size - height) / 2);
				dstTri[1] = cv::Point2f((size - width) / 2 + source.cols - 1, (size - height) / 2);
				dstTri[2] = cv::Point2f((size - width) / 2, (size - height) / 2 + source.rows - 1);
				warp_mat = getAffineTransform(srcTri, dstTri);
				cv::warpAffine(source, warp_dst, warp_mat, warp_dst.size(), CV_INTER_LINEAR | CV_WARP_FILL_OUTLIERS, 0, cv::Scalar(255, 255, 255));
				cv::warpAffine(warp_dst, result, rot_mat,
					cv::Size(size, size),
					CV_INTER_LINEAR | CV_WARP_FILL_OUTLIERS, 0, cv::Scalar(255, 255, 255));
				warp_dst = result;
				dstTri[0] = cv::Point2f(-(size - height) / 2 -2, 0);
				dstTri[1] = cv::Point2f(-(size - height) / 2 -2 + source.cols - 1, 0);
				dstTri[2] = cv::Point2f(-(size - height) / 2 -2 ,  source.rows - 1);
				warp_mat = getAffineTransform(srcTri, dstTri);
				cv::warpAffine(warp_dst, result, warp_mat, cv::Size(width_rotate, height_rotate), CV_INTER_LINEAR | CV_WARP_FILL_OUTLIERS, 0, cv::Scalar(255, 255, 255));
			}
		}
	}

	if (1 == x_y_z) {
		int width_rotate = width*cos(_radian(abs(degree)));
		int height_rotate = height;
		dstTri[0] = cv::Point2f(0, 0);
		dstTri[1] = cv::Point2f((source.cols)*cos(_radian(abs(degree))) - 1, 0);
		dstTri[2] = cv::Point2f(0, source.rows - 1);
		warp_mat = getAffineTransform(srcTri, dstTri);
		cv::warpAffine(source, result, warp_mat, cv::Size(width_rotate, height_rotate), 1, 0, cv::Scalar(255, 255, 255));
	}

	if (2 == x_y_z) {
		int width_rotate = width;
		int height_rotate = height*cos(_radian(abs(degree)));
		dstTri[0] = cv::Point2f(0, 0);
		dstTri[1] = cv::Point2f(source.cols - 1, 0);
		dstTri[2] = cv::Point2f(0, source.rows*cos(_radian(abs(degree))) - 1);
		warp_mat = getAffineTransform(srcTri, dstTri);
		cv::warpAffine(source, result, warp_mat, cv::Size(width_rotate, height_rotate), 1, 0, cv::Scalar(255, 255, 255));
	}
	
}
Image::Image(cv::String file) {
	fileName = file;
}

Image::Image(int ** array,int row,int column)
{
	
	srcImage = cv::Mat(row, column, 0);
	for(int i=0;i<row;i++)
		for (int j = 0; j < column; j++) {
			srcImage.at<uchar>(i, j) = array[i][j];
		}
}

cv::Mat& Image::getImage() {
	return srcImage;
}
void Image::loadImage(int type) {
	srcImage = imread(fileName,type);
}

void Image::showImage(cv::String title) {
	imshow(title, srcImage);
	cvWaitKey(0);
}

void Image::saveImage(cv::String filename) {
	/*std::vector<int> p(3);
	p[0] = CV_IMWRITE_JPEG_QUALITY;
	p[1] = 10;
	p[2] = 0;
	imwrite(filename, srcImage,p);*/
	imwrite(filename, srcImage);
}