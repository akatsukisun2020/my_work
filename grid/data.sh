#!/bin/bash

rm -f ./data_dir/*
cp ./data/data/training_file.txt ./data_dir/data1.txt
cp ./data/data/validation_file.txt ./data_dir/data2.txt
cp ./data/data/testing_file.txt ./data_dir/data3.txt

# line_number=`cat ./data_dir/data1.txt | wc -l`
# echo $line_number
# sed -i '1,$line_number d' ./data_dir/data1.txt
