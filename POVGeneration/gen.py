#!/usr/bin/env python

import sys
import argparse
import json
from POVGenDriver import POVGen
from bibliopixel.drivers.SimPixel import SimPixel
from bibliopixel import Strip, log
from bibliopixel.project.importer import import_symbol

# log.set_log_level('debug')

NUM = 144


def main(anim_cls, fps, json_params, sim, num, args):
    params = {}
    if json_params:
        params = json.loads(json_params)

    if anim_cls.startswith('BPAS'):
        anim_cls = anim_cls.replace('BPAS', 'BiblioPixelAnimations.strip')

    if sim:
        driver = SimPixel(num=144)
        num = 0
    else:
        num = num - 1
        filename = './POV_Files/' + anim_cls.split('.')[::-1][0] + '.' + str(fps).zfill(3)
        cfg_filename = './POV_Files/' + anim_cls.split('.')[::-1][0] + '_' + str(fps) + '.json'
        if args:
            with open(cfg_filename, 'w') as cfg_json:
                json.dump(args, cfg_json)

        driver = POVGen(num=144, filename=filename)
        fps = None

    layout = Strip(driver)

    anim_obj = import_symbol(anim_cls)
    anim = anim_obj(layout, **params)

    anim.run(fps=fps, max_steps=num)
    driver.cleanup()


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Gen POVSaber Files')

    parser.add_argument('-f', action='store', default=90, type=int, help='FPS Value')
    parser.add_argument('-c', action='store', required=False, help='Animation class name')
    parser.add_argument('-j', action='store', default=None, help='JSON Params for Aniamtion')
    parser.add_argument('-s', action='store_true', default=False, help='Simulate')
    parser.add_argument('-n', action='store', type=int, default=NUM * 2 - 1, help='Number of frames to run')
    parser.add_argument('-a', action='store', default=None, help='Args from JSON')

    args = vars(parser.parse_args(sys.argv[1:]))

    from_file = False
    if args['a']:
        with open(args['a'], 'r') as arg_file:
            args = json.load(arg_file)
        from_file = True

    args.pop('a', None)
    main(args['c'], args['f'], args['j'], args['s'], args['n'], None if from_file else args)
