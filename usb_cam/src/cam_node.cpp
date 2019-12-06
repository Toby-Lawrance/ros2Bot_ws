#include "rclcpp/rclcpp.hpp"
#include<iostream>
#include<chrono>
#include<image_transport/image_transport.h>
#include<opencv2/core.hpp>
#include<opencv2/imgproc.hpp>
#include<opencv2/opencv.hpp>
#include<cv_bridge/cv_bridge.h>
#include<sensor_msgs/image_encodings.hpp>
#include<camera_info_manager/camera_info_manager.h>

using namespace std;
using namespace cv;
using namespace chrono;

class CameraDriver : public rclcpp::Node
{
public:
    CameraDriver(int width, int height, int frameRate) : Node("usb_cam_driver")
    {
        rmw_qos_profile_t custom_qos_profile = rmw_qos_profile_default;
        camera_pub = image_transport::create_publisher(this,"image_raw",custom_qos_profile);

        //cinfo_manager_ = make_shared<camera_info_manager::CameraInfoManager>(this);

        cap.open(0);
        cap.set(CV_CAP_PROP_FRAME_WIDTH, width);
        cap.set(CV_CAP_PROP_FRAME_HEIGHT, height);
	cap.set(CV_CAP_PROP_FPS, frameRate);

        auto delay = milliseconds(1000/frameRate);

        timer = this->create_wall_timer(delay, bind(&CameraDriver::ImgCallBack,this));
    }
private:
    	rclcpp::TimerBase::SharedPtr timer;
	Mat frame;

	cv::VideoCapture cap;

	cv_bridge::CvImage img_bridge;

    	//shared_ptr<camera_info_manager::CameraInfoManager> cinfo_manager_;
	image_transport::Publisher camera_pub;
	shared_ptr<sensor_msgs::msg::Image> image_msg;

	void ImgCallBack()
    {
	    cap >> frame;
	    //cout << "Received a frame" << endl;
	    if(!frame.empty())
        {
	        image_msg = ConvertFrameToMessage(frame);

            //sensor_msgs::msg::CameraInfo::SharedPtr camera_info_msg_(new sensor_msgs::msg::CameraInfo(cinfo_manager_->getCameraInfo()));

	        rclcpp::Time timestamp = this->get_clock()->now();

	        image_msg->header.stamp = timestamp;
            //camera_info_msg_->header.stamp = timestamp;

            camera_pub.publish(image_msg);
        }
    }

	shared_ptr<sensor_msgs::msg::Image> ConvertFrameToMessage(const Mat frame)
	{
        std_msgs::msg::Header header;
        img_bridge = cv_bridge::CvImage(header, sensor_msgs::image_encodings::BGR8, frame);
        image_msg = img_bridge.toImageMsg(); // from cv_bridge to sensor_msgs::msg::Image
        return image_msg;
	}
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc,argv);
  cout << "Init: " << CV_VERSION << endl;
  auto driver = make_shared<CameraDriver>(640,480,30);
  rclcpp::spin(driver);
  rclcpp::shutdown();
  return 0;
}
