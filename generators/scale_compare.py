import numpy as np
import matplotlib.pyplot as plt
from typing import Callable

def generate_diff_matrix(fun: Callable[[int, int], int]) -> np.ndarray:
  """Generates a matrix of differences between exact and fast scaling."""
  diff_matrix = np.zeros((256, 256), dtype=np.int8)
  for c in range(256):
    for b in range(256):
      exact = (c * b) // 255
      fast_scaled = fun(c, b)
      diff_matrix[b, c] = fast_scaled - exact
  return diff_matrix

def plot_diff_heatmap(diff_matrix, axe, title, vmin, vmax):
  """Plots the difference matrix as a heatmap."""
  im = axe.imshow(diff_matrix, cmap='plasma', origin='lower',
                  extent=(0, 255, 0, 255),
                  interpolation='nearest',
                  vmin=vmin, vmax=vmax)
  axe.set_title(title)
  axe.set_xlabel('Color')
  axe.set_ylabel('Bright')
  return im

def plot() -> None:
  fig = plt.figure(figsize=(13, 5))
  gs = fig.add_gridspec(2, 3, height_ratios=[1, 0.05], hspace=0.3)
  axes = [fig.add_subplot(gs[0, i]) for i in range(3)]

  funcs = [
      lambda c, b: (c * b) >> 8,
      lambda c, b: (c * b + 127) >> 8,
      lambda c, b: (c * (b + 1)) >> 8
  ]

  titles = [
      "(Color * Bright) >> 8\nScaleMethod::CxB",
      "(Color * Bright + 127) >> 8\nScaleMethod::CxBplus127",
      "(Color * (Bright + 1)) >> 8\nScaleMEthod::CxBplusC"
  ]

  diff_matrices = [generate_diff_matrix(f) for f in funcs]
  vmin = min(diff.min() for diff in diff_matrices)
  vmax = max(diff.max() for diff in diff_matrices)

  ims = [plot_diff_heatmap(diff, ax, title, vmin, vmax) for ax, diff, title in zip(axes, diff_matrices, titles)]

  cbar_ax = fig.add_subplot(gs[1, :])
  cbar = fig.colorbar(ims[0], cax=cbar_ax, orientation='horizontal')
  cbar.set_label('Fast - Exact')

  plt.show()


if __name__ == "__main__":
  plot()