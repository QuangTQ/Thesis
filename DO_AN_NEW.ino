
#include <Adafruit_Fingerprint.h>
#include <LiquidCrystal.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <Keypad.h>

#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)
SoftwareSerial mySerial(PA10, PA9);
#else
#define mySerial Serial1
#endif

//-- xây dựng ma trận KEY---//
const byte HANG = 4;
const byte COT = 4;
char Ma_Key[HANG][COT] = {
    {'1','2','3','A'},
    {'4','5','6','B'},
    {'7','8','9','C'},
    {'*','0','#','D'}
  };

#define Password_Length 6
char Master_Password[Password_Length] = {'0','0','0','0','0','0'};
char Custom_Password[Password_Length] = {' ',' ',' ',' ',' ',' '};
byte Pin_Hang[HANG] = {PA0,PA1,PA2,PA3}; 
byte Pin_Cot[COT] = {PB4,PB5,PB6,PB7};

#define SS_PIN  PA4 // SDA
#define RST_PIN PA3 //RESET noi vao 3.3 V
#define ROW 2
#define COLUM 4

#define LED_G   PC13 // led 1
#define BUZZER  PB0 // coi 
#define KHOA    PB1 // khoa dien
#define BUTTON  PA15 // nut mo cua phia trong

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
Keypad customKeypad = Keypad(makeKeymap(Ma_Key),Pin_Hang,Pin_Cot,HANG,COT);

char data;
char customKey;
int UID[ROW][COLUM];
int ID1[4]={0xD3,0xC3,0x72,0x0D};
int ID2[4]={0xA3,0xEE,0x75,0x11};
int ID_MASTER[4];
uint8_t id=0;

int dem=0,flag=0,ID=0;
int i,temp1=0,temp2=0;
int buttonADD; 
char count=0;
int dem2=0,dem3=0,dem4=0;

const int rs=PB10,en=PB11,d4=PB12,d5=PB13,d6=PB14,d7=PB15;
LiquidCrystal lcd(rs,en,d4,d5,d6,d7);


void setup()
{
              
              SPI.begin();      // Initiate  SPI bus
              mfrc522.PCD_Init();   // Initiate MFRC522
              
              pinMode(LED_G,OUTPUT);
              pinMode(BUZZER,OUTPUT);
              pinMode(KHOA,OUTPUT);
              pinMode(BUTTON,INPUT_PULLUP);
              digitalWrite(KHOA,LOW);
              digitalWrite(LED_G,LOW); // led tat 
              
      //lcd
              lcd.begin(16,2);
              lcd.setCursor(0,0);
              lcd.print("Do an tot nghiep");
              lcd.setCursor(0,1);
              lcd.print("Nguyen Duy Quang");
              delay(2000);
              
     // set the data rate for the sensor serial port
              finger.begin(57600);
              delay(5);
              finger.getParameters();
              finger.getTemplateCount();
  }

void loop()                     // run over and over again
{
  /**************************************************************************************/

  /**************************************************************************************/
      customKey = customKeypad.getKey();
//      lcd.clear();
//      lcd.setCursor(0,0);
//      lcd.print("MOI NHAP VAN TAY");
//      lcd.setCursor(0,1);
//      lcd.print("  MOI QUET THE  ");
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("VT A:THEM VT");
//        lcd.setCursor(10,0);
//        lcd.print("B:XOA");
        lcd.setCursor(10,1);
        lcd.print("D:THAY");
        lcd.setCursor(0,1);
        lcd.print("MK C:NHAP");
      //noTone(BUZZER);
  /**************************************************************************************/

  /**************************************************************************************/
      getFingerprintID(); // luôn nhận vân tay
      delay(5);  
       
        if(digitalRead(BUTTON) == 0)
          { 
           // while(digitalRead(BUTTON) == 0);
            //delay(10);
            open_door();
          }
      if(customKey == 'A')// them ID FP
      {
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print(customKey);
          flag=0;lcd.clear();
          customKey = ' ';
          while(flag < 2)
          {
            kiemtra_Password();
          }
           ID=ID+1; 
           flag =0;
           lcd.clear();
           customKey = ' ';
                    while(flag < 2)
                           {
                              while (!  getFingerprintEnroll() ); 
                           }
      }

      
//    if(customKey == 'B')// xoa ID FP
//      {
//          uint8_t id ;
//                lcd.clear();
//                flag=0;
//                customKey = ' ';
//                while(flag < 2)
//                {
//                  kiemtra_Password();
//                }
//                 flag =0;
//                 lcd.clear();
//                 lcd.setCursor(0,0);
//                 lcd.print("NHAP ID CAN XOA");
//                 delay(1000);
//                 id = customKeypad.getKey();
//                 lcd.setCursor(0,1);
//                 lcd.print(id);
//                 delay(1000);
//                  //lay id xoa van tay
//                  if (id == 0) {// ID #0 not allowed, try again!
//                     return;
//                  }
//                    lcd.clear();
//                    lcd.setCursor(0,0);
//                    lcd.print("DA XOA ID:");
//                    lcd.setCursor(0,1);
//                    lcd.print(id);
//                    deleteFingerprint(id);
//      }

      if ( customKey == 'C') // nhập MK 
          {
              flag =0;lcd.clear();
              customKey = ' ';
              while(flag < 2)
              {
                nhap_Password();
              }
          }
    
    if ( customKey == 'D' ) // thay đổi mật khẩu
        {
              flag=0;lcd.clear();
              lcd.setCursor(0,0);
              customKey = ' ';
              while(flag<2)
              {
                set_Password();
              }
        }
  // neu dem thay nhap sai mk 3 lan hoac sai van tay 3 lan thi canh bao      
     if((dem2==3)||(dem3==3)||(dem4==3))
      {
          canhBao();  
      }
      // nhan the 
      if ( ! mfrc522.PICC_IsNewCardPresent())
            {
              return;
            }
      
      // Select one of the cards
      if ( ! mfrc522.PICC_ReadCardSerial())
            {
              return;
            }
          
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(" MA THE:");
      
     for (byte j = 0; j < ROW; j++)
      {
           dem=0;
        for(byte i = 0; i < mfrc522.uid.size; i++)
               {
                      UID[j][i]=mfrc522.uid.uidByte[i];
                      lcd.setCursor(dem,1);
                      lcd.print(UID[j][i],HEX);
                      lcd.print(" ");
                      dem=dem+3;
               }
     }
     //lcd.clear();
     delay(1000);
         
      if ((UID[0][2] ==ID1[2])||(UID[1][1] ==ID2[1]) )
            {
              open_door();
            }
    
      else
            {
              wrong();
            }
            
}

  /**************************************************************************************/

  /**************************************************************************************/
// lay van tay
uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      break;
    case FINGERPRINT_NOFINGER:
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      return p;
    case FINGERPRINT_IMAGEFAIL:
      return p;
    default:
      return p;
  }
   //OK success! 

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      break;
    case FINGERPRINT_IMAGEMESS:  
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:  
      return p;
    case FINGERPRINT_FEATUREFAIL:  
      return p;
    case FINGERPRINT_INVALIDIMAGE:    
      return p;
    default:  
      return p;
  }

  // OK converted!
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    dem3=0;
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {  
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
      dem3++;
      lcd.clear();
      lcd.setCursor(0,0);lcd.print("VAN TAY SAI ! ! ! ");
      // coi keu 3 lan
      error();
      delay(1000);
      lcd.clear();
    return p;
  } else {  
    return p;
  }
  /*************************************************************************/
  // found a match! nhận diện thành công vân tay
      //dem3=0;
      lcd.clear();
      lcd.setCursor(0,0);lcd.print("  TIM THAY VT     ");
      lcd.print(finger.fingerID);
      delay(1000);
      open_door();
      lcd.clear();
      
  return finger.fingerID;
}
  /**************************************************************************************/

  /**************************************************************************************/
void open_door()
{
  // coi keu 0.5 giay
    right();  
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("* CUA DANG MO * ");
    
    digitalWrite(LED_G, HIGH);// den sang
    digitalWrite(KHOA, HIGH);// mơ chot cua
    
    delay(5000);   // thoi gian mo cua 
    lcd.clear();  
    digitalWrite(KHOA, LOW);
    digitalWrite(LED_G, LOW); // den tat 
}

  /**************************************************************************************/

  /**************************************************************************************/
// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;

  // found a match!
      
  return finger.fingerID;
}
////----------------------------------------

// them van tay
uint8_t getFingerprintEnroll() {
  
  int p = -1;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("VUI LONG DAT VT");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      break;
    case FINGERPRINT_NOFINGER:
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      break;
    case FINGERPRINT_IMAGEFAIL:
      break;
    default:    
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      break;
    case FINGERPRINT_IMAGEMESS:     
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:     
      return p;
    case FINGERPRINT_FEATUREFAIL:     
      return p;
    case FINGERPRINT_INVALIDIMAGE:  
      return p;
    default:
      return p;
  }

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("DANG KY VT: ");lcd.print(ID);
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  
  p = -1;
  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("  MOI NHAP LAI ");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      break;
    case FINGERPRINT_NOFINGER:
      break;
      
    case FINGERPRINT_PACKETRECIEVEERR:
      break;
    case FINGERPRINT_IMAGEFAIL:    
      break;
    default:   
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      break;
    case FINGERPRINT_IMAGEMESS:
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      return p;
    case FINGERPRINT_FEATUREFAIL:   
      return p;
    case FINGERPRINT_INVALIDIMAGE:     
      return p;
    default:     
      return p;
  }
  // OK converted! 
  flag=2;
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("THANH CONG ID:");lcd.print(ID);
    delay(1000);
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {  
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("  SAI ID LAN 1 ");lcd.print(ID);
    delay(1000);
    ID=ID-1;
    if(ID<0){ID=0;}
    
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {   
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("  SAI ID LAN 2  ");lcd.print(ID);
    delay(1000);
    ID=ID-1;if(ID<0){ID=0;}
//    lcd.clear();
//    lcd.setCursor(0,0);  
    return p;
  }
  else {    
    return p;
  }
  
  p = finger.storeModel(ID);
  if (p == FINGERPRINT_OK) {    
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {  
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("  SAI ID LAN 3  ");lcd.print(ID);
    delay(1000);
    ID=ID-1;if(ID<0){ID=0;}
    lcd.clear();
    lcd.setCursor(0,0);   
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) { 
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("  SAI ID LAN 4  ");lcd.print(ID);
    delay(1000);
    ID=ID-1;if(ID<0){ID=0;}
    lcd.clear();
    lcd.setCursor(0,0); 
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {  
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("  SAI ID LAN 5  ");lcd.print(ID);
    delay(1000);
    ID=ID-1;if(ID<0){ID=0;}
    lcd.clear();
    lcd.setCursor(0,0); 
    return p;
  } else {   
    return p;
  }
  return true;
}
  /**************************************************************************************/

  /**************************************************************************************/
void wrong()
{    
  lcd.setCursor(0, 0);
  lcd.print("***  SAI THE ***");
  delay(1000);
}

uint8_t readnumber(void) {
  uint8_t num = 0;

  while (num == 0) {
    while (! Serial.available());
    num = Serial.parseInt();
  }
  return num;
}

// xoa van tay
uint8_t deleteFingerprint(uint8_t id) {
  uint8_t p = -1;
  p = finger.deleteModel(id);
  if (p == FINGERPRINT_OK) {
  lcd.clear();
  lcd.setCursor(0,1);
  lcd.print(" XOA THANH CONG");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
//    Serial.println("Communication error");
  lcd.clear();
  lcd.setCursor(0,1);
  lcd.print("  XOA LOI ");
  } else if (p == FINGERPRINT_BADLOCATION) {
//    Serial.println("Could not delete in that location");
  } else if (p == FINGERPRINT_FLASHERR) {
//    Serial.println("Error writing to flash");
  } else {
//    Serial.print("Unknown error: 0x"); Serial.println(p, HEX);
  }

  return p;
}

// tieng chuong mo cua 
void right()
{
      digitalWrite(BUZZER, HIGH);
      delay(500);
      digitalWrite(BUZZER, LOW);
}

void error()
{
// coi keu 2 lan
    for(int b=0;b<2;b++)
    {
       digitalWrite(BUZZER, HIGH);
       delay(300);
       digitalWrite(BUZZER, LOW);
       delay(300);
    }
}


// kiem tra mat khau
void kiemtra_Password()
{
  lcd.setCursor(0,0);
  lcd.print("NHAP MK KIEM TRA");
  Noi_Dung_Nhap();
  if( customKey == '#' )
  {
      delay(200);
      lcd.clear();
      if (Custom_Password[0] == Master_Password[0] && 
          Custom_Password[1] == Master_Password[1] &&
          Custom_Password[2] == Master_Password[2] &&
          Custom_Password[3] == Master_Password[3] &&
          Custom_Password[4] == Master_Password[4] &&
          Custom_Password[5] == Master_Password[5] )
      {
          flag=2;
          dem4=0;
      }
      else
        {
          dem4++;
          lcd.print("  SAI MAT KHAU  ");
          delay(1000);
          clearData();
        }
        clearData();
        lcd.clear();
  }
}

//--------------------
// thay doi mat khau
void set_Password()
{
  lcd.setCursor(0,0);
  lcd.print("NHAP MK CU:    ");
  Noi_Dung_Nhap();
  if( customKey == '#' )
    {
      delay(200);
      lcd.clear();
      if (Custom_Password[0] == Master_Password[0] && 
          Custom_Password[1] == Master_Password[1] &&
          Custom_Password[2] == Master_Password[2] &&
          Custom_Password[3] == Master_Password[3] &&
          Custom_Password[4] == Master_Password[4] &&
          Custom_Password[5] == Master_Password[5] )
      {
        clearData();
        while(flag<2)
        {
          MK_NEW();  
        }
        clearData();
        lcd.clear();
      }
      else
        {
          lcd.print("  SAI MAT KHAU  ");
          delay(1000);
          clearData();
        }
    }
    if (customKey == 'D')
    {
      flag=2;
      clearData();
      lcd.clear(); 
    }
}

void MK_NEW()
{
  lcd.setCursor(0,0);
  lcd.print("NHAP MK MOI:  ");
  Noi_Dung_Nhap();
  if( customKey == '#' )
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("  THANH CONG ");
    Master_Password[0] = Custom_Password[0];
    Master_Password[1] = Custom_Password[1];
    Master_Password[2] = Custom_Password[2];
    Master_Password[3] = Custom_Password[3]; 
    Master_Password[4] = Custom_Password[4];
    Master_Password[5] = Custom_Password[5]; 
    delay(1000);
    flag=2;
  }
  if (customKey == 'D')
    {
      flag=2;
      clearData();
      lcd.clear(); 
    }
}

void Noi_Dung_Nhap()
{     
      lcd.setCursor(0,1);
      customKey = customKeypad.getKey();
      if(customKey)
      {
        if(customKey != '#')
          {
            Custom_Password[dem] = customKey;
            lcd.setCursor(dem,1);
            lcd.print(Custom_Password[dem]);
            delay(200);
            lcd.setCursor(dem,1);
            lcd.print("*");
            dem++;
          }
      }
}

void nhap_Password()
{
    lcd.setCursor(0,0);
    lcd.print("MOI NHAP MK:   ");
    Noi_Dung_Nhap();
    
 //---Enter Mat Khau---------------------   
    if( customKey == '#' )
    {
      delay(200);
      lcd.clear();
      
      if (Custom_Password[0] == Master_Password[0] && 
          Custom_Password[1] == Master_Password[1] &&
          Custom_Password[2] == Master_Password[2] &&
          Custom_Password[3] == Master_Password[3] &&
          Custom_Password[4] == Master_Password[4] &&
          Custom_Password[5] == Master_Password[5] )
      {
        dem2=0;
        lcd.print(" MAT KHAU DUNG ");
        delay(1000);
        open_door();
        flag =2;
      } 
      else
      {
        dem2++;
        lcd.print("  SAI MAT KHAU  ");
        delay(1000);
        if(dem2<3)
        {
              flag =0;
        }
        if(dem2==3)
        {
              flag =2;
        }
      } 
      clearData();
      lcd.clear();  
    } 

    if (customKey == 'D')
    {
      flag=2;
      clearData();
      lcd.clear(); 
    }
}

void clearData()
{
  while (dem>0)
  {
        Custom_Password[dem] = 0;
        dem--;
  }
  dem = 0;
  
}

void canhBao()
{
  lcd.clear();
  lcd.setCursor(0,0);lcd.print("*** CANH BAO ***");
  dem2=0;
  dem3=0;
      for(int index=0;index<20;index++)
    {
       digitalWrite(BUZZER, HIGH);
       digitalWrite(LED_G, HIGH);
       delay(200);
       digitalWrite(BUZZER, LOW);
       digitalWrite(LED_G, LOW);
       delay(200);
    }
    
}
