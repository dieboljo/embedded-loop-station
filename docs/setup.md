# Development Environment Setup

While the Arduino IDE is the most common environment for deploying to the
device, it is also possible to use command line tools. Both methods will be
outlined below.

## IDE

### Install Software & Libraries

1. Version 2.0.0 of the Arduino IDE ships with native support for the Teensy
   board. Platform-specific installers can be found at
   [https://www.arduino.cc/en/software](https://www.arduino.cc/en/software).
2. In the IDE, add the Teensy board manager by clicking _File > Preferences_,
   then copying the link `https://www.pjrc.com/teensy/package_teensy_index.json`
   into the **Additional boards manager URLs** field and clicking **OK**.
3. Verify the board manager is added by opening the **Boards Manager** sidebar
   menu and typing "teensy" into the search box. The Teensy package should be
   present and display **INSTALLED**. If the package is present but there is no
   **INSTALLED** indicator, click the **INSTALL** button.
4. This software requires the Teensy Audio Library. To install it into the
   Arduino IDE, first download the library's ZIP file from
   [https://github.com/PaulStoffregen/Audio/releases/tag/v1.03](https://github.com/PaulStoffregen/Audio/releases/tag/v1.03).
5. Without extracting the ZIP file, navigate to _Sketch > Include Library > Add
   .ZIP Library_ in the IDE, then select the ZIP file downloaded in the previous
   step.
6. Restart the IDE.

### Deploy to the device

1. To test compiling a project and deploying it to the device, a Teensy example
   will be used. To execute the program, a Teensy board with an attached audio
   adaptor is required, as well as a FAT32 formatted SD card.
2. Go to
   [https://www.pjrc.com/teensy/td_libs_AudioDataFiles.html](https://www.pjrc.com/teensy/td_libs_AudioDataFiles.html)
   and save the four WAV files to the SD card, then insert the SD card into the
   slot on the audio adaptor.
3. Open the IDE, and connect the device to the computer with a USB cord.
4. In the IDE, click _File > Examples > Audio > Tutorial >
   Part_1_03_Playing_Music_ to open the example program.
5. Click **Verify**, then wait for the program to compile.
6. Once finished, click **Upload**.
7. Plug headphones into the audio adaptor, music should be playing.

## CLI

### Install Software & Libraries

1. Navigate to
   [https://arduino.github.io/arduino-cli/0.27/installation/](https://arduino.github.io/arduino-cli/0.27/installation/),
   and follow the instructions for your development platform to install the
   Arduino CLI.
2. Initialize a configuration file by executing the command
   `arduino-cli config init`. By default, this is written to the `.arduino15`
   directory in the user home directory. This contains available configuration
   options with their default values.
3. Open the new configuration file, and add the Teensy board manager URL
   (`https://www.pjrc.com/teensy/package_teensy_index.json`) to the
   _additional_urls_ array under the _board_manager_ option.
4. This software requires the Teensy Audio Library. To install it, first
   download the library's ZIP file from
   [https://github.com/PaulStoffregen/Audio/releases/tag/v1.03](https://github.com/PaulStoffregen/Audio/releases/tag/v1.03).
5. Unzip the library into the Arduino user libraries folder. By default this is
   `$HOME/Arduino/libraries` (create the `libraries` folder first if it doesn't
   exist).

### Deploy to the device

1. To test compiling a project and deploying it to the device, a Teensy example
   will be used. To execute the program, a Teensy board with an attached audio
   adaptor is required, as well as a FAT32 formatted SD card.
2. Go to
   [https://www.pjrc.com/teensy/td_libs_AudioDataFiles.html](https://www.pjrc.com/teensy/td_libs_AudioDataFiles.html)
   and save the four WAV files to the SD card, then insert the SD card into the
   slot on the audio adaptor.
3. Connect the board to your computer.
4. Update the local cache of available platforms and libraries by running the
   command `arduino-cli core update-index`.
5. Verify that the board has been recognized by executing the command
   `arduino-cli board list`. Take note of the values of the **FQBN** and
   **Core** fields, as well as the **Port** field for the row with type
   **Serial**.
6. Install the core for the board by executing
   `arduino-cli core install [value_from_core_field]`, then verify the
   installation with `arduino-cli core list`.
7. The example "sketch" (Arduino's term for a project) can be found in a
   subdirectory of the installed audio library. By default, this is
   `$HOME/Arduino/libraries/Audio-X.XX/examples/Tutorial/Part_1_03_Playing_Music`.
   To compile the project, execute the following command from the `Tutorial`
   directory:

```
arduino-cli compile --fqbn [value_from_fqbn_field] Part_1_03_Playing_Music
```

8. Upon success, use the serial port noted previously to upload the program to
   the device:

```
arduino-cli upload -p [value_from_serial_port_field] --fqbn [value_from_fqbn_field] Part_1_03_Playing_Music
```

9. Plug headphones into the audio adaptor, music should be playing.
