#include <chrono>
#include <cstdlib>
#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "example_interfaces/srv/add_two_ints.hpp"

using namespace std::chrono_literals;

/**
 * 简易计算器 - 两整数相加客户端
 * 用法: ros2 run Calculator add_two_ints_client <a> <b>
 */
int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);

  if (argc != 3) {
    RCLCPP_ERROR(
      rclcpp::get_logger("add_two_ints_client"),
      "用法: add_two_ints_client <整数a> <整数b>");
    return 1;
  }

  auto node = rclcpp::Node::make_shared("add_two_ints_client");
  auto client = node->create_client<example_interfaces::srv::AddTwoInts>("add_two_ints");

  // 等待服务可用
  while (!client->wait_for_service(1s)) {
    if (!rclcpp::ok()) {
      RCLCPP_ERROR(node->get_logger(), "等待服务时被中断");
      return 1;
    }
    RCLCPP_INFO(node->get_logger(), "等待服务 add_two_ints ...");
  }

  auto request = std::make_shared<example_interfaces::srv::AddTwoInts::Request>();
  request->a = std::atoll(argv[1]);
  request->b = std::atoll(argv[2]);

  auto future = client->async_send_request(request);
  if (rclcpp::spin_until_future_complete(node, future) ==
    rclcpp::FutureReturnCode::SUCCESS)
  {
    RCLCPP_INFO(
      node->get_logger(), "结果: %ld + %ld = %ld",
      request->a, request->b, future.get()->sum);
  } else {
    RCLCPP_ERROR(node->get_logger(), "调用服务失败");
    return 1;
  }

  rclcpp::shutdown();
  return 0;
}
