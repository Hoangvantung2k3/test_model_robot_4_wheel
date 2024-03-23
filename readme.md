
đang fix chương trình điều khiển từ bàn phím ( teleop )

thêm các file navigation slam_gmapping openslam_gmapping thành công

hướng dẫn chạy thử :
- mở terminal
- cd nơi muốn lưu file
- git clone link
- catkin_make
- source devel/setup.bash
- roslaunch carmodel_description 02-drive.launch 
( lúc này sẽ hiển thị model có sẵn ) ( đã có model chuẩn)
- roslaunch carmodel_teleop carmodel_teleop_key.launch 
( đang sửa file python tại mục này ) ( đã sửa xong, đã điều khiển được nhưng khó điều khiển, sửa lại file điều khiển)

- model mới : roslaunch carmodel_description spawn.launch
- rosrun gazebo_ros gazebo --verbose
- rosrun carmodel_description o + tab + .py
- rosrun teleop_twist_keyboard teleop_twist_keyboard.py : lệnh này chạy cho nhanh, thay vì roslaunch file teleop
- mô hình 2d : roslaunch m2wr_description rviz.launch

