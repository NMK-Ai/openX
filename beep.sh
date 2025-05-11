#!/bin/bash

# 导出 GPIO42
echo 42 > /sys/class/gpio/export

# 设置 GPIO42 为输出
echo "out" > /sys/class/gpio/gpio42/direction

# 将 GPIO42 输出为高电平（1）
echo 1 > /sys/class/gpio/gpio42/value

# 等待一段时间（可选）
sleep 0.1

# 将 GPIO42 输出为低电平（0）
echo 0 > /sys/class/gpio/gpio42/value

# 取消导出 GPIO42（可选）
echo 42 > /sys/class/gpio/unexport
