from PIL import Image
from io import BytesIO
import numpy as np
import matplotlib.pyplot as plt

filename = '成.jpg'

# 画像ファイルパスから読み込み
img = Image.open(filename)

img2 = img.resize((64, 64))

img_array = np.array(img2.convert("L"))    # グレイスケールに変換
#img_array = np.array(img2)
#img_array -= img_array[0]

# img_array[img_array < 150] = 0
# img_array[img_array >= 150] = 1

plt.imshow(img_array)
plt.show()

img_array -= np.min(img_array)
img_array[img_array > 0] = 10
img_array[img_array == 0] = 1
img_array[img_array == 10] = 0

#print(img_array)

img_str = 'static const unsigned char PROGMEM logo[] = {\n'
for row in img_array:
    img_str += '\t'
    for i in range(int(len(row)/8)):
        img_str += 'B'
        img_str += ''.join([str(n) for n in row[i*8:(i+1)*8]])
        img_str += ', '
    img_str += '\n'
img_str += '};'

print(img_str)

# plt.imshow(img_array)
# plt.show()

# array = [
#     0b00000000, 0b00000000, 0b00000000, 0b00000000,
#     0b00000000, 0b01110000, 0b00000001, 0b10011000,
#     0b00000011, 0b11111100, 0b00000001, 0b11000000,
#     0b00001111, 0b11000000, 0b00011111, 0b11111100,
#     0b00111111, 0b00110000, 0b00011111, 0b11110000,
#     0b00000011, 0b10110000, 0b00011000, 0b11000000,
#     0b00000111, 0b11100000, 0b00011000, 0b01100100,
#     0b00011101, 0b10001111, 0b00111000, 0b01111100,
#     0b00001101, 0b10111100, 0b00111110, 0b00011000,
#     0b00000001, 0b11011000, 0b01100110, 0b00111000,
#     0b00000111, 0b11000000, 0b01100110, 0b00111000,
#     0b00011111, 0b10000000, 0b01000100, 0b01101100,
#     0b01111001, 0b10000000, 0b11001100, 0b11000000,
#     0b11100001, 0b10000000, 0b10000000, 0b00000000,
#     0b11000001, 0b10000000, 0b00000000, 0b00000000,
#     0b00000000, 0b10000000, 0b00000000, 0b00000000
# ]