#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <Math.h>
#include <MPU6050.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
MPU6050 accelerometer;


const float pi = 3.141592; 
const int sample_no = 100; // no of samples for aproximation
//int test = 0; //licznik pomiarów
int16_t ax, ay, az;
float x, y, z;
int  sample;
float _angle_x, angle_x, _angle_y, angle_y;
float angle_x_R = 0;
float angle_y_R = 0;
long ax_sum, ay_sum, az_sum;

int buzzTime = 0;
float maxAngle = 0;
float buzzF = 0;
unsigned long aktualnyCzas = 0;
unsigned long aktualnyCzas2 = 0;
int time1 = 0;

void lcdInit(){
  Wire.setClock(10000);
  lcd.begin(16,2);   // Inicjalizacja LCD 2x16  
  lcd.backlight(); // zalaczenie podwietlenia 
  lcd.clear();
  lcd.setCursor(0,0); // Ustawienie kursora w pozycji 0,0 (pierwszy wiersz, pierwsza kolumna)
  lcd.print("LCD OK...");
  delay(2000);
}

void angleR(){
  angle_x_R = angle_x;
  angle_y_R = angle_y;  
}

void accelerometerInit(){
  accelerometer.initialize();
  if(accelerometer.testConnection()){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("MPU6050 OK...");
    delay(2000);
  }else{
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("MPU6050 ERROR...");
    delay(2000);
  }
  lcd.clear();
}

void angle(){
  accelerometer.getAcceleration(&ax, &ay, &az);
  ax_sum = ax_sum + ax;
  ay_sum = ay_sum + ay;
  az_sum = az_sum + az; 
  sample++;
//  test++;
    
  if (sample == sample_no){
   // mean values
   x = ax_sum/sample_no;
   y = ay_sum/sample_no;
   z = az_sum/sample_no;
  
   // Calculate of roll and pitch in deg
   angle_x = atan2(x, sqrt(square(y) + square(z)))/(pi/180);
   angle_y = atan2(y, sqrt(square(x) + square(z)))/(pi/180);
   
   // Reset values for next aproximation   
   sample=0;
   ax_sum = 0;
   ay_sum = 0;
   az_sum = 0;

   lcd.setCursor(0,0);
   lcd.print("X:");
   lcd.setCursor(3,0);   
   lcd.print(angle_x - angle_x_R);
   lcd.setCursor(0,1);
   lcd.print("Y:");
   lcd.setCursor(3,1);   
   lcd.print(angle_y - angle_y_R);
   
   //lcd.setCursor(10,1);
   //lcd.print(test);
  }
}

void buzzerInit(){
  pinMode(8, OUTPUT);
  digitalWrite(8, HIGH);
  delay(200);
  digitalWrite(8, LOW);
}

void buzzer(){
  
  buzzF = (1/maxAngle)*10000;
  if(aktualnyCzas == 0) aktualnyCzas = millis();

  if(aktualnyCzas2 != 0 && aktualnyCzas2+200<millis()){
    digitalWrite(8, LOW);
    aktualnyCzas = 0;
    aktualnyCzas2 = 0;
  }else if(aktualnyCzas > 0 && aktualnyCzas+buzzF<millis()){
    digitalWrite(8, HIGH);
    aktualnyCzas = -1;
    aktualnyCzas2 = millis();
  }
}


void setup() {
  attachInterrupt(digitalPinToInterrupt(2), angleR, LOW); //Przerwanie na pinie 2
  lcdInit();
  accelerometerInit();
  buzzerInit();
  buzzer();
}

void loop() {
  // put your main code here, to run repeatedly:
  angle();
  maxAngle = fmaxf(sqrt((angle_x - angle_x_R)*(angle_x - angle_x_R)), sqrt((angle_y - angle_y_R)*(angle_y - angle_y_R)));
  if(maxAngle>=10.0f) buzzer();
  else digitalWrite(8, LOW);
}
