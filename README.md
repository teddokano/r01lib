# r01lib

## What is this?

**"r01lib"** is a source code for **class libraries** to abstract MCUs and peripheral devices.  
This library provides application code simple APIs for.. **GPIO**, **I²C**, **I3C**, **SPI**, **Serial**, **Pin-interrupt** and **timer (ticker)**.   

This is a repository which works as a submodule of [**"r01lib_prj_generator"**](https://github.com/teddokano/r01lib_prj_generator).   
The code is intended to build in MCUXpresso library projects.   
Those libraries projects are..

|library name|MCU board|
|---|---|
|_r01lib_frdm_mcxa153|FRDM-MCXA153|
|_r01lib_frdm_mcxa156|FRDM-MCXA156|
|_r01lib_frdm_mcxc444|FRDM-MCXC444|
|_r01lib_frdm_mcxn236|FRDM-MCXN236|
|_r01lib_frdm_mcxn947|FRDM-MCXN947|

Each of these projects have MCU dependent driver code and settings for code building. The "r01lib" code is identical in all projects. Those can be just cpoied into the `source` folder and built.  

**"r01lib"** is including some device class drivers for easy device operations.  

|category|type|interface|
|---|---|---|
|AFE|NAFE33352|SPI|
|AFE|NAFE13388|SPI|
|LCD driver|PCA8561|I²C|
|LCD module|AE_AQM0802|I²C|
|LCD module|ACM2004D_FLW_FBW_IIC|I²C|
|LCD module|ACM1602NI_FLW_FBW|I²C|
|LED driver|PCA9955B|I²C|
|LED driver|PCA9956B|I²C|
|LED driver|PCA9957|SPI|
|GPIO expander|PCA9554|I²C|
|GPIO expander|PCA9555|I²C|
|GPIO expander|PCAL6408A|I²C|
|GPIO expander|PCAL6416A|I²C|
|GPIO expander|PCAL6524|I²C|
|GPIO expander|PCAL6534|I²C|
|GPIO expander|PCAL9722|SPI|
|RTC|PCF2131|I²C/SPI|
|RTC|PCF85053A|I²C|
|RTC|PCF85063A|I²C|
|RTC|PCF85063TP|I²C|
|RTC|PCF85263A|I²C|
|temperature sensor|LM75B|I²C|
|temperature sensor|P3T1035|I²C/I3C|
|temperature sensor|P3T1085|I²C/I3C|
|temperature sensor|P3T1755|I²C/I3C|
|temperature sensor|P3T2030|I²C/I3C|
|temperature sensor|PCT2075|I²C|
|I²C bus switch|PCA9846|I²C|
|EEPROM|M24C02|I²C|
|Potentiometer|AD5161|I²C|

## Shared with mcx-arduino-core

`r01lib/` is the same source as [mcx-arduino-core](https://github.com/teddokano/mcx-arduino-core)'s
`hardware/nxp/mcx/cores/arduino/r01lib/`, byte for byte, but for the SPI files' names: `spi.cpp`/`spi.h`
here, `r01lib_spi.cpp`/`r01lib_spi.h` there (Arduino's `SPI.h` has the same name on a file system that
ignores case). `r01lib.h` and `spi.cpp` pick the header with `__has_include`, and only Arduino builds
(`ARDUINO_ARCH_MCX`) leave `Ticker.h` out of `r01lib.h`. A change to one goes to the other.

Coming from the Arduino core in this branch, these are new since 0.42.0.

### Classes on more boards

| Class | Before | Now |
|---|---|---|
| `AnalogIn` | FRDM-MCXA153 only | all five boards |
| `PwmOut` | FRDM-MCXA153 only | all five boards |

The library projects need more SDK drivers for them: FRDM-MCXA156 `lpadc` and `pwm`, FRDM-MCXN947 and
FRDM-MCXN236 `lpadc` and `vref`, FRDM-MCXC444 `adc16` and `tpm` (in r01lib_prj_generator).

### New public member functions

| Class | Functions |
|---|---|
| `Serial` | `format( bits, parity, stop_bits )`, `end()`, `peek()`, `available()`, `availableForWrite()`, `flush()`, `apply_pin_mux()`, `static panic_write( const char* )` |
| `I2C` | `pin_low_timeout( timeout_us )`, `wait_bus_idle()`, `kinetis_base()` (FRDM-MCXC444), `lpi2c_base()` and `lpi2c_clock()` (MCX) |
| `SPI` | `bit_order( order )`, `transfer_byte( out )`, `clock_freq()` |
| `InterruptIn` | `change( callback )`, `low( callback )`, `disable()` |
| `PwmOut` | `frequency( hz )`, `write_ratio( num, den )`, `claim_pin()`, `release_pin()`, `static is_pwm_pin( pin )`, `static shares_running_period( pin )` |
| `DigitalInOut` | `input_buffer( enable )`, `gpio_base()`, `gpio_bit()` |

### New functions and types outside the classes

| Function or type | Header | What for |
|---|---|---|
| `flexcomm_keep_shared( instance )` | `mcu.h` | FRDM-MCXN947 and FRDM-MCXN236: one FLEXCOMM shared by a UART and an I2C |
| `pwm_tpm_service( now )` | `PwmOut.h` | FRDM-MCXC444 only: called every millisecond to write what waits for the TPM's buffered registers |
| `pin_registry_note()`, `pin_registry_forget()` | `pin_registry.h` | record which object holds which pins (weak; a debugging library can replace them) |
| `pin_registry_pin_name()`, `pin_registry_read_pcr()`, `PinPcrInfo` | `pin_registry.h` | a pin's name, and what its PCR (pin control register) holds |

### Hooks defined outside r01lib

| Function | Where | What for |
|---|---|---|
| `r01lib_micros()` | `PwmOut.cpp` (declared weak) | the time in microseconds for FRDM-MCXC444's `PwmOut`, which then times its writes to the TPM's buffered registers; without it, a write goes out at once |
| `mcx_sketch_wdt_event( event )` | `mcu.cpp` (declared weak) | tells a watchdog's user about `panic()` and a HardFault |
| `mcx_fault_report( frame, exc_return )` | `mcu.cpp` | called by `semihost_hardfault.c` for a fault other than semihosting, to report it on the serial port instead of hanging |

### Other changes

- GPIO interrupt handlers for every board in `irq.c` (FRDM-MCXA156's `GPIO4` and FRDM-MCXN236's were
  missing). The app templates' `_r01lib_irq/` copies have to match.
- FRDM-MCXA153 and FRDM-MCXA156 wait at start, before the application runs, while a debugger is
  attached (`DHCSR.C_DEBUGEN`), for 250ms at most: probe-rs's reset halts the core about 100ms after
  reset and loses the IT state as it lets it go ([probe-rs#4757](https://github.com/probe-rs/probe-rs/issues/4757)).
- Fixes in `I2C`, `I3C`, `SPI`, `Serial`, `InterruptIn` and `BusInOut`: pin mux values per pin pair,
  input buffers turned on, recovery from a NAK on STOP, and more.

## References

### Sample code
A sample code repositories are available for the **r01lib** demo: [**"r01lib_MCUXpresso"**](https://github.com/teddokano/r01lib_MCUXpresso).  
This is a repo generated by  **r01lib_prj_generator**.  

Memo: `git submodule update --remote` needed to be done in **r01lib_prj_generator** repository after this repository update. 

### API document
API information is available at [r01lib_prj_generator/docs/r01lib_docs](https://github.com/teddokano/r01lib_prj_generator/tree/main/docs/r01lib_docs)
These documents are generated from [`Doxyfile`](https://github.com/teddokano/r01lib/blob/main/Doxyfile) 

## History

Project which has been moved from [r01lib_old](https://github.com/teddokano/r01lib_old).  


