[]() Overview {#overview .western style="margin-top: 0cm"}
=============

This document defines how a controller will communicate with the Robot.
It defines the messages that is available on the robot with the
parameters that can be sent for each parameter. It also defines the
message format which includes a standard header which programmers must
use to interpret the command being sent and the payloads.

\

This document assumes that the controller will be connected to the robot
over a bidirectional bluetooth serial link. The controller will transmit
a stream of messages and then wait for a configurable amount of time for
any response from the robot before continuing with its next stream of
commands.

\

Commands are broken into four major categories:

-   -   -   -   

\

The sections below will first introduce the Message Format to be used
and then it will explain each command category in more detail.

[]() Message Format {#message-format .western style="margin-top: 0cm"}
===================

All messages will consist of a Fixed Message Header followed by an
optional Payload section. The commands will be sent to the robot as a
Byte stream in big-endian order: higher order bytes precede lower order
bytes. A 16-bit word is presented on the wire as Most Significant Byte
(MSB), followed by Least Significant Byte (LSB).

\

Each command will have an absolute maximum length of 1KB, including
header and payload. The command header will always be 8 bytes (4 words)
followed by the variable length payload which differs in length based on
the command type being executed.

\

[]() Command Header {#command-header .western align="justify" style="margin-top: 0cm"}
-------------------

Byte 1: Command Index

The command index is a running counter (0-255) which represent the
sequence of a particular command in the stream of commands. All commands
will be sent and processed in sequence and the counter will
automatically reset and start at 0 again once the maximum value has been
reached. The command index will be used in events, notifications and
error handling to indicate last successfully executed command.

\

Byte 2: Command Behaviour

The command behaviour byte consists of a set of flags which will control
how the command is executed. The behaviour flags are listed and
explained below:

\

  ------- ---------- ------- ------- ---------- ---------- ---------- ----------
  **0**   **1**      **2**   **3**   **4**      **5**      **6**      **7**
  Sync    Required   Echo    Debug   Download   Reserved   Reserved   Reserved
  ------- ---------- ------- ------- ---------- ---------- ---------- ----------

\

Flag 0: The Sync flag indicates whether a command is executed
synchronously or asynchronously. When the flag is set to 0 (default),
the robot will execute the command in a non-blocking fashion. When the
flag is set to 1, the robot will execute the command in a blocking
fashion meaning it will respond with an acknowledgement back to the
controller when the command has been executed successfully.

\

Flag 1: The Required flag indicates whether the command will raise an
error message if it was not executed successfully. When set to 0
(default), the robot will not notify the controller if the command was
not executed successfully. When set to 1, the robot will raise an error
notification if the command was not executed successfully.

\

Flag 2: The echo flag indicates whether the command should be echoed
back to the controller once execution has been completed.

\

Flag 3: The debug flag indicates whether the command should be executed
in debug mode.

\

Flag 4: The download flag indicates that this command should not be
executed but its payload should be saved into the downloads buffer on
the robot.

\

Flag 5-7: These flags are reserved for future use.

\

Byte 3-4: Command Length

The command length byte specifies the number of bytes in the command,
including the header and payload. The command length can be used to
ensure that all the bytes have been received before starting to process
a command. The maximum value of command length is 1024.

\

Byte 5-7: Reserved

Bytes 5, 6 and 7 are reserved for future use.

\

Byte 8: Command Type

Byte 8 represents a UTF-8 encoded ASCII character which indicates the
command that has to be executed. The list of available commands are
described below.

[]() Command Payload {#command-payload .western align="justify" style="margin-top: 0cm"}
--------------------

Byte 9 - 20(max)

The command payload is a variable length payload which consists of all
the parameters for each command.

\

[]() Movement Commands {#movement-commands .western align="justify" style="margin-top: 0cm"}
======================

Movement Commands are a subset of commands embedded on the robot which
will provide us basic functionality to move the robot around and control
it either from a controller, AI logic or onboard programming logic. The
movement commands include the following:

-   -   -   -   -   -   -   -   

\

The commands are described in more detail below:

[]() Move {#move .western align="justify" style="margin-top: 0cm"}
---------

### []() Command Type {#command-type .western align="justify" style="margin-top: 0cm"}

Character code: M

Binary value: 0100 1101

### []() Behaviour {#behaviour .western align="justify" style="margin-top: 0cm"}

When receiving the Move command the robot will move in the wheels
direction at the specified speed for the amount of time specified. If
the specified amount of time is 0 then the robot will move forward at
the specified speed until another move command (forward, back, Stop) has
been received.

### []() Parameters {#parameters .western align="justify" style="margin-top: 0cm"}

\

  ---------------- ------------- ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- ----------
  Name             Length        Description                                                                                                                                                                                                                                                         Required

  SpeedLeft        **1 Byte**    This parameter specifies the speed at which the robot’s left wheel will move. The value has a range from Signed Int (-128, - 127)                                                                                                                                   Yes
                                                                                                                                                                                                                                                                                                     
                   \                                                                                                                                                                                                                                                                                 
                                                                                                                                                                                                                                                                                                     

  SpeedRight       **1 Byte**    This parameter specifies the speed at which the robot’s rightwheel will move. The value has a range from Signed Int (-128, - 127)                                                                                                                                   Yes

  DirectionLeft    **1 Byte**    specifies Direction 0 forward, 1 backwards                                                                                                                                                                                                                          Yes

  DirectionRight   **1 Byte**    specifies Direction 0 forward, 1 backwards                                                                                                                                                                                                                          Yes

  Time             **2 Bytes**   This parameter specifies the number of milliseconds that the robot will execute the command. If the value 0 is sent through, the robot will execute the command until it receives another move command (Forward, Back,Stop) The value has a range from 0 - 65 535   Yes
                                                                                                                                                                                                                                                                                                     
                   \                                                                                                                                                                                                                                                                                 
                                                                                                                                                                                                                                                                                                     
  ---------------- ------------- ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- ----------

\

\

\

  --- --- --- ---
  \   \   \   \
              

  \   \   \   \
              

  \   \   \   \
              
  --- --- --- ---

\

[]() Stop {#stop .western align="justify" style="margin-top: 0cm"}
---------

### []() Command Type {#command-type-1 .western style="line-height: 100%; page-break-inside: avoid; page-break-after: avoid"}

Character code: S

Binary value: 0101 0011

### []()\
 {#section .western style="margin-top: 0cm"}

### []() Behaviour {#behaviour-1 .western style="margin-top: 0cm"}

### []() <span style="font-weight: normal">When receiving the stop command the robot will stop any commands.</span> {#when-receiving-the-stop-command-the-robot-will-stop-any-commands. .western align="justify" style="line-height: 100%; page-break-inside: avoid; page-break-after: avoid"}

### []() Parameters {#parameters-1 .western style="line-height: 100%; page-break-inside: avoid; page-break-after: avoid"}

None

\

[]() Shiver {#shiver .western align="justify" style="margin-top: 0cm"}
-----------

### []() Command Type {#command-type-2 .western align="justify" style="line-height: 100%; page-break-inside: avoid; page-break-after: avoid"}

Character code: ????

Binary value: 0011 1111

\

### []() Behaviour {#behaviour-2 .western align="justify" style="line-height: 100%; page-break-inside: avoid; page-break-after: avoid"}

When receiving the Shiver command the robot will shake like it is
afraid.

\

### []() Parameters {#parameters-2 .western align="justify" style="line-height: 100%; page-break-inside: avoid; page-break-after: avoid"}

\

  ----------- ------------ -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- --------------
  **Name**    **Length**   **Description**                                                                                                                                                                                                                  **Required**
                                                                                                                                                                                                                                                            
              \                                                                                                                                                                                                                                             
                                                                                                                                                                                                                                                            

  Intensity   1 Byte       This parameter specifies the intensity of the recoil. The value has a range from 0 - 255                                                                                                                                         Yes

  On Time     2 Bytes      This parameter specifies the number of milliseconds that the robot will execute the command. The value has a range from 0 - 65 535                                                                                               Yes

  Repeat      1 Byte       This parameter is an unsigned SHORT which gives an indication of the amount of times the shiver command should be executed. If the value is 0, it means that the shiver should be repeated until the STOP command is received.   Yes

  Delay       2 Bytes      This parameter specifies the number of milliseconds of the delay between repeating a shiver. The value has a range from 0 - 65 535. This parameter is only required if the shiver is being repeated more than once               No
  ----------- ------------ -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- --------------

\

[]() Mate {#mate .western align="justify" style="margin-top: 0cm"}
---------

### []() Command Type {#command-type-3 .western align="justify" style="line-height: 100%; page-break-inside: avoid; page-break-after: avoid"}

Character code: XXX

Binary value: 0100 1101

\

### []() Behaviour {#behaviour-3 .western align="justify" style="line-height: 100%; page-break-inside: avoid; page-break-after: avoid"}

When receiving the Mate command the robot will execute the mating
ritual.

\

### []() Parameters {#parameters-3 .western align="justify" style="line-height: 100%; page-break-inside: avoid; page-break-after: avoid"}

\

  ---------- ------------ --------------------------------------------------------------------------------------------------------------------------- --------------
  **Name**   **Length**   **Description**                                                                                                             **Required**
                                                                                                                                                      
             \                                                                                                                                        
                                                                                                                                                      

  DeviceID   1 Byte       This parameter specifies the ID of the robot for which the mating ritual is performed. The value has a range from 0 - 255   Yes
  ---------- ------------ --------------------------------------------------------------------------------------------------------------------------- --------------

\

[]() Greet {#greet .western align="justify" style="line-height: 100%; page-break-inside: avoid; page-break-after: avoid"}
----------

### []() Command Type {#command-type-4 .western align="justify" style="line-height: 100%; page-break-inside: avoid; page-break-after: avoid"}

Character code: XXX

Binary value: 0100 1101

\

### []() Behaviour {#behaviour-4 .western align="justify" style="line-height: 100%; page-break-inside: avoid; page-break-after: avoid"}

When receiving the Greet command the robot will execute the greeting
ritual.

\

### []() Parameters {#parameters-4 .western align="justify" style="line-height: 100%; page-break-inside: avoid; page-break-after: avoid"}

\

  ---------- ------------ --------------------------------------------------------------------------------------------------------------------------- --------------
  **Name**   **Length**   **Description**                                                                                                             **Required**
                                                                                                                                                      
             \                                                                                                                                        
                                                                                                                                                      

  DeviceID   1 Byte       This parameter specifies the ID of the robot for which the mating ritual is performed. The value has a range from 0 - 255   Yes
  ---------- ------------ --------------------------------------------------------------------------------------------------------------------------- --------------

\

\

[]() Waggle {#waggle .western align="justify" style="margin-top: 0cm"}
-----------

### []() Command Type {#command-type-5 .western align="justify" style="line-height: 100%; page-break-inside: avoid; page-break-after: avoid"}

Character code: W

Binary value: 0101 0111

\

### []() Behaviour {#behaviour-5 .western align="justify" style="line-height: 100%; page-break-inside: avoid; page-break-after: avoid"}

When receiving the Waggle command the robot will execute a waggle.

\

### []() Parameters {#parameters-5 .western align="justify" style="line-height: 100%; page-break-inside: avoid; page-break-after: avoid"}

\

  ----------- ------------ -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- --------------
  **Name**    **Length**   **Description**                                                                                                                                                                                                                  **Required**
                                                                                                                                                                                                                                                            
              \                                                                                                                                                                                                                                             
                                                                                                                                                                                                                                                            

  Intensity   1 Byte       This parameter specifies the intensity of the waggle. The value has a range from 0 - 255                                                                                                                                         Yes

  On Time     2 Bytes      This parameter specifies the number of milliseconds that the robot will execute the command. The value has a range from 0 - 65 535                                                                                               Yes

  Repeat      1 Byte       This parameter is an unsigned SHORT which gives an indication of the amount of times the waggle command should be executed. If the value is 0, it means that the waggle should be repeated until the STOP command is received.   Yes

  Delay       2 Bytes      This parameter specifies the number of milliseconds of the delay between repeating a waggle. The value has a range from 0 - 65 535. This parameter is only required if the shiver is being repeated more than once               No
  ----------- ------------ -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- --------------

[]()\
 {#section-1 .western align="justify" style="margin-top: 0cm"}
=====

[]() Recoil {#recoil .western align="justify" style="line-height: 100%; page-break-inside: avoid; page-break-after: avoid"}
-----------

### []() Command Type {#command-type-6 .western align="justify" style="line-height: 100%; page-break-inside: avoid; page-break-after: avoid"}

Character code: ??

Binary value: 0101 0111

\

### []() Behaviour {#behaviour-6 .western align="justify" style="line-height: 100%; page-break-inside: avoid; page-break-after: avoid"}

When the robot was hit he must look sad, reverse rapidly for 5 cm, and
then shiver while making obscene noises and flashing eye LEDs...

\

### []() Parameters {#parameters-6 .western align="justify" style="line-height: 100%; page-break-inside: avoid; page-break-after: avoid"}

\

  ----------- ------------ -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- --------------
  **Name**    **Length**   **Description**                                                                                                                                                                                                                  **Required**
                                                                                                                                                                                                                                                            
              \                                                                                                                                                                                                                                             
                                                                                                                                                                                                                                                            

  Intensity   1 Byte       This parameter specifies the intensity of the waggle. The value has a range from 0 - 255                                                                                                                                         Yes

  On Time     2 Bytes      This parameter specifies the number of milliseconds that the robot will execute the command. The value has a range from 0 - 65 535                                                                                               Yes

  Repeat      1 Byte       This parameter is an unsigned SHORT which gives an indication of the amount of times the waggle command should be executed. If the value is 0, it means that the waggle should be repeated until the STOP command is received.   Yes

  Delay       2 Bytes      This parameter specifies the number of milliseconds of the delay between repeating a waggle. The value has a range from 0 - 65 535. This parameter is only required if the shiver is being repeated more than once               No
  ----------- ------------ -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- --------------

[]()\
 {#section-2 .western align="justify" style="line-height: 100%; page-break-inside: avoid; page-break-after: avoid"}
=====

[]() Sensor & Actuator Commands {#sensor-actuator-commands .western align="justify" style="margin-top: 0cm"}
===============================

Sensor & Actuator Commands are a subset of commands embedded on the
robot which will provide us basic functionality to control the robot’s
sensory devices, including LEDs, IR and sounds. These commands include
the following:

-   -   -   -   -   

\

These commands are described in more detail below:

\

[]() Eye Pattern {#eye-pattern .western align="justify" style="margin-top: 0cm"}
----------------

### []() Command Type {#command-type-7 .western style="margin-top: 0cm"}

Character code: E

Binary value: <span style="background: #f1f1f1">0100 0101</span>

### []() Behaviour {#behaviour-7 .western style="margin-top: 0cm"}

### []() <span style="font-weight: normal">Command to create an expression on each of the eyes.</span> {#command-to-create-an-expression-on-each-of-the-eyes. .western align="justify" style="margin-top: 0cm"}

### []() Parameters {#parameters-7 .western style="margin-top: 0cm"}

\

  ----------------- ------------ ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- --------------
  **Name**          **Length**   **Description**                                                                                                                                                                     **Required**
                                                                                                                                                                                                                     
                    \                                                                                                                                                                                                
                                                                                                                                                                                                                     

  spare             1 Byte       To keep message the same format as Toggle command. Set this byte to 0x00.                                                                                                           Yes

  Row LED pattern   6 Bytes      Each eye consists of a matrix of 6 rows with each row having 6 LEDs. The most-significant 2 bits of the first byte is used to select the right or left eye that the data applies.   Yes
                                                                                                                                                                                                                     
                                 See “Eye Expression Description” section at end of document for additional detail.                                                                                                  
  ----------------- ------------ ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- --------------

\

\

[]() Toggle {#toggle .western align="justify" style="line-height: 100%; page-break-inside: avoid; page-break-after: avoid"}
-----------

### []() Command Type {#command-type-8 .western style="line-height: 100%; page-break-inside: avoid; page-break-after: avoid"}

Character code: T

Binary value: 0101 0100

### []() Behaviour {#behaviour-8 .western style="line-height: 100%; page-break-inside: avoid; page-break-after: avoid"}

### []() <span style="font-weight: normal">The toggle command allows the controller to switch on/off LEDs and actuators linked to the robot. The three default states are Switch ON, Switch OFF or intermittently switch ON/Off.</span> {#the-toggle-command-allows-the-controller-to-switch-onoff-leds-and-actuators-linked-to-the-robot.-the-three-default-states-are-switch-on-switch-off-or-intermittently-switch-onoff. .western align="justify" style="line-height: 100%; page-break-inside: avoid; page-break-after: avoid"}

### []() Parameters {#parameters-8 .western style="line-height: 100%; page-break-inside: avoid; page-break-after: avoid"}

\

  -------------- ------------ ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ ---------------
  **Name**       **Length**   **Description**                                                                                                                                                                                                                            **Required**
                                                                                                                                                                                                                                                                         
                 \                                                                                                                                                                                                                                                       
                                                                                                                                                                                                                                                                         

  DeviceSelect   2 Bytes      This parameter specifies an array of devices should be toggled by this command. The value of this byte addresses a specific device. The action to be performed on the device is determined by next (state) byte. The list of devices are   Yes
                                                                                                                                                                                                                                                                         
                              Value 0 : unused                                                                                                                                                                                                                           
                                                                                                                                                                                                                                                                         
                              Value 1 : unused                                                                                                                                                                                                                           
                                                                                                                                                                                                                                                                         
                              Value 2 : Left Laser                                                                                                                                                                                                                       
                                                                                                                                                                                                                                                                         
                              Value 3 : Right Laser                                                                                                                                                                                                                      
                                                                                                                                                                                                                                                                         
                              Value 4: Multicolor LED - RED                                                                                                                                                                                                              
                                                                                                                                                                                                                                                                         
                              Value 5 : Multicolor LED - GREEN                                                                                                                                                                                                           
                                                                                                                                                                                                                                                                         
                              Value 6 : Multicolor LED - BLUE                                                                                                                                                                                                            
                                                                                                                                                                                                                                                                         
                              Value 7: Multicolor LED - RED + GRN                                                                                                                                                                                                        
                                                                                                                                                                                                                                                                         
                              Value 8 : Multicolor LED - RED + BLUE                                                                                                                                                                                                      
                                                                                                                                                                                                                                                                         
                              Value 9 : Multicolor LED - GRN + BLUE                                                                                                                                                                                                      
                                                                                                                                                                                                                                                                         
                              Value 10: Multicolor LED -                                                                                                                                                                                                                 
                                                                                                                                                                                                                                                                         
                              RED + GREEN + BLUE                                                                                                                                                                                                                         
                                                                                                                                                                                                                                                                         
                              \                                                                                                                                                                                                                                          
                                                                                                                                                                                                                                                                         
                              Value 11 - 65535 : unused                                                                                                                                                                                                                  

  State          1 Byte       The state indicates the toggle state to execute and include the following:                                                                                                                                                                 Yes
                                                                                                                                                                                                                                                                         
                              1: Off                                                                                                                                                                                                                                     
                                                                                                                                                                                                                                                                         
                              2: On                                                                                                                                                                                                                                      
                                                                                                                                                                                                                                                                         
                              3: Flash                                                                                                                                                                                                                                   

  On Time        2 bytes      This parameter specifies the number of milliseconds that the device will be in the On state if the command state is Flash. The value has a range from 0 - 65 535. Practical values range from 10ms to 3000ms.                              Yes for Flash

  Off Time       2 Bytes      This parameter specifies the number of milliseconds that the device will be in the Off state if the command state is Flash. The value has a range from 0 - 65 535. Practical values range from 10ms to 3000ms.                             Yes for Flash
  -------------- ------------ ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ ---------------

\

[]() Play {#play .western style="margin-top: 0cm"}
---------

### []() Command Type {#command-type-9 .western style="line-height: 100%; page-break-inside: avoid; page-break-after: avoid"}

Character code: P

Binary value: 0101 0000

### []() Behaviour {#behaviour-9 .western style="line-height: 100%; page-break-inside: avoid; page-break-after: avoid"}

### []() <span style="font-weight: normal">The play command allows the controller to play pre-programmed sounds on the robot. The controller can indicate whether a sound should be played only once or whether it should be repeated a number of times.</span> {#the-play-command-allows-the-controller-to-play-pre-programmed-sounds-on-the-robot.-the-controller-can-indicate-whether-a-sound-should-be-played-only-once-or-whether-it-should-be-repeated-a-number-of-times. .western align="justify" style="line-height: 100%; page-break-inside: avoid; page-break-after: avoid"}

### []() Parameters {#parameters-9 .western style="line-height: 100%; page-break-inside: avoid; page-break-after: avoid"}

\

  ---------- ------------ ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- --------------
  **Name**   **Length**   **Description**                                                                                                                                                                                                         **Required**
                                                                                                                                                                                                                                                  
             \                                                                                                                                                                                                                                    
                                                                                                                                                                                                                                                  

  SoundIdx   1 Byte       This parameter is an unsigned SHORT which gives an index to the sound file that should be played. The value range is from 0-255 and current soudns that are configured are:                                             Yes
                                                                                                                                                                                                                                                  
                          0: Greet Friend                                                                                                                                                                                                         
                                                                                                                                                                                                                                                  
                          1: Greet Stranger                                                                                                                                                                                                       
                                                                                                                                                                                                                                                  
                          2: Shoot                                                                                                                                                                                                                
                                                                                                                                                                                                                                                  
                          3: New Friend                                                                                                                                                                                                           
                                                                                                                                                                                                                                                  
                          4: Happy                                                                                                                                                                                                                
                                                                                                                                                                                                                                                  
                          5: Sad                                                                                                                                                                                                                  

  Repeat     1 Byte       This parameter is an unsigned SHORT which gives an indication of the amount of times a sound file should be played. If the value is 0, it means that the sound should be repeated until the STOP command is received.   Yes

  Delay      2 Bytes      This parameter specifies the number of milliseconds of the delay between repeating a sound. The value has a range from 0 - 65 535. This parameter is only required if the sound is being repeated more than once        No
  ---------- ------------ ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- --------------

\

[]() Interact {#interact .western style="margin-top: 0cm"}
-------------

### []() Command Type {#command-type-10 .western style="line-height: 100%; page-break-inside: avoid; page-break-after: avoid"}

Character code: I

Binary value: 0100 1001

### []() Behaviour {#behaviour-10 .western style="line-height: 100%; page-break-inside: avoid; page-break-after: avoid"}

### []() <span style="font-weight: normal">The interact command allows the controller to instruct the robot to interact with other robots. Interactions can either be a broadcast to all robots or an interaction with a specific robot. </span> {#the-interact-command-allows-the-controller-to-instruct-the-robot-to-interact-with-other-robots.-interactions-can-either-be-a-broadcast-to-all-robots-or-an-interaction-with-a-specific-robot. .western align="justify" style="line-height: 100%; page-break-inside: avoid; page-break-after: avoid"}

### []() Parameters {#parameters-10 .western style="line-height: 100%; page-break-inside: avoid; page-break-after: avoid"}

\

  ------------------ ------------ --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- --------------
  **Name**           **Length**   **Description**                                                                                                                                                                                           **Required**
                                                                                                                                                                                                                                            
                     \                                                                                                                                                                                                                      
                                                                                                                                                                                                                                            

  Interaction Type   1 Byte       The interaction type is an unsigned short that represents the command that will be sent to the other robot(s). Current commands include                                                                   Yes
                                                                                                                                                                                                                                            
                                  0: Transmit ID                                                                                                                                                                                            
                                                                                                                                                                                                                                            
                                  1: Shoot                                                                                                                                                                                                  
                                                                                                                                                                                                                                            
                                  2: Greet                                                                                                                                                                                                  
                                                                                                                                                                                                                                            
                                  The value has a range of 0-255                                                                                                                                                                            

  SourceID           1 byte       The SourceID is an unsigned short that represents the ID of the robot that is transmitting the message/command                                                                                            Yes

  DestinationID      4 Byte       The DestinationID is an unsigned short that represents the ID of the robot for which the message is intended. If the Destination ID is 0, the message is a broadcast to all the robots in the vicinity.   Yes
  ------------------ ------------ --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- --------------

\

[]() Shoot {#shoot .western align="justify" style="margin-top: 0cm"}
----------

### []() Command Type {#command-type-11 .western style="line-height: 100%; page-break-inside: avoid; page-break-after: avoid"}

Character code: X

Binary value: 0101 1000

### []() Behaviour {#behaviour-11 .western style="line-height: 100%; page-break-inside: avoid; page-break-after: avoid"}

### []() <span style="font-weight: normal">The shoot command allows the controller to instruct the robot to attack other robots. The robot will perform its pre-shot routine to locate the target, and it will then shoot and recoil. </span> {#the-shoot-command-allows-the-controller-to-instruct-the-robot-to-attack-other-robots.-the-robot-will-perform-its-pre-shot-routine-to-locate-the-target-and-it-will-then-shoot-and-recoil. .western align="justify" style="line-height: 100%; page-break-inside: avoid; page-break-after: avoid"}

### []() Parameters {#parameters-11 .western style="line-height: 100%; page-break-inside: avoid; page-break-after: avoid"}

\

  ----------- ------------ ---------------------------------------------------------------------------------------------------- --------------
  **Name**    **Length**   **Description**                                                                                      **Required**
                                                                                                                                
              \                                                                                                                 
                                                                                                                                

  Intensity   1 Byte       This parameter specifies the intensity of the shot being fired. The value has a range from 0 - 255   Yes

  Target ID   4 Byte       The ID of the target robot /Team/Guild being fired at                                                Yes
  ----------- ------------ ---------------------------------------------------------------------------------------------------- --------------

\

[]() Scan {#scan .western align="justify" style="line-height: 100%; page-break-inside: avoid; page-break-after: avoid"}
---------

### []() Command Type {#command-type-12 .western style="line-height: 100%; page-break-inside: avoid; page-break-after: avoid"}

Character code: X

Binary value: 0101 1000

### []() Behaviour {#behaviour-12 .western style="line-height: 100%; page-break-inside: avoid; page-break-after: avoid"}

### []() ~~<span style="font-weight: normal">The shoot command allows the controller to instruct the robot to attack other robots. The robot will perform its pre-shot routine to locate the target, and it will then shoot and recoil. </span>~~ {#the-shoot-command-allows-the-controller-to-instruct-the-robot-to-attack-other-robots.-the-robot-will-perform-its-pre-shot-routine-to-locate-the-target-and-it-will-then-shoot-and-recoil.-1 .western align="justify" style="line-height: 100%; page-break-inside: avoid; page-break-after: avoid"}

The Scan command will result in the robot doing a 360 spin while
searching for a specific robot’s IR emissions (ID, partial key); the
robot will stop the spin as soon as the target robot has been detected
(i.e. robot will point more or less in the direction of the target
robot).

### []() Parameters {#parameters-12 .western style="line-height: 100%; page-break-inside: avoid; page-break-after: avoid"}

\

  -------------- ------------ -------------------------------------------------------------------------------------------------------- --------------
  **Name**       **Length**   **Description**                                                                                          **Required**
                                                                                                                                       
                 \                                                                                                                     
                                                                                                                                       

  ~~Duration~~   ~~1 Byte~~   ~~This parameter specifies the intensity of the shot being fired. The value has a range from 0 - 255~~   ~~Yes~~

  Target ID      4 Byte       The ID of the target robot /Team/Guild scan                                                              Yes
  -------------- ------------ -------------------------------------------------------------------------------------------------------- --------------

\

[]() Notifications {#notifications .western align="justify" style="margin-top: 0cm"}
==================

Notifications are messages sent from the Robot to the Smartphone.

[]() Obstacle Detected {#obstacle-detected .western align="justify" style="margin-top: 0cm"}
----------------------

### []() Command Type {#command-type-13 .western style="line-height: 100%; page-break-inside: avoid; page-break-after: avoid"}

Character code: O

Binary value: 0100 1111

### []() Behaviour {#behaviour-13 .western style="line-height: 100%; page-break-inside: avoid; page-break-after: avoid"}

### []() <span style="font-weight: normal">When the robot has detected any obstacle it can notify the smartphone of the obstacle by sending a notification that include the direction of the obstacle and the distance.</span> {#when-the-robot-has-detected-any-obstacle-it-can-notify-the-smartphone-of-the-obstacle-by-sending-a-notification-that-include-the-direction-of-the-obstacle-and-the-distance. .western align="justify" style="line-height: 100%; page-break-inside: avoid; page-break-after: avoid"}

### []() Parameters {#parameters-13 .western style="line-height: 100%; page-break-inside: avoid; page-break-after: avoid"}

\

  ------------- ------------ ------------------------------------------------------------------------------------------------------------------------------------------------------ --------------
  **Name**      **Length**   **Description**                                                                                                                                        **Required**
                                                                                                                                                                                    
                \                                                                                                                                                                   
                                                                                                                                                                                    

  Distance      1 Byte       The detected distance to the obstacle in Near/Far. I will only get it if it is another robot                                                           No
                                                                                                                                                                                    
                             \                                                                                                                                                      
                                                                                                                                                                                    

  Direction     2 Bytes      The direction in Left/Right/Both. All obstacles                                                                                                        Yes

  Obstacle ID   1 Byte       The Device ID of the obstacle that was detected, if known. As an example if the robot detected another robot, he can pass the ID to the smart phone.   No
  ------------- ------------ ------------------------------------------------------------------------------------------------------------------------------------------------------ --------------

\

[]() Interaction Receive {#interaction-receive .western align="justify" style="margin-top: 0cm"}
------------------------

### []() Command Type {#command-type-14 .western style="line-height: 100%; page-break-inside: avoid; page-break-after: avoid"}

Character code: i

Binary value: <span style="background: #f1f1f1">0110 1001</span>

### []() Behaviour {#behaviour-14 .western style="line-height: 100%; page-break-inside: avoid; page-break-after: avoid"}

### []() <span style="font-weight: normal">When the robot receives an IR command from another robot, the robot should notify the smartphone of the interaction that was received.</span> {#when-the-robot-receives-an-ir-command-from-another-robot-the-robot-should-notify-the-smartphone-of-the-interaction-that-was-received. .western align="justify" style="line-height: 100%; page-break-inside: avoid; page-break-after: avoid"}

### []() Parameters {#parameters-14 .western style="line-height: 100%; page-break-inside: avoid; page-break-after: avoid"}

\

  ------------------- ------------ --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- --------------
  **Name**            **Length**   **Description**                                                                                                                                                                                                                         **Required**
                                                                                                                                                                                                                                                                           
                      \                                                                                                                                                                                                                                                    
                                                                                                                                                                                                                                                                           

  Interaction Type    1 Byte       The interaction type is an unsigned short that represents the command that was received from the other robot(s). Current commands include                                                                                               Yes
                                                                                                                                                                                                                                                                           
                                   0: Transmit ID                                                                                                                                                                                                                          
                                                                                                                                                                                                                                                                           
                                   1: Shoot                                                                                                                                                                                                                                
                                                                                                                                                                                                                                                                           
                                   2: Greet                                                                                                                                                                                                                                
                                                                                                                                                                                                                                                                           
                                   The value has a range of 0-255                                                                                                                                                                                                          

  SourceID            1 Byte       The SourceID is an unsigned short that represents the ID of the robot that transmitted the message/command                                                                                                                              Yes

  ~~DestinationID~~   ~~4 Byte~~   ~~The DestinationID is an unsigned short that represents the ID of the robot for which the message is intended. If the Destination ID is 0, the message is a broadcast to all the robots in the vicinity. Either a team/guild/robot~~   ~~Yes~~
  ------------------- ------------ --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- --------------

\

[]() Error {#error .western align="justify" style="margin-top: 0cm"}
----------

### []() Command Type {#command-type-15 .western style="line-height: 100%; page-break-inside: avoid; page-break-after: avoid"}

Character code: !

Binary value: <span style="background: #f1f1f1">0020 0001</span>

### []() Behaviour {#behaviour-15 .western style="line-height: 100%; page-break-inside: avoid; page-break-after: avoid"}

### []() <span style="font-weight: normal">When an error occurred on the robot, the robot should notify the smartphone by raising an Error notification.</span> {#when-an-error-occurred-on-the-robot-the-robot-should-notify-the-smartphone-by-raising-an-error-notification. .western align="justify" style="line-height: 100%; page-break-inside: avoid; page-break-after: avoid"}

### []() Parameters {#parameters-15 .western style="line-height: 100%; page-break-inside: avoid; page-break-after: avoid"}

\

  --------------- ------------ ------------------------------------------------------------------------------------------------- --------------
  **Name**        **Length**   **Description**                                                                                   **Required**
                                                                                                                                 
                  \                                                                                                              
                                                                                                                                 

  Error Code      2 Bytes      The Error Code is an unsigned INT that represents the error which occurred                        Yes

  Command Index   1 Byte       The command index indicates the command that the robot was busy executing when the error arose.   Yes
  --------------- ------------ ------------------------------------------------------------------------------------------------- --------------

\

[]() Status Notification {#status-notification .western align="justify" style="margin-top: 0cm"}
------------------------

### []() Command Type {#command-type-16 .western style="line-height: 100%; page-break-inside: avoid; page-break-after: avoid"}

Character code: s

Binary value: <span style="background: #f1f1f1">0111 0011</span>

### []() Behaviour {#behaviour-16 .western style="line-height: 100%; page-break-inside: avoid; page-break-after: avoid"}

### []() <span style="font-weight: normal">The status notification is either a response after the robot received a status request, or during debug mode the robot can transmit the status notification to the smartphone at preset intervals. The status notification will contain an array of status of all the sensors, actuators, motors, LEDs, etc… that is available on the robot. It is basically a data dump of what the robot knows.</span> {#the-status-notification-is-either-a-response-after-the-robot-received-a-status-request-or-during-debug-mode-the-robot-can-transmit-the-status-notification-to-the-smartphone-at-preset-intervals.-the-status-notification-will-contain-an-array-of-status-of-all-the-sensors-actuators-motors-leds-etc-that-is-available-on-the-robot.-it-is-basically-a-data-dump-of-what-the-robot-knows. .western align="justify" style="line-height: 100%; page-break-inside: avoid; page-break-after: avoid"}

### []()\
 {#section-3 .western align="justify" style="margin-top: 0cm; line-height: 100%; page-break-inside: auto; page-break-after: auto"}

[]() Handshake {#handshake .western align="justify" style="margin-top: 0cm; line-height: 100%; page-break-inside: auto; page-break-after: auto"}
--------------

[]()\
 {#section-4 .western align="justify" style="margin-top: 0cm; line-height: 100%; page-break-inside: auto; page-break-after: auto"}
-----

\

### []()\
 {#section-5 .western style="line-height: 100%; page-break-inside: avoid; page-break-after: avoid"}

### []() Parameters {#parameters-16 .western style="line-height: 100%; page-break-inside: avoid; page-break-after: avoid"}

\

  -------------- ------------ ---------------------------------------------------------------------------------------------------------------------------------------------------------- --------------
  **Name**       **Length**   **Description**                                                                                                                                            **Required**
                                                                                                                                                                                         
                 \                                                                                                                                                                       
                                                                                                                                                                                         

  Status Array   Max Bytes    The status array will consist of a list of StatusIDs (1 Byte) followed by a status value (2 Bytes) that will enumerate all the statii known by the robot   Yes
  -------------- ------------ ---------------------------------------------------------------------------------------------------------------------------------------------------------- --------------

\

[]() Administrative Commands {#administrative-commands .western align="justify" style="margin-top: 0cm"}
============================

Status Request

Version Request

Download

Firmware Update

Restart

Reset

**SECTION TWO - Communication between Main Controller and IR
Controller**

\

(Text defined 14 April 2016)

\

The interface is based on memory registers written to / read by the Main
Controller as well as the the IR Controller.

\

The interface is based on I2C communication between the 2 circuit boards
and is byte oriented. The I2C device address is 0x4F. Thus for a write
operation, the value of 0x9E will be on the bus - for a read operation,
the value of 0x9F will be on the bus.

\

\

  -------- ----------------------- --------------------------------------------- -----------
  Offset   Item                    Description                                   Operation

  24       Number of Time Slots    Maximum number of time slots                  MCW

  23       My\_ID, lsb             \                                             MCW
                                                                                 

  22       My\_ID, msb             ID assigned by Main Controller                MCW

  21       Eye off time, lsb       \                                             MCW
                                                                                 

  20       Eye off time, msb       Off-time as set by Toggle Command             MCW

  19       Eye on time, lsb        \                                             MCW
                                                                                 

  18       Eye on time, msb        On-time as set by Toggle Command              MCW

  17       Eye expression, row6    \                                             MCW
                                                                                 

  16       Eye expression, row5    \                                             MCW
                                                                                 

  15       Eye expression, row4    \                                             MCW
                                                                                 

  14       Eye expression, row3    \                                             MCW
                                                                                 

  13       Eye expression, row2    \                                             MCW
                                                                                 

  12       Eye expression, row1    Eye pattern, see description below            MCW

  11       RGB LED off time, lsb   \                                             MCW
                                                                                 

  10       RGB LED off time, msb   Off-time as set by Toggle Command             MCW

  9        RGB LED on time, lsb    \                                             MCW
                                                                                 

  8        RGB LED on time, msb    On-time as set by Toggle Command              MCW

  7        RGB LED state           State set as per Toggle Command               MCW

  6        RGB LED select          Select R/G/B LED as per Toggle Command        \
                                                                                 

  5        Robot1\_ID, lsb         \                                             IRCW
                                                                                 

  4        Robot1\_ID, msb         ID of second robot detected by IR subsystem   IRCW

  3        Robot0\_ID, lsb         \                                             IRCW
                                                                                 

  2        Robot0\_ID, msb         ID of first robot detected by IR subsystem    IRCW

  1        STATUS1                 Reserved for future use                       IRCW

  0        STATUS0                 See below                                     IRCW,
                                                                                 
                                                                                 MCW
  -------- ----------------------- --------------------------------------------- -----------

\

MCW Main Controller Write

MCR Main Controller Read

IRCW IR Controller Write

\

An MCW operation at STATUS0 will result in a time-slot synchronisation
operation of the IR subsystem.

\

STATUS0

Bit0 Obstacle detected, if set.

Bit1 PIR detected, if set.

Bit2 Robot0 detected, if set.

Bit3 Robot0, near / far indication. Near, if set.

Bit4 Robot1 detected, if set.

Bit5 Robot1, near / far indication. Near, if set.

Bit6 spare1.

Bit7 spare2.

\

**EYE EXPRESSION DESCRIPTION**

\

Each eye consists of 6 rows of 6 LEDs. The upper and lower rows only
have 4 LEDs.

\

The LEDs are only present in the bit positions high-lighted below.

\

Bit6 and Bit7 of Row1 also indicate which eye is required to draw the
pattern.

Bit6 = 1: pattern is drawn on left eye

Bit7 = 1: pattern is drawn on right eye

\

If same pattern is to be drawn on both eyes, then bits 6 and 7 are set
simultaneously.

\

\

  ----------------------------------------------- ----------------------------------------------- ----------------------------------------------- ----------------------------------------------- ----------------------------------------------- ----------------------------------------------- --- -------
  bit5                                            <span style="background: #e06666">bit4</span>   <span style="background: #e06666">bit3</span>   <span style="background: #e06666">bit2</span>   <span style="background: #e06666">bit1</span>   bit0                                            \   Row 1
                                                                                                                                                                                                                                                                                                      

  <span style="background: #e06666">bit5</span>   <span style="background: #e06666">bit4</span>   <span style="background: #e06666">bit3</span>   <span style="background: #e06666">bit2</span>   <span style="background: #e06666">bit1</span>   <span style="background: #e06666">bit0</span>   \   Row 2
                                                                                                                                                                                                                                                                                                      

  <span style="background: #e06666">bit5</span>   <span style="background: #e06666">bit4</span>   <span style="background: #e06666">bit3</span>   <span style="background: #e06666">bit2</span>   <span style="background: #e06666">bit1</span>   <span style="background: #e06666">bit0</span>   \   Row 3
                                                                                                                                                                                                                                                                                                      

  <span style="background: #e06666">bit5</span>   <span style="background: #e06666">bit4</span>   <span style="background: #e06666">bit3</span>   <span style="background: #e06666">bit2</span>   <span style="background: #e06666">bit1</span>   <span style="background: #e06666">bit0</span>   \   Row 4
                                                                                                                                                                                                                                                                                                      

  <span style="background: #e06666">bit5</span>   <span style="background: #e06666">bit4</span>   <span style="background: #e06666">bit3</span>   <span style="background: #e06666">bit2</span>   <span style="background: #e06666">bit1</span>   <span style="background: #e06666">bit0</span>   \   Row 5
                                                                                                                                                                                                                                                                                                      

  bit5                                            <span style="background: #e06666">bit4</span>   <span style="background: #e06666">bit3</span>   <span style="background: #e06666">bit2</span>   <span style="background: #e06666">bit1</span>   bit0                                            \   Row 6
                                                                                                                                                                                                                                                                                                      
  ----------------------------------------------- ----------------------------------------------- ----------------------------------------------- ----------------------------------------------- ----------------------------------------------- ----------------------------------------------- --- -------

\

\

**I2C Write Protocol**

\

The following byte sequence is defined for a write operation:

Byte0 : address

Byte1: register offset

Byte2: number of data bytes in payload

Byte3: first data byte to be written.

ByteN: nth data byte to be written

\

**I2C Read Protocol**

\

The following byte sequence is defined for a read operation:

Byte0 : address

Byte1: register offset

Byte2: first data byte to be read at offset position

ByteN: subsequent bytes from next registers. Offset pointer increments
with ever read

\

\

\

**SECTION THREE - Notes on the Operation of the IR Subsystem**

\

(Text defined 28 May 2016)

\

The nature of the IR subsystem is that one robot will transmit an IR
packet at a time, while all the others will be potentially receiving a
transmission from another robot. A robot will also receive its own
transmission, which can be used for obstacle detection.

\

The data that is transmitted via the IR transmitters will include each
robot’s ID information, so that a robot can determine if 1 or 2 robots
are in front of it.

\

The timing is based on the allocation a 50ms time-slot to each robot. It
is assumed that there is a synchronization mechanism by the Main
Controller to keep the 50ms time-slots on each of the robots fairly
closely aligned, to ultimately prevent flooding of IR signals, which
will make detection of useful IR information impossible.

\

Currently 6 time-slots are defined (0-5), for the scenario of 6 robots
in play.

\

Future considerations will use the Robot ID number to determine which
timeslot the IR message / signature will be transmitted. However, for
the first iteration, the following messages / IDs will be used:

0xAA, 0xCC, 0xE1, 0x25, 0x00 robot 1,

0xAA, 0xCC, 0xD2, 0x24, 0x00 robot 2, etc

0xAA, 0xCC, 0xB4, 0x22, 0x00

0xAA, 0xCC, 0x96, 0x20, 0x00

0xAA, 0xCC, 0x87, 0x1F, 0x00

0xAA, 0xCC, 0x78, 0x1E, 0x00

\

The 0x00 at the end of the sequence is to facilitate string operations
in the firmware, but only the first 4 bytes of the string are
transmitted.

\

Byte 0 & 1 are sync bytes. Byte 2 is Robot ID, Byte 3 is the CRC which
is made up of the sum of first 3 bytes and then &gt;&gt; 4.

\

\

\
