#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import numpy as np
from pulse_produce import produce_one_signal

signal_numbers = 1000


def getkey(x):
    return x[0]


def produce_signals():
    # produce signal
    # signal1 = produce_one_signal(35, 3, 600, 0.5, 10, signal_numbers, 0)
    # signal2 = produce_one_signal(21, 90, 860, 6.0, 50, signal_numbers, 1)
    # signal3 = produce_one_signal(69, 45, 1100, 23, 90, signal_numbers, 2)
    signal1 = produce_one_signal(1, 1, 1, 1, 1, signal_numbers, 0)
    signal2 = produce_one_signal(50, 50, 50, 50, 50, signal_numbers, 1)
    signal3 = produce_one_signal(100, 100, 100, 100, 100, signal_numbers, 2)

    # 合并所有的信号list
    signals = []
    signals += signal1
    signals += signal2
    signals += signal3
    # 依据到达时间进行排序
#    signals.sort(key=getkey)
    np.random.shuffle(signals)  # 随机shuffle一下

    return signals


data_dir = './data'
training_file = data_dir + '/training_file.txt'
testing_file = data_dir + '/testing_file.txt'
validation_file = data_dir + '/validation_file.txt'


def write_list(fd, data):
    for num in data:
        fd.write(str(num))
        fd.write(' ')


# 将产生的信号数据给写入到list文件之中
def generate_file(signals):
    if not os.path.exists(data_dir):
        os.makedirs(data_dir)
    length = len(signals) // 2
    training_data = signals[: length]
    tmp_data = signals[length:]
    length1 = length // 2
    testing_data = tmp_data[length1:]
    validation_data = tmp_data[: length1]

    fd1 = open(training_file, 'w')
    i = 0
    for i in range(length):
        # fd1.write(str(training_data[i]))
        write_list(fd1, training_data[i])
        fd1.write('\n')
    fd1.close()

    fd2 = open(testing_file, 'w')
    j = 0
    for j in range(length1):
        # fd2.write(str(testing_data[j]))
        write_list(fd2, testing_data[j])
        fd2.write('\n')
    fd2.close()

    fd3 = open(validation_file, 'w')
    k = 0
    for k in range(length1):
        # fd2.write(str(testing_data[j]))
        write_list(fd3, validation_data[k])
        fd3.write('\n')
    fd3.close()


# 从文件中读取，形成数组， 注意， 都是浮点数
def read_file(file):
    fd = open(file, 'r')
    list_data = []
    for line in fd.readlines():
        linestr = line[0: -2]  # 截取字符串, 构造的时候， 每行多加了2个字符 ‘ ’ ， ‘\n’
        tmpstr = linestr.split(' ')
        linelist = list(map(float, tmpstr))
        list_data.append(linelist)
    fd.close()

    return list_data


def generate_batch(batch_size, signals):
    n_chunk = len(signals) // batch_size
    x_batches = []
    y_batches = []

    for i in range(n_chunk):
        start_index = i * batch_size
        end_index = start_index + batch_size

        batches = signals[start_index:end_index]

        x_data = np.zeros((batch_size, 5))  # todo
        y_data = np.zeros((batch_size, 1), dtype=int)
        for row in range(batch_size):
            batch = batches[row]
            x_data[row, :] = batch[: -1]  # 实际上就是取数据， 然后生成对应的batch
            y_data[row, :] = int(batch[-1])  # 转化为整形

        x_batches.append(x_data)
        y_batches.append(y_data)

    return x_batches, y_batches


# 外部接口函数 1
# 产生信号输入文件
def generate_input_files():
    signals = produce_signals()
    generate_file(signals)


# 外部接口函数 2
# 读取训练数据， 形成批次
def get_training_data(batch_size):
    signals = read_file(training_file)
    x_batches, y_batches = generate_batch(batch_size, signals)
    return x_batches, y_batches


def get_validation_data(batch_size):
    signals = read_file(validation_file)
    x_batches, y_batches = generate_batch(batch_size, signals)
    return x_batches, y_batches


# 外部接口函数 3
# 读取测试数据， 形成批次
def get_testing_data(batch_size):
    signals = read_file(testing_file)
    x_batches, y_batches = generate_batch(batch_size, signals)
    return x_batches, y_batches
