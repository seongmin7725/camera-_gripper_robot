# camera_gripper_robot

명지대학교 응용로봇공학 중간고사 프로젝트로 플라스틱병과 콜라켄을 각각 파랑색 빨간색 구역으로 옮기는 프로젝트를 진행하였다.

# [1. 사용한 부품]
1. tetrix 로봇 키트
2. 허스키렌즈


# [2. 알고리즘]
1. 허스키렌즈의 object tracking 기능을 이용해 로봇이 플라스틱병으로 이동하게 한다. 이때 로봇과 플라스틱병 사이의 거리는 ultrasonic sensor를 활용하여 측정하였다.
2. 허스키렌즈의 object classification 기능을 통해서 사전에 학습된 목표지점과 주변환경을 활용해 목표지점이 나올때까지 회전하였다. 목표지점이 인식되면 180도 회전하였다.
3. 허스키렌즈의 color recognition을 활용해 빨간색 혹은 파란색이 인식될때 까지 후진하고 인식되면 일정 거리만큼 전진후 오른쪽으로 90도 회전하였다.
4. 플라스틱병은 파란색 구역으로 옮겨야함으로 허스키렌즈의 color recognition으로 파란색을 인식해 플라스틱 물병을 파란색 구역에 내려 놓은다.
5. 콜라켄을 찾기 위해서 ultrasonic sensor를 활용해 로봇이 회전하면서 특정 거리 이하로 되는 곳을 향해서 직진한다. 
6. 이후 로봇이 콜라켄을 빨간색 위치까지 옮기는 방법은 플라스틱병을 허스키렌즈의 object classification과 color recognition을 활용해 정해진 위치로 옮기는 방법과 동일하다.

# [3. 맵]
![map](https://user-images.githubusercontent.com/102838128/168476737-5ebc5b26-bed1-41c3-9daa-7f75acc07be9.jpg)

# [4. 동영상]
https://youtu.be/RutjSwYbgHE

# [5. 문제점]
ultrasonic sensor을 활용해서 콜라켄을 잡기 위해서 회전시 주변에 다른 물체가 있으면 그 물체 방향으로 직진한다.

# [6. TO-DO]
1. 콜라켄을 잡기위한 다른 센서를 사용해 보기
2. ESP32를 추가적으로 장착해서 ros로 pc와 통신해서 gazebo simulator를 사용해 보기
