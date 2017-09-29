#include "SendRTP.h"
#include "SendSocket.h"
#include "H264Encoder.h"
#include "JPEGEncoder.h"
#include "JPEGDecoder.h"
#include <opencv2/opencv.hpp>
#include "zip.h"
#include "unzip.h"
#include <Windows.h>

#if 0
int main(void)
{
    cv::VideoCapture capture(0);

    SendRTP rtp;
    rtp.Init();

    CRTPEncoder encoder(640, 480);  //width=640,height=480;
    while (1)
    {
        cv::Mat image;
        capture.read(image);
        cv::cvtColor(image, image, CV_BGR2YUV_I420);
        cv::imshow("image", image);
        char c = cv::waitKey(33);
        if (c == 'q')
        {
            break;
        }
        encoder.Encode(image.data);
        rtp.SendH264Nalu(encoder.packet.data, encoder.packet.size);
    }
    rtp.DeInit();
    system("pause");
    return 0;
}
#else

int main()
{
    cv::Mat srcImage = cv::imread("zebra.jpg");
    int in_w = srcImage.cols;
    int in_h = srcImage.rows;
    in_w = in_w - in_w % 32;
    in_h = in_h - in_h % 2;
    cv::resize(srcImage, srcImage, cv::Size(in_w, in_h));
    std::cout << srcImage.channels() << "srcImage size : " << srcImage.cols << " * " << srcImage.rows << std::endl;
    cv::cvtColor(srcImage, srcImage, CV_BGR2YUV_I420);
    cv::imshow("send image", srcImage);
    cv::waitKey(0);

    //SendRTP rtp;
    //rtp.init();

    //CJPEGDecoder decoder;
    CJPEGEncoder encoder(in_w, in_h);
    encoder.Encode(srcImage.data);


    //while (1)
    //{
        //rtp.SendJPEG(encoder.packet.data, encoder.packet.size);        
    //}
    std::cout << "packet size: " <<encoder.packet.size << std::endl;

    CSendSocket s;
    s.Connect2Server();
    s.GetSize(in_w, in_h);
    while (1)
    {
        
        s.Send2Server((char*)encoder.packet.data, encoder.packet.size);
        double start = GetTickCount();
        s.RecvRes();
        double end = GetTickCount();
        std::cout << "GetTickCount: " << end - start << std::endl;
    }

    //char* pImage = new char[MAX_IMAGE_SIZE];
    //int sizeImage;
    //s.RecvImage(pImage, sizeImage);

    //CJPEGDecoder decoder;
    //if (!decoder.Decode((uint8_t*)pImage, sizeImage))
    //{
    //    std::cout << "decoder successful!" << std::endl;
    //}

    //int width;
    //int height;
    //decoder.GetSize(width, height);
    //std::cout << width << " && " << height << std::endl;
    //cv::Mat dstImage(cv::Size(480, 272 * 3 / 2), CV_8UC1, cv::Scalar(255));
    //decoder.GetData(dstImage.data);
    //cv::cvtColor(dstImage, dstImage, CV_YUV2BGRA_I420);
    //cv::imshow("result", dstImage);
    //cv::waitKey(0);

    ////rtp.sess.BYEDestroy(jrtplib::RTPTime(10, 0), 0, 0);


    system("pause");
    return 0;
}
//#else
//
//int main()
//{
//    HZIP sendZip = CreateZip("sendZip.zip", 0);
//    ZipAdd(sendZip,  "zncuc_view.jpg", "cuc_view.jpg");
//    ZipAdd(sendZip, "znlena.jpg", "lena.jpg");
//    CloseZip(sendZip);
//}
//
#endif
