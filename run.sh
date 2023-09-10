#!/usr/bin/bash

cd ./build/
cmake .. && make && ./RayTracingTheNextWeek 1 > image.ppm
cd ..

