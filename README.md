# Invader

Space Invader Clone for the 32Blit Handheld.

## Usage

For local build:
```
mkdir build
cd build
cmake -D32BLIT_DIR=/path/to/32blit-sdk/ ..
make
```

For 32Blit build:
```
mkdir build.stm32
cd build.stm32
cmake .. -D32BLIT_DIR="/path/to/32blit-sdk" -DCMAKE_TOOLCHAIN_FILE=/path/to/32blit-sdk/32blit.toolchain
make
```
