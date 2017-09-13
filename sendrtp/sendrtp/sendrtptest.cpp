/*
Here's a small IPv4 example: it asks for a portbase and a destination and
starts sending packets to that destination.
*/
#include "SendRTP.h"

int main(void)
{
	CvCapture* pCapture;
	IplImage* pCVFrame;
	cv::Mat image;

	pCapture = cvCreateCameraCapture(0);
	pCVFrame = NULL;

	SendRTP rtp;
	rtp.init();

	CEncoder encoder(640, 480);  //width=640,height=480;
	while (1)
	{
		pCVFrame = cvQueryFrame(pCapture);
		image = cv::Mat(pCVFrame);
		cv::cvtColor(image, image, CV_BGR2YUV_YV12);
		cv::imshow("image", image);

		char c = cv::waitKey(33);
		if (c == 'q')
		{
			break;
		}
		encoder.Encode(image.data);
		rtp.SendH264Nalu(encoder.packet.data, encoder.packet.size);
		encoder.FreePacket();
	}

	rtp.sess.BYEDestroy(RTPTime(10, 0), 0, 0);
    return 0;
}

