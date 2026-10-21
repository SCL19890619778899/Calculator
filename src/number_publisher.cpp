#include <chrono>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/int64.hpp"

using namespace std::chrono_literals;

/**
 * 简易计算器 - 数字发布器
 * 周期性向话题 /calculator/number 发布递增整数
 */
class NumberPublisher : public rclcpp::Node
{
public:
  NumberPublisher()
  : Node("number_publisher"), count_(0)
  {
    publisher_ = this->create_publisher<std_msgs::msg::Int64>("calculator/number", 10);
    timer_ = this->create_wall_timer(
      500ms, std::bind(&NumberPublisher::timer_callback, this));
    RCLCPP_INFO(this->get_logger(), "数字发布器已启动，话题: calculator/number");
  }

private:
  void timer_callback()
  {
    auto message = std_msgs::msg::Int64();
    message.data = count_++;
    RCLCPP_INFO(this->get_logger(), "发布数字: %ld", message.data);
    publisher_->publish(message);
  }

  rclcpp::Publisher<std_msgs::msg::Int64>::SharedPtr publisher_;
  rclcpp::TimerBase::SharedPtr timer_;
  int64_t count_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<NumberPublisher>());
  rclcpp::shutdown();
  return 0;
}
