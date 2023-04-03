  /**
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
#include <NanoEdgeAI.h>

#include <LSM6DSOSensor.h>
#include <lsm6dso_reg.h>

/**
 * UNcomment DATA_LOGGING define ->  DATA LOGGING mode
 * Comment DATA_LOGGING define ->  LEARNING and INFERENCE mode 
 */
#define DATA_LOGGING

#define DATA_INPUT_USER 256 //buffer size
#define AXIS_NUMBER 3
#define NUMBER_LEARN 100

#define SerialPort Serial

float acc_buffer[DATA_INPUT_USER * AXIS_NUMBER] = { 0 };
float acc_x = 0.F;
float acc_y = 0.F;
float acc_z = 0.F;
float last_acc_x = 0.F;
float last_acc_y = 0.F;
float last_acc_z = 0.F;
int32_t accelerometer[3];  // x,y,z accelerometer values in mg

// Sensor component creation 
LSM6DSOSensor Acc(&Wire);

// The code inside the void setup will be executed once, at the beginning of the program
void setup() {
  
  // Initialize led.
  pinMode(LED_BUILTIN, OUTPUT);

  // Initialize serial for output.
  SerialPort.begin(115200);
  
  // Initialize I2C bus.
  Wire.begin();

  //Initialize sensor.
  Acc.begin();
  if (Acc.Enable_X() == LSM6DSO_OK) {
    Serial.println("Success enabling accelerometer.");
  } else {
    Serial.println("Error enabling accelerometer.");
    abort();
  }

  // Set accelerometer Output Data Rate. Available values are: 1.6, 12.5, 26, 52, 104, 208, 417, 833, 1667, 3333, 6667 Hz
  Acc.Set_X_ODR(833.0f); 
  
  // Set accelerometer scale. Available values are: 2, 4, 8, 16 G
  Acc.Set_X_FS(2); 

/* LEARNING and INFERENCE mode */
#ifndef DATA_LOGGING
  Serial.println("****LEARNING MODE***");
  neai_anomalydetection_init();
  for (int i = 0; i < NUMBER_LEARN; i++) {
    fill_acc_buffer();  
    neai_anomalydetection_learn(acc_buffer);
    Serial.print("Learning number = ");
    Serial.println(i);
  }
  
  Serial.println("****INFERENCE MODE***");
#endif

  
}

//The code inside the void loop will be executed repeatedly
void loop() {
  
  fill_acc_buffer();

/* DATA LOGGING mode */  
#ifdef DATA_LOGGING
  for (int i = 0; i < DATA_INPUT_USER; i++) {
    Serial.print(acc_buffer[AXIS_NUMBER * i]);
    Serial.print(" ");
    Serial.print(acc_buffer[AXIS_NUMBER * i + 1]);
    Serial.print(" ");
    Serial.print(acc_buffer[AXIS_NUMBER * i + 2]);
    Serial.print(" ");
  }
  Serial.println(" ");

/* LEARNING and INFERENCE mode */
#else
  uint8_t similarity = 0;
  enum neai_state detectionStatus;
    
  detectionStatus = neai_anomalydetection_detect(acc_buffer, &similarity);

  if(detectionStatus == NEAI_OK){
    if(similarity < 90){
        Serial.print("similarity = ");
        Serial.print(similarity);
        Serial.println(" -> ABNORMAL CONDITION");
      }
    else{
        Serial.print("similarity = ");
        Serial.print(similarity);
        Serial.println(" -> REGULAR CONDITION");
        }
    }

#endif


}

/**
 * @brief This function collects accelerometer samples until to fill the buffer of 1 NEAI signal. 
 * @param void.
 * @return void.
 */
void fill_acc_buffer() {
    for (int i = 0; i < DATA_INPUT_USER; i++) {
      get_acc_values();
      acc_buffer[AXIS_NUMBER * i] = acc_x;
      acc_buffer[AXIS_NUMBER * i + 1] = acc_y;
      acc_buffer[AXIS_NUMBER * i + 2] = acc_z;
  }
}


/**
 * @brief This function collect 1 accelerometer sample (3 values : x,y,z)
 * @param void.
 * @return void.
 */
void get_acc_values()
{
  /* Polling method to get a complete buffer */
  do {
    Acc.Get_X_Axes(accelerometer); //Get accelerometer data (x,y,z)
    acc_x = (float)accelerometer[0];
    acc_y = (float)accelerometer[1];
    acc_z = (float)accelerometer[2];
  }
  while (acc_x == last_acc_x && acc_y == last_acc_y && acc_z == last_acc_z);

  last_acc_x = acc_x;
  last_acc_y = acc_y;
  last_acc_z = acc_z;
}
