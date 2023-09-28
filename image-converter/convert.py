# coding: utf-8
from PIL import Image
import sys

# 引数チェック
if len(sys.argv) < 2:
    print('Usage: python3 convert.py FILENAME.img')
    sys.exit(-1)

# 第1引数で指定されたファイルを読み込み
img = Image.open(sys.argv[1])
img = img.convert('RGB')
width, height = img.size

# ピクセル(R, G, Bのタプル)が白かどうか
def is_white(p):
    r, g, b = p
    if r == 255 and g == 255 and b == 255:
        return True

    return False

# 色があるピクセルを'*', それ以外は'_'で表現
# Cの配列として出力
for y in range(height):
    print('{ ', end='')
    for x in range(width):
        p = img.getpixel((x, y))
        is_white(p)
        print('*' if not is_white(p) else '_', end=', ' if x != width - 1 else '')
    print(' }')

print(f'{width}x{height}')
