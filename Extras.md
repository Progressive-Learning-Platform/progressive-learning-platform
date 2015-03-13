# Additional Hardware Support #

PLP supports additional hardware not considered to be a part of the "vanilla" PLP system. For consistency and compatibility, this support is left out of the main PLP build. The supported hardware is mainly for Digilent PMOD devices and internal hardware.

Some additional hardware support conflicts with other additional hardware or hardware in the main PLP build. See below for more information.

# Using Additional Hardware #

All additional hardware components are available as patches to the main PLP source, and are available in the repository under the `extras` directory. To add support, clone the repository and patch the main source with patch:
```
hg clone https://progressive-learning-platform.googlecode.com/hg plp
cd plp
patch -p1 < extras/MY_PATCH.patch
```

# Additional Hardware Support #

## Digilent PMOD i2s Audio Module ##

Available in `extras/pmod_audio`, this patch adds support for the Digilent PMOD i2s audio module.

| Hardware connection | PMOD port C, top row |
|:--------------------|:---------------------|
| Control registers | 0xf0b00000 |
| Buffer | 0xf0b10000 - 0xf0b10400 |
| Audio channels | 2 |
| Bits per channel | 8 |
| Sampling frequency | 11025 kHz |
| Buffer depth | 512 samples |

Data is packed with two full samples in each data word as:
| MSB | | | LSB |
|:----|:|:|:----|
| sample A / left channel | sample A / right channel | sample A+1 / left channel | sample A+1 / right channel |

The control interface uses a single memory mapped register and a buffer. When the control register read, it returns the internal buffer pointer (0-1020). Each slot uses a fully packed data word as shown above. Using the internal buffer position and a software maintained buffer position, one can refill the buffer with new data.