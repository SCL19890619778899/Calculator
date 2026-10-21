#include <chrono>
#include <memory>
#include <string>

#include "gtest/gtest.h"
#include "rclcpp/rclcpp.hpp"
#include "example_interfaces/srv/add_two_ints.hpp"

using namespace std::chrono_literals;

/**
 * add_two_ints 服务单元测试（ROS 2 gtest，对应 ROS 1 rostest）
 * 在同一进程内创建服务端与客户端，覆盖 5 个以上代表性用例
 */
class AddTwoIntsTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    // 每个用例使用独立节点名，避免命名冲突
    static int case_id = 0;
    const std::string suffix = std::to_string(case_id++);

    server_node_ = rclcpp::Node::make_shared("test_add_server_" + suffix);
    client_node_ = rclcpp::Node::make_shared("test_add_client_" + suffix);

    service_ = server_node_->create_service<example_interfaces::srv::AddTwoInts>(
      "add_two_ints_test_" + suffix,
      [](const std::shared_ptr<example_interfaces::srv::AddTwoInts::Request> request,
      std::shared_ptr<example_interfaces::srv::AddTwoInts::Response> response)
      {
        response->sum = request->a + request->b;
      });

    client_ = client_node_->create_client<example_interfaces::srv::AddTwoInts>(
      "add_two_ints_test_" + suffix);

    executor_ = std::make_shared<rclcpp::executors::SingleThreadedExecutor>();
    executor_->add_node(server_node_);
    executor_->add_node(client_node_);
  }

  void TearDown() override
  {
    executor_->cancel();
    executor_->remove_node(server_node_);
    executor_->remove_node(client_node_);
    client_.reset();
    service_.reset();
    client_node_.reset();
    server_node_.reset();
  }

  // 调用加法服务并返回结果
  int64_t call_add(int64_t a, int64_t b)
  {
    if (!client_->wait_for_service(2s)) {
      ADD_FAILURE() << "服务未就绪";
      return 0;
    }

    auto request = std::make_shared<example_interfaces::srv::AddTwoInts::Request>();
    request->a = a;
    request->b = b;

    auto future = client_->async_send_request(request);
    auto status = executor_->spin_until_future_complete(future, 3s);
    if (status != rclcpp::FutureReturnCode::SUCCESS) {
      ADD_FAILURE() << "服务调用超时或失败";
      return 0;
    }
    return future.get()->sum;
  }

  rclcpp::Node::SharedPtr server_node_;
  rclcpp::Node::SharedPtr client_node_;
  rclcpp::Service<example_interfaces::srv::AddTwoInts>::SharedPtr service_;
  rclcpp::Client<example_interfaces::srv::AddTwoInts>::SharedPtr client_;
  rclcpp::executors::SingleThreadedExecutor::SharedPtr executor_;
};

// 用例1：两个正整数相加
TEST_F(AddTwoIntsTest, PositiveNumbers)
{
  EXPECT_EQ(call_add(3, 5), 8);
  EXPECT_EQ(call_add(100, 200), 300);
}

// 用例2：两个负整数相加
TEST_F(AddTwoIntsTest, NegativeNumbers)
{
  EXPECT_EQ(call_add(-3, -5), -8);
  EXPECT_EQ(call_add(-10, -20), -30);
}

// 用例3：与零相加（加法单位元）
TEST_F(AddTwoIntsTest, AddWithZero)
{
  EXPECT_EQ(call_add(42, 0), 42);
  EXPECT_EQ(call_add(0, -7), -7);
  EXPECT_EQ(call_add(0, 0), 0);
}

// 用例4：正负混合相加
TEST_F(AddTwoIntsTest, MixedSigns)
{
  EXPECT_EQ(call_add(10, -3), 7);
  EXPECT_EQ(call_add(-15, 20), 5);
  EXPECT_EQ(call_add(8, -8), 0);
}

// 用例5：较大整数（边界附近）
TEST_F(AddTwoIntsTest, LargeNumbers)
{
  EXPECT_EQ(call_add(1000000, 2000000), 3000000);
  EXPECT_EQ(call_add(-1000000, 1000000), 0);
  EXPECT_EQ(call_add(999999999LL, 1LL), 1000000000LL);
}

// 用例6（额外）：连续多次调用一致性
TEST_F(AddTwoIntsTest, RepeatedCallsConsistency)
{
  for (int i = 0; i < 5; ++i) {
    EXPECT_EQ(call_add(i, i + 1), 2 * i + 1);
  }
}

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  testing::InitGoogleTest(&argc, argv);
  int result = RUN_ALL_TESTS();
  rclcpp::shutdown();
  return result;
}
