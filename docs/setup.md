# Development Environment Setup

This project uses the
[PlatformIO](https://docs.platformio.org/en/latest/what-is-platformio.html) tool
for configuration and deployment. This is available as a VSCode extension, which
includes PlatformIO Core, and as a standalone PlatformIO CLI tool.

## IDE

### Install Software & Libraries

1. Clone this repository.
2. Install [Visual Studio Code](https://code.visualstudio.com/download).
3. In VSCode, open the package manager (_Extensions_).
4. Search for official `platformio ide` extension.
5. Install **PlatformIO IDE**.

### Deploy to the device

1. Connect the Teensy to the development computer via USB.
2. Open the project's `app` directory in VSCode.
3. VSCode will recognize the project as a PlatformIO project, and add a
   PlatformIO toolbar to the VSCode Status Bar.
4. In the PlatformIO toolbar, select the check mark icon (&check;) to build the
   application.
5. In the same toolbar, select the right arrow (&#10142;) to deploy the
   application to the Teensy board.

See the
[PlatformIO IDE docs](https://docs.platformio.org/en/latest/integration/ide/pioide.html)
for more details.

## CLI

### Install Software & Libraries

1. Clone this repository.
2. Install the PlatformIO Core CLI. The simplest way to do this is to use the
   recommended installer script, found at
   https://raw.githubusercontent.com/platformio/platformio-core-installer/master/get-platformio.py.
   This script will install all dependencies into a designated Python virtual
   environment. Copy or download the script, then navigate to the directory in
   which the script is located and run it with:

```
python get-platformio.py
```

3. Complete the OS-specific instructions found at
   [Install Shell Commands](https://docs.platformio.org/en/latest/core/installation/shell-commands.html).
4. If running a Linux OS, follow the instructions for
   [Installing `udev` Rules](https://docs.platformio.org/en/latest/core/installation/udev-rules.html)

See the
[PlatformIO Core (CLI) docs](https://docs.platformio.org/en/latest/core/index.html)
for more details.

### Deploy to the device

1. Connect the Teensy to the development computer via USB.
2. In the `app` directory of the project, execute the following command to
   compile and upload the application:

```
pio run -t upload
```

3. After the upload is complete, execute the following command to monitor the
   program's output:

```
pio device monitor
```
