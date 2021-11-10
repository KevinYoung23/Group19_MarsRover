![卡通人物 描述已自动生成](media/e9af0f0092378b3f6eb8f0801c3b56ad.png)

ELEC50003/ELEC50008 Engineering Design Project 2 Mars Rover

Energy: Yuhan Diao 01711760

Drive: Songyu Huang 01716233

Integration: YingKai Yang 01516794

Command: Yinglong Liang 01726205

Vision: Ibrahim Mohamed 01520585

Control: Adam Rehman 01720256

# Table of Contents

[**Intro
3**](https://docs.google.com/document/d/1Y76b4QcbxUh7y1ZbvUL4kBDzhJN1IPMXwkZcHJfqmvw/edit#heading=h.6v39c5w96z3e)

[**Project Management
4**](https://docs.google.com/document/d/1Y76b4QcbxUh7y1ZbvUL4kBDzhJN1IPMXwkZcHJfqmvw/edit#heading=h.b214hs3uslnr)

[**Energy
5**](https://docs.google.com/document/d/1Y76b4QcbxUh7y1ZbvUL4kBDzhJN1IPMXwkZcHJfqmvw/edit#heading=h.lm4u2s32nl30)

[Battery Charging profile
5](https://docs.google.com/document/d/1Y76b4QcbxUh7y1ZbvUL4kBDzhJN1IPMXwkZcHJfqmvw/edit#heading=h.t03ncj2rfsbl)

[How to connect batteries and PV panels (Parallel or in Series)
6](https://docs.google.com/document/d/1Y76b4QcbxUh7y1ZbvUL4kBDzhJN1IPMXwkZcHJfqmvw/edit#heading=h.cdhbkpxwaisf)

[How to estimate State of Charge (SoC) of the cells
7](https://docs.google.com/document/d/1Y76b4QcbxUh7y1ZbvUL4kBDzhJN1IPMXwkZcHJfqmvw/edit#heading=h.j4yoo2u4jq6i)

[How to monitor the State of Health of the cells
9](https://docs.google.com/document/d/1Y76b4QcbxUh7y1ZbvUL4kBDzhJN1IPMXwkZcHJfqmvw/edit#heading=h.czqm656dj3bl)

[How to integrate the energy module into the main system theoretically
9](https://docs.google.com/document/d/1Y76b4QcbxUh7y1ZbvUL4kBDzhJN1IPMXwkZcHJfqmvw/edit#heading=h.ddwo38edvp9z)

[**Drive
11**](https://docs.google.com/document/d/1Y76b4QcbxUh7y1ZbvUL4kBDzhJN1IPMXwkZcHJfqmvw/edit#heading=h.94dbfr74z97j)

[Speed control
11](https://docs.google.com/document/d/1Y76b4QcbxUh7y1ZbvUL4kBDzhJN1IPMXwkZcHJfqmvw/edit#heading=h.ejyi1lpkfkcc)

[Turning control
12](https://docs.google.com/document/d/1Y76b4QcbxUh7y1ZbvUL4kBDzhJN1IPMXwkZcHJfqmvw/edit#heading=h.jdhxmztt6caa)

[Direction control (moving forward and backward)
14](https://docs.google.com/document/d/1Y76b4QcbxUh7y1ZbvUL4kBDzhJN1IPMXwkZcHJfqmvw/edit#heading=h.sih799fbqh5w)

[Distance measurement
15](https://docs.google.com/document/d/1Y76b4QcbxUh7y1ZbvUL4kBDzhJN1IPMXwkZcHJfqmvw/edit#heading=h.laqiqxkgt8xf)

[**Control
17**](https://docs.google.com/document/d/1Y76b4QcbxUh7y1ZbvUL4kBDzhJN1IPMXwkZcHJfqmvw/edit#heading=h.89c7ne801n2m)

[The logic of this system
18](https://docs.google.com/document/d/1Y76b4QcbxUh7y1ZbvUL4kBDzhJN1IPMXwkZcHJfqmvw/edit#heading=h.dkul6uuvcvas)

[Determine what data to be transmitted
18](https://docs.google.com/document/d/1Y76b4QcbxUh7y1ZbvUL4kBDzhJN1IPMXwkZcHJfqmvw/edit#heading=h.3m3q1gp51pg6)

[Between each submodule:
18](https://docs.google.com/document/d/1Y76b4QcbxUh7y1ZbvUL4kBDzhJN1IPMXwkZcHJfqmvw/edit#heading=h.6eu01c4pbftp)

[Between the Rover and users:
19](https://docs.google.com/document/d/1Y76b4QcbxUh7y1ZbvUL4kBDzhJN1IPMXwkZcHJfqmvw/edit#heading=h.mdyj0rwt1lgf)

[How to communicate with each submodule
19](https://docs.google.com/document/d/1Y76b4QcbxUh7y1ZbvUL4kBDzhJN1IPMXwkZcHJfqmvw/edit#heading=h.2wh33vwrnsop)

[Command
20](https://docs.google.com/document/d/1Y76b4QcbxUh7y1ZbvUL4kBDzhJN1IPMXwkZcHJfqmvw/edit#heading=h.3nmr9c6bx670)

[Energy
20](https://docs.google.com/document/d/1Y76b4QcbxUh7y1ZbvUL4kBDzhJN1IPMXwkZcHJfqmvw/edit#heading=h.w9t14ewk74uf)

[Drive
20](https://docs.google.com/document/d/1Y76b4QcbxUh7y1ZbvUL4kBDzhJN1IPMXwkZcHJfqmvw/edit#heading=h.c9q5pnkll8we)

[Vision
23](https://docs.google.com/document/d/1Y76b4QcbxUh7y1ZbvUL4kBDzhJN1IPMXwkZcHJfqmvw/edit#heading=h.mkd2dcr6htem)

[**Intellectual Property
24**](https://docs.google.com/document/d/1Y76b4QcbxUh7y1ZbvUL4kBDzhJN1IPMXwkZcHJfqmvw/edit#heading=h.qtjn9zp5ne7g)

[**Bibliography
25**](https://docs.google.com/document/d/1Y76b4QcbxUh7y1ZbvUL4kBDzhJN1IPMXwkZcHJfqmvw/edit#heading=h.8nlw3mnbvkwm)

[**Appendix
26**](https://docs.google.com/document/d/1Y76b4QcbxUh7y1ZbvUL4kBDzhJN1IPMXwkZcHJfqmvw/edit#heading=h.bc2td8nsgcvh)

[Control and Command Instruction Sets
26](https://docs.google.com/document/d/1Y76b4QcbxUh7y1ZbvUL4kBDzhJN1IPMXwkZcHJfqmvw/edit#heading=h.iqxsktppz2sd)

# Intro

The goal of this project is to create a functioning Mars Rover that can perform
autonomous cruising/manual control, obstacle identification and avoidance while
receiving information from a command webpage via ESP32 microprocessing unit. It
can charge itself ![桌子上的自行车
中度可信度描述已自动生成](media/2e677b88ce16befe5f1c34e4bdf8e19e.jpeg)with four
PV panels into the batteries which it carries.

*Figure 1: Assembled Mars Rover*

By instruction, the project has been divided into 6 sections: Energy, Drive,
Command, ![图示
描述已自动生成](media/09e32e8db25884dbc9fbd6a85ec4c349.png)Control, Vision and
integration. The flowchart of the overall design is shown on *Figure 2.*

*Figure 2: Flowchart of basic functions of each subsystem*

# Project Management

In this project, we have assigned six subsystems to six of us, however, due to
Adam’s mental issue, he has contributed nothing towards the team. The detailed
assignment is shown as following:

-   Energy: Yuhan Diao 01711760

-   Drive: Songyu Huang 01716233

-   Command: Yinglong Liang 01726205

-   Vision: Ibrahim Mohamed 01520585

-   Control: Songyu Huang & Yinglong Liang & Yingkai Yang

-   Integration: Yingkai Yang 01516794

# Energy

The key aim of the energy subsystem is to charge the batteries with PV panels,
keep track of the State of Charge (SoC) of the batteries and monitor the State
of Health (SoH) of the cells. During the process, the following options need to
be figured out:

-   Battery charging profile

-   How to connect batteries and PV panels (Parallel or in Series)

-   How to estimate State of Charge (SoC) of the cells

-   How to monitor the State of Health of the cells

-   How to integrate the energy module into the main system theoretically

## **Battery Charging profile**

To charge batteries, there are two methods: Constant Current (CC) and Constant
Voltage (CV). In order to find the best ways, both of the methods have been
tested. Phil’s example code is the first to be implemented which is to charge a
single battery with Constant Current of 250mA. Cell 1 capacity is calculated via
*Equation 1* and it is 468.11mAh*.*

*Equation 1:Cell Capacity = Isum3600s*

According to source [1], the battery can be charged by Constant Current up till
80% - 90% of the cell, because if it starts with Constant Voltage, the current
will be way too big for cells and damage it. By switching to Constant Voltage
after 80% of capacity is charged, overcharging can be avoided and fully charge
LiFePo4 cells. The same cell can obtain a capacity of 563.7mAh, *Figure 3* shows
the discharging time of both cell 1 with two different methods and a great
difference in discharging time can be seen. This is the key reason in selecting
combining both CC and CV due to the increase in cell capacity which can
ultimately increase the range of the rover on Mars.

*![图示
低可信度描述已自动生成](media/e81b80933940c2aca06d3869d73f7fcb.png)Figure 3:
Discharge time of Cell 1 under two different charging methods*

## **How to connect batteries and PV panels (Parallel or in Series)**

For cells, parallel connections are chosen. The main reason is that batteries in
series connection will be completely useless if a single cell is broken and
considering the inhabitable environment on Mars and the possibility of cells
getting damaged is quite high. Energy wise, though Mars Rover requires 5V input
voltage and we can only provide 3.6V max. There is another SMPS on the drive
system, therefore batteries can be connected into the boost mode on the drive
system and then supply 5V for the entire system. To balance these cells, Open
Circuit Voltages of the cells need to be obtained one by one every 60 seconds,
and if their gaps have exceeded over 200mV, they will be sent to the balanced
state where they will be discharged via Dis port on the battery holder. *Figure
4* shows the state diagram for the three parallel cells system.

For PV panels, from the data sheet that the maximum voltage and current are 5.0V
and 230mA, therefore all four PV panels are in parallel connection, because for
the Constant Voltage charging process, the PV panel already has enough voltage
as well as more current is needed for the Constant Current operation. For SMPS,
to have synchronous Buck will be the only solution.

From Piazza[2], when PV panels are creating less current than constant current
requires, the current controller PID will increase the duty cycle until it
reaches its maximum and current going into batteries will be 0. So combining the
MPPT algorithm with Constant Current will not meet this circumstance. From
lecture notes[3], the MPPT algorithm is to compare the power with the previous
one and alter the PWM wave to alter the power in a Perturb and Observe Tracking
System. MPPT will start to work if the current measure is smaller than the
current reference (with some space to move) and go into Constant Current if the
current measure is bigger than the current reference. ![图示
描述已自动生成](media/9f3318727ce9258fa059e96ab3ac68bf.png)

*Figure 4: State Diagram of triple parallel cells charging*

## **How to estimate State of Charge (SoC) of the cells**

Firstly, an upper limit (100% of SoC) of cells will be set, so at Constant
Voltage process, cell is charged for 60 seconds and then for 10 seconds, relay
is turned on to measure the cell open circuit voltage (OCV) and then continue
this loop until the mean of OCV reaches around 3.6V, however from *Figure 5*,
the maximum value the cell can achieve is around 3530mV in four hours time.

*![图形用户界面, 图表, 散点图
描述已自动生成](media/9c7f3ad5d8308e02f19629ec8913db5c.jpeg)Voltage Lookup Table
Figure 5: Vmean values over time*

| Open Circuit Voltage (mV) | SoC (%) |
|---------------------------|---------|
| 3530                      | 100     |
| 3241.7                    | 90      |
| 3232.2                    | 80      |
| 3215.7                    | 70      |
| 3207.0                    | 60      |
| 3197.1                    | 50      |
| 3185.3                    | 40      |
| 3167.6                    | 30      |
| 3134.6                    | 20      |
| 3084.8                    | 10      |
| 2500                      | 0       |

At the beginning of the charging state, a voltage lookup table is needed to
determine the initial SoC. And soon it leaves state 0 and enters state 1, the
Coulomb Counting method shall be used, it essentially measures the current flows
in and out of the battery and uses it to calculate the SoC. From this
journal[4], *Equation 2* can be found to calculate the SoC in both charging and
discharging process of the system, where Qnis the cell capacity. And with SoC
status, the remaining range of our Rover can be estimated by *Equation 3.*

Equation 2:SoC(t) =SoC(t-1) + I(t)Qnt

Equation 3:Range/m = Qtotal/mAh 3600s SoC/%Current drawn by the Rover/ mACurrent
speed of the Rover / ms-1

## **How to monitor the State of Health of the cells**

The State of Health(SoH) is a measurement that displays the general condition of
a battery after several cycles of use and compares it with a new battery. Due to
its complexity over different cells and different users, there is no direct
equation linked to it, therefore it is a rather subjective point of view.

For the SoH, cell capacity is monitored by using a current measure counter; it
is a function that records all the discharge current in the discharge process.
With this result, the change of cell capacity within its life cycle can be
recorded, and when one certain cell has a much smaller cell capacity than
others, the relay is turned on and never lets it get charged or discharged.
Also, in order to prevent low cell capacity from happening, individual states
can be constructed for each individual cell to avoid overvoltage/undervoltage.

## **How to integrate the energy module into the main system theoretically**

Following information is based on theoretical assumptions due to technical
issues that we can not actually implement an energy module on the rover. As
shown in *Figure 4*, in order to initiate the charging process, the control
system will need to send a command of ‘c’ to our Arduino Nano Every via UART
port, and to terminate the charging the control system will ![图形用户界面,
文本, 应用程序, 电子邮件
描述已自动生成](media/48aaa767fb75820cb903be2068927bbf.png)need to deliver a ‘r’
message. Meanwhile, the SoC and current measure of the discharging

*Figure 6: Receiving and Transmitting information with Control module*

process to the control part and then can be transmitted to the Command Web page
via ESP32, so users can track the SoC status in real time. In order to simulate
the discharging process on the Rover, a discharging process is added, it is not
included in *Figure 4,* it essentially does the same thing as constant current
charging with balance except current is -500mA. Codes to do this can be shown in
*Figure 6*.

In order to physically implement the energy subsystem in theory, batteries can
be connected to the SMPS on drive. Drive will be changed into a boost system and
can boost up the battery voltage from 3.6V up to 5V by regulating the PWM.
However, due to the fact that cells only have a range from 2.4V to 3.6V, the
output voltage of the drive system can only be from 2.4V to 5V. This can cause
the drive system to lose certain speed selection. Another method is to have an
extra SMPS, which means that there will be three SMPS in total. One is in buck
mode for PV panels to charge cells, one is in boost mode to boost the cells
voltage up to 5V and the last one is in buck mode and to alter PWM wave to
change Rover’s velocity. Moreover, if time and equipment is available, a ‘Low
Battery Mode’ can be added to the design. When the second SMPS boost cannot
sustain output of 5V with the current cells’ voltage, it can either use ‘Low
Battery Mode’ to keep the current duty cycle and output voltage will be lower,
then the Rover will be slower. Or, a PID controller of constant voltage can be
added into Arduino on Boost SMPS, it will remain 5V but a higher current will be
drawn out from batteries, causing shorter range.

Also, we decided to design our energy system as an internal system that can be
mounted onto our Mars Rover. It has one significant advantage when compared with
an external charging station. Rover will not be restricted since it can travel
wherever it wants and start to charge anywhere when its cell capacity is low.

# Drive

The main purpose of the drive system is to control the movement of the Mars
Rover, the components of the drive system include the optical sensor, motors,
and the rover frame. The movement of the rover can be splitted into 4 different
parts:

-   Speed control

-   Turning control

-   Direction control

-   Distance measurement

The position controller contains the turning controller and direction
controller. The ![图示
描述已自动生成](media/90c81c6d9e307355ca9c9cd6837b47f0.png)overall controller
follows the block diagram *Figure 7*:

*Figure 7: Schematic of Drive subsystem*

## **Speed control**

The speed of the rover is controlled through the H bridge circuit. Therefore it
also can be controlled by the output voltage of the SMPS. By adjusting the duty
cycle of the PWM wave can change the output voltage of the SMPS. The original
method to change the speed is by using a single parameter to change the PWM duty
cycle, which is at Pin6 of the Arduino.

*Figure 8: PI controller*![文本, 信件
描述已自动生成](media/0f6bcc429a61bd526e7c0b5f57024aff.png)

Instead, a PI controller is used, as shown in *Figure 8*. The output of the PID
function can be used as the input of the speed control function. The speed of
the rover will change depending on the difference between the current position
and the destination (error):

-   When the current position is far away from the destination, the speed will
    be high, and when the rover approaches the destination, the rover will move
    slower.

-   When the rover is stuck in a fixed position during an operation, for
    example, the rover may fall into a pit, the rover will increase its speed in
    order to get out of the pit. By changing the duty cycle of the PWM wave, the
    speed can be controlled.

All of the controllers below will include the speed control.

## **Turning control**

The first control method is to set up an open-loop controller, the motor is
controlled by using time. The rover can turn different degrees depending on the
operating time of the motor; however, this is not very precise, for example when
the rover encounters some friction during its rotation, the rover cannot reach
the destination and may cause further problems. The sensor will measure the arc
length that the rover rotates across as shown in *Figure 9* and the y-direction
distance will not change. *Figure 9: Angle Calculation*![形状
描述已自动生成](media/8f4a1aa2a9e1ddfe22bfddd6799dcae0.png)

By using this property the rotation degree can be calculated by using *Equation
3*:

Equation 3: destinationx=a=degree360× 2π × r

Where the radius is the distance between the sensor and the middle of the motor.
The first attempt is to use a close loop controller with the following design
shown in *Figure 10*, the rover needs to do the correction itself. Also, there
is a 0.5cm tolerance designed to make sure the rover operates successfully due
to the lack of precision of the optical sensor.

![图示 描述已自动生成](media/13504a350e513b8b48588336f24371fa.png)

*Figure 10: Initial Turning Close Loop correction controller*

However, during the first attempt, several issues need to be solved:

-   In the initial design, the Rover cannot adjust itself when it rotates more
    than the assigned degree due to the environmental influences. This is due to
    the fact that once the Rover reaches the dedicated degree (which the error
    calculation will be between -0.5 & 0.5), the Rover will stop. This can be
    fixed by adding a detection time, Rover needs to stay within the range for
    at least 5 seconds, if not, the Rover will microadjust itself to reach the
    expected angle.

-   When the Rover encounters any serious issues on Mars and it cannot finish
    the dedicated operation, it will get trapped in the single operation loop.
    This can be solved by adding a time measurement to record the time cost in a
    single operation. The maximum time for a single operation will be decided
    according to different types of functions.

The final design flowchart is shown below in *Figure 11,* and the right turn has
the same logic as the left turn.

![图示 描述已自动生成](media/403ff905a06f4e018c38fbb3cf11fd4d.png)

*Figure 11: Final design of the Turning Control*

## **Direction control (moving forward and backward)**

The rover needs it to reach the destination when moving forward and backward, an
open-loop controller is being designed first, it is based on time. However, it
is not precise and cannot make corrections due to other influences. The same
logic as the turning control is being used, by using the parameter distance_y to
represent the current distance that the rover moves: it can move backward when
the rover moves beyond the destination(same as the turning control). The rover
may not be moving in a perfectly straight line when moving forward and backward.
Therefore by using the distance measured in the x_direction (the arc length), a
controller can be shown in *Figure 12*.

However, this method does not operate as it should be, the rover does the
correction too much and does not meet the requirements on the flowchart. This
may be caused by the reading speed of the optical sensor. After asking the
lecturer about this problem, the rover should be assumed to be moving in a
perfectly straight line, this design can be abandoned. Therefore only a
y-direction controller is being used, the same logic as the turning controller.

![图示 描述已自动生成](media/a4da6fea792ba61c31fc7b22d2cf3acf.png)

*Figure 12: Initial Flowchart on Direction Control*

## **Distance measurement**

The drive subsystem needs to send the current position of the rover
corresponding to the starting position to the control module.![图示, 示意图
描述已自动生成](media/abd339291d6953c36f9945fab2a91e56.png)

The diagram in *Figure 13* shows the algorithm, the angle is calculated using
the formula of the arc length (explained in the speed control module above),
total_x is the arc length, and can be used to calculate the degree; where the
coordinate is (c+bsin(θ), bcos(θ)+d), and (c,d) is the *Figure 13: Coordinates
Algorithm*

previous coordinate and b is distance_y. With the current coordinate, the
distance from the origin can be calculated using x2+y2 . An assumption is being
made: the position of the rover is not changing when it rotates. This can
measure the coordinate of the rover at any point with the starting point as the
origin. As said in the previous direction control section, assuming the rover
moves in a perfectly straight line, therefore a tolerance on the degree is
needed: When the rover rotates within the 10° margin, its x coordinate stays the
same, assuming only y-coordinate changes.

The code for the coordinates algorithm is shown in *Figure 14*. ![图形用户界面,
文本 描述已自动生成](media/8ca307eabb24d39568dc8eaa7fb2c0aa.png)

*Figure 14: The code to determine coordinates*

Also, same as shown in Figure 11, once the Rover reaches the designated
position, sometimes it might go over the position, it will go back/forward in
order to stay inside the error range.

# Control

The key aim of the control subsystem is to build the connection between each
submodule. According to the diagram below, all the submodules only communicate
with the control subsystem directly. Therefore, the whole system is based on a
star connection with Control as the central server. ![图示
描述已自动生成](media/9eb0db4b9b7bcdb498b8146363083743.png)

Here are several bullet points that should be considered when developing the
control module:

-   The logic of this system

-   Determine data to be transmitted

    -   Between each submodule

    -   Between the Rover and users

-   How to communicate with each subsystem

    -   Drive

    -   Energy

    -   Vision

    -   Command

## **The logic of this system**![图示 描述已自动生成](media/322d93bf79207dcbff3fda4d46f36b74.png)

*Figure 15: Structural Diagram of the whole system*

In general, there are two modes that users can use: one is the Auto Cruising
Mode, and the other is the Manual Mode. After users select the mode, it will
enter two different algorithms. Details will be introduced later in this report.

## **Determine what data to be transmitted**

To guarantee all submodules can work properly with each other and users get the
information they expect, some data should be transmitted between each submodule
via Control.

### **Between each submodule:**

Vision provides the obstacle information, Drive is responsible for taking the
Rover to the designated destination and reflecting kinematics information,
Command interacts with users and delivers commands to the Control and Energy
notifies the system whether the battery level is enough. Figure 16 displays the
detailed transmitted data.

*Figure 16: Data transmission of each submodule with Control subsystem*![图示
描述已自动生成](media/82259d70fe63c95a5e5a9406b777564b.png)

### **Between the Rover and users:**

Users should first decide which mode (Manual or Auto Cruising) they want for the
Rover to operate under. If it is in Manual Mode, users should then decide which
direction they want the Rover to move. Also, users should get access to the SoC,
estimated range, current direction of the Rover and obstacle direction from the
real time mapping on the Web page of Command subsystem.

## **How to communicate with each submodule**

To ensure each submodule communicates with Control successfully, the same code
should be applied between two submodules. Also, all the communications are based
on UART (Universal Asynchronous Receiver/Transmitter) ports. Therefore, at the
end of each message, the transmitter should add ‘\\n’ to inform the receiver
that the current instruction ends. Besides, another big challenge is to make
sure that there is no conflict and error when transmitting data between each
submodule.

There are three UART ports defined in ESP32: Serial, Serial1, and Serial2.
Serial uses the micro-USB to display messages on the serial monitor on the
designer's PC to debug. Afterward, this port is set to connect with Energy’s
Arduino Nano Every Serial1. Control’s Serial1 is reserved for Vision, and
Serial2 uses pins 16 and 17 that are responsible to communicate with Drive’s
Arduino nano every Serial1.

### **Command**

The communication between these two modules is based on a wireless connection
\<WiFi.h\> that may not be as stable as a wired connection. Due to the loss of
transmitted data, the receiver should always send a feedback message to the
transmitter when the instruction is received which can help to reduce errors;
otherwise, the transmitter will keep sending commands. The table in the appendix
illustrates the code being applied and how Command and Control communicate.

### **Energy**

**After receiving the main instruction from the Command, Control requests for
SoC and estimated range from Energy. If the battery level is enough to cover the
range, the Rover then starts to operate the given instruction. Otherwise, the
Rover should keep charging until it is fully charged.**

### **Drive**

The UART communication between Control and Drive is the most complicated part.
The reason is that they require multiple times of data exchanges during one
execution. Therefore, both sides must be aware of each other’s state to
determine when to send instructions and avoid any conflicts or losses of data.
Also, both sides should ignore “wrong data” (data with wrong number of bits). To
achieve this goal, all the data transmitted from one side should have the same
length: 5 bits from Control to Drive and 9 bits from Drive to Control.

When the program starts, the Control part should generate a vector of
Instructions based on users’ requirements. If the user selects Manual Mode, only
two elements will be stored in the vector: the first one which is the current
instruction, and the second one is the stop instruction “04000” to inform the
Drive system that all the instructions have been executed and make the rover
stop. Elsewise, when users select Auto Cruising Mode, a vector that contains a
series of instructions will be generated based on the parameter that the users
want the Rover to explore. This function is implemented by a function named
route design which takes max_x and max_y as input parameters and returns a
vector that contains a series of operations. *Figure 17* demonstrates the
desired route for this rover to explore within a certain area. ![图片包含
图形用户界面 描述已自动生成](media/cce7817a047f93fda894eb576a1b8c17.png)

*Figure 17: Designed Route for Auto Cruising Mode with given area parameters*

Afterward, the Control will send the vector to the Drive system element by
element. Each element contains 5 bits: the first bit is the speed_factor that
controls the speed of movement, the second is the opcode that controls the
direction of the movement, and the last 3 bits are the operand that implements
the moving distance or turning angle, depending on the opcode. For example,
“11015” means move forward for 15 cm with a standard speed, and “23090” means
turn right for 90 degrees with a speed level of 2.

The Drive system sends such information to the Control as well. Before taking
any new instruction, the Drive informs the Control that it is ready to receive
commands by sending code “11000000”. After executing the current instruction,
the Drive system tells the Control that the current instruction is finished by
sending “33xxxyyy” where the last 6 bits implement the current coordinate of the
rover after the current instruction. Then, the Control will send the current
coordinate to the Command part which visualizes the current position and perform
a live mapping for users. When Stop “04000” is received by Drive, it means that
the current instruction set is finished. Drive sends back “22000000” to the
Control module to terminate the current mode. Afterward, users can reselect the
Mode and instruction and start from the beginning again. Remember all the codes
should not start with “0” because “0” at the beginning may cause an error or
misunderstanding. *Figure 18* illustrates the communication between the Control
and the Drive. Due to the limitation of the size of the vector in Arduino, only
up to 12 elements can be stored in the Command vector. Therefore, in the demo
video, the working area is limited. Consequently, the demo of obstacle avoidance
(will be mentioned in the vision part) and Auto Cruising Mode are recorded
separately.![图示 描述已自动生成](media/39d303c2679599133aa29187e552734e.png)

*Figure 18: Communication between Control and Drive*

### **Vision**

Unlike the other three parts, Vision sends data to Control and does not receive
any data back. Therefore, no feedback message is required for the Control
System.

In general, the Vision sends the **Device ID** and five groups of coordinates.
The **Device Name?**  indicates the start of new groups of coordinates. The five
groups of coordinates are related to five different colored ping-pong balls.
Each set contains the x and y coordinates of the bottom-left and top-right
corner. The location of the obstacle is defined by the coordinate of the
bounding boxes, and the distance between the rover and the obstacle is
determined by the area of the bounding box.

If the ball is located in the green zone, the rover won’t collide with the
obstacle even if it keeps moving forward, nothing happens. However, when the
ball is not in the green zone and part of the ball begins to disappear from the
bottom of the screen (Ay becomes smaller while By stays at 0), the rover seems
to be approaching the obstacle. As a result, either warning message or obstacle
avoidance algorithms will intervene depending on the Mode. ![图表
描述已自动生成](media/6095dcca2762861b26127888f4c4806f.png)

If it is in the Auto Mode, the obstacle avoidance algorithms will be initiated
by inserting a series of operations to the command vector. It first reduces
speed to standard speed, and goes around the obstacle. Meanwhile, it will send
the approximate coordinate of the obstacle to the Command system. *Figure 19*
visually displays the algorithm.

![图示, 示意图 描述已自动生成](media/95636b1a821cb5250b3787ca0967e80e.png)

*Figure 19: Obstacle Avoidance Route Algorithm*

If it is in the manual mode, a warning message containing the color information
is sent to the Command.

For further development, I assume that the more accurate location estimation for
the obstacle can be calculated from the rate of change of the area of the
bounding boxes. As the diagram shown below, when the rover moves forward for a
certain distance, the area of the bounding box of the closer obstacle increases
faster than the further one. By connecting the rate of change and distance with
some trigonometry equation, more accurate distance can be calculated. This is
hard to achieve due to the low resolution of the camera.

![图片包含 图表 描述已自动生成](media/0142140baf8cfb8a104e37408c3fba6b.png)

# Integration

The key aim of the integration is to test each submodule and ensure each module
works properly with other modules.

# Intellectual Property

Intellectual property is something that we have created with our own minds, it
can be concluded as four main sections: copyright, design, trademark and patent.
Copyright protects the original expression. This can be applied to our Command
subsystem, while developing our own Webpage to send commands to our Rover, our
group member designs his own function in order to achieve data translation
between javascript and python. Design preserves the right of appearance of a
product, but not the function of the product. For our Mars Rover, if we manage
to give it a distinct look, then a registered design will be owned among our
group members. Patent serves the purpose to conserve new inventions. If we ever
manage to design our original microprocessor (a new arduino), we can apply for a
patent and it can stop others from using/making/selling/importing the new
invention. Last but not least, a trademark serves as the sign of a series of
products. For this aspect, we can design a unique brand and slogan for the Rover
and it can bring us benefits in a long term run.

After researching at imperial website [5], I found out that even though our Mars
Rover has been using equipment that belongs to the college, we can keep the
intellectual property rights to ourselves if we ever design/invent something out
of it. This is due to the fact that we don’t meet any of the criterion as I
quote from the website:

“1. they generate IP which is subject to governing terms or an agreement with an
external organisation whereby the IP vests with College or a third party; or

2\. they generate IP which builds upon existing IP generated by College Employees
or Associates; or

3\. they generate IP jointly with College Employees or Associates; or

4\. they are, or have the status of, College Employees (in which case they are
treated by College and the law as employees).”

# Bibliography

[1] "A Designer's Guide to Lithium (Li-ion) Battery Charging", Digi-Key
Electronics, 2016. [Accessed: 23 - May - 2021] [Online]. Available:
<https://www.digikey.com/en/articles/a-designer-guide-fast-lithium-ion-battery-charging>.

[2] P. Clemow, "Current through PV panel", Piazza.com, 2021. [Accessed: 07 - Jun
\- 2021] [Online]. Available: <https://piazza.com/class/koiow33gz3578w?cid=245>.

[3] T. C. Green, Class Lecture Notes, Topic: "Photo-Voltaic Energy”, ELEC50012,
Department of Electrical and Electronic Engineering, Imperial College London,
London [Accessed: 26 - May - 2021]

[4] W. Y. Chang, "The State of Charge Estimating Methods for Battery: A Review",
International Scholarly Research Notices, vol. 2013, Article ID 953792, 7 pages,
2013\. [Accessed: 27 - May - 2021] [Online]. Available:
<https://doi.org/10.1155/2013/953792>

[5] "Student IP guidance", Imperial College London, 2021. [Accessed: 09- Jun-
2021] [Online]. Available:
[https://www.imperial.ac.uk/enterprise/staff/industry-partnerships-and-commercialisation/commercialisation/intellectual-property-guidance/student-ip-guidance/.](https://www.imperial.ac.uk/enterprise/staff/industry-partnerships-and-commercialisation/commercialisation/intellectual-property-guidance/student-ip-guidance/)

# Appendix

## **Control and Command Instruction Sets**

| Step | Instruction               | Explanation                                                                                                                                                                                                                                                                                          | Direction          |
|------|---------------------------|------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|--------------------|
|      | Main Instruction (9 bits) | xx (mode selection)  10 for manual, 11 for auto + xxxxxx (command/area)  **Manual:**  “000000” turn left 90 degree “111111” move forward for 20cm “222222” move backward for 20cm “333333” turn right for 90 degree **Auto** first 3 bits: define max_x last 3 bits: define max_y + x (speed factor) | Command to Control |
|      | Instruction Received      | “10000000”                                                                                                                                                                                                                                                                                           | Control to Command |
|      | Feedback Information      | 11/10 (mode)+ xxx (current x) + xxx (current y) +x (direction 0 left, 1 forward, 2 backward, 3 right) + obstacle flag (0 for none, 1 for yes)                                                                                                                                                        | Control to Command |
