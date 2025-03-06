import numpy as np
from PIL import Image, ImageOps
from lr1.parcer import v


SIZE = 2000
RATIO = 10_000
color = [0, 167, 225]
bg = [30, 25, 51]
img_mat = np.zeros((SIZE, SIZE, 3), dtype=np.uint8)
img_mat[:] = bg


for i in v:
    img_mat[round(i[1] * RATIO + 500), round(i[0] * RATIO + 1000)] = color

img = Image.fromarray(img_mat, mode='RGB')
img = ImageOps.flip(img)
img.save('images\\dotted_bunny.png')
