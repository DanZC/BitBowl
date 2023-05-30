import sys
import pystache

def bin_2_src(in_file:str, fname:str):
    with open(in_file, 'rb') as f:
        binary = f.read()

    bstr = ''
    for byte in binary:
        bstr += f' 0x{byte:02X},'
    
    ctx = {
        'name':fname, 
        'size':len(binary), 
        'bytes': bstr
    }
    
    with open('templates/bin_header.mustache') as f:
        header_temp = f.read()
    
    with open(f'src/data/{fname}.hpp', 'w') as f:
        f.write(pystache.render(header_temp, ctx))

    with open('templates/bin_src.mustache') as f:
        src_temp = f.read()
    
    with open(f'src/data/{fname}.cpp', 'w') as f:
        f.write(pystache.render(src_temp, ctx))

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("Usage: bin2src <in_file> <fname>")
    bin_2_src(sys.argv[1], sys.argv[2])