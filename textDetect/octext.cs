using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
  
using OpenCvSharp;
using OpenCvSharp.Extensions;
using OpenCvSharp.Utilities;
  
namespace OpenCvTest
{
    class Program
    {
        static void Main(string[] args)
        {
            //����Դ�ļ�
            var src = IplImage.FromFile("source.jpg");
                 
            //ת�����Ҷ�ͼ
            var gray = Cv.CreateImage(src.Size, BitDepth.U8, 1);
            Cv.CvtColor(src, gray, ColorConversion.BgrToGray);
                 
            //��һ�����ͣ�x��y����������ϵ����ͬ
            //ʹ����Erode��������ʴ��������԰�ɫ�������Ե�Ч�ڶ����ֽ���������
            var kernal = Cv.CreateStructuringElementEx(5, 2, 1, 1, ElementShape.Rect);
            Cv.Erode(gray, gray, kernal, 2);
                 
            //��ֵ��
            Cv.Threshold(gray, gray, 0, 255, ThresholdType.BinaryInv | ThresholdType.Otsu);
                 
            //�����ͨ��ÿһ����ͨ����һϵ�еĵ��ʾ��FindContours����ֻ�ܵõ���һ����
            var storage = Cv.CreateMemStorage();
            CvSeq<CvPoint> contour = null;
            Cv.FindContours(gray, storage, out contour, CvContour.SizeOf, ContourRetrieval.CComp, ContourChain.ApproxSimple);
            var color = new CvScalar(0, 0, 255);
                 
            //��ʼ����
            while (contour != null)
            {
                //�õ������ͨ�������Ӿ���
                var rect = Cv.BoundingRect(contour);
                     
                //����߶Ȳ��㣬���߳����̫С����Ϊ����Ч���ݣ�����Ѿ��λ���ԭͼ��
                if(rect.Height > 10 && (rect.Width * 1.0 / rect.Height) > 0.2)
                    Cv.DrawRect(src, rect, color);
                         
                //ȡ��һ����ͨ��
                contour = contour.HNext;
            }
            Cv.ReleaseMemStorage(storage);
                 
            //��ʾ
            Cv.ShowImage("Result", src);
            Cv.WaitKey();
            Cv.DestroyAllWindows();
        }
    }
}