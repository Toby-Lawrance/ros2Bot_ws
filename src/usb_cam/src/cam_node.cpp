#include "rclcpp/rclcpp.hpp"
#include<iostream>
#include<image_transport/image_transport.h>
#include<opencv2/core.hpp>
#include<cv_bridge/cv_bridge.h>
#include<sensor_msgs/image_encodings.hpp>
#include <camera_info_manager/camera_info_manager.h>

using namespace std;
using namespace cv;

class CameraDriver : public rclcpp::Node
{
public:
    CameraDriver(int width, int height, int frameRate) : Node("usb_cam_driver")
    {
        rmw_qos_profile_t custom_qos_profile = rmw_qos_profile_default;
        camera_pub = image_transport::create_camera_publisher(this,"image_raw",custom_qos_profile);

        cinfo_manager_ = make_shared<camera_info_managers::CameraInfoManager>(this);

        cap.open(0);
        cap.set(CAP_PROP_FRAME_WIDTH, width);
        cap.set(CAP_PROP_FRAME_HEIGHT, height);

        int delay = 1000/30;

        timer = this->create_wall_timer(delay, bind(ImgCallBack,this));
    }
private:
    rclcpp::TimerBase::SharedPtr timer;
	Mat frame;
	VideoCapture cap;
	cv_bridge::CvImage img_bridge;
	int img_h;
	int img_w;

    shared_ptr<camera_info_manager::CameraInfoManager> cinfo_manager_;
	image_transport::CameraPublisher camera_pub;
	shared_ptr<sensor_msgs::msg::Image> image_msg;

	void ImgCallBack()
    {
	    cap >> frame;
	    if(!frame.empty())
        {
	        image_msg = ConvertFrameToMessage(frame);

            sensor_msgs::msg::CameraInfo::SharedPtr camera_info_msg_(new sensor_msgs::msg::CameraInfo(cinfo_manager_->getCameraInfo()));

	        rclcpp::Time timestamp = this->get_clock()->now();

	        image_msg->header.stamp = timestamp;
            camera_info_msg_->header.stamp = timestamp;

            camera_pub.publish(image_msg, camera_info_msg_);
        }
    }

	shared_ptr<sensor_msgs::msg::Image> ConvertFrameToMessage(const Mat frame)
	{
        std_msgs::msg::Header header;
        img_bridge = cv_bridge::CvImage(header, sensor_msgs::image_encodings::BGR8, frame);
        image_msg_ = img_bridge.toImageMsg(); // from cv_bridge to sensor_msgs::msg::Image
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
