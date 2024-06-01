

hướng dẫn chạy thử :
- mở terminal
- cd nơi muốn lưu file 
- git clone link
- lưu ý đây là bao gồm toàn bộ workspace nên nếu cần thì tự tạo workspace khác xong cop sang 
- catkin_make
- source devel/setup.bash

- model mới : roslaunch carmodel_description spawn.launch
- rosrun carmodel_description o + tab + .py
- rosrun teleop_twist_keyboard teleop_twist_keyboard.py : lệnh này chạy cho nhanh, thay vì roslaunch file teleop (rosrun carmodel_teleop carmodel_teleop_key.launch)
- khởi chạy PID :   rosrun carmodel_description BotControl
- chỉ định địa chỉ : 
<!--cop toàn bộ nội dung sau vào terminal-->
rosservice call /gazebo/set_model_state "model_state:
  model_name: 'bot_description'
  pose:
    position: { x: 5.0, y: 5.0, z: 0.0 }
    orientation: { x: 0.0, y: 0.0, z: 0.707, w: 0.707 }
  twist:
    linear: { x: 0.0, y: 0.0, z: 0.0 }
    angular: { x: 0.0, y: 0.0, z: 0.0 }
  reference_frame: 'world'"
<!--cho đến hết đoạn này -->

mở rviz : đã khởi chạy luôn trong file launch
xem thông số tọa độ thông qua camera : rostopic echo /T265_camera/odom/sample 
lưu ý: nhiều file cần cài thêm thư viện khi chạy, trong trường hợp trong máy ko có, post lỗi hiển thị ( khi khởi động ) lên mạng, sẽ có lệnh cài : sudo ... ( thường thiếu ở navigation dùng để chạy trong rviz)