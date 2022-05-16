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
    prizm.setServoSpeed(3,15);// initialize PRIZM

    prizm.setMotorInvert(1,1);
    Wire.begin();
    while (!huskylens.begin(Wire))
    {
        Serial.println(F("Begin failed!"));
        Serial.println(F("1.Please recheck the \"Protocol Type\" in HUSKYLENS (General Settings>>Protocol Type>>I2C)"));
        Serial.println(F("2.Please recheck the connection."));
        delay(100);
    }

    huskylens.writeAlgorithm(ALGORITHM_OBJECT_TRACKING); //Switch the algorithm to object tracking.
    prizm.setServoPosition(1,180);
    prizm.setServoPosition(2,0);
    prizm.setServoPosition(3,90);
}

int xLeft = 160-30;
int xRight = 160+30;

bool isInside(int value, int min, int max)
{
    return (value >= min && value <= max);
}


int lift()
{
  prizm.setMotorPowers(125,125);
  prizm.setServoPosition(1,0);
  prizm.setServoPosition(2,180);
  delay(2000);
  prizm.setServoPosition(3,90);
  delay(3000);

  return 0;
}


int down()
{
  prizm.setMotorPowers(125,125);
  prizm.setServoPosition(1,180);
  prizm.setServoPosition(2,0);
  delay(2000);
  prizm.setServoPosition(3,90);
  delay(3000);

  return 0;
}

int detect()
{
    void loop();
    {
        if (!huskylens.request()) Serial.println(F("Fail to request data from HUSKYLENS, recheck the connection!"));
        else if(!huskylens.isLearned()) Serial.println(F("Nothing learned, press learn button on HUSKYLENS to learn one!"));
        else if(!huskylens.available()) Serial.println(F("No block or arrow appears on the screen!"));
        else
        {
            Serial.println(F("###########"));
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
    if(prizm.readSonicSensorCM(2) >20)
    {  
        if (!huskylens.request()) Serial.println(F("Fail to request objects from HUSKYLENS!"));
        else if(!huskylens.isLearned()) {Serial.println(F("Object not learned!")); prizm.setMotorPowers(125,125);}
        else if(!huskylens.available()) Serial.println(F("Object disappeared!"));
        else
        {  
            HUSKYLENSResult result = huskylens.read();
            if (isInside(result.xCenter, 0, xLeft))
            {
                prizm.setMotorSpeeds(0,200);
            }
            else if(isInside(result.xCenter, xLeft, xRight))
            {
                prizm.setMotorSpeeds(200,200);
            }
            else if(isInside(result.xCenter, 0, xRight))
            {
                prizm.setMotorSpeeds(200,0);
            }
            else
            {
                prizm.setMotorSpeeds(100,0);
            }
        }
    }
    else if(prizm.readSonicSensorCM(2) < 20)
    {
        while(prizm.readSonicSensorCM(2) < 50)
        {
            if(prizm.readSonicSensorCM(2) > 5)
            {
                prizm.setMotorSpeeds(50,50);
            }
            else if(prizm.readSonicSensorCM(2) < 5)
            {
                huskylens.writeAlgorithm(ALGORITHM_OBJECT_CLASSIFICATION);
                if(detect()==2||detect()==0)
                {
                    lift();
                    while(detect()==2||detect()==0)
                    {
                        prizm.setMotorSpeeds(70,-70);
                        delay(1000);
                        if(detect()==1)
                        {
                            while(detect()==1)
                            {
                                prizm.setMotorSpeeds(-100,100);
                                delay(5600);
                                huskylens.writeAlgorithm(ALGORITHM_COLOR_RECOGNITION);
                                while(prizm.readSonicSensorCM(2) >1)
                                {
                                    prizm.setMotorSpeeds(-200,-200);
                                    if(detect()==1 || detect()==2 || detect()==3)
                                    {
                                        prizm.setMotorSpeeds(200,200);
                                        delay(5000);
                                        prizm.setMotorSpeeds(200,-200);
                                        delay(1600);
                                        while(prizm.readSonicSensorCM(2)>1)
                                        {
                                            prizm.setMotorSpeeds(-200,-200);
                                            if(detect()==1)
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
                                                    delay(2500);
                                                    while(prizm.readSonicSensorCM(2)>1)
                                                    {   
                                                        prizm.readSonicSensorCM(2);
                                                        prizm.setMotorSpeeds(10,-10);
                                                        delay(500);
                                                        if(prizm.readSonicSensorCM(2)<150)
                                                        {
                                                            prizm.setMotorSpeeds(10,-10);
                                                            delay(500);
                                                            while(prizm.readSonicSensorCM(2)<150)
                                                            {
                                                                prizm.setMotorSpeeds(200,200);
                                                                if(prizm.readSonicSensorCM(2)<5)
                                                                {
                                                                    huskylens.writeAlgorithm(ALGORITHM_OBJECT_CLASSIFICATION);
                                                                    lift();
                                                                    if(detect()==2||detect()==0)
                                                                    {
                                                                        while(detect()==2||detect()==0)
                                                                        {
                                                                            prizm.setMotorSpeeds(70,-70);
                                                                            delay(1000);
                                                                            if(detect()==1)
                                                                            {
                                                                                while(detect()==1)
                                                                                {
                                                                                    prizm.setMotorSpeeds(-100,100);
                                                                                    delay(5600);
                                                                                    huskylens.writeAlgorithm(ALGORITHM_COLOR_RECOGNITION);
                                                                                    while(prizm.readSonicSensorCM(2) >1)
                                                                                    {
                                                                                        prizm.setMotorSpeeds(-200,-200);
                                                                                        if(detect()==1 || detect()==2 || detect()==3)
                                                                                        {
                                                                                            prizm.setMotorSpeeds(200,200);
                                                                                            delay(5000);
                                                                                            prizm.setMotorSpeeds(200,-200);
                                                                                            delay(1600);
                                                                                            while(prizm.readSonicSensorCM(2)>1)
                                                                                            {
                                                                                                prizm.setMotorSpeeds(-200,-200);
                                                                                                if(detect()==1)
                                                                                                {
                                                                                                prizm.setMotorSpeeds(200,200);
                                                                                                delay(1000);
                                                                                                down();
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
