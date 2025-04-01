import numpy as np
from PIL import Image, ImageOps


SIZE = 2000
img_mat = np.zeros((SIZE, SIZE, 3), dtype=np.uint8)
z_buffer = np.zeros((SIZE, SIZE), dtype=np.float16)
z_buffer[:] = np.inf
angles = [0, 45, 0]
shift = [0, 0, 1]

# Параметры проекции
ax = 8000
ay = 7000
u0 = v0 = SIZE//2


def barycentric_coordinates(point0, point1, point2, x, y):
    (x0, y0), (x1, y1), (x2, y2) = point0, point1, point2
    lambda0 = ((x - x2) * (y1 - y2) - (x1 - x2) * (y - y2)) / ((x0 - x2) * (y1 - y2) - (x1 - x2) * (y0 - y2))
    lambda1 = ((x0 - x2) * (y - y2) - (x - x2) * (y0 - y2)) / ((x0 - x2) * (y1 - y2) - (x1 - x2) * (y0 - y2))
    lambda2 = 1.0 - lambda0 - lambda1
    return lambda0, lambda1, lambda2


def normal(point0, point1, point2):
    (x0, y0, z0), (x1, y1, z1), (x2, y2, z2) = point0, point1, point2
    v1 = np.array([x1 - x0, y1 - y0, z1 - z0])
    v2 = np.array([x2 - x0, y2 - y0, z2 - z0])
    return np.cross(v1, v2)


def calc_light(point0, point1, point2):
    l = np.array([0, 0, 1])
    n = normal(point0, point1, point2)
    norm_n = np.linalg.norm(n)
    # if norm_n < 1e-6:
    #     return 0
    return np.dot(n, l) / (norm_n * np.linalg.norm(l))


def draw_triangle(point0, point1, point2, image):
    (x0, y0, z0), (x1, y1, z1), (x2, y2, z2) = point0, point1, point2

    # pxi/pyi – это спроецированные и сдвинутые в центр изображения
    # a = 10000
    # px = [x * a + 1000 for x in (x0, x1, x2)]
    # py = [y * a + 1000 for y in (y0, y1, y2)]

    # Проективное преобразование
    px = [
        ax * x0/z0 + u0,
        ax * x1/z1 + u0,
        ax * x2/z2 + u0
    ]
    py = [
        ay * y0/z0 + v0,
        ay * y1/z1 + v0,
        ay * y2/z2 + v0
    ]

    xmin, ymin = max(int(np.floor(min(px))), 0), max(int(np.floor(min(py))), 0)
    xmax, ymax = min(int(np.ceil(max(px))), SIZE-1), min(int(np.ceil(max(py))), SIZE-1)

    light = calc_light(point0, point1, point2)
    if light >= 0: return
    color = [-255 * light, -255 * light, -255 * light]

    for x in range(xmin, xmax + 1):
        for y in range(ymin, ymax + 1):
            l0, l1, l2 = barycentric_coordinates([px[0], py[0]], [px[1], py[1]], [px[2], py[2]], x, y)
            if all(l >= 0 for l in (l0, l1, l2)):
                z = l0 * z0 + l1 * z1 + l2 * z2
                if z < z_buffer[x, y]:
                    image[int(round(y)), int(round(x))] = color
                    z_buffer[x, y] = z


def twist(point, angles, shift):
    a, b, c = np.radians(angles)
    m1 = np.array([
        [1, 0, 0],
        [0, np.cos(a), np.sin(a)],
        [0, -np.sin(a), np.cos(a)]
    ])
    m2 = np.array([
        [np.cos(b), 0, np.sin(b)],
        [0, 1, 0],
        [-np.sin(b), 0, np.cos(b)]
    ])
    m3 = np.array([
        [np.cos(c), np.sin(c), 0],
        [-np.sin(c), np.cos(c), 0],
        [0, 0, 1],
    ])
    R = np.dot(np.dot(m1, m2), m3)
    return list(np.dot(R, np.array(point)) + np.array(shift))


with open('../model_1.obj') as file:
    v = []
    f = []
    for line in file:
        s = line.split()
        if s[0] == 'v':
            v.append([float(s[1]), float(s[2]), float(s[3])])
        if s[0] == 'f':
            f.append([int(s[1].split('/')[0]), int(s[2].split('/')[0]), int(s[3].split('/')[0])])

# Поворот всех точек однократно
for i in range(len(v)):
    x, y, z = v[i]
    v[i] = twist([x, y, z], angles, shift)

for i in range(len(f)):
    points = []
    for j in range(3):
        x, y, z = v[f[i][j] - 1]
        points.append([x, y, z])
    # Передаём в функцию 3D точки до масштабирования и сдвига в центр изображения
    draw_triangle(points[0], points[1], points[2], img_mat)

img = Image.fromarray(img_mat, mode='RGB')
img = ImageOps.flip(img)
img.save('../images/projective.png')
