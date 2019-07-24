/*I2C_OLED12864动态显示数字
*/
/*OLED INIT*/
#include <MsTimer2.h>
#include <Wire.h>             //I2C、SPI通讯用库文件
#include <Adafruit_GFX.h>  
#include <Adafruit_SSD1306.h>       //OLED12864库文件

#define OLED_RESET 4  
Adafruit_SSD1306 display(OLED_RESET);
#define LOGO16_GLCD_HEIGHT 16 //定义显示高度  
#define LOGO16_GLCD_WIDTH  16 //定义显示宽度  

char a[8], Tclock[8] = { '0' , '0' , ':' , '0' , '0' , ':' , '0' , '0', }, Tset[8] = { '0' , '0' , ':' , '0' , '0' , ':' , '0' , '0', };
int flag = 0;
int s2 = 0, s1 = 1;
int f2 = 1, f1 = 1;
int m2 = 0, m1 = 0;
int change, num;
int s2set = 2, s1set = 4;
int led = 13;
int flag_m;		//定时到fflag
int flag_clock = 0;
int flag_set = 0;
int flag_apm = 0;//0--am 1--pm
int flag_menu = 0;//菜单启动flag;
int flag_change12_24 = 0;//切换12制和24制
//Menu
int arrow_pos = 8;//箭头位置
int layer = 1, layer_b;
int menu = 12, up = 9, down = 8, y = 7, n = 6;
int pinmenu, pinup, pindown, piny, pinn;
//clock
int settime_clk = 0;

void interrupt1()
{
	flag_m = 1;
	//  m1++;
}

void setup() {

	pinMode(6, INPUT);
	pinMode(7, INPUT);
	pinMode(8, INPUT);
	pinMode(9, INPUT);
	pinMode(12, INPUT);
	pinMode(13, OUTPUT);
	digitalWrite(13, LOW);
	//Time setup
	MsTimer2::set(1000, interrupt1);
	MsTimer2::start();
	change = m1;
	num = m1;
	display.clearDisplay();
	/*OLED Setup*/
	Serial.begin(9600);
	display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)  
	display.clearDisplay();
	for (int i = 0; i < 8; i++)
	{
		a[i] = i + 48;
	}
	a[2] = ':';
	a[5] = ':';
}

void Time_Interrupt()
{
	if (flag_m == 1)
	{
		m1++;
		flag_m = 0;
	}
	if (m1 > 9)
	{
		m2++; m1 = 0;
		if (m2 > 5)
		{
			f1++; m2 = 0;
			if (f1 > 9)
			{
				f2++; f1 = 0;
				if (f2 > 5)
				{
					s1++; f2 = 0;
					if (s1 > s1set)
						s2++, s1 = 0;
					if ((s2 >= s2set) && (s1 >= s1set)) //进位判断，24进制：24-->1,12进制12-->1
					{
						s2 = 0; s1 = 1; f2 = 0; f1 = 0;
						if ((s2set == 1) && (s1set == 2))//12H check am pm
						{
							if (flag_apm == 0)
								flag_apm = 1;
							else
								flag_apm = 0;
						}
					}
					else if (s1 > 9)
					{
						s2++; s1 = 0;

					}
				}
			}
		}
	}
	if ((s2set == 2) && (s1set == 4))//24H check am pm
	{
		if ((s2 * 10 + s1) > 12)
			flag_apm = 1;
		else
			flag_apm = 0;
	}

	if (change != m1)
	{
		change = m1;
		a[0] = s2 + '0'; a[1] = s1 + '0'; a[2] = ':';
		a[3] = f2 + '0'; a[4] = f1 + '0'; a[5] = ':';
		a[6] = m2 + '0'; a[7] = m1 + '0';
		//Serial.print(s2); Serial.print(s1); Serial.print(" : ");
		//Serial.print(f2); Serial.print(f1); Serial.print(" : ");
		//Serial.print(m2); Serial.print(m1); Serial.println();
	}
}

/*
 * 函 数 名：oled_i2c_go
 * 功能描述：动态显示3位数，时间间隔由delay决定
 * 输入参数：无
 * 返 回 值：无
 */
void oled_i2c_go()        //OLED_I2C动态显示
{
	display.clearDisplay();

	//英文字符显示  
	display.setTextSize(1);               //置字体大小  
	display.setTextColor(WHITE);          //设置字体颜色白色  
	display.setCursor(47, 1);              //设置字体的起始位置  
	display.print("T I M E");        //输出字符并换行 
	//   delay(2);
	display.setTextSize(2);               //置字体大小  
	display.setCursor(17, 14);              //设置字体的起始位置  
	for (int i = 0; i < 8; i++)        //将所得到的数字显示在OLED上
	{
		display.print(a[i]);
	}
	display.display();                  //显示以上 
}

void change12_24()
{
	if (flag_change12_24 == 1) {
		int sum;
		if (flag_set == 1)//24--->12
		{
			s2set = 1; s1set = 2;
			sum = s2 * 10 + s1;
			if (sum > 12)
			{
				s2 = (sum - 12) / 10;
				s1 = (sum - 12) % 10;
			}
		}
		else if (flag_set == 2)//12--->24
		{
			sum = s2 * 10 + s1;
			s2set = 2, s1set = 4;
			if ((sum > 12) || (flag_apm == 1))
			{

				s2 = (sum + 12) / 10;
				s1 = (sum + 12) % 10;
			}
		}
	}
	flag_change12_24 = 0;
}
void first_menu()
{
	display.clearDisplay();
	display.setTextSize(1);             //设置字体大小
	display.setCursor(0, arrow_pos);
	display.print("-->");

	display.setCursor(43, 0);
	display.println("M E N U");
	display.setCursor(20, 8);
	display.print("1.change 1224");
	display.setCursor(20, 16);
	display.print("2.set clock");
	display.setCursor(20, 24);
	display.print("3.set time");
	display.display();

}
void change_1224()
{
	display.clearDisplay();
	display.setTextSize(1);             //设置字体大小
	display.setCursor(0, arrow_pos);
	display.print("-->");

	display.setCursor(20, 0);
	display.print("chang 12_24");
	display.setCursor(20, 8);
	display.println("choose 12");
	display.setCursor(20, 16);
	display.println("choose 24");


	display.display();
	if (digitalRead(y) == 1)
	{
		if ((arrow_pos / 8) == 1)
			flag_set = 1;
		else if ((arrow_pos / 8) == 2)
			flag_set = 2;
		flag_change12_24 = 1;
		layer = 1;
	}
}
void set_clock()
{
	int flag_back = 0;
	settime_clk = 0;
	while (1)
	{
		display.clearDisplay();
		display.setTextSize(1);             //设置字体大小
		display.setCursor(43, 0);
		display.println("Set Clock");
		display.setTextSize(2);
		display.setCursor(17, 14);

		if (digitalRead(down) == 1)
		{
			delay(10);
			if (digitalRead(down) == 1)
			{
				Tclock[settime_clk]++;
				if (Tclock[settime_clk] > '9')
					Tclock[settime_clk] = '0';
			}
		}
		else if (digitalRead(up) == 1)
		{
			delay(10);
			if (digitalRead(up) == 1)
			{
				Tclock[settime_clk]--;
				if (Tclock[settime_clk] < '0')
					Tclock[settime_clk] = '9';
			}
		}
		else if (digitalRead(y) == 1)
		{
			delay(10);
			if (digitalRead(y) == 1)
			{
				settime_clk++;
				if (settime_clk == 2 || settime_clk == 5)
					settime_clk++;
				flag_back++;
			}
		}
		else if ((digitalRead(n) == 1) && (settime_clk > 0))
		{
			delay(10);
			if ((digitalRead(n) == 1) && (settime_clk > 0))
			{
				settime_clk--;
				if (settime_clk == 2 || settime_clk == 5)
					--settime_clk;
				flag_back--;
			}
		}
		for (int i = 0; i < 8; i++)
			display.print(Tclock[i]);
		display.display();
		if (flag_back == 6)
			break;
	}
	flag_clock = 1;
	layer = 1;//跳出while循环返回菜单
}
void set_time()
{
	int flag_back = 0;
	settime_clk = 0;
	while (1)
	{
		display.clearDisplay();
		display.setTextSize(1);             //设置字体大小
		display.setCursor(43, 0);
		display.println("Set Time");
		display.setTextSize(2);
		display.setCursor(17, 14);

		if (digitalRead(down) == 1)
		{
			Tset[settime_clk]++;
			if (Tset[settime_clk] > '9')
				Tset[settime_clk] = '0';
		}
		else if (digitalRead(up) == 1)
		{
			Tset[settime_clk]--;
			if (Tset[settime_clk] < '0')
				Tset[settime_clk] = '9';
		}
		else if (digitalRead(y) == 1)
		{
			settime_clk++;
			if (settime_clk == 2 || settime_clk == 5)
				settime_clk++;
			flag_back++;
		}
		else if ((digitalRead(n) == 1) && (settime_clk > 0))
		{
			settime_clk--;
			if (settime_clk == 2 || settime_clk == 5)
				--settime_clk;
			flag_back--;
		}

		for (int i = 0; i < 8; i++)
			display.print(Tset[i]);
		display.display();
		if (flag_back == 6)
			break;
	}
	for (int i = 0; i < 8; i++)
		* (a + i) = Tset[i];
	s2 = a[0] - '0'; s1 = a[1] - '0'; f2 = a[3] - '0'; f1 = a[4] - '0'; m1 = a[6] - '0'; m2 = a[7] - '0';
	layer = 1;//跳出while循环返回菜单
}

void keyboard()
{
	if (digitalRead(up) == 1)
	{
		delay(10);
		if (digitalRead(up) == 1)
		{
			arrow_pos = arrow_pos - 8;
			if (arrow_pos < 8)
				arrow_pos = 24;
		}
	}

	if (digitalRead(down) == 1)
	{
		delay(10);
		if (digitalRead(down) == 1)
		{
			arrow_pos = arrow_pos + 8;
			if (arrow_pos > 26)
				arrow_pos = 8;
		}
	}

	if (digitalRead(y) == 1)//未插开关恒为0，插上开关为1，按下开关为0
	{
		delay(10);
		if ((digitalRead(y) == 1) && (layer < 13))
		{
			layer_b = layer;
			layer = 10 + arrow_pos / 8;
		}
	}
	
	if (digitalRead(n) == 1)
	{
		delay(10);
		if (digitalRead(n) == 1)
		{
			if (layer == 1)
				flag_menu = 0;
			layer = layer_b;
		}
	}
}

void bee()
{
	int i;
	int j = 0;
	for (i = 0; i < 8; i++)
	{
		if ((a[i] - Tclock[i]) == 0)
			j++;
	}
	Serial.print("j=="); Serial.println(j);
	if (j == 8)
	{
		digitalWrite(led, HIGH); Serial.print("ok");
	}
}

void Menu()
{
	switch (layer)
	{
	case 1:first_menu(); break;
	case 11:change_1224(); break;
	case 12:set_clock(); break;
	case 13:set_time(); break;
	default:break;
	}
	keyboard();
}

void loop() {
	if (flag_menu == 0)
	{
		Time_Interrupt();
		oled_i2c_go();
		change12_24();
	}
	else
	{
		Time_Interrupt();
		Menu();
	}

	if (flag_clock == 1)
		bee();

	if (digitalRead(menu) == 1)
		flag_menu = 1;
}