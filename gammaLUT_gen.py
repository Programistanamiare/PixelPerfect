import numpy as np
import sys
import matplotlib.pyplot as plt

GAMMA = 2.8
FILE_NAME = "gammaTableLUT.h"

def gammaCorrection(v: int) -> np.uint8:
  x = v / 255
  y = x ** (1 / GAMMA)
  return np.uint8(round(255 * y))

def printLUT() -> None:
  print("""
#ifndef _GAMMA_TABLE_LUT_H
#define _GAMMA_TABLE_LUT_H

#include <stdint.h>
#ifdef __AVR__
#define TO_FLASH __attribute__((progmem))
#else
#define TO_FLASH 
#endif
  """)
  print("const uint8_t TO_FLASH __GAMMA8_TABLE[256] = {\n  ", end='')
  for i in range(256):
    print(f"{gammaCorrection(i):3}", end=", ")
    if i % 16 == 15:
      print()
      if i != 255:
        print(end='  ')
  print("};\n")
  print("#endif")

def generateLUT() -> None:
  with open(FILE_NAME, "w") as f:
    sys.stdout = f
    printLUT();

def plot() -> None:
  gamma_values = np.fromiter((gammaCorrection(i) for i in range(256)), dtype=np.uint8)
  x = np.arange(0, 256, dtype=np.uint8)
  _, axes = plt.subplots(4, 1, figsize=(8, 6), sharex=True, gridspec_kw={'height_ratios': [3, 1, 1, 1]})
  axes[0].plot(x, gamma_values, label='Value')
  axes[0].set_title("Color value gamma corection")
  for i in range(3):
    arr = np.zeros((256, 3), dtype=np.uint8)
    arr[:, i] = gamma_values
    arr_norm = arr / 255.0
    img = np.tile(arr_norm[np.newaxis, :, :], (1, 1, 1))
    axes[i+1].imshow(img, aspect='auto')
    axes[i+1].axis('off')
  plt.tight_layout()
  plt.show()


if __name__ == "__main__":
  generateLUT()
  plot()
