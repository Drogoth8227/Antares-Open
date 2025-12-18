from this folder use https://docs.zephyrproject.org/latest/develop/getting_started/index.html

use Antares folder for code, leave zephyrproject for build environment


youll also need jlink: https://www.segger.com/downloads/jlink/


device manager for com port of console

open putty, input com port, port 115200


helpful commands: 

`cd \GitHub\Antares-Open\Current_DEV\Zephyr`

`zephyrproject\.venv\Scripts\activate.bat`

`cd \GitHub\Antares-Open\Current_DEV\Zephyr\zephyrproject\zephyr`

`west build -p always -b decawave_dwm3001cdk \GitHub\Antares-Open\Current_DEV\Zephyr\Antares`

`west flash`
