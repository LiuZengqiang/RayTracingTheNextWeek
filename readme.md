## Ray tracing the next week by C++

### 运行：
```shell
mkdir build
./run.sh
```
通过修改``run.sh``中的参数选择需要渲染的场景。运行完成后结果默认存储在``/build/image.ppm``中。  
### 示例结果：  
> 实际结果与[Ray Tracing: The Next Week](https://raytracing.github.io/books/RayTracingTheNextWeek.html)内的展示结果有些许不同。  
#### 动态模糊:
![动态模糊](./output/image1.png)    
#### 网格纹理:  
![网格纹理](./output/image2.png)  
#### 纹理贴图:  
![贴图纹理](./output/image3.png)  
#### perlin noise 纹理:  
![perlin noise纹理](./output/image4.png)  
#### 四边形:  
![四边形](./output/image5.png)  
#### 自发光物体:  
![自发光物体](./output/image6.png)  
#### 基础Cornel Box:  
![基础Cornel Box](./output/image7.png)  
#### 烟雾材质Cornel Box:  
![烟雾材质Cornel Box](./output/image8.png)  
#### 最终场景(高分辨率):  
> 该场景内由多个小球组成的正方体中，部分小球上好像存在小的凸起，导致该问题的原因还未找到。
![最终场景(高分辨率)](./output/image9.png)  
#### 最终场景(低分辨率):  
![最终场景(低分辨率)](./output/image10.png)