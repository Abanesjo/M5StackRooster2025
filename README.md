# M5StackRooster2025
## CPE Term Project: A Smart Alarm System that Wakes You Up
### by John Abanes, Druce Chua, and Sri Pranav Srivatsavai

## Brief Description 
This is a smart alarm clock that can be strapped to your wrist when you sleep. You set an alarm time and a buffer time. For example, let's have an 8-hour alarm with a 15-minute buffer time. You input these, strap it to your wrist, and go to sleep. During the last 15 minutes of your alarm, it will begin recording movement. This is because, on a very rudimentary level, the more you move, the shallower your sleep is. Waking up in a deep stage may cause grogginess and so the alarm will instead wake you up when it detects a lot of movement. If it doesn't detect much movement, then it will just wake you up when the 8-hour mark is reached, so as to not ruin your schedule. It will then launch a simple cognitive game to serve as a brain exercise to further wake you up (and prevent you from simply turning it off and going back to sleep!). Below is an image containing all the screens in this project. 

![Program Sequence](https://i.imgur.com/ZmG79Wo.jpg)

### More specific information has been detailed in this project's final report. 

## User Manual
To run this program, an M5Stack Core 2 is needed (no other peripherals required). Make sure to install the drivers and other necessary software that allows the core to run Arduino code. More information can be found here: [Core 2 Setup](https://docs.m5stack.com/en/quick_start/core2/arduino). 

After doing the setup above, simply download the Rooster directory which should have the source file and the sound data file. Compile it using your Arduino IDE and execute it using the core. If all goes well, that should be it and your smart alarm system should be ready! The features in the core are self-explanatory and most screens have a back button as well. 

Also note that the analysis of the core's accelerometer is done through MATLAB and the relevant files are located in the MATLAB folder. 

## Challenges
This section discusses the challenges and the corresponding solutions in the implementation of the project. Because most features have already been discussed in the previous sections, this section simply highlights some of the most troubling issues and how they were addressed. 

### Irregular Position Change Measurements
Upon first implementation of the gyro-based movement detection, it seemed to work perfectly fine. However, as we further increased the program’s complexity, we noticed that the reported deviations began to vary. We presumed that this was due to the changes in the core’s performance. As it ran more processes, it started running more slowly. This increased the delay between readings, affecting the measured difference. As such, we decided that in all areas of the code where the detectMovement() function was called, a delay(150) function would also be called to set the interval uniformly to 150ms. In doing so, the delay would be standardized regardless of performance. 

### Screen Flickering
One of the issues that we encountered was that the screen would flicker when two elements were overlaid over each other. We first tried telling the program to clear the screen before every iteration. However, this resulted in the whole screen flickering instead. The solution to this issue was the division of each screen into its static and dynamic components, as discussed earlier. Instead of clearing the screen, the static elements were drawn once and the dynamic elements were overwritten on top of each other each iteration. Because there weren’t any moving or animated objects in the project, this was a successful solution to the flickering. 

### Delay() Function Decreasing Accuracy
One issue in implementing the functions for the vibration and the ringtone was that these functions were periodic in the sense that they had to be continuously repeated. However, there needed to be a delay between activation otherwise the vibration and ringtone would simply be monotonous. Initially, we used the delay() function for this delay. However, this interfered with the other functions. For example, buttons had to be touched for the duration of the delay. Although the delay() function could work with the movement detection software, it wasn’t a big issue as there weren’t many buttons when movement detection was used and the delay was small. As such, for other applications, we instead used a variable to keep track of time. Rather than using the delay() function, we set a variable equal to the system time + a certain delay. If the system time was greater than the variable, then we executed the vibration and ringtone functions and once again redefined the variable in a similar manner. This method was explained earlier in the project development section and this allowed us to overcome the issues with the delay() function.

## Further Improvements
### Display Local Time and Date 
Apart from displaying the time remaining until the alarm time, showing the local time and date would also be a useful feature to have. This requires connecting the M5Stack Core2 to a WiFi network and syncing the time with the Network Time Protocol (NTP) server. Moreover, instead of showing both the alarm timer and the local time on the display simultaneously, potentially the best way to implement this, design wise, would be to add a button that switches the type of time displayed when pressed. This ensures that the M5Stack Rooster 2025’s clean and simple user interface is maintained despite the added feature.
![Alarm Timer and Local Time Display](https://i.imgur.com/XY4XKK5.png)

### Incorporate More Games
The following mini-games could be added for variety:
* Calculator Game
  * Incorporate simple math problems involving addition, subtraction, and multiplication.
* Shaking Game 
  * Requires the user to shake the core, whilst attached to his arm, at a specified speed for a set time duration. This can be implemented using the core’s accelerometer.
* Speaking Game
  * Requires the user to speak or scream at a certain frequency and volume range for a given duration of time. This can be incorporated by acquiring audio input through the core’s I2S amplifier.

**The type of game used shall be selected at random. Specific games can also be disabled if desired.*

### Alarm Tune Options
Allowing the user to choose the alarm sound according to his/her needs and preferences would be conducive to meeting the project’s main objective of minimizing sleep inertia and enhancing the user’s mood when waking up. Having an assortment of alarm tones would involve including an SD card to be able to store the different sound files.

(Challenges and Potential Improvements are taken from final report). 
