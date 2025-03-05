import numpy as np
from PIL import Image
import draw


LENGHT = 100
N_BEAM = 13
img_mat = np.zeros((LENGHT*2, LENGHT*2, 3), dtype=np.uint8)
img_mat[:] = [30, 25, 51]


for i in range(N_BEAM):
    x0 = y0 = LENGHT
    x1 = int(LENGHT + 95*np.cos(i*2*np.pi / N_BEAM))
    y1 = int(LENGHT + 95*np.sin(i*2*np.pi / N_BEAM))
    draw.bresanham(img_mat, [x0, y0], [x1, y1], [0, 167, 225])


img = Image.fromarray(img_mat, mode='RGB')
img.save('images\\asterisk.png')
