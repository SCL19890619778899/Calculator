#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/int64.hpp"

/**
 * 简易计算器 - 数字订阅器
 * 订阅话题 /calculator/number 并打印收到的整数
 */
class NumberSubscriber : public rclcpp::Node
{
public:
  NumberSubscriber()
  : Node("number_subscriber")
  {
    subscription_ = this->create_subscription<std_msgs::msg::Int64>(
      "calculator/number", 10,
      std::bind(&NumberSubscriber::topic_callback, this, std::placeholders::_1));
    RCLCPP_INFO(this->get_logger(), "数字订阅器已启动，话题: calculator/number");
  }

private:
  void topic_callback(const std_msgs::msg::Int64::SharedPtr msg) const
  {
    RCLCPP_INFO(this->get_logger(), "收到数字: %ld", msg->data);
  }

  rclcpp::Subscription<std_msgs::msg::Int64>::SharedPtr subscription_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<NumberSubscriber>());
  rclcpp::shutdown();
  return 0;
}
