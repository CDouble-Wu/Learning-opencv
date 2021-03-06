// C++ source file
/***********************************************
# Copyright (c) 2018, Wuhan
# All rights reserved.
#
# @Filename: 5-2-5_Sobel.cpp
# @Version：V1.0
# @Author: Frank Liu - frankliu624@gmail.com
# @Description: 基本边缘检测算子——Sobel
# @Create Time: 2018-10-07 13:01:52
# @Last Modified: 2018-10-07 13:01:52
***********************************************/
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

/*********** 5-2. 非极大值抑制实现Sobel数值细化边缘 ************/

bool SobelVerEdge(Mat src, Mat& dst)
{
    CV_Assert(src.channels() == 1);
    src.convertTo(src, CV_32FC1);
    // 水平方向的Sobel算子
    Mat sobelX = (Mat_<float>(3,3) << -0.125, 0, 0.125,
                                       -0.25, 0, 0.25,
                                      -0.125, 0, 0.125);
    Mat ConResMat;   // 卷积运算结果
    // 卷积运算
    filter2D(src, ConResMat, src.type(), sobelX);
    // 计算梯度的幅度
    Mat graMagMat;
    multiply(ConResMat, ConResMat, graMagMat);
    // 根据梯度幅度及参数设置阈值
    int scaleVal = 4;
    double thresh = scaleVal * mean(graMagMat).val[0];
    Mat dstTempMat = Mat::zeros(graMagMat.size(), graMagMat.type());
    float* pDataMag = (float*)graMagMat.data;
    float* pDataDst = (float*)dstTempMat.data;
    const int nRows = ConResMat.rows;
    const int nCols = ConResMat.cols;
    for(int i = 1; i != nRows - 1; ++i)
    {
        for(int j = 1; j != nCols - 1; ++j)
        {
            // 计算改点梯度与水平或垂直梯度值的大小并比较结果
            bool b1 = pDataMag[i * nCols + j] > pDataMag[i * nCols +j -1];
            bool b2 = pDataMag[i * nCols + j] > pDataMag[i * nCols +j +1];
            bool b3 = pDataMag[i * nCols + j] > pDataMag[(i - 1) * nCols + j];
            bool b4 = pDataMag[i * nCols + j] > pDataMag[(i + 1) * nCols + j];
            // 判断领域梯度是否满足大于水平或垂直梯度的条件，
            // 并根据自适应阈值参数进行二值化
            pDataDst[i * nCols + j] = 255 * (pDataMag[i * nCols + j] > thresh && (b1 && b2 || b3 && b4));
        }
    }
    dstTempMat.convertTo(dstTempMat, CV_8UC1);
    dst = dstTempMat.clone();
    return true;
}

/**************** 5-3. 图像直接卷积实现Sobel ********************/

bool sobelEdge(const Mat& src, Mat& dst, uchar threshold)
{
    CV_Assert(src.channels() == 1);
    // 初始化水平核因子
    Mat sobelX = (Mat_<double>(3,3) << 1, 0, -1, 
                                       2, 0, -2, 
                                       1, 0, -1);
    // 初始化垂直核因子
    Mat sobelY = (Mat_<double>(3,3) << 1, 2, 1,
                                       0, 0, 0,
                                       -1, -2, -1);
    dst = Mat::zeros(src.rows - 2, src.cols - 2, src.type());  
    double edgeX = 0, edgeY = 0, graMag = 0;
    for(int k = 1; k < src.rows - 1; ++k)
    {
        for(int n = 1; n < src.cols - 1; ++n)
        {
            edgeX = 0;
            edgeY = 0;
            // 遍历计算水平和垂直梯度
            for(int i = -1; i <= 1; ++i)
            {
                for(int j = -1; j <= 1; ++j)
                {
                    edgeX += src.at<uchar>(k+i, n+j) * sobelX.at<double>(i+1, j+1);
                    edgeY += src.at<uchar>(k+i, n+j) * sobelY.at<double>(i+1, j+1);
                }
            }
            // 计算梯度模长
            graMag = sqrt(pow(edgeX, 2) + pow(edgeX, 2));
            // 二值化
            dst.at<uchar>(k-1, n-1) = graMag > threshold? 255 : 0;
        }
    }
    return true;
}

/************* 5-4. 图像卷积实现Sobel非极大值抑制 **************/

bool sobelOptaEdge(const Mat& src, Mat& dst, int flag)
{
    CV_Assert(src.channels() == 1);
    // 初始化Sobel水平核因子
    Mat sobelX = (Mat_<double>(3,3) << 1, 0, -1,
                                       2, 0, -2,
                                       1, 0, -1);
    // 初始化Sobel垂直核因子
    Mat sobelY = (Mat_<double>(3,3) << 1, 2, 1, 
                                       0, 0, 0,
                                       1, -2, -1);
    // 计算水平与垂直卷积
    Mat edgeX, edgeY;
    filter2D(src, edgeX, CV_32F, sobelX);
    filter2D(src, edgeY, CV_32F, sobelY);
    // 根据传入参数确定计算水平或垂直边缘
    int paraX = 0, paraY = 0;
    switch(flag)
    {
    case 0:
        paraX = 1;
        paraY = 0;
        break;
    case 1:
        paraX = 0;
        paraY = 1;
        break;
    case 2:
        paraX = 1;
        paraY = 1;
        break;
    default: break;
    }
    edgeX = abs(edgeX);
    edgeY = abs(edgeY);
    Mat graMagMat = paraX * edgeX.mul(edgeX) + paraY * edgeY.mul(edgeY);
    // 计算阈值
    int scaleVal = 4;
    double thresh = scaleVal * mean(graMagMat).val[0];
    dst = Mat::zeros(src.size(), src.type());
    for(int i = 1; i < src.rows - 1; ++i)
    {
        float* pDataEdgeX = edgeX.ptr<float>(i);
        float* pDataEdgeY = edgeY.ptr<float>(i);
        float* pDataGraMag = graMagMat.ptr<float>(i);
        // 阈值化和极大值抑制
        for(int j = 1; j < src.cols - 1; ++j)
        {
            // 判断当前邻域梯度是否大于阈值与大于水平或垂直梯度
            if(pDataGraMag[j] > thresh && (
            (pDataEdgeX[j] > paraX * pDataEdgeY[j] && 
             pDataGraMag[j] > pDataGraMag[j-1] && 
             pDataGraMag[j] > pDataGraMag[j+1]) ||
            (pDataEdgeY[j] > paraY * pDataEdgeX[j] &&
             pDataGraMag[j] > pDataGraMag[j-1] &&
             pDataGraMag[j] > pDataGraMag[j+1])))
                dst.at<uchar>(i,j) = 255;
        }
    }
    return true;
}

//test
int main()
{
    Mat srcImg = imread("../../imgdata/building.jpg", 0);
    if(srcImg.empty())
        return -1;
    imshow("srcImage", srcImg);
    Mat resultImg;
    // 方法1：非极大值抑制细化竖直Sobel检测
    SobelVerEdge(srcImg, resultImg);
    imshow("SobelVerEdge", resultImg);
    Mat resultImg2;
    // 方法2：图像直接卷积实现Sobel检测
    sobelEdge(srcImg, resultImg2, 100);
    imshow("sobelEdge", resultImg2);
    Mat resultImg3;
    int flag = 2;
    // 方法3：图像卷积下非极大值抑制
    sobelOptaEdge(srcImg, resultImg3, flag);
    imshow("sobelOptaEdge", resultImg3);
    waitKey(0);
    return 0;
}
