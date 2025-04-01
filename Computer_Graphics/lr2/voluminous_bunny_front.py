import numpy as np
from PIL import Image, ImageOps
from lr1.parcer import v, f
from voluminous_bunny_behind import barycentric_coordinates


SIZE = 2000
RATIO = 10_000
img_mat = np.zeros((SIZE, SIZE, 3), dtype=np.uint8)
z_buffer = np.zeros((SIZE, SIZE), dtype=np.float16)
z_buffer[:] = -np.inf


def normal(point0, point1, point2):
    (x0, y0, z0), (x1, y1, z1), (x2, y2, z2) = point0, point1, point2
    return np.cross(-np.array([x1-x2, y1-y2, z1-z2]), np.array([x1-x0, y1-y0, z1-z0]))


def calc_light(point0, point1, point2):
    l = np.array([0, 0, 1])
    n = normal(point0, point1, point2)
    norm_n = np.linalg.norm(n)
    if norm_n < 1e-6:
        return 0
    return np.dot(n, l) / (norm_n * np.linalg.norm(l))


def draw_triangle(point0, point1, point2, image):
    (x0, y0, z0), (x1, y1, z1), (x2, y2, z2) = point0, point1, point2

    xmin = int(np.floor(max(min(x0, x1, x2), 0)))
    ymin = int(np.floor(max(min(y0, y1, y2), 0)))
    xmax = int(np.ceil(min(max(x0, x1, x2), SIZE)))
    ymax = int(np.ceil(min(max(y0, y1, y2), SIZE)))

    light = calc_light(point0, point1, point2)
    color = [-255 * light, -255 * light, -255 * light]

    if light >= 0:
        return

    for x in range(xmin, xmax+1):
        for y in range(ymin, ymax+1):
            l0, l1, l2 = barycentric_coordinates(point0, point1, point2, x, y)
            if all(l >= 0 for l in [l0, l1, l2]):
                z = l0*z0+l1*z1+l2*z2
                if z > z_buffer[x, y]:
                    image[int(round(y)), int(round(x))] = color
                    z_buffer[x, y] = z


x = y = z = 0
for i in range(len(f)):
    points = []
    for j in range(3):
        x = v[f[i][j] - 1][0] * RATIO + 500
        y = v[f[i][j] - 1][1] * RATIO + 1000
        z = v[f[i][j] - 1][2] * RATIO
        points.append([x, y, z])
    draw_triangle(points[0], points[1], points[2], img_mat)

img = Image.fromarray(img_mat[976:, :1024], mode='RGB')
img = ImageOps.flip(img)
img.save('images\\voluminous_bunny_front.png')
