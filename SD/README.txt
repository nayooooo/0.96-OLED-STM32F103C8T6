本例程往同目录下的0.96OLED-SPI中添加了SD模块
后续将添加python串口通信结合STM32写SD

汉语：
1. 工程文件路径是./MDK-ARM
2. 文件输出路径./Obj
3. 若要使用SD模块，应先根据SRAM大小合理分配内存管理空间
测试时15K内存没有遇到无法使用的情况
4. MMC_SD_User编写中，编写完成后记得删除本条
5. python会上传至https://github.com/nayooooo/Pictures-Processing.git目录下，上传后重新编写此条
