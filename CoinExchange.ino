#include <Servo.h>
//include 舵机库
#define RS 22
#define RW 23
//LED 屏引脚命名
#define E 24
#define M 34
//步进电机 1 引脚命名
#define N 35
#define O 36
#define P 37
#define M1 38
//步进电机 2 引脚命名
#define N1 39
#define O1 40
#define P1 41
int pinD[8];
//LED8 个数据引脚命名
Servo myservo;
//6 个舵机命名 myservo 为硬币初速度控制闸门舵机
Servo yourservo;
//yourservo 为等待分类舵机完成的闸门舵机
Servo hisservo;
//hisservo 为 1 元硬币存储桶控制掉落的舵机
Servo herservo;
//herservo 为 5 角硬币存储桶控制掉落的舵机
Servo itsservo;
//itsservo 为硬币退出模块的舵机
Servo himservo;
//himservo 为纸币退出模块的舵机
void setup() {
myservo.attach(2);
//为舵机分配控制端口
yourservo.attach(3);
hisservo.attach(4);
herservo.attach(5);
itsservo.attach(6);
himservo.attach(7);
myservo.write(120);
//设置各个舵机初始位置
yourservo.write(120);
hisservo.write(90);
herservo.write(90);
itsservo.write(180);
himservo.write(120);
Serial.begin(9600);
//通信串口设置
pinMode(33,INPUT_PULLUP); //设置按钮信号输入口
int i;
//设置 22-32 34-41 为输出端口
for(i=22;i<=32;i++)pinMode(i,OUTPUT);
for(i=34;i<=41;i++)
pinMode(i,OUTPUT);
for(i=0;i<8;i++)
pinD[i]=25+i;
//将数组 pin[]赋值为输出的端口,用于 LED 数据传送
}
double sum;
//定义全局变量 sum 因为有 5 角,用.5 表示,数据类型为
//double
void loop() {
long long t;
//定义变量 t,用于存储时间
LCD_Setup();
//自定义函数 用于初始化 LCD 屏幕
display_Insert();
//自定义函数 使 LCD 显示屏显示“Insert coins”
if(analogRead(A2)>=900&&analogRead(A2)<=1000) //当纸币进入模块的红外接受
对被阻断
{
//即 A2 端口读到红外接收管的数值在
//900 到 1000 之间时(当纸币遮挡红外线时仍有红外线会被接收到,数值经过试验为 900-
1000,说明有纸币进入,跳转至纸币进入模块
Note_insert();
//自定义函数,纸币进入模块
}
if(!digitalRead(33))
//在 loop 函数循环过程中,若检测到接
入 33 号端口的按钮(拉高)被按下,则进入硬币投入金额的判断
{
if((int)sum%5==0&&sum!=0&&(int)(sum-0.5)%5!=0) //若投入硬币金额是 5 的整
倍数且不为 0 或者不为 X.5
{
Note_Exchange();
//进入纸币兑换模块
}
else
{
display_Inadequate();
//否则,显示“Please insert more coins”
delay(1000);
//显示 1 秒后回到 loop 函数
}
}
display_Figure();
//显示当前的硬币金额总数
if(analogRead(A1)>200)
//如果硬币轨道最前端红外被阻断,说明有硬币
进入
{
delay(400);
//延迟 100 毫秒,待硬币静止后
myservo.write(60);
//打开闸口
delay(10);
//延迟,等待硬币通过硬币大小识别与前一个闸口间
的轨道
}
if(analogRead(A0)>=600)
//如果硬币大小识别的红外接受对检测到被阻断
{t=millis();
//将此时的时间赋给 t;
while(1)
//进入死循环
{
if(analogRead(A0)<=600) //直到重新接收到红外信号,即硬币通过后
{
t=millis()-t; //计算被阻断的时间,即硬币通过的时间
Serial.println((int)t);
myservo.write(120); //关闭上一个闸口,防止再有硬币进入
if(t<90&&t>=80) //实验得出的 1 元硬币通过时间的范围
{
hisservo.write(30); //1 元硬币存储桶上方闸口打开
herservo.write(90); //5 角硬币存储桶上方闸口关闭
yourservo.write(60); //闸门打开,硬币滑入分类轨道
sum+=1;
//金币总额加 1
}
else if(t>=60&&t<=75) //实验得出的 5 角硬币通过时间的范围
{
hisservo.write(90); //1 元硬币存储桶上方闸口打开
herservo.write(30); //5 角硬币存储桶上方闸口关闭
yourservo.write(60); //闸门打开,硬币滑入分类轨道
sum+=0.5;
//金币总额加 0.5
}
else
//若都不符合上述要求
{
hisservo.write(90); //1 元、5 角存储桶闸口均关闭
herservo.write(90);
yourservo.write(60);//闸门打开,硬币通过无法识别硬币
轨道退出装置
}
break; //退出循环
}
}
delay(500);
yourservo.write(120);
//延迟 500 毫秒后
//yourservo 复位
}
}
void LCD_Setup()
{
sendByte(0x38,0);
delay(5);
//自定义函数初始化 LCD 显示屏的函数sendByte(0x38,0);
delay(5);
sendByte(0x38,0);
delay(5);
sendByte(0x38,0);
delay(1);
sendByte(0x08,0);
delay(1);
sendByte(0x01,0);
delay(1);
sendByte(0x06,0);
delay(1);
sendByte(0x0C,0);
delay(1);
}
void sendByte(byte x,int Datain) //向 LCD 显示屏发送数据或指令的函数
{
int i;
byte mask=0x01;
if(Datain)
digitalWrite(RS, HIGH);
else
digitalWrite(RS, LOW);
for(i=0;i<8;i++)
{
if(mask&x)
digitalWrite(pinD[i],HIGH);
else
digitalWrite(pinD[i],LOW);
mask<<=1;
}
digitalWrite(E, HIGH);
delayMicroseconds(1);
digitalWrite(E, LOW);
delayMicroseconds(1);
}
void display_Insert() //自定义函数 显示”Insert coins”字样
{
sendByte('I',1);
sendByte('n',1);
sendByte('s',1);
sendByte('e',1);sendByte('r',1);
sendByte('t',1);
sendByte(' ',1);
sendByte('c',1);
sendByte('o',1);
sendByte('i',1);
sendByte('n',1);
sendByte('s',1);
sendByte(':',1);
}
void display_Exchange() //自定义函数 两行显示“Exchaning please wait”字样
{
LCD_Setup();
char *p="Exchanging
Please wait";
while(*p!=0)
{
sendByte(*p,1);
p++;
}
}
void display_Inadequate()//自定义函数 两行显示“Please insert more coins”字样
{
LCD_Setup();
char *p="Please insert
while(*p!=0)
{
sendByte(*p,1);
p++;
}
}
void display_Figure() //自定义函数 显示当前投入金额
{
if(sum<10)
{
sendByte((int)sum/1+'0',1);
sendByte('.',1);
sendByte(((int)(sum*10))%10+'0',1);
}
else if(sum>=10)
{
more coins";sendByte((int)sum/10+'0',1);
sendByte((int)sum%10+'0',1);
sendByte('.',1);
sendByte(((int)(sum*10))%10+'0',1);
}
}
void Note_Exchange() //函数 纸币兑换函数
{
int i;
long long t;
//定义 t 存储时间
t=millis();
//t 赋值为当前时间
display_Exchange(); //显示“Exchanging please wait”
for(i=1;i<=(sum/5);i++) //退出相应张数 5 元纸币
{
while(millis()-t<=15000) //运行时间为 15 秒钟,滚轮顺向旋转
{
Phase_A();
Phase_A1();
//设置 A、A1 相位
delay(3);
Phase_B();
Phase_B1();
delay(3); //设置 B、B1 相位
Phase_C();
Phase_C1();
delay(3); //设置 C、C1 相位
Phase_D();
//设置 D、D1 相位
Phase_D1();
delay(3);
if(millis()-t==500)
//当运行 500 毫秒后
{
himservo.write(180); //让纸币退出模块舵机旋转一定角度将纸币塞入滚轮
delay(1000);
//等待舵机完成动作
}
}
himservo.write(120); //15 秒后,纸币退出,该舵机复位
}
sum=0;
}
//投入总金额清零void Note_insert() //函数 纸币进入函数
{
sum=sum+5; //金额总数加 5
display_Figure(); //显示当前金额总数
while(1) //滚轮逆向旋转
{
if(!digitalRead(33)) //若按下兑换按钮,停止循环
break;
Phase_D();
//设置 D 相位
Phase_D1();
delay(3);
Phase_C();
Phase_C1();
delay(3); //设置 C 相位
Phase_B();
Phase_B1();
delay(3); //设置 B 相位
Phase_A();
Phase_A1();
delay(3);
}
Coin_output();
//设置 A 相位
//函数 硬币退出模块
}
void Coin_output() //函数
{
display_Exchange();
int i;
for(i=1;i<=sum;i++)
{
itsservo.write(70);
delay(1000);
itsservo.write(180);
delay(2000);
}
sum=0;
}
void Phase_A()
{
digitalWrite(M,HIGH);
硬币退出模块
//显示“Exchanging please wait”
//推硬币舵机工作 5 次
//舵机旋转推出硬币
//舵机归位,下一枚硬币滑落
//金币总额清零
//A1 引脚高电平digitalWrite(N,LOW);
digitalWrite(O,LOW);
digitalWrite(P,LOW);
}
void Phase_A1()
{
digitalWrite(M1,HIGH);
digitalWrite(N1,LOW);
digitalWrite(O1,LOW);
digitalWrite(P1,LOW);
}
void Phase_B()
{
digitalWrite(M,LOW);
digitalWrite(N,HIGH);
digitalWrite(O,LOW);
digitalWrite(P,LOW);
}
void Phase_B1()
{
digitalWrite(M1,LOW);
digitalWrite(N1,HIGH);
digitalWrite(O1,LOW);
digitalWrite(P1,LOW);
}
void Phase_C()
{
digitalWrite(M,LOW);
digitalWrite(N,LOW);
digitalWrite(O,HIGH);
digitalWrite(P,LOW);
}
void Phase_C1()
{
digitalWrite(M1,LOW);
digitalWrite(N1,LOW);
digitalWrite(O1,HIGH);
digitalWrite(P1,LOW);
}
//A1 引脚高电平
//B1 引脚高电平
//B1 引脚高电平
//C1 引脚高电平
//C1 引脚高电平void Phase_D()
{
digitalWrite(M,LOW);
digitalWrite(N,LOW);
digitalWrite(O,LOW);
digitalWrite(P,HIGH); }
void Phase_D1()
{
digitalWrite(M1,LOW);
digitalWrite(N1,LOW);
digitalWrite(O1,LOW);
digitalWrite(P1,HIGH);
//D 引脚高电平
}
//D1 引脚高电平
