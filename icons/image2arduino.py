#!/usr/bin/env python3

from PIL import Image, ImageOps, ImageColor
import sys
import argparse
import os

parser = argparse.ArgumentParser(description='Convert images to C++ Arduino arrays')
parser.add_argument('image', help='image filename', type=argparse.FileType('rb'))
parser.add_argument('--resize', '-r', nargs=2, metavar=('width', 'height'), type=int, help='Resize image to given parameters')
parser.add_argument('--output', '-o', type=str, help='Variable name for c++ array output')
parser.add_argument('--prefix', '-p', type=str, help='Variable name prefix for c++ array output')
parser.add_argument('--white-threshold', '-w', type=int, default=127, help='White level threshold (default: 127)')
parser.add_argument('--background-colour', '-b', type=str, default='#000000', help='Colour background for alpha channel replacement (default: black, #000000)')
parser.add_argument('--invert', '-i', action='store_true', help='Invert image (default: false)')
#parser.add_argument('--resize-width', '-w', help='resize image to')
args = parser.parse_args()

im = Image.open(args.image)

output_variable = args.output
if not output_variable:
    output_variable = os.path.splitext(os.path.basename(args.image.name))[0]

image_size = im.size
if args.resize:
    image_size = args.resize

if image_size[0] % 8 != 0 or image_size[1] % 8 != 0:
    raise RuntimeError('Image size {}x{} not compatible (need to be multiple of 8).'.format(image_size[0], image_size[1]))

bg_colour = ImageColor.getrgb(args.background_colour)
bg = Image.new("RGB", im.size, bg_colour)
bg.paste(im, mask=im.split()[3])

im = bg.convert('L')

if args.invert:
    im = ImageOps.invert(im)

if args.resize:
    im = im.resize(args.resize, Image.ANTIALIAS)

im_to_ba = []

for index, pixel in enumerate(im.getdata()):
    ba_index = int(index/8)
    if index % 8 == 0:
        im_to_ba.append(0)
#    print('array: {}, index: {}, index_mod: {}'.format(im_to_ba, index, ba_index))
    im_to_ba[ba_index] = (im_to_ba[ba_index] << 1) + (1 if pixel >= args.white_threshold else 0)

variable_prefix = args.prefix if args.prefix is not None else ''
sys.stdout.write('const unsigned char {}{} [] PROGMEM = {{\n    '.format(variable_prefix, output_variable))
for index, i in enumerate(im_to_ba):
    sys.stdout.write('0x{:02x}, '.format(i))
sys.stdout.write('\n};\n')

    
