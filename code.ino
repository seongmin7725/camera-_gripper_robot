#include "HUSKYLENS.h"
#include <PRIZM.h>
PRIZM prizm;    

HUSKYLENS huskylens;

void setup() 
{

    Serial.begin(115200); 
    prizm.PrizmBegin();
    prizm.readSonicSensorCM(2);
     prizm.setServoSpeed(1,15);
    prizm.setServoSpeed(2,15);
    prizm.setServoSpeed(3,15);

    prizm.setMotorInvert(1,1);
    Wire.begin();
    while (!huskylens.begin(Wire))
    {
        Serial.println(F("Begin failed!"));
        Serial.println(F("1.Please recheck the \"Protocol Type\" in HUSKYLENS (General Settings>>Protocol Type>>I2C)"));
        Serial.println(F("2.Please recheck the connection."));
        delay(100);
    }

    huskylens.writeAlgorithm(ALGORITHM_OBJECT_TRACKING);
    prizm.setServoPosition(1,180);
    prizm.setServoPosition(2,0);
    prizm.setServoPosition(3,90);
}

int xLeft = 160-30; // 화면 좌측 범위 설정
int xRight = 160+30; //화면 우측 범위 설정

bool isInside(int value, int min, int max) // 화면 좌우 범위 설정 함수
{
    return (value >= min && value <= max);
}


int lift() // 물체를 들어올리는 함수
{
  prizm.setMotorPowers(125,125);
  prizm.setServoPosition(1,0);
  prizm.setServoPosition(2,180);
  delay(2000);
  prizm.setServoPosition(3,90);
  delay(3000);

  return 0;
}


int down() // 물체를 내려놓는 함수
{
  prizm.setMotorPowers(125,125);
  prizm.setServoPosition(1,180);
  prizm.setServoPosition(2,0);
  delay(2000);
  prizm.setServoPosition(3,90);
  delay(3000);

  return 0;
}

int detect() // 카메라로 물체, 목표지점 등을 인식하는 함수
{
    void loop();
    {
        if (!huskylens.request()) Serial.println(F("Fail to request data from HUSKYLENS, recheck the connection!"));
        else if(!huskylens.isLearned()) Serial.println(F("Nothing learned, press learn button on HUSKYLENS to learn one!"));
        else if(!huskylens.available()) Serial.println(F("No block or arrow appears on the screen!"));
        else
        {
            while (huskylens.available())
            {
                HUSKYLENSResult result = huskylens.read();
                if(result.ID==0)
                {
                    return 0;
                }
                else if(result.ID==1)
                {
                    return 1;
                }
                else if(result.ID==2) 
                {
                    return 2;
                }
            }
        }
    }
}


void loop()
{
    if(prizm.readSonicSensorCM(2) >= 20) // 로봇과 물체 사이의 거리가 20cm 이상일때 
    {  
        
        if (!huskylens.request()) Serial.println(F("Fail to request objects from HUSKYLENS!"));
        else if(!huskylens.isLearned()) {Serial.println(F("Object not learned!")); prizm.setMotorPowers(125,125);}
        else if(!huskylens.available()) Serial.println(F("Object disappeared!"));
        else
        {  //@@플라스틱병 탐지 시작@@//
            HUSKYLENSResult result = huskylens.read();
            if (isInside(result.xCenter, 0, xLeft)) // 물체가 화면 왼쪽에 있으면 좌회전
            {
                prizm.setMotorSpeeds(0,200);
            }
            else if(isInside(result.xCenter, xLeft, xRight)) // 물체가 화면 중앙에 있으면 직진
            {
                prizm.setMotorSpeeds(200,200);
            }
            else if(isInside(result.xCenter, 0, xRight)) // 물체가 화면 오른쪽에 있으면 우회전
            {
                prizm.setMotorSpeeds(200,0);
            }
            else
            {
                prizm.setMotorSpeeds(100,0); // 탐지가 안되면 물체를 찾기위해 우회전
            }
        }
    }
    else if(prizm.readSonicSensorCM(2) < 20)  // 로봇과 물체 사이의 거리가 20cm 미만일때
    {
        while(prizm.readSonicSensorCM(2) < 50)
        {
            if(prizm.readSonicSensorCM(2) >= 5) // 5cm 이상이면 속도줄이기
            {
                prizm.setMotorSpeeds(50,50);
            }
            else if(prizm.readSonicSensorCM(2) < 5) // 5cm 미만이면 잡기
            {
                huskylens.writeAlgorithm(ALGORITHM_OBJECT_CLASSIFICATION);   // OBJECT_CLASSIFICATION 모드로 전환
                if(detect()==2||detect()==0) 
                {
                    lift(); 
                    while(detect()==2||detect()==0) 
                    {
                        prizm.setMotorSpeeds(70,-70); // 목표지점이 나올때 까지 회전
                        delay(1000);
                        if(detect()==1)  // 탐지
                        {
                            while(detect()==1)
                            {
                                prizm.setMotorSpeeds(-100,100); 180도 회전
                                delay(5600);
                                huskylens.writeAlgorithm(ALGORITHM_COLOR_RECOGNITION); // COLOR_RECOGNITION 모드로 전환
                                while(prizm.readSonicSensorCM(2) >1)
                                {
                                    prizm.setMotorSpeeds(-200,-200); // 빨, 파, 초 색상이 입력 될때까지 후진
                                    if(detect()==1 || detect()==2 || detect()==3)
                                    {
                                        prizm.setMotorSpeeds(200,200);
                                        delay(5000);
                                        prizm.setMotorSpeeds(200,-200);
                                        delay(1600);
                                        while(prizm.readSonicSensorCM(2)>1)
                                        {
                                            prizm.setMotorSpeeds(-200,-200);
                                            if(detect()==1) //파란색 입력하면 내려놓기
                                            {
                                                prizm.setMotorSpeeds(200,200);
                                                delay(5500);
                                                down();
                                                if(prizm.readSonicSensorCM(2)>=0)
                                                {
                                                    prizm.setMotorSpeeds(-200,-200);
                                                    delay(5000);
                                                    prizm.setMotorSpeeds(-200,200);
                                                    delay(1500);
                                                    prizm.setMotorSpeeds(200,200);
                                                    delay(6000);
                                                    prizm.setMotorSpeeds(-200,200);
                                                    delay(2500);                        //음파로 물체 탐지를 위한 준비
                                                    while(prizm.readSonicSensorCM(2)>1)
                                                    {   // @@콜라캔 탐지 시작@@//
                                                        prizm.readSonicSensorCM(2);
                                                        prizm.setMotorSpeeds(10,-10);
                                                        delay(500);
                                                        if(prizm.readSonicSensorCM(2)<150)
                                                        {
                                                            prizm.setMotorSpeeds(10,-10);
                                                            delay(500);                         //천천히 회전하면서 물체 탐지
                                                            while(prizm.readSonicSensorCM(2)<150)
                                                            {
                                                                prizm.setMotorSpeeds(200,200);
                                                                if(prizm.readSonicSensorCM(2)<5)
                                                                {
                                                                    huskylens.writeAlgorithm(ALGORITHM_OBJECT_CLASSIFICATION); //OBJECT_CLASSIFICATION 모드로 변경
                                                                    lift();
                                                                    if(detect()==2||detect()==0)
                                                                    {
                                                                        while(detect()==2||detect()==0)
                                                                        {
                                                                            prizm.setMotorSpeeds(70,-70);  //목표지점이 나올때 까지 회전
                                                                            delay(1000);
                                                                            if(detect()==1)
                                                                            {
                                                                                while(detect()==1)
                                                                                {
                                                                                    prizm.setMotorSpeeds(-100,100); // 180도 회전후 COLOR_RECOGNITION모드로 전환
                                                                                    delay(5600);
                                                                                    huskylens.writeAlgorithm(ALGORITHM_COLOR_RECOGNITION);
                                                                                    while(prizm.readSonicSensorCM(2) >1)
                                                                                    {
                                                                                        prizm.setMotorSpeeds(-200,-200);
                                                                                        if(detect()==1 || detect()==2 || detect()==3)  // 빨, 파, 초가 나올때까지 후진
                                                                                        {
                                                                                            prizm.setMotorSpeeds(200,200);
                                                                                            delay(5000);
                                                                                            prizm.setMotorSpeeds(200,-200);
                                                                                            delay(1600);
                                                                                            while(prizm.readSonicSensorCM(2)>1)
                                                                                            {
                                                                                                prizm.setMotorSpeeds(-200,-200);
                                                                                                if(detect()==1) //파란색이 나타나면
                                                                                                {
                                                                                                prizm.setMotorSpeeds(200,200); // 캔과 플라스틱 병의 가는 거리가 다름
                                                                                                delay(1000);
                                                                                                down(); // 물체 내려놓기
                                                                                                }
                                                                                            }
                                                                                        }
                                                                                    }
                                                                                }
                                                                            }
                                                                        }
                                                                    }   
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}





void printResult(HUSKYLENSResult result)
{
    if (result.command == COMMAND_RETURN_BLOCK)
    {
        Serial.println(String()+F("Block:xCenter=")+result.xCenter+F(",yCenter=")+result.yCenter+F(",width=")+result.width+F(",height=")+result.height+F(",ID=")+result.ID);
    }
    else if (result.command == COMMAND_RETURN_ARROW)
    {
        Serial.println(String()+F("Arrow:xOrigin=")+result.xOrigin+F(",yOrigin=")+result.yOrigin+F(",xTarget=")+result.xTarget+F(",yTarget=")+result.yTarget+F(",ID=")+result.ID);
    }
    else
    {
        Serial.println("Object unknown!");
    }
}
