exec >> "running.log" 2> "error.log"
ros2 run v4l2_camera v4l2_camera_node &
ros2 launch turtlebot3_bringup robot.launch.py &