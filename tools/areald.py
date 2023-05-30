from dataclasses import dataclass
import json 
import sys
import pystache


def area_ld(infile: str, areaname: str):
    with open(infile, 'r') as f:
        j = json.load(f)

    lyr = j['layers'][0]
    w = lyr['width']
    h = lyr['height']

    area_tiles = bytearray()
    _mt = 0
    _oddtile = 0

    for mt in lyr['data']:
        mtid = mt - 1
        if mtid > 0xF:
            print("metatile greater than 15")
            exit(-1)
        _mt = (_mt << 4) | (mtid & 0xF)
        if _oddtile == 1:
            area_tiles.append(_mt)
            _mt = 0
        _oddtile ^= 1

    og = j['layers'][1]

    objects = []

    _objidx = 0
    for obj in og['objects']:
        if 'type' not in obj or obj['type'] not in ('pin', 'ball', 'person'):
            continue
        name = '' if 'name' not in obj else obj['name']
        x = obj['x']
        y = obj['y'] - obj['height']
        ty = obj['type']
        objects.append({'x': x, 'y': y, 'type': 'OBJ_TYPE_' + ty.upper(), 'name': name, 'idx': _objidx})
        _objidx += 1

    ctx = {}
    ctx['name'] = areaname
    ctx['width'] = w
    ctx['height'] = h
    area_data_str = ''
    for tl in area_tiles:
        area_data_str += f' 0x{tl:02X},'
    ctx['area_data_bytes'] = area_data_str
    ctx['area_data_length'] = len(area_tiles)

    ctx['objects'] = objects
    ctx['objectCount'] = len(objects)

    with open('templates/area_header.mustache') as f:
        header_temp = f.read()
    
    with open(f'src/data/areas/{areaname}.hpp', 'w') as f:
        f.write(pystache.render(header_temp, ctx))

    with open('templates/area_src.mustache') as f:
        src_temp = f.read()
    
    with open(f'src/data/areas/{areaname}.cpp', 'w') as f:
        f.write(pystache.render(src_temp, ctx))


if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("Usage: areald <in_file> <name>")
    area_ld(sys.argv[1], sys.argv[2])