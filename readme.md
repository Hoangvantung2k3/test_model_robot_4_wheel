

hướng dẫn chạy thử :
- mở terminal
- cd nơi muốn lưu file 
- git clone link
- lưu ý đây là bao gồm toàn bộ workspace nên nếu cần thì tự tạo workspace khác xong cop sang 
- catkin_make
- source devel/setup.bash

- model mới : roslaunch carmodel_description spawn.launch
- rosrun gazebo_ros gazebo --verbose
- rosrun carmodel_description o + tab + .py
- rosrun teleop_twist_keyboard teleop_twist_keyboard.py : lệnh này chạy cho nhanh, thay vì roslaunch file teleop
- mô hình 2d : roslaunch carmodel_description rviz.launch

mở rviz : ở terminal mặc định chạy lệnh rviz -> chọn open config -> tìm đến carmodel_description/launch -> chọn file default.rviz 

lưu ý: nhiều file cần cài thêm thư viện khi chạy, trong trường hợp trong máy ko có, post lỗi hiển thị ( khi khởi động ) lên mạng, sẽ có lệnh cài : sudo ...