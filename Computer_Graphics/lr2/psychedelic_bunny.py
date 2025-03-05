import numpy as np
from PIL import Image, ImageOps
from random import randint
from parcer import v, f


SIZE = 2000
RATIO = 10_000
img_mat = np.zeros((SIZE, SIZE, 3), dtype=np.uint8)


def barycentric_coordinates(point0, point1, point2, x, y):
    (x0, y0, z0), (x1, y1, z1), (x2, y2, z2) = point0, point1, point2

    lambda0 = ((x - x2) * (y1 - y2) - (x1 - x2) * (y - y2)) / ((x0 - x2) * (y1 - y2) - (x1 - x2) * (y0 - y2))
    lambda1 = ((x0 - x2) * (y - y2) - (x - x2) * (y0 - y2)) / ((x0 - x2) * (y1 - y2) - (x1 - x2) * (y0 - y2))
    lambda2 = 1.0 - lambda0 - lambda1

    return lambda0, lambda1, lambda2


def rand_triangle(point0, point1, point2, image):
    (x0, y0, z0), (x1, y1, z1), (x2, y2, z2) = point0, point1, point2

    xmin = int(np.floor(max(min(x0, x1, x2), 0)))
    ymin = int(np.floor(max(min(y0, y1, y2), 0)))
    xmax = int(np.ceil(min(max(x0, x1, x2), SIZE)))
    ymax = int(np.ceil(min(max(y0, y1, y2), SIZE)))

    color = [randint(0, 255), randint(0, 255), randint(0, 255)]

    for x in range(xmin, xmax+1):
        for y in range(ymin, ymax+1):
            if all(l >= 0 for l in barycentric_coordinates(point0, point1, point2, x, y)):
                image[int(round(y)), int(round(x))] = color


x = y = z = 0
for i in range(len(f)):
    points = []
    for j in range(3):
        x = v[f[i][j] - 1][0] * RATIO + 500
        y = v[f[i][j] - 1][1] * RATIO + 1000
        z = v[f[i][j] - 1][2] * RATIO
        points.append([x, y, z])
    rand_triangle(points[0], points[1], points[2], img_mat)

img = Image.fromarray(img_mat[976:, :1024], mode='RGB')
img = ImageOps.flip(img)
img.save('images\\psychedelic_bunny.png')
