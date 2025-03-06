import numpy as np
from PIL import Image, ImageOps
from lr1.draw import bresanham
from lr1.parcer import v, f


SIZE = 2000
RATIO = 10_000
color = [0, 167, 225]
bg = [30, 25, 51]
img_mat = np.zeros((SIZE, SIZE, 3), dtype=np.uint8)
img_mat[:] = bg


x = y = 0
for i in range(len(f)):
    points = []
    for j in range(3):
        x = int(v[f[i][j] - 1][0] * RATIO + 500)
        y = int(v[f[i][j] - 1][1] * RATIO + 1000)
        points.append([x, y])
    bresanham(img_mat, points[0], points[1], color)
    bresanham(img_mat, points[0], points[2], color)
    bresanham(img_mat, points[1], points[2], color)

img = Image.fromarray(img_mat[976:,:1024], mode='RGB')
img = ImageOps.flip(img)
img.save('images\\linear_bunny.png')
