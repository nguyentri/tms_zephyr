# Zephyr project folder cloned with `west init` and `west update`
# The Zephyr project folder should be in the same parent folder as this 3rd_parties directory

3rd_parties/
├── zephyr/
│   ├── zephyr # Zephyr kernel and modules
│   └── modules/
│       ├── lib/
│       │   ├── picolibc
│       │   └── cmsis-dsp
│       ├── hal/
│       │   ├── nxp
│       │   └── cmsis
│       └── debug
├── open-amp
├── mcuboot
├── mbedtls
├── libmetal
└── fatfs