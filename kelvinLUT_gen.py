import csv
import sys
import numpy as np
import matplotlib.pyplot as plt

def load_cie_data(filename):
  wavelengths = []
  x_bar = []
  y_bar = []
  z_bar = []
  with open(filename, newline='') as f:
    reader = csv.reader(f)
    for row in reader:
      wavelengths.append(int(row[0]))
      x_bar.append(float(row[1]))
      y_bar.append(float(row[2]))
      z_bar.append(float(row[3]))
  
  return np.array(wavelengths), np.array(x_bar), np.array(y_bar), np.array(z_bar)

def filter_cie_data(wavelenghts, x_bar, y_bar, z_bar):
  mask = (wavelenghts >= 380) & (wavelenghts <= 700) & (wavelenghts % 5 == 0)
  return wavelenghts[mask], x_bar[mask], y_bar[mask], z_bar[mask]

def planck(wl_m, T):
  h = 6.62607015e-34
  c = 2.99792458e8
  k = 1.380649e-23
  return (2*h*c**2) / (wl_m**5) / (np.exp((h*c)/(wl_m*k*T)) - 1)

def kelvin_to_xyz(T, wavelenghts, x_bar, y_bar, z_bar):
  wl_m = wavelenghts * 1e-9
  M = planck(wl_m, T)
  X = np.trapezoid(M * x_bar, wl_m)
  Y = np.trapezoid(M * y_bar, wl_m)
  Z = np.trapezoid(M * z_bar, wl_m)
  return X, Y, Z

def xyz_to_rgb(X, Y, Z):
  M_xyz_to_rgb = np.array([
    [ 3.2406,  -1.5372,  -0.4986],
    [-0.9689,   1.8758,   0.0415],
    [ 0.0557,  -0.2040,   1.0570]
  ])
  rgb = np.dot(M_xyz_to_rgb, np.array([X, Y, Z]))
  rgb = np.clip(rgb, 0, None)

  max_val = max(rgb.max(), 1e-10)
  rgb = rgb / max_val

  gamma_correct = lambda u: 12.92*u if u <= 0.0031308 else 1.055*(u**(1/2.4)) - 0.055 
  
  rgb = np.array([gamma_correct(u) for u in rgb])
  rgb = np.clip(rgb*255, 0, 255).astype(int)
  return tuple(rgb)

CIE_PATH = "CIE_xyz_1931_2deg.csv"
FILE_PATH = "kelvinTableLUT.h"
T_min = 520
T_max = 10000
n_points = 256

def printLUT(temperatures, wavelenghts, x_bar, y_bar, z_bar) -> None:
  print(f"""
#ifndef _COLOR_TEMP_TABLE_H
#define _COLOR_TEMP_TABLE_H

#include <stdint.h>
        
#ifdef __AVR__
#include <avr/pgmspace.h>
#define TO_FLASH __attribute__((progmem))
#else
#define TO_FLASH 
#endif

constexpr uint16_t _COLOR_TEMP_MIN = {T_min};
constexpr uint16_t _COLOR_TEMP_MAX = {T_max};
constexpr uint16_t _COLOR_TEMP_TABLE_SIZE = {n_points};

""")
  print("const uint8_t TO_FLASH __COLOR_TEMP_TABLE[][3] = {\n  ", end='')
  for idx, T in enumerate(temperatures):
    X, Y, Z = kelvin_to_xyz(T, wavelenghts, x_bar, y_bar, z_bar)
    r, g, b = xyz_to_rgb(X, Y, Z)
    print(f"{{ {r:3}, {g:3}, {b:3} }}, /*c={idx:3}, T={T:5}*/", end=" ")
    if idx % 3 == 2:
      print()
      print(end='  ')
  print("\n};\n")
  print("#endif")

def generateLUTfile(temperatures, wavelenghts, x_bar, y_bar, z_bar) -> None:
  with open(FILE_PATH, "w") as f:
    sys.stdout = f
    printLUT(temperatures, wavelenghts, x_bar, y_bar, z_bar)

def plot(temperatures, wavelenghts, x_bar, y_bar, z_bar):
  rgb = np.zeros((n_points, 3))
  _, axes = plt.subplots(2, 1, figsize=(12,6), gridspec_kw={'height_ratios':[3,1]}, sharex=True)
  for idx, T in enumerate(temperatures):
    X, Y, Z = kelvin_to_xyz(T, wavelenghts, x_bar, y_bar, z_bar)
    rgb[idx][0], rgb[idx][1], rgb[idx][2] = xyz_to_rgb(X, Y, Z)
  axes[0].scatter(temperatures, rgb[:,0], c='red', label='R', s=10)
  axes[0].scatter(temperatures, rgb[:,1], c='green', label='G', s=10)
  axes[0].scatter(temperatures, rgb[:,2], c='blue', label='B', s=10)
  axes[0].set_ylabel("RGB values")
  axes[0].set_title("RGB depending on Kelvin temperature")
  tile_normalized = rgb.astype(float) / 255.0
  img = np.tile(tile_normalized[np.newaxis, :, :], (50, 1, 1))
  axes[1].imshow(img, aspect='auto', extent=[temperatures.min(), temperatures.max(), 0, 1])
  axes[1].set_yticks([])
  axes[1].set_xlabel("Temperature [K]")
  axes[1].set_title("Color gradient")

  plt.tight_layout()
  plt.show()
  
    



if __name__ == '__main__':
  wavelenghts, x_bar, y_bar, z_bar = load_cie_data(CIE_PATH)
  wavelenghts, x_bar, y_bar, z_bar = filter_cie_data(wavelenghts, x_bar, y_bar, z_bar)
  temperatures = np.linspace(T_min, T_max, n_points)
  temperatures = temperatures.astype(int)
  plot(temperatures, wavelenghts, x_bar, y_bar, z_bar)
  generateLUTfile(temperatures, wavelenghts, x_bar, y_bar, z_bar)