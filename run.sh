#!/usr/bin/bash
cd ./build/
cmake .. && make && ./RayTracingTheNextWeek > image.ppm
cd ..

