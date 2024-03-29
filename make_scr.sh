#!/bin/bash
echo $(pwd)
cd lvgl
echo $(pwd)
make all
cp -f -v liblvglWidgets.a ..
echo $(pwd)
cd ..
echo $(pwd)
rm -f -v loadTester
make all
echo $(pwd)
