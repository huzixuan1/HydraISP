# HydraISP

**A High Performance Mobile ISP Framework and Kernel Development Platform**


## Overview

HydraISP is a cross-platform ISP (Image Signal Processing) framework designed for mobile imaging system development.

The project provides an extensible environment for:

* RAW Bayer image processing
* ISP pipeline simulation
* ISP kernel development
* CPU / SIMD / DSP / GPU acceleration research
* Mobile SoC backend integration

HydraISP follows modern mobile camera software architecture concepts by separating ISP algorithms from hardware acceleration platforms through unified kernel interfaces and backend abstraction.

---

# Goals

The long-term architecture:

```
                 ISP Pipeline

                       |
                       |

              Kernel Abstraction Layer

                       |

        +--------------+--------------+

        CPU          NEON          DSP

        Hexagon      OpenCL        MTK

```

Main goals:

* Hardware-independent ISP algorithms
* Multi-platform kernel implementation
* High-performance image processing
* ISP software simulation environment
* Mobile imaging SDK development

---

# Features

## ISP Pipeline

```
MIPI RAW10 / RAW12 / RAW14

            |
            v

          Unpack

            |
            v

           BLC

            |
            v

           DPC

            |
            v

           LSC

            |
            v

       White Balance

            |
            v

        Demosaic

            |
            v

            CCM

            |
            v

          Gamma

            |
            v

         Sharpen

            |
            v

          RGB/YUV

```

---

# Supported Formats

## RAW

Supported:

* MIPI RAW10
* RAW12
* RAW14
* Bayer Pattern:

  * RGGB
  * BGGR
  * GRBG
  * GBRG

## Internal Buffer

Unified formats:

* uint16 RAW buffer
* RGB buffer
* YUV buffer

---

# Build

## Requirements

* CMake >= 3.20
* C++17 Compiler

## Compile

```bash

mkdir build

cd build

cmake ..

cmake --build .

```

---

# Testbed

HydraISP provides an independent ISP simulation environment.

Capabilities:

* RAW loading
* Pipeline execution
* Output comparison
* Performance measurement

Example:

```bash

./testbed_runner \
    --config configs/camera/example.json \
    --input assets/raw/input.raw

./isp_testbed ../config/isp_config_cannon.json

./isp_testbed ../config/isp_config_dsc.json

```

---

# Applications

HydraISP can be used for:

* Mobile Camera ISP development
* Sensor tuning
* ISP algorithm research
* SoC kernel optimization
* AI vision preprocessing
* High performance image computing

---

# License

MIT License
