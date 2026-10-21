cmake_minimum_required(VERSION 3.8)
project(Calculator)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# 查找依赖
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(example_interfaces REQUIRED)

# ---------- 发布器 ----------
add_executable(number_publisher src/number_publisher.cpp)
ament_target_dependencies(number_publisher rclcpp std_msgs)

# ---------- 订阅器 ----------
add_executable(number_subscriber src/number_subscriber.cpp)
ament_target_dependencies(number_subscriber rclcpp std_msgs)

# ---------- 加法服务端 ----------
add_executable(add_two_ints_server src/add_two_ints_server.cpp)
ament_target_dependencies(add_two_ints_server rclcpp example_interfaces)

# ---------- 加法客户端 ----------
add_executable(add_two_ints_client src/add_two_ints_client.cpp)
ament_target_dependencies(add_two_ints_client rclcpp example_interfaces)

# 安装可执行文件
install(TARGETS
  number_publisher
  number_subscriber
  add_two_ints_server
  add_two_ints_client
  DESTINATION lib/${PROJECT_NAME}
)

# 安装 launch 文件
install(DIRECTORY launch
  DESTINATION share/${PROJECT_NAME}
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  set(ament_cmake_copyright_FOUND TRUE)
  set(ament_cmake_cpplint_FOUND TRUE)
  # xmllint 需在线拉取 XSD，离线环境跳过
  set(ament_cmake_xmllint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()

  # gtest 单元测试（ROS 2 中对应 ROS 1 rostest/gtest）
  find_package(ament_cmake_gtest REQUIRED)
  ament_add_gtest(test_add_two_ints test/test_add_two_ints.cpp)
  if(TARGET test_add_two_ints)
    ament_target_dependencies(test_add_two_ints rclcpp example_interfaces)
  endif()

  # launch_testing 集成测试（对应 rostest 启动测试）
  find_package(launch_testing_ament_cmake REQUIRED)
  add_launch_test(test/test_add_two_ints.launch.py)
endif()

ament_package()
