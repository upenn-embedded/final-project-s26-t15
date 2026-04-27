[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/-Acvnhrq)

# Final Project

**Team Number: Team 15**

**Team Name: Argus**

| Team Member Name | Email Address          |
| ---------------- | ---------------------- |
| Seth Lee         | sethlee@seas.upenn.edu |
| Jerry Zhang      | jerryjz@seas.upenn.edu |
| Thomas Liu       | tholiu@seas.upenn.edu  |

**GitHub Repository URL: https://github.com/upenn-embedded/final-project-s26-t15.git**

**GitHub Pages Website URL:** https://upenn-embedded.github.io/final-project-s26-t15

## Final Project Proposal

### 1. Abstract

Our final project (Argus) is a pair of augmented reality glasses that utilizes a HUD and sensor suite to collect and display movement information along with live timers that always keep the user up to date with their activities. Using a STM32 MCU and XIAO ESP32S3 processors, we can wirelessly transmit data from the glasses to our pocket. Featuring a transparent OLED for the HUD, 9-axis IMU for spatial tracking, and capacitive touch sensors, we combine portability with performance. The ESP32s will relay data wirelessly from the glasses to the STM32 in the user's pocket which serves as the compute hub.

### 2. Motivation

Argus helps bridge the gap between physical activity and real-time data accessibility without sacrificing situational awareness. While commercial AR glasses often prioritize complex media ecosystems and overlays, Argus focuses on lightweight, purpose-focused architecture that offloads heavy processing to a pocket-based STM32 to minimize weight. The sensors then provide a distraction-free, hands-free HUD for users. Instead of having to look at a watch or pull out a phone, users can find relevant telemetry in their line of sight. The emphasis on power efficiency and simplicity makes it perfect for tasks like checking the direction of travel while running or biking, where looking down could be unsafe. We hope that this project can provide a base for more advanced sensors and features.

### 3. System Block Diagram

![1773671196132](image/README/1773671196132.png)

### 4. Design Sketches

The frame of the glasses will be customized through 3D printing and laser cutting in order to house the hardware components.

![1773644038002](image/README/1773644038002.jpg)

### 5. Software Requirements Specification (SRS)

**5.1 Definitions, Abbreviations**

STM32 - STM32F411RE microcontroller on the NUCLEO-F411RE development board.

Base ESP32 - the ESP32 module connected to the STM32 via UART.

Glasses ESP32 - The ESP32 module mounted on the glasses frame. Receives data from the Base ESP32 and interacts with peripherals.

IMU - Inertial Measurement Unit. Accelerator/gyroscope sensor for step detection.

**5.2 Functionality**

| ID     | Description                                                                                                                                                                                              |
| ------ | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| SRS-01 | The STM32 shall read the IMU accelerometer data over I2C at a minimum rate of 50 Hz to enable accurate step detection.                                                                                   |
| SRS-02 | The step detection algorithm shall detect a walking step within 400ms of it occuring, with no more than 10% error over a 20-step walk test.                                                             |
| SRS-03 | The display shall update its shown content within 500ms of a button press. This includes the full round-trip: glasses ESP32 → base ESP32 → STM32 → base ESP32 → glasses ESP32 → display.            |
| SRS-04 | The STM32 shall synchronize its RTC with NTP time via the base ESP32 over UART once at startup.                                                                                                         |
| SRS-05 | The display shall cycle through atleast three different modes: time view, step count view, and compass view.                                                                                             |
| SRS-06 | The STM32 shall receive and process data from the base ESP32 over UART within 100ms of transmission.                                                                                                     |
| SRS-07 | The STM32 shall compute the compass direction from the magnetometer data from the IMU and display cardinal direction (N, NE, E, SE, S, SW, W, NW) accurate to within +/- 22.5 degrees after calibration. |
| SRS-08 | The OLED display will communicate using SPI translated through UART wirelessly between the two ESP32S3 modules to maximize screen data bandwidth.                                                        |

### 6. Hardware Requirements Specification (HRS)

**6.1 Definitions, Abbreviations**

See Section 5.1 for all definitions and abbreviations.

**6.2 Functionality**

| ID     | Description                                                                                                                                                                                                                   |
| ------ | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| HRS-01 | The system shall use an IMU with atleast 3-axis accelerometer, 3-axis gyroscope, and 3-axis magnetometer capabilities, communicating over I2C, to provide motion data for step-count and compass data for direction.         |
| HRS-02 | The display shall be smaller enough to mount on glasses frames and support atleast two lines of text output.                                                                                                                  |
| HRS-03 | Several push buttons shall be mounted on the glasses frame and be able to register a press within 100ms of acutuation to enable various functionalities such as toggling modes, changing parameters, and powering on and off. |
| HRS-04 | Two ESP32 modules shall be able to maintain wireless connection with a round-trip latency of less than 500ms within 5m of distance.                                                                                           |
| HRS-05 | The STM32 shall communicate with the base ESP32 over UART at a baud rate of atleast 9600 bps.                                                                                                                                 |
| HRS-06 | The glasses shall be powered by a battery capable of powering the glasses for atleast 15-30 minutes continuously.                                                                                                             |

### 7. Bill of Materials (BOM)

The major components needed are as follows: Nucleo STM32 F411RE Devboard, 2x XIAO ESP32S3 modules (wifi communication), DFR0934 monochrome transparent OLED (HUD), 9-DOF IMU (motion tracking), a LED to indicate power and debug features, external buck converter to isolate OLED from possibly noisy ESP32 LDO, 5 switches (mode, up, down, toggle, on/off), and a battery.

| Component        | Part Selection      | Justification & Requirement Link                                                                                                                                                                                       |
| ---------------- | ------------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| Main Processor   | NUCLEO-F411RE       | Master Controller: Handles the step-detection algorithm (SRS-02), RTC synchronization (SRS-04), and I2C sensor polling at 50 Hz (SRS-01). Selected for its ability to use interrupts and also industry standard usage. |
| Wireless Bridge  | 2x XIAO ESP32S3     | Communication Relay: Facilitates the 500ms round-trip display update (SRS-03) and provides the wireless link required by HRS-04. Operates as a simple relay with no application code to satisfy system constraints.    |
| Transparent HUD  | DFR0934 OLED        | Visual Interface: Meets HRS-02 for small-form-factor mounting and supports the multi-mode display requirements (Time, Steps, Compass) outlined in SRS-05.                                                              |
| Motion Tracking  | 9-DOF IMU (BNO085)  | Navigation & Fitness: Provides the 3-axis Accel/Gyro/Mag data required by HRS-01. Enables compass accuracy within 22.5 degrees (SRS-07) and step detection (SRS-02).                                                   |
| Power Isolation  | Buck Converter      | System Stability: Ensures that WiFi bursts do not cause brownouts or electrical issues, allowing the battery to meet the 15 to 30 minute continuous run-time requirement (HRS-06).                                     |
| User Input       | 5x Tactile Switches | Manual Control: Satisfies HRS-03 for registration within 100ms. Enables mode toggling (SRS-05) and system power management.                                                                                            |
| Power Source     | 1S LiPo Battery     | Portable Battery: Selected to meet the weight requirements for glasses-mounting while satisfying the run-time specifications of HRS-06.                                                                                |
| Status Indicator | Power LED           | Visual Feedback: Provides an immediate hardware-level On state to confirm the success of power-on sequences defined in HRS-03.                                                                                         |

https://docs.google.com/spreadsheets/d/1X19fjBgSUE6MPIu0bsC3vZN--DqyP1meB3pUoyBd7XM/edit?usp=sharing

### 8. Final Demo Goals

On Demo day, the smart glasses will be demonstrated by having someone wear them. The STM32 with its base ESP32 will have remain close by when the user is using the smart glasses to reduce latency and improve connectivity between the two ESP32 modules. Features will be tested by activating them using the side push buttons and moving around which will verify functionality of IMU features (cardinal direction and step counting) and the screen updating.

### 9. Sprint Planning

| Milestone  | Functionality Achieved                                                                                                                                                                                                                                                                                                                            | Distribution of Work                                                                        |
| ---------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------- |
| Sprint #1  | 1] Get individual components to connect to each other, i.e STM32 and ESP32.<br />2] Verify UART connection between STM32 and ESP32, wifi-connection, and confirm that STM32 can recieve button press.<br />3] Begin prototyping glasses frame + begin calibrating screen for close-up view.<br />4] synchronize RTC time with NTP time via wifi. | Seth - Firmware<br />Jerry - Display + Firmware<br />Thomas - Glasses Frame                 |
| Sprint #2  | 1] Get STM32 to receive IMU data over I2C and display steps / compass direction<br />2] Get display to show text/time.<br />3] Finalize power source for the glasses display + ESP32.<br />4] Finalize Glasses Frame and test it for proper fitting/balance.                                                                                      | Seth - Firmware<br />Jerry - Display + Peripheral Integration<br />Thomas - Glasses Frame  |
| MVP Demo   | 1] Get steps algorithm and compass direction working.<br />2] Fully integrate ESP32 and display onto glasses frame.<br />3] Ensure that calibration + synchronizing of RTC time work as expected.                                                                                                                                                 | Seth - Firmware<br />Jerry - Peripheral Integration + Calibration<br />Thomas - Integration |
| Final Demo | Ensure that final product is fully functional and meets requirements.                                                                                                                                                                                                                                                                             |                                                                                             |

**This is the end of the Project Proposal section. The remaining sections will be filled out based on the milestone schedule.**

## Sprint Review #1

### Last week's progress

1. Finalized most of BOM and determined general design of prototype that we are now working towards.
2. Ordered parts for parts of the deisgn that we are more certain on
3. Planning design and build schedule.

### Current state of project

1. Found several issues with current design mostly surrounding difficulty of writing I2C driver with our IMU.

-> Strange behavior listed in errata of F411 where clock stretching results in miscommunication between master and slave, which is problematic since the previous IMUs chosen (BNO085 or BNO055) both use it. Also discovered these devices don't contain built in step counter and I2C will be very difficult to write.

-> Switched to LSM6DS0 because it contains step counter and works over SPI. Since screen also uses SPI, it makes it convenient so we can write one driver that is also easier to write.

2. Button presses and interacting with the STM32F411 figured out

-> Basic interrupt or GPIO functionality

3. Calculated object distance (d0) constraints from a target virtual image distance (d1) and magnifcation (m0) constraint.

   [d0 calculator (32)](https://www.desmos.com/calculator/zhws0yhlh0)

   ![1775258313158](image/README/1775258313158.png)
4. Designed the mechanical layout for the hardware and optics Determined methods to reduce d0 due to limited focal length, and reduce mount size by adding a secondary mirror.

   ![1775258730277](image/README/1775258730277.jpg)
5. Prototyped mounts to hold hardware so they can be clipped onto glasses.

   ![1775259339481](image/README/1775259339481.png)

![1775239160908](image/README/1775239160908.png)

### Next week's plan

1. Finish full implementation of step counter (Seth)

-> Requires full functionality of SPI drivers

-> Get it to read out onto a serial output

-> Est. time: 1hr if SPI driver is working

2. (If screen arrives) Write library and implement basic functionality of screen (Jerry)

-> Allow for some basic interfacing with screen, determine how to set the screen color or edit pixels

-> Est. time: 2-3hrs

3. Write SPI driver (Jerry)

-> SPI driver has all necessary features for at least the IMU at this time, can initialize and operate independently without excessive debugging or compatibility issues with devices

-> Est. time: 3-6hrs

4. Write IMU driver (Seth)

-> Implement all major functions (get linear movement, step tracking, etc)

-> Est. time: 3-6hrs

5. CAD and print out a prototype (Thomas)

-> 3D-print a prototype (or gather a pair of real glass) and mount a dummy weight mimicking our device onto glasses and test weight distribution.

-> Est. time: 3-6hrs

Hopefully we also get some parts in.
Work Distribution:
Seth: Firmware and development of STM32 portions
Jerry: Electrical hardware and writing SPI driver
Thomas: Mechanical design and prototyping

In case anyone is not available to work on something, everyone is reading the STM32 textbook and is proficient in mechanical design so we can assist each other if extra work is needed.

## April 9th, 2026

As of now, the main SPI driver is written and the IMU driver is in progress. Using the textbook as a reference, we're trying to write the driver for the LSM6DS0 so that it can use SPI properly. The mechanical design is nearly done (insert screenshot). We are also able to register basic GPIO input and some input capture using buttons so far on the STM32.

## Sprint Review #2

### Last week's hardware progress

This week, we targeted integrating various sensors and drivers, making sure that our devices are taking and outputting the proper data. Additionally, we hoped to get some of the new parts (OLED, lenses, Adafruit Blackpill STM32) so that we could move to our intended hardware setup. While the lenses and Blackpill STM32 have not arrived, the OLED has arrived.

After analyzing the hardware components' sizes and the constrains on the optical components, new designs analyzed. The final design for prototyping will resemble design one, but with the hardware shifted to account for proper lens spacing. Later, prototypes will look like design three if space is too constrained.

![1775961422875](image/README/1775961422875.png)

To attempt to reduce the total thickness of the mounts, the furthest distance between the mirror and the OLED was minimized. Onshape was used to simulate raytracing by creating variables and minimizing the base length through different configurations.
![1775961446992](image/README/1775961446992.png)

Once the hardware arrived, components were measured with calipers to verify their measurements. A pair or glasses was also measured as a reference. Then, a canvas scaled to the glasses' size was imported into Fusion 360, where components were resized and the mount was scaled. Printing is underway to enable testing of the optics once the mirrors and lens arrives.![1775961470511](image/README/1775961470511.png)

### Last week's electrical progress

Last week, we made some important progress on the integration of sensors and development of the blueprint of the device. Using the basic plan we had devised last week to primarily work on device drivers, communication protocols, and developing the optical system, we have reached a position where we are satisfied with current progress and are on track to create a MVP.

The table below reiterates some of our barebones MVP guidelines:

| MVP Goal                               | Description                                                                                                                                                                                                           | Status                                                                                                                                                                                    |
| -------------------------------------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| #1. IMU Step Counter                   | The smartglasses will contain basic motion tracking capabilities, using an IMU to track the wearer's steps.                                                                                                           | Complete. The SPI and IMU drivers have been fully written for the LSM6DS0, but there are other issues (written below).                                                                    |
| #2. On-Screen Timer System             | A wearer will be able to set and be notified of timers for use such as in timing activities like cooking or taking a practice exam.                                                                                   | Incomplete. The timer system will be created through the use of hardware timers, the software has not been written yet.                                                                   |
| #3. 2-4 Buttons for Screen Control     | The smartglasses will have several buttons to interface with the GUI to interact with features such as the step counter, timer system, or compass.                                                                    | Incomplete. Basic GPIO features have been tested, but software must be written. This will be complete after all other subsystems are operable.                                            |
| #4. Optical Waveguide HUD              | The smartglasses will contain an optical waveguide to translate the light of the OLED to a beamsplitter which will reflect the light into the wearer's eyes, giving the illusion of a "floating" HUD in their vision. | In Progress. The OLED has arrived and its drivers are currently being developed. The calculations for the lens size and mirrors have also been completed.                                 |
| #5. CAD of Glasses or Waveguide Holder | The smartglasses will exist in the form of either a clip-on attachment to an existing pair of glasses or as a standalone frame.                                                                                       | In Progress. The CAD has partially been completed with some additional testing and printing of the parts needed. Additionally, not all of the optical hardware (mirrors) has arrived yet. |
| #6. Navigation System                  | The smartglasses will contain a basic navigation system consisting of a cardinal compass to allow the user to receive navigation data. This may extend outwards into the usage of GPS or other features.              | The initial IMU used (LSM6DS0) does not contain the magnetometer required for the compass. A version with the magnetometer has been ordered.                                              |

### Current state of project

To summarize, we have written drivers and tested functionality for the IMU step counter subsystem. The video below shows (poorly) the operation of the step counter:

https://drive.google.com/file/d/1mDCpxerKAkAiZ3uGaSlx3KKA8HsJcfL3/view?usp=sharing

This link will be updated to a YT video in the future.

We found that the step counter works well, although there is a delay in counting steps. This is by design as to avoid overcounting steps as a result of other oscillatory motion.

The SPI and IMU drivers were also written, taking about the expected amount of time as listed in Sprint Review #1. Unforunately, the OLED did not arrive until recently, so that was not able to be tested yet. However, the driver for that is in progress. Since it also uses SPI, we expect setup to be easier along with our past lab experience in writing a portion of a screen driver.

Here is the step counter:

<img src="image/README/1775964295827.jpg" width="500">

This image shows some of our commits since the last sprint review.

<img src="image/README/1775963073351.png" width="500">

Unfortunately, we reailzed that the existing parts list was inaccurate and we were unable to find certain parts, forcing us to place some orders later than we wanted to. For one, we wanted to use a 9-DOF IMU so that we could also take advantage of the magnetometer to create a compass. However, Detkin does not stock any dedicated magnetometers and their supply of IMUs is inaccurate to their parts list, only having the LSM6DS0 and some other IMUs which did not have features which we required. This led us to order the Adafruit LSM9DS1, which is effectively the same IMU (saves us time writing the driver) with a magnetometer.

The initial lens choice also proved more difficult to work with than initially thought. Due to the short focal length of the lens (32mm), we found it difficult to place all of our components within that space. However, we found a workaround where we were able to place the lens closer to mitigate this issue.

The battery has also arrived and we are considering using power management to properly adjust the 3.0 - 4.2V LiPo to the proper operating voltages so that we can use it for the entirety of its battery voltage range.

### Next week's plan

The table below illustrates our plans for the following week. To summarize, we hope to test functionality of the OLED, develop a functional CAD and 3d-printed MVP of the device, and develop the UI and user optical system.

| Goal                                                                                                        | Owner  | Est. Time |
| ----------------------------------------------------------------------------------------------------------- | ------ | --------- |
| Develop OLED drivers, write screen functions                                                                | Seth   | 4-6 Hrs   |
| Implement components of UI and improve upon IMU functionality by adding compass and optimizing step counter | Jerry  | 3-4 Hrs   |
| Finish CAD and optical system design                                                                        | Thomas | 3-4 Hrs   |
| Update SPI drivers to account for OLED                                                                      | Jerry  | 1 Hr      |
| Write software to handle display switching and update main loop                                             | Seth   | 2-3 Hrs   |
| 3D print CAD designs and build first MVP (if parts all arrive)                                              | Thomas | 2-3 Hrs   |

In general, the goal is to prepare for the MVP demo and to develop a usable hardware system that is modular and lightweight. The OLED drivers will be written and some basic functionality involving GPIO and switching screens should also be implemented as well.

We are well on track and keeping each other up to date with progress.

## MVP Demo

**Refined System Diagram:**

![1776481041381](image/README/1776481041381.png)

**Firmware Impementation:**

The main firmware is in the main.c file, which calls functions from every other .c file, including the oled.c, spi.c, imu.c, files. The main.c file is where the state of the screen is handled and where all the initialize functions are called. Then, in oled.c, spi.c, and imu.c (which are our drivers), we have the actual function code that instantiates SPI, which is used for both the OLED screen and IMU.

More specifically:

In spi.c: we have two independent SPI buses that run. One SPI bus is for the OLED screen and the other one is for the IMU.

In imu.c: we have functions that read the register from the IMU that stores the number of steps taken. This file also handles interrupts that come from the IMU, which refresh the OLED screen to display the updated step count.

In oled.c: This is the main driver for our SSD1308 oled screen. This has functions for OLED brightness, resetting the OLED screen, and drawing pixels on the OLED screen.

In font.c: This file has various draw functions that use the draw_pixel function from oled.c.

Below is an Image of the working OLED. The text on the OLED will eventually be cast to the front of the lens on the AR glasses.

![1776478366829](image/README/1776478366829.png)

**Software Impementation:**

We have achieved some of our Software Requirements. Specifically, we have satisfied SRS-01, which is to read the IMU accelerometer data for accurate step detection. Originally, we wanted to have this refresh at 50 Hz. However, this was not possible due to the slow nature of the IMU register updating. However, this is not a big deal since we’re still accurately getting step count.

We also have satisfied SRS-02 (accurate step-count).

We decided to not use the ESP-32 due to extra unnecessary complexity. Therefore, SRS-03 and SRS-04 and SRS-06 are no longer relevant.

SRS-05 is partially completed since we have the basic functionality of cycling between various screens. However, we do not have the magnetometer setup yet (SRS-07), so we are still working on that.

For SRS-08, the OLED works through SPI. However, we decided not to use the ESP32 so the ESP32 part of the requirement is irrelevant. The OLED is fully functional, however.

Below is a picture of the IMU setup.

![1776480969079](image/README/1776480969079.png)

**Hardware Implementation:**

For HRS-01: we are partially done with this hardware requirement except we currently are using a 6-axis accelerometer because the 9-axis accelerometer we bought requires using I2C, which we do not have setup yet.

For HRS-02: we have satisfied this since our OLED fits on the glasses frame extension and also can display more than 2 lines of text.

For HRS-03: several push buttons register within 100ms and do various functions, such as changing what is on the OLED display.

For HRS-04/HRS-05: Not relevant because we aren’t using ESP32.

For HRS-06: We have not tested battery life yet. However, eliminating the use of the ESP32 saves a lot of power since the OLED and STM32 alone are relatively power efficient and not too power hungry.

**Other Project Elements:**

The lens drives many dimensions of the container that encloses the components. Because it has not arrived yet, we decided to create a simple, flexible chasis using thin, single layer cardboard. Based off of the measurements validated with a caliper, and the latest design, a cardboard cutout was made.

![1776478420983](image/README/1776478420983.png)

The components were then housed inside the cardboard cutout, proving that they could be connected and fit into these dimensions. The buttons were also placed and connected internally.

![1776478376650](image/README/1776478376650.png)

To enable connection to a pair of glasses, slots were added onto the side of the enclosure. During testing, it was found that the orientation of teh enclosure deviated form expected due to the weight of the hardware. A solution was developed for the CAD where the frame of the glasses would be built into the enclosure rather than using a detachable design.

![1776478385772](image/README/1776478385772.png)

The CAD was also updated with the new dimensions of the mirror.

![1776478407053](image/README/1776478407053.png)

**Risks:**

Due to the lens not arriving yet, the optical design could not be validated. To ensure no major design changes were needed, we conducted an analysis of the design from an optical standpoint, verifying that the virtual image could be created with our latest dimensions and components.

![1776478445147](image/README/1776478445147.png)

We also verified by analyzing the rays in an optics simulation, verifiying that our new lens and beamsplitter would create the expected image.

![1776478674170](image/README/1776478674170.png)

## Final Updates

A new lens was ordered due to supply chain delays with the original lens. The new lens is a 25mm diameter lens with a focal point of 71mm. Initially, the object distance remained the same as before, creating a new image distance and magnification. However, a new constraint was discovered: the human eye focuses best at distances greater than 25cm. Thus, the object (the OLED) must be moved so that the virtual image is at an absolute distance of greater than 250mm. Using the equations for a virtual image, a new object distance of 56mm was chosen. Thus, the encolosure had to be redesigned to account for this.

![1776715744627](image/README/1776715744627.png)

While adjusting the CAD, it became apparent that the placement and orientation of the mirrors affected the object distance, or the distance traveled by the light before reaching the lens. To account for light emmited from all edges of the lens, the mirrors must be placed parallel to each other so that the horiontal distance remains constant.

![1776715873380](image/README/1776715873380.png)

Finally, the enclosure could be printed.

![1776715911116](image/README/1776715911116.png)

Assembly of hardware components began:

![1777042244777](image/README/1777042244777.jpg)

Testing of the projection:
![1777042292677](image/README/1777042292677.jpg)

![1777042298013](image/README/1777042298013.jpg)

## Final Report

Don't forget to make the GitHub pages public website!
If you’ve never made a GitHub pages website before, you can follow this webpage (though, substitute your final project repository for the GitHub username one in the quickstart guide):  [https://docs.github.com/en/pages/quickstart](https://docs.github.com/en/pages/quickstart)

### 1. Video

### 2. Images

Demo Day Success:

![1777258583581](image/README/1777258583581.jpg)

Final Assembly:

![1777258599590](image/README/1777258599590.jpg)

Visible Projection:

![1777258625467](image/README/1777258625467.jpg)

Final CAD:

![1777258663523](image/README/1777258663523.png)

### 3. Results

Optical System: The optical subsystem was highly successful, creating a visible and clear projection of the OLED in the user's view. The text on the OLED was reflected by two mirrors and directed through a convex lens, hitting a beamsplitter that brought half the light into the user's eye. A virtual image was created 256mm in front, overlaid on the rest of the field of view to create the augmented reality effect. The final measurements and constraints for the optical system are: A convex lens with a diameter of 25mm and a focal point of 71mm. Two first-surface reflecting mirrors, each with a length of 35mm and a height of 15mm. A 50/50 beamsplitter with a length of 50mm and heigh of 30mm. The mirrors were placed at 45 degrees relative to the OLED, and parellel to ensure consistent distances traveled by light across the length of the mirror. The center of the first mirror is 11mm from the screen, and the distance between the two centers of the mirrors were 34mm. The distance from the center of the second mirror iss 11mm from the lens. This creates a total distance of 56mm, being less than the 71mm focal point in order to create a virtual image at -265mm and with a magnification of 4.73. One major change took place during the design of the optical system. A change from a focal point of 32mm to 71mm was required in order to enable a virtual image distance greater than 250mm, the minimum to create a clear image for the human eye. By using optical simulation software, the team was able to design and implment the optical system confidently.

Mechanical System: This system includes the enclousure and lid that houses the optical and hardware systems. This was designed in Fusion 360, with some subsystems designed in Onshape for optimization testing. The enclosure was able to fit all of the hardware including the STM32, OLED, IMU, and buttons. Due to difficulty soldering, not all component ended up being placed inside the enclosure. However, with thinner, more flexible wires, parts would comfortably fit inside as measurement were validated with calipers and assembled with CAD. The optical distances were accurately reflected in the CAD design, which enabled a clear image to be seen on the first attempt.

Hardware System:

Firmware System:

#### 3.1 Software Requirements Specification (SRS) Results

| ID     | Description                                                                                               | Validation Outcome                                                                          |
| ------ | --------------------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------- |
| SRS-01 | The IMU 3-axis acceleration will be measured with 16-bit depth every 100 milliseconds +/-10 milliseconds. | Confirmed, logged output from the MCU is saved to "validation" folder in GitHub repository. |

#### 3.2 Hardware Requirements Specification (HRS) Results

| ID     | Description                                                                                                                        | Validation Outcome                                                                                                      |
| ------ | ---------------------------------------------------------------------------------------------------------------------------------- | ----------------------------------------------------------------------------------------------------------------------- |
| HRS-01 | A distance sensor shall be used for obstacle detection. The sensor shall detect obstacles at a maximum distance of at least 10 cm. | Confirmed, sensed obstacles up to 15cm. Video in "validation" folder, shows tape measure and logged output to terminal. |
|        |                                                                                                                                    |                                                                                                                         |

### 4. Conclusion

Overall, the project was mostly successful and the most important features were completed and demonstrated.

## References
