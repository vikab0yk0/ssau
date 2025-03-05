import numpy as np


def dotted_line(image, point0, point1, color):
    (x0, y0), (x1, y1) = point0, point1
    count = np.sqrt((x0-x1)**2 + (y0-y1)**2)
    step = 1.0 / count
    for i in np.arange(0, 1, step):
        x = round((1.0 - i) * x0 + i * x1)
        y = round((1.0 - i) * y0 + i * y1)
        image[y, x] = color


def x_loop_line(image, point0, point1, color):
    (x0, y0), (x1, y1) = point0, point1
    xchange = 0
    if abs(x0-x1) < abs(y0-y1):
        x0, y0 = y0, x0
        x1, y1 = y1, x1
        xchange = 1

    if x0 > x1:
        x0, x1 = x1, x0
        y0, y1 = y1, y0

    for x in range(x0, x1):
        t = (x-x0)/(x1-x0)
        y = round((1.0-t)*y0 + t*y1)
        image[x if xchange else y, y if xchange else x] = color


def bresanham(image, point0, point1, color):
    (x0, y0), (x1, y1) = point0, point1
    xchange = 0
    if abs(x0 - x1) < abs(y0 - y1):
        x0, y0 = y0, x0
        x1, y1 = y1, x1
        xchange = 1

    if x0 > x1:
        x0, x1 = x1, x0
        y0, y1 = y1, y0

    y = y0
    dy = 2 * abs(y1 - y0)
    derror = 0
    y_update = 1 if y0 < y1 else -1

    for x in range(x0, x1):
        image[x if xchange else y, y if xchange else x] = color
        derror += dy
        if derror > (x1 - x0):
            derror -= 2 * (x1 - x0)
            y += y_update
