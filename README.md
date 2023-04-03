# NanoEdge - Vibration abnormality detection

**Description** : This readme describes how to integrate NanoEdge AI Studio library in Arduino IDE and how to reproduce the Anomaly Detection demonstration based on vibration data (3D accelerometer). 

## Hardware requirements 

- NUCLEO-F411RE : https://www.st.com/en/evaluation-tools/nucleo-f411re.html
- X-NUCLEO-IKS01A3 (use of the 3D accelemeter of the LSM6DSO sensor) : https://www.st.com/en/ecosystems/x-nucleo-iks01a3.html
- 3 speeds USB fan : https://www.amazon.com/Fixget-Portable-Desktop-Rotation-Personal/dp/B083GLQDNX/
- USB cable (for the NUCLEO-F411RE)

## Software requirements 

- Arduino IDE (v1.8.19) : https://www.arduino.cc/en/software
- Arduino Sensor Library : STM32duino LSM6DSO (v2.0.3)
    - download from Arduino Library Manager (Tools -> Manage Libraries)
    - *extra ressource* : https://github.com/stm32duino/LSM6DSO
 - PuTTY (v0.78) : https://www.putty.org/
    - **NOTE** : Put the plink.exe in your PATH (e.g, C:\Program Files\PuTTY in environment variable)
 - NanoEdge AI Studio (v3.2.1 or lastest) : https://stm32ai.st.com/download-nanoedgeai/
 - *STM32duino_LSM6DSO.ino* (Arduino project presents in the GitHub repository)
  
## Steps 
1) Install Arduino IDE (see Software requirements )

2) Open a New Arduino project (sketch_{dateX})

3) Configure **STM32duino** in Arduino IDE to allow Arduino code execution on Nucleo boards (see below ressources) : 
    - https://github.com/stm32duino/Arduino_Core_STM32/wiki/Getting-Started
    - https://www.youtube.com/watch?v=FzxLPDNBqng&t=171s

4) Run a Blink example to validate the STM32duino configuration : https://github.com/stm32duino/Arduino_Core_STM32/wiki/Blink-example

5) Close the New Arduino project (sketch_{dateX}) and the Blink project

6) Copy-paste the *STM32duino_LSM6DSO** Arduino project folder in your Arduino folder 
    - **NOTE** : Your path should be similar as : ../Arduino/_STM32duino_LSM6DSO/STM32duino_LSM6DSO.ino

7) In Arduino IDE, install the **STM32duino LSM6DSO library** (see Software requirements)
    - Result #1 : You should be able to see a "STM32duino_LSM6DSO" folder in Arduino/libraries
    - Result #2 : You should be able to add the sensor header files (LSM6DSOSensor.h and lsm6dso_reg.h) from Stetch -> Include Library -> STM32duino LSM6DSO
    - NOTE : No need to do the result #2 because the sensor header file are already included inside STM32duino_LSM6DSO.ino

8) Do the DATA COLLECTION for the regular and abnormal conditions of the fan

    8.1) Install the harware material
    
    - Connect the X-NUCLEO-IKS01A3 to the NUCLEO-F411RE 
    - Put NUCLEO-F411RE + X-NUCLEO-IKS01A3 on the top of the fan
    - Connect the USB cable of fan to the PC
    - Connect the USB cable of the NUCLEO-F411RE to the PC

    8.2) In STM32duino_LSM6DSO.ino, be sure that "#define DATA_LOGGING" is UNCOMMENTED

    8.3) Flash the Arduino code in the DATA LOGGING mode on the board by clicking on the "Upload" button (arrow icon)
    - **NOTE** : You can see on a console (e.g, Tera Term or Arduino Serial Monitor)

    8.4) Save the sensor data to a csv file thanks to PuTTY
     - Install PuTTY (see Requirements)
     - Configure PuTTY
        - In **"Session" tab** : Put "Connection type" = Serial -> put the correct COM number in "Serial line" and put 115200 in "Speed"
        - In **"Session/Logging" tab** :    
            - Check "Printable output" in "Session logging"
            - In "Log file name" put the location of the csv file (e.g, .../datalogging/regular.csv or .../datalogging/abnormal.csv)
         - Again in **"Session" tab**:
            - Save this configuration by typing a name in "Saved Sessions" (e.g, arduino_datalogging) and click on "Save" 
            - **NOTE** : If you open later PuTTy you can load your saved session by selecting the name of the savec session and by clicking on "Load"
        
        **Extra PuTTY ressource** : https://www.circuitbasics.com/logging-arduino-data-to-files-on-a computer/#:~:text=Enter%20the%20COM%20port%20(e.g.,save%20the%20file%20you%20logged

    8.5) Collect REGULAR condition
    - Put the fan in the REGULAR condition (e.g at the speed1 without any anomalies)
    - Be sure of your PuTTy settings and name the output csv file "regular.csv" (see 8.4)
    - On PuTTY, click on "Open" to launch the collection
    - Wait a bit more than 1min in order to have around 100 lines (100 signals) in the output csv file
    - After more than 1min close the PuTTY console

    8.6) Collect ABNORMAL condition
     - Put the fan in the ABNORMAL condition (e.g put a block of paper in front of the fan to generate an obstruction)
     - Reproduce the step 8.5. The only difference is to name the output csv file "abnormal.csv"

9) Give the Regular and Abnormal datasets to NanoEdge AI Studio (NEAI) to generate the proper AI library
    - Parameters in NEAI
        - Project : Anomaly Detection
        - Target : STM32F4
        - Sensor Type : Accelerometer 3 axes

10) Integrate the NEAI library inside STM32duino_LSM6DSO.ino
    - Create a NanoEdgeAI folder in "Arduino/libraries" with the below structure and put/replace "NanoEdgeAI.h" and  "libneai.a" files in the right location:

        - Arduino
            - libraries
                - STM32duino_LSM6DSO
                - NanoEdgeAI
                    - src
                        - NanoEdgeAI.h -> replace the file from the latest AI library generated by NEAI!
                        - cortex-m4
                            - libneai.a -> replace the file from the latest AI library generated by NEAI!
                - readme.txt
                
    - Be sure that the "#include <NanoEdgeAI.h>" is present. Otherwise you can add it from Stetch -> Include Library -> NanoEdgeAI

11) In STM32duino_LSM6DSO.ino, be sure that "#define DATA_LOGGING" is COMMENTED

12) Flash the Arduino code in the LEARNING and INFERENCE mode on the board by clicking on the "Upload" button (arrow icon)

13) Open a console (e.g, Tera Term or Arduino Serial Monitor)

14) Realize the LEARNING of the NORMAL condition
    - Put the fan in the NORMAL condition (e.g fan at the speed 1) 
    - Press the reset button of the board in order to launch the LEARNING for 100 signals (define in the Arduino code by #define NUMBER_LEARN 100)
    - Wait until the end of the LEARNING

15) At the end of the LEARNING phase the code go automatiquelly in the INFERENCE mode where you can check NORMAL and  ABNORMAL conditions



