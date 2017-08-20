import sys
import struct
from PIL import Image
from bibliopixel.colors.gamma import APA102


def main(img_file, out_file):
    img = Image.open(img_file)
    img = img.convert(mode='RGB')
    w, h = img.size
    print(w, h)
    data = list(img.getdata())
    # rows = [data[x:x+w] for x in range(0, len(data), w)]
    flat_data = [APA102.get(c) for pixel in data for c in pixel]
    print('Writing to {}'.format(out_file))
    # print(flat_data)
    with open(out_file, 'wb') as out:
        out.write(struct.pack('<{}B'.format(w * h * 3), *flat_data))


if __name__ == '__main__':
    main(sys.argv[1], sys.argv[2])
