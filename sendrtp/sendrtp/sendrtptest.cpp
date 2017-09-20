#include "SendRTP.h"
#include "SendSocket.h"
#include "H264Encoder.h"
#include "JPEGEncoder.h"
#include "JPEGDecoder.h"
#include <opencv2/opencv.hpp>

#if 1
int main(void)
{
	cv::VideoCapture capture(0);

	SendRTP rtp;
	rtp.Init();

	//Send a audio/vedio stream to server
    CRTPEncoder encoder(640, 480);  //width=640,height=480;
	while (1)
	{
		cv::Mat image;
		capture.read(image);
		cv::cvtColor(image, image, CV_BGR2YUV_I420);
		cv::imshow("image", image);
		cv::waitKey(30);
		char c = cv::waitKey(33);
		if (c == 'q')
		{
			break;
		}
		encoder.Encode(image.data);
		rtp.SendH264Nalu(encoder.packet.data, encoder.packet.size);
	}


    rtp.sess.BYEDestroy(jrtplib::RTPTime(10, 0), 0, 0);
	system("pause");
    return 0;
}
#else

int main()
{
	cv::Mat srcImage = cv::imread("cuc_view.jpg");
	int in_w = srcImage.cols;
	int in_h = srcImage.rows;
	cv::cvtColor(srcImage, srcImage, CV_BGR2YUV_I420);

	cv::imshow("send image", srcImage);
	cv::waitKey(0);

	//SendRTP rtp;
	//rtp.init();

	//CJPEGDecoder decoder;
	CJPEGEncoder encoder(480,272);
	encoder.SetSize(in_w, in_h);
	encoder.Encode(srcImage.data);


	//while (1)
	//{
	    //rtp.SendJPEG(encoder.packet.data, encoder.packet.size);		
	//}
    std::cout << encoder.packet.size << std::endl;

    CSendSocket s;
    s.Connect2Server();
    s.GetSize(in_w, in_h);
    s.Send2Server((char*)encoder.packet.data, encoder.packet.size);

	//rtp.sess.BYEDestroy(jrtplib::RTPTime(10, 0), 0, 0);


    system("pause");
	return 0;
}
#endif