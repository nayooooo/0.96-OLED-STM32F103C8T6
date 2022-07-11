# 0.96_OLED-STM32F103C8T6

一些搜集到的图形函数和自己编写的函数。（这篇README只涉及0.96-OLED-STM32F103C8T6）

Some collected graphic functions and self written functions.

中文：
1. 我将需要频繁导入的头文件放在了sys.h中。
2. 接线方式请移步spi.h。
3. 工程文件放在Project文件夹中，hex文件放在./Project/Objects。
4. 预编译指令中涉及到的宏定义一般写在对应的头文件中。

en:
1. I put the header files that need to be imported frequently in sys.h.
2. Please look over spi.h to pick up imformation whitch is about how to wire.
3. I put the engineering document in "Project" folder, and the "hex" file in the "./Project/Objects".
4. You may look for macro definition used in precompiled instruction at its corresponding header file.

更新日志：
1. 新增了IIC模式，但是效果很差 --2022.06.15--
2. 新增了硬件SPI模式，但由于还没有调试好DMA模式，因此体验不佳 --2022.07.11--
3. 在spi.h和oled.h中新增了一些选择宏定义，可以使用他们来选择SPI的实现方式，是否使用DMA以及屏幕刷新方式 --2022.07.11--
