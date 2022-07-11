# STM32F103C8T6-SPI-OLED #

## SPI的实现 ##

此模块能够实现通过STM32F103C8T6控制4线SPI模式的OLED，并且可以自主选择三种控制模式，它们分别是：**软件模拟SPI控制OLED** 、 **硬件SPI控制OLED** 和 **硬件SPI+DMA控制OLED**这些控制方式都写在spi和dma文件中。

spi文件中只写了两个函数，它们分别是**void SPI_GPIO_Init(void)**和**void SPI_WR_Byte(u8 dat, u8 cmd)**，下面，我将详细讲解一下这两个函数。

### void SPI_GPIO_Init(void) ###

    void SPI_GPIO_Init(void)
	{
	#if USE_SPI_MODE == 0
		GPIO_InitTypeDef  GPIO_InitStructure;
	 	
	 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
		GPIO_InitStructure.GPIO_Pin = SPI_CS_Pin|SPI_SCLK_Pin|SPI_SDIN_Pin;
	 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	 	GPIO_Init(GPIOA, &GPIO_InitStructure);
	 	GPIO_SetBits(GPIOA,SPI_SCLK_Pin|SPI_SDIN_Pin|SPI_CS_Pin);
		
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
		GPIO_InitStructure.GPIO_Pin = SPI_RST_Pin|SPI_DC_Pin;
	 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	 	GPIO_Init(GPIOB, &GPIO_InitStructure);
	 	GPIO_SetBits(GPIOB,SPI_RST_Pin|SPI_DC_Pin);
	#elif USE_SPI_MODE == 1
		GPIO_InitTypeDef  GPIO_InitStructure;
		SPI_InitTypeDef SPI_InitStructure;
	 	
	 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_SPI1|\
								RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE);
	
		// SCL、SDA
		GPIO_InitStructure.GPIO_Pin = SPI_SCLK_Pin|SPI_SDIN_Pin;
	 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	 	GPIO_Init(GPIOA, &GPIO_InitStructure);
	 	GPIO_SetBits(GPIOA,SPI_CS_Pin);
	
		// CS
		GPIO_InitStructure.GPIO_Pin = SPI_CS_Pin;
	 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	 	GPIO_Init(GPIOA, &GPIO_InitStructure);
	 	GPIO_SetBits(GPIOA,SPI_CS_Pin);
	
		//RES、DC
		GPIO_InitStructure.GPIO_Pin = SPI_RST_Pin|SPI_DC_Pin;
	 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	 	GPIO_Init(GPIOB, &GPIO_InitStructure);
	 	GPIO_SetBits(GPIOB,SPI_RST_Pin|SPI_DC_Pin);
	
		SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;  // 2分频
		SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;  // 在第一个跳变沿采集数据
		SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;  // 串行同步时钟空闲状态为低电平
		SPI_InitStructure.SPI_CRCPolynomial = 7;  // CRC计算的多项式
		SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;  // SPI发送接收8位帧结构
		SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;  // SPI单线只发送
		SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;  // MSB先发送
		SPI_InitStructure.SPI_Mode = SPI_Mode_Master;  // SPI主机模式
		SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;  // 软件控制片选（CS）
		SPI_Init(SPI1, &SPI_InitStructure);
	#if USE_DMA
		SPI1->CR2 = 1 << 1;  // 允许DMA往缓冲区内发送数据
	#endif
		SPI_Cmd(SPI1, ENABLE);
	#endif
	}

这个函数大体上由两部分组成，从上往下由预编译指令分割成软件SPI、硬件SPI（最下面突然跳出来的“#if”是用来选择是否使用DMA的）。

#### 软件SPI ####

使用软件SPI需要将对应的IO口初始化并全部置位。

#### 硬件SPI ####

硬件SPI需要将对应的IO口初始化，不过我们需要使用硬件控制SCLK、MOSI两个引脚，因此需要将它们设置成“GPIO_Mode_AF_PP”模式，可以看到，在初始化这两个引脚后，我将CS引脚置位了，这是为什么呢？哈哈哈，其实是我修改代码的时候忘了删除了，可以看到下方初始化CS引脚后还是将其置位了，不过它并不影响代码的运行，因此先不管它吧。我们接着来看初始化RES和DC，这两个引脚和CS引脚相同，记得初始化后将他们置位哦~

最后就是初始化SPI了，相信大家结合代码旁边的注释能够看明白配置方案的！不过在初始化SPI的过程中突然插进去了一句。`SPI1->CR2 = 1 << 1;`这是大家需要注意的，不加上这条语句的话，是无法使用SPI->TX的DMA的，最后记得使能SPI1哦~

### void SPI_WR_Byte(u8 dat, u8 cmd) ###

    void SPI_WR_Byte(u8 dat, u8 cmd)
	{
		if(cmd) SPI_DC_Set();
		else SPI_DC_Clr();
		SPI_CS_Clr();
	#if USE_SPI_MODE == 0
		u8 i;
		for(i=0;i<8;i++)
		{
			SPI_SCLK_Clr();
			if(dat&0x80) SPI_SDIN_Set();
			else SPI_SDIN_Clr();
			SPI_SCLK_Set();
			dat<<=1;   
		}
	#elif USE_SPI_MODE == 1
		u8 retry = 0;
		while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)==RESET)
		{
			retry++;
			if(retry>200) return;
		}
		SPI_I2S_SendData(SPI1, dat);
		retry = 0;
		while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE)==RESET)
		{
			retry++;
			if(retry>200) return;
		}
	#endif
		SPI_CS_Set();
		SPI_DC_Set();
	}

可以看到，它同样被预编译指令分成了两个部分，它们分别是软件SPI、硬件SPI。接下来我详细地讲解一下。

#### 软件SPI ####

首先通过传入的参数`cmd`判断将要向OLED传输指令还是数据，如果是数据，将拉低DC线，如果是数据，将拉高DC线，然后拉低CS线选中OLED。接下来就是发送数据了，不过因为最下面还有一小部分两种实现方式共有的部分，我就先讲解一下最后的取消片选了。发送完数据后，将先拉高CS先取消片选，然后再拉高DC线（这一步好像没啥用，优化的时候可以选择去除，当然我没有尝试过）。

传输数据，先传输高位再传输低位，根据OLED中的芯片而定。

#### 硬件SPI ####

这里我只介绍硬件SPI的数据传输部分，首先需要检查SPI1的`SPI_I2S_FLAG_TXE`标志，当SPI1的发送缓冲为空时再填入数据，填入数据后检查SPI1的`SPI_I2S_FLAG_RXNE`标志，当SPI1的接收缓冲非空时再退出，当然为了防止硬件SPI卡死，设置了`retry`变量，当检测次数超过200次后任未完成，将直接退出。

## OLED的驱动 ##

OLED的驱动主要分为：OLED初始化、OLED显存的写入、OLED缓存的修改这三个部分，接下来我将详细讲解一下这三个部分。

### OLED初始化 ###

OLED的初始化对于单片机初学者来说真的是太多了，当时我被它给整蒙了，不过经过一段时间的使用之后，我慢慢窥探到了它的一层面纱，废话不多说，让我们看看初始化代码吧！

	void OLED_Init(void)
	{
		OLED_GPIO_Init();
	#if USE_DMA && REFRESH_MODE == 2
		MyDMA_Init();
	#endif
						  
		OLED_WR_Byte(0xAE,OLED_CMD);  // 关闭OLED显示
		OLED_WR_Byte(0xD5,OLED_CMD);  // 设置时钟分频因子、震荡频率
		OLED_WR_Byte(80,OLED_CMD);    // [3,0]分频因子，[7,4]震荡频率
		OLED_WR_Byte(0xA8,OLED_CMD);  // 设置驱动路数
		OLED_WR_Byte(0X3F,OLED_CMD);  // 默认0X3F(1/64)
		OLED_WR_Byte(0xD3,OLED_CMD);  // 设置显示偏移
		OLED_WR_Byte(0X00,OLED_CMD);  // 默认为0
	
		OLED_WR_Byte(0x40,OLED_CMD);  // 设置显示开始行，[5,0]行数
									
		OLED_WR_Byte(0x8D,OLED_CMD);  // 电荷泵设置
		OLED_WR_Byte(0x14,OLED_CMD);  // bit2，开始/关闭
		OLED_WR_Byte(0x20,OLED_CMD);  // 设置内存地址模式
		OLED_WR_Byte(0x01,OLED_CMD);  // [1,0]，01->列地址模式；10->行（页）地址模式；默认10
		OLED_WR_Byte(0xA1,OLED_CMD);  // 段重定义设置，bit0：0->0；1->127
		OLED_WR_Byte(0xC0,OLED_CMD);  // 设置COM扫描方向；[3,0]，这里选用普通模式
		OLED_WR_Byte(0xDA,OLED_CMD);  // 设置COM硬件引脚配置
		OLED_WR_Byte(0x12,OLED_CMD);  // [5,4]配置
			 
		OLED_WR_Byte(0x81,OLED_CMD);  // 对比度设置
		OLED_WR_Byte(0x7F,OLED_CMD);  // 亮度设置（越大越亮）[7,0]，默认0X7F
		OLED_WR_Byte(0xD9,OLED_CMD);  // 设置预充电周期
		OLED_WR_Byte(0xF1,OLED_CMD);  // [3,0]PHASE 1；[7,4]PHASE 2
		OLED_WR_Byte(0xDB,OLED_CMD);  // 设置VCOMH电压倍率
		OLED_WR_Byte(0x30,OLED_CMD);  // [6,4]：000->0.65*VCC；001->0.77*VCC；011->0.83*VCC
	
		OLED_WR_Byte(0xA4,OLED_CMD);  // 全局显示开启，bit0：1->开启；0->关闭（白屏/黑屏）
		OLED_WR_Byte(0xA6,OLED_CMD);  // 设置显示方式：bit0：1->反向显示；0->正常显示
		OLED_WR_Byte(0xAF,OLED_CMD);  // 开启显示
		
		OLED_Clear();
	}

每一步写入命令的旁边都标上了相应的记号，大家可以结合着SSD1306的数据手册看，当然也可以选择跳过这一部分，因为单是显示的话，不太需要修改这一部分，在这里面还分别用到了`OLED_GPIO_Init()`、`MyDMA_Init()`和`OLED_Clear()`，其中，MyDMA_Init()是开启DMA时要用到的，在此我先不做解释，我们先看到OLED_GPIO_Init(),它调用的是`SPI_GPIO_Init()`，而这个函数在上面已经提到过了，它的作用是初始化SPI对应的IO口和SPI1，再看到函数OLED_Clear()，它的作用是清屏，代码附上：

    void OLED_Clear(void)  
	{  
		u8 i,n;  
		for(i=0;i<OLED_PAGE_MAX;i++)for(n=0;n<OLED_COL_MAX;n++)OLED_GRAM[n][i]=0X00;  
		OLED_Refresh_Gram();
	}

它在将OLED缓存全部清零后调用OLED_Refresh_Gram()将缓存加载进显存，从而达到清屏的目的，这个函数我将马上提到。

### OLED显存的写入 ###

现存写入的实现，就是上方所提到的函数`OLED_Refresh_Gram()`，代码附上：

	void OLED_Refresh_Gram(void)
	{
	#if REFRESH_MODE == 0
		// 水平地址模式下的刷新函数
		u8 i,n;
		for(i=0;i<OLED_PAGE_MAX;i++)
		{
			OLED_SetPos(REFRESH_START_POS_COL, (REFRESH_START_POS_PAGE+i)%8);
			for(n=0;n<OLED_COL_MAX;n++)OLED_WR_Byte(OLED_GRAM[n][i],OLED_DATA);
		}
	#elif REFRESH_MODE == 1
		// 垂直地址模式下的刷新函数
		u8 i,n;
		OLED_SetPos(REFRESH_START_POS_COL, REFRESH_START_POS_PAGE);
		for(i=0;i<OLED_COL_MAX;i++)
			for(n=0;n<OLED_PAGE_MAX;n++)
				OLED_WR_Byte(OLED_GRAM[i][n],OLED_DATA);
	#elif USE_DMA && REFRESH_MODE == 2
		// 垂直地址模式下的DMA刷新函数
	#if REFRESH_DMA_PREVENT_DISLOCATION_MODE
		// 预防显示错位
		OLED_SetPos(REFRESH_START_POS_COL, REFRESF_START_POS_PAGE);
	#endif
		MyDMA_Enable();
	#endif
	}

预编译指令将其分成三部分：水平地址模式、垂直地址模式、垂直地址模式+DMA传输。可以注意到的是，在这三种方式中都用到了一个函数`void OLED_SetPos(void a, void b)`，我先讲解一下这个函数，完整代码如下：

	static void OLED_SetPos(u8 col, u8 page)
	{
		OLED_WR_Byte (0xb0+page,OLED_CMD);			// 设置页坐标
		OLED_WR_Byte (0x00+col&0X0F,OLED_CMD);		// 设置列地址（低4位）
		OLED_WR_Byte (0x10+((col&0XF0)>>4),OLED_CMD);	// 设置列地址（高4位）
	}

首先设置光标位置的页坐标，然后设置列坐标，在设置列坐标的时候需要分开设置低4位和高4位。

#### 水平地址模式 ####

此过程需要按照水平地址模式写入数据（一行一行写入），不过在水平寻址模式下，当写完一页后，光标不会跳到下一页，任在这一页中写入数据，因此每写完一页就要重新设置光标位置。

#### 垂直地址模式 ####

此过程需要按照垂直地址模式写入数据（一列一列写入数据），垂直地址模式下，每写完一列，光标将自动跳转到下一列，因此不需要多次设置光标位置。

#### 垂直地址模式+DMA传输 ####

这种传输方式的前提是，需要将DMA设置成非循环模式。此过程需要开启一次DMA传输，当然这是有条件的，不能在其他地方设置光标位置，如果需要这么操作，请修改`REFRESH_DMA_PREVENT_DISLOCATION_MODE`的宏定义，使之能在开启一次DMA传输之前重置光标位置，代价就是传输速率将变慢。

### OLED缓存的修改 ###

想要了解怎么修改OLED缓存，我们先得知道OLED缓存的结构

	//OLED的显存
	//存放格式如下（箭头左边为缓存数组的下标，右边为OLED页标）：
	// 0 -> [7]0 1 2 3 ... 127
	// 1 -> [6]0 1 2 3 ... 127
	// 2 -> [5]0 1 2 3 ... 127
	// 3 -> [4]0 1 2 3 ... 127
	// 4 -> [3]0 1 2 3 ... 127
	// 5 -> [2]0 1 2 3 ... 127
	// 6 -> [1]0 1 2 3 ... 127
	// 7 -> [0]0 1 2 3 ... 127
	//每个字节存储的数据对应于OLED，高位在y轴方向（坐标原点在第7页第0列）
	u8 OLED_GRAM[OLED_COL_MAX][OLED_PAGE_MAX];

OLED缓存是一个定义在RAM中的二维数组`OLED_GRAM`，由于OLED摆放的习惯问题，我喜欢将市面上占大多数的OLED屏引脚朝上，当然这就到来了一个问题，序号最大的页数在最上面，而习惯上左上角的坐标是(0,0)，这就与习惯相反了，不过还好x轴上的是对应着的，因此我们只需要考虑y轴，既然习惯难改，那我们就修改“修改缓存的函数”啦，为适应这个函数，我们将二维数组的页下标最小值对应于OLED的第7页（对于最大为8页的OLED来说），然后定义一个修改缓存的函数，函数如下：

	void OLED_DrawPoint(u8 x,u8 y,u8 mode)
	{
		u8 pos,bx,temp=0;
		if(x>OLED_COL_MAX-1||y>OLED_ROW_MAX-1)return;
		pos=7-y/8;
		bx=y%8;
		temp=1<<(7-bx);
		if(mode)OLED_GRAM[x][pos]|=temp;
		else OLED_GRAM[x][pos]&=~temp;
	#if SHOW_EVERY_STEEP
		OLED_Refresh_Gram();
	#endif
	}

首先需要解释一下，为社么命名为“OLED_DrawPoint”，因为大多数画图函数都要回归到画点嘛！其他没有回归到画点的函数拥有着同样的原理。

因为在定义缓存时，x轴时没有改变的，因此不需要对它进行特殊处理，而页坐标进行了简单的修改，即逆转了一下方向，同时它本身具有着8位为一页的属性，我们需要得到它的基地址和偏移地址，分别通过`pos=7-y/8;`和`bx=y%8;`得到，接下来要做的就是修改缓存了，我就不再赘述了。

再讲一下预编译指令中的语句，它是一个OLED屏幕刷新函数，作用就是在需要调试的时候可以将它开启，观察每一次画点过程。

## DMA的开启 ##

还没有调试成功哦~

代码放在dma文件夹中，需要的请自行研究，要注意到的是，若使用DMA需要将其初始化添加到OLED初始化函数中，在初始化OLED的时候将DMA一并初始化。

需要注意的是，开启DMA需要开启SPI，因为在这个模块中，它的作用是不占用CPU时间将内存中的数据传送至SPI发送缓冲中。

## 一些宏定义 ##

### spi中的宏 ###

#### USE_SPI_MODE ####

	// SPI的实现方式
	// 0->软件SPI
	// 1->硬件SPI1
	#define USE_SPI_MODE 0

切换SPI实现方式的宏，具体用法已经在注释中提及。

#### USE_DMA ####

	// 是否使用DMA
	// 0->不使用
	// 1->使用
	#define USE_DMA 0

选择是否使用DMA。

### oled中的宏 ###

#### REFRESH_START_POS_COL ####

	#define REFRESH_START_POS_COL 0

设置OLED显存刷新时的起始光变的列坐标，可应用于水平地址模式。

#### REFRESH_START_POS_PAGE ####

	#define REFRESH_START_POS_PAGE 0

设置OLED显存刷新时的起始光变的页坐标，可运用于水平地址模式。

#### REFRESH_MODE ####

	// 选择刷新函数的实现方式（修改模式前请先修改OLED初始化配置）
	// 0->水平地址模式下的刷新函数
	// 1->垂直地址模式下的刷新函数
	// 2->垂直地址模式下的DMA刷新函数
	#define REFRESH_MODE 1

选择OLED显存刷新方式。

#### REFRESH_DMA_PREVENT_DISLOCATION_MODE ####

	// 如果您使用了OLED_SetPos，为预防显示错位请将下方的宏定义为1
	#define REFRESH_DMA_PREVENT_DISLOCATION_MODE 0

设置DMA模式下是否在开启一次DMA传输之前重置光标位置。

#### SHOW_EVERY_STEEP ####

	#define SHOW_EVERY_STEEP 0  // 每变化一个点都将调用一次OLED_Refresh_Gram函数

设置是否显示每次画点步骤。默认不显示(0)。