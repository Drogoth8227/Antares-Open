use stm32f4xx_hal::{
    delay::Delay,
    gpio::{Alternate, AF5, AF6, Output, PushPull},
    pac::{self, SPI1, SPI2},
    prelude::*,
    spi::{Mode, Phase, Polarity, Spi},
};
use dwm1000::{
    mac,
    spidev::{self, SpidevTransfer},
};

fn main() {
    // Initialize peripherals
    let cp = cortex_m::Peripherals::take().unwrap();
    let dp = pac::Peripherals::take().unwrap();
    let mut rcc = dp.RCC.constrain();
    let mut pwr = dp.PWR.constrain(&mut rcc.apb1r1);
    let mut gpioa = dp.GPIOA.split(&mut rcc.ahb1);
    let mut gpiob = dp.GPIOB.split(&mut rcc.ahb1);
    let mut gpioc = dp.GPIOC.split(&mut rcc.ahb1);

    // Configure delay
    let mut delay = Delay::new(cp.SYST, &rcc);

    // Configure SPI1 for ICM42688
    let spi1_pins = (
        gpioa.pa5.into_alternate_af5(), // SCK
        gpioa.pa6.into_alternate_af5(), // MISO
        gpioa.pa7.into_alternate_af5(), // MOSI
    );
    let spi1_mode = Mode {
        polarity: Polarity::IdleLow,
        phase: Phase::CaptureOnFirstTransition,
    };
    let spi1 = Spi::spi1(
        dp.SPI1,
        spi1_pins,
        spi1_mode,
        1_000_000.hz(),
        &mut rcc.apb2,
    );
    let mut icm_cs = gpioa.pa4.into_push_pull_output();
    icm_cs.set_high().unwrap();

    // Configure SPI2 for DWM3000
    let spi2_pins = (
        gpiob.pb13.into_alternate_af5(), // SCK
        gpiob.pb14.into_alternate_af5(), // MISO
        gpiob.pb15.into_alternate_af5(), // MOSI
    );
    let spi2_mode = Mode {
        polarity: Polarity::IdleLow,
        phase: Phase::CaptureOnFirstTransition,
    };
    let spi2 = Spi::spi2(
        dp.SPI2,
        spi2_pins,
        spi2_mode,
        1_000_000.hz(),
        &mut rcc.apb1,
    );
    let mut dwm_cs = gpioc.pc2.into_push_pull_output();
    dwm_cs.set_high().unwrap();

    // Create DWM3000 SPI device
    let mut dwm_spidev = spidev::Spidev::new(spi2, dwm_cs);

    loop {
        // Read accelerometer x-axis data from ICM42688
        let mut buf = [0; 6];
        icm_cs.set_low().unwrap();
        spi1.transfer(&mut buf).unwrap();
        icm_cs.set_high().unwrap();
        let acc_x = i16::from_be_bytes([buf[0], buf[1]]) as f32 * 0.244;

        // Send data over DWM3000
        let msg = mac::Message::new(0xFFFF, format!("Acc X: {:.2} m/s^2".as_bytes())).unwrap();

        let mut tx_buf = [0; 256];
        let tx_data_len = msg.write_to(&mut tx_buf).unwrap();
        dwm_cs.set_low().unwrap();
        dwm_spidev.transfer(&tx_buf[..tx_data_len]).unwrap();
        dwm_cs.set_high().unwrap();

        // Wait for a short time before sending the next message
        delay.delay_ms(100u32);
    }
}