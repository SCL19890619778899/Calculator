# Calculator 功能包测试报告 / Test Report

**功能包名称 (Package):** Calculator  
**版本 (Version):** 1.0.0  
**ROS 发行版 (ROS Distro):** ROS 2 Humble  
**语言 (Language):** C++  
**日期 (Date):** 2026-10-21  
**作者 (Author):** scl  

---

## 一、概述 / Overview

本报告记录简易计算器功能包 Calculator 的功能实现与测试结果。该功能包基于 ROS 2，使用 C++ 实现话题通信、服务调用，并在 gtest / launch_testing（对应 ROS 1 的 rostest）框架下完成单元测试与集成测试。

This report documents the features and test results of the Calculator ROS package. It implements topic communication and service calls in C++ on ROS 2 Humble, with unit/integration tests under gtest and launch_testing (ROS 2 equivalents of rostest).

---

## 二、功能清单 / Feature List

| 序号 | 功能 | 节点 / 文件 | 状态 |
|------|------|-------------|------|
| 1 | 功能包创建与编译 | `package.xml`, `CMakeLists.txt` | 通过 Pass |
| 2 | 数字发布器 (Publisher) | `number_publisher` | 通过 Pass |
| 3 | 数字订阅器 (Subscriber) | `number_subscriber` | 通过 Pass |
| 4 | 加法服务端 (Server) | `add_two_ints_server` | 通过 Pass |
| 5 | 加法客户端 (Client) | `add_two_ints_client` | 通过 Pass |
| 6 | Launch 启动文件 | `launch/calculator.launch.py` | 通过 Pass |
| 7 | 单元测试 (≥5) | `test/test_add_two_ints.cpp` | 通过 Pass |
| 8 | Launch 测试 (rostest 对应) | `test/test_add_two_ints.launch.py` | 通过 Pass |

### 2.1 话题通信 / Topic Communication

- **话题 (Topic):** `calculator/number`
- **消息类型 (Message):** `std_msgs/msg/Int64`
- **发布器:** 每 500ms 发布一个递增整数
- **订阅器:** 接收并打印整数

### 2.2 服务通信 / Service Communication

- **服务名 (Service):** `add_two_ints`
- **服务类型 (Type):** `example_interfaces/srv/AddTwoInts`
- **请求 (Request):** `a`, `b`（int64）
- **响应 (Response):** `sum = a + b`（int64）

### 2.3 启动方式 / How to Launch

```bash
# 编译
cd ~/文档/Calculator
source /opt/ros/humble/setup.bash
colcon build --packages-select Calculator
source install/setup.bash

# 使用 launch 同时启动发布器、订阅器、服务端
ros2 launch Calculator calculator.launch.py

# 单独调用客户端
ros2 run Calculator add_two_ints_client 3 5
```

---

## 三、测试环境 / Test Environment

| 项目 Item | 内容 Content |
|-----------|--------------|
| 操作系统 OS | Linux |
| ROS | ROS 2 Humble Hawksbill |
| 构建工具 Build | colcon + ament_cmake |
| 测试框架 Framework | ament_cmake_gtest + launch_testing |
| 编译器 Compiler | g++ (C++17) |

说明：ROS 2 中传统 ROS 1 的 `rostest` 由 `ament_cmake_gtest`（单元测试）与 `launch_testing`（启动集成测试）替代，本项目按此方式复现 rostest 工作流。

Note: In ROS 2, classic rostest is replaced by ament_cmake_gtest and launch_testing; this package follows that workflow.

---

## 四、单元测试用例 / Unit Test Cases

测试文件：`test/test_add_two_ints.cpp`  
目标服务：`add_two_ints`（`example_interfaces/srv/AddTwoInts`）

| 编号 | 用例名称 | 测试意图 | 输入示例 | 期望结果 | 结果 |
|------|----------|----------|----------|----------|------|
| T1 | PositiveNumbers | 两个正整数相加 | 3+5, 100+200 | 8, 300 | 通过 Pass |
| T2 | NegativeNumbers | 两个负整数相加 | -3+(-5), -10+(-20) | -8, -30 | 通过 Pass |
| T3 | AddWithZero | 与零相加（单位元） | 42+0, 0+(-7), 0+0 | 42, -7, 0 | 通过 Pass |
| T4 | MixedSigns | 正负混合 | 10+(-3), -15+20, 8+(-8) | 7, 5, 0 | 通过 Pass |
| T5 | LargeNumbers | 较大整数 | 1e6+2e6, ±1e6, 999999999+1 | 3e6, 0, 1e9 | 通过 Pass |
| T6 | RepeatedCallsConsistency | 连续多次调用一致性 | i+(i+1), i=0..4 | 2i+1 | 通过 Pass |

### 4.1 测试方法说明 / Test Method

每个用例在同一进程内创建临时服务端与客户端，通过 `SingleThreadedExecutor` 完成服务调用，验证 `sum == a + b`。该方式等价于 rostest 下对服务节点的黑盒单元测试。

Each case creates an in-process server and client, spins a SingleThreadedExecutor, and asserts `sum == a + b`, equivalent to rostest-style black-box service tests.

### 4.2 运行命令 / Run Command

```bash
source ~/文档/Calculator/install/setup.bash
colcon test --packages-select Calculator --event-handlers console_direct+
colcon test-result --verbose
```

---

## 五、集成测试 / Integration Test

测试文件：`test/test_add_two_ints.launch.py`

| 编号 | 用例 | 说明 | 结果 |
|------|------|------|------|
| I1 | test_server_starts | 验证 `add_two_ints_server` 经 launch 正常启动 | 通过 Pass |
| I2 | test_exit_codes | 验证测试结束后进程退出码在允许范围内 | 通过 Pass |

---

## 六、功能验证步骤 / Manual Verification

1. **编译:** `colcon build --packages-select Calculator` → 成功  
2. **Launch:** `ros2 launch Calculator calculator.launch.py` → 三节点同时运行  
3. **话题:** `ros2 topic echo /calculator/number` → 可见递增整数  
4. **服务:** `ros2 run Calculator add_two_ints_client 7 8` → 输出 `7 + 8 = 15`  
5. **自动化测试:** `colcon test --packages-select Calculator` → 全部通过  

---

## 七、Git 工作流 / Git Workflow

| 分支 Branch | 内容 Content | 合并到 master |
|-------------|--------------|---------------|
| `master` | 初始功能包骨架 | — |
| `feature/publisher-subscriber` | 发布器与订阅器 | 是 Yes |
| `feature/service-client` | 服务端与客户端 | 是 Yes |
| `feature/launch` | Launch 启动文件 | 是 Yes |
| `feature/rostest` | 测试目录与单元/集成测试 | 是 Yes |
| `feature/test-report` | 本测试报告 | 是 Yes |

各功能在独立分支开发，测试通过后合并回 `master` 主分支，并推送至 GitHub。

Features were developed on independent branches and merged into `master` after tests passed, then pushed to GitHub.

---

## 八、结论 / Conclusion

Calculator 功能包已完成话题发布/订阅、加法服务端/客户端、Launch 启动、以及不少于 5 个代表性单元测试。自动化测试与手工验证均通过，满足课程/项目要求。

The Calculator package delivers publisher/subscriber, add_two_ints server/client, launch files, and at least five representative unit tests. Automated and manual verification all passed.

---

## 九、附录：目录结构 / Appendix: Tree

```
Calculator/
├── CMakeLists.txt
├── package.xml
├── docs/
│   └── test_report.md
├── launch/
│   └── calculator.launch.py
├── src/
│   ├── number_publisher.cpp
│   ├── number_subscriber.cpp
│   ├── add_two_ints_server.cpp
│   └── add_two_ints_client.cpp
└── test/
    ├── test_add_two_ints.cpp
    └── test_add_two_ints.launch.py
```
