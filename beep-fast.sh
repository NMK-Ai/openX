#!/bin/bash

# 导出 GPIO42
sudo echo 42 > /sys/class/gpio/export

# 设置 GPIO42 为输出
sudo echo "out" > /sys/class/gpio/gpio42/direction

# 设置快速切换频率的次数
for i in {1..20}
do
  # 将 GPIO42 输出为高电平（1）
  sudo echo 1 > /sys/class/gpio/gpio42/value

  # 等待短时间
  sleep 0.05

  # 将 GPIO42 输出为低电平（0）
  sudo echo 0 > /sys/class/gpio/gpio42/value

  # 等待短时间
  sleep 0.05
done

# 取消导出 GPIO42（可选）
sudo echo 42 > /sys/class/gpio/unexport
