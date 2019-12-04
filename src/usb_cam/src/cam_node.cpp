#include "rclcpp/rclcpp.hpp"
#include<iostream>
#include<image_transport/image_transport.h>
#include<opencv2/core.hpp>
#include<cv_bridge/cv_bridge.h>
#include<sensor_msgs/image_encodings.hpp>

using namespace std;
using namespace cv;

class CameraDriver : public rclcpp::Node("usb_cam_driver")
{
   private:
	Mat frame;
	VideoCapture cap;
	cv_bridge::CvImage img_bridge;
	int img_h;
	int img_w;

	shared_ptr<sensor_msgs::msg::Image> image_msg;

	shared_ptr<sensor_msgs::msg::Image> ConvertFrameToMessage(const Mat frame)
	{
		return nullptr;
	}
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc,argv);
  cout << "Init: " << CV_VERSION << endl;
  auto driver = make_shared<CameraDriver>();
  rclcpp::spin(driver);
  rclcpp::shutdown();
  return 0;
}
