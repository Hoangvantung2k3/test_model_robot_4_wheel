26/2/2024 test model : chạy thử được model mẫu có sẵn

đang fix chương trình điều khiển từ bàn phím ( teleop )

thêm các file navigation slam_gmapping openslam_gmapping thành công

hướng dẫn chạy thử :
- mở terminal
- cd nơi muốn lưu file
- git clone link
- catkin_make
- source devel/setup.bash
- roslaunch carmodel_description 02-drive.launch 
( lúc này sẽ hiển thị model có sẵn )
- roslaunch carmodel_teleop carmodel_teleop_key,launch 
( đang sửa file python tại mục này )