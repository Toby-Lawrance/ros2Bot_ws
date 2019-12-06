exec >> "running.log" 2> "error.log"
ros2 run usb_cam cam_node &
ros2 launch turtlebot3_bringup robot.launch.py &