import re

def get_nodes(lines: list[str]) -> list[str]:
    node_name = []

    enum_pattern = re.compile(r'AT_([a-zA-Z]*?)[, ]')
    for line in lines:
        ms = enum_pattern.findall(line)
        node_name.extend(ms)
    return node_name

def enum2struct(enum_name: str) -> str:
    cs = []
    for c in enum_name:
        if c.isupper():
            cs.append('_')
            cs.append(c.lower())
        else:
            cs.append(c)
    return ''.join(cs)[1:] # remove the leading '_'

def spilt_type_name(code: str) -> tuple[str, str]:
    l = code.split(' ')
    if len(l) == 3:
        return (l[1], l[2])
    elif len(l) == 2:
        return (l[0], l[1])
    else: raise RuntimeError(f'Cannot spilt {code}')

NFS = dict[str, dict[str, str]]
def get_node_fields(lines: list[str]) -> NFS:
    nfs = {}

    lines = list(map(lambda x: x.strip(), lines))
    for node_name in get_nodes(lines):
        end_line = f'}} {enum2struct(node_name)};'

        fields = {}
        if end_line in lines:
            idx = lines.index(end_line) - 1
            while (line := lines[idx]) != 'struct {':
                t, n = line[:-1].split(' ')
                fields[n] = t
                idx -= 1

        nfs[node_name] = { k: v for k, v in reversed(fields.items())}
    
    return nfs

def gen_cons_decl(node_name: str, fields: dict[str, str]) -> str:
    params = ', '.join([f'{t} {n}' for n, t in fields.items()])
    return f'Ast_Node ast_{node_name}({params})'
    

def gen_cons_def(node_name: str, fields: dict[str, str]) -> list[str]:
    struct_name = enum2struct(node_name)
    return [
        gen_cons_decl(node_name, fields) + ' {',
        f'    Ast_Node p = checked_malloc(sizeof(*p));',
        f'    ',
        f'    p->type = AT_{node_name};',
        *map(lambda n: f'    p->u.{struct_name}.{n} = {n};', fields.keys()),
        f'    ',
        f'    return p;',
        f'}}'
    ]


from itertools import starmap

def deal(ast_path: str, constructor_path: str):
    lines = []
    with open(ast_path, 'r') as f:
        lines = f.readlines()

    nfs = get_node_fields(lines)
    
    # print(lines)
    i = lines.index('//@ cons_header\n')
    m = re.compile('#include <(.*?)>').match(lines[i+1])
    if not m: raise RuntimeError('Do not spec cons_decl_path')
    decl_path = m[1]

    with open(decl_path, 'w') as f:
        f.write('// This file is automatically generate by ast_gen_constructor.py, do NOT modify it by hand\n\n')
        f.write(';\n'.join(starmap(gen_cons_decl, nfs.items())) + ';')
    
    with open(constructor_path, 'w') as f:
        f.write('// This file is automatically generate by ast_gen_constructor.py, do NOT modify it by hand\n\n')
        f.write('#include "util.h"\n')
        f.write(f'#include "{ast_path}"\n\n\n')
        f.write('\n\n'.join(map(lambda l: '\n'.join(l), starmap(gen_cons_def, nfs.items()))))
        
if __name__ == '__main__':
    import sys
    if len(sys.argv) != 3:
        print(f'Usage: {sys.argv[0]} <ast_path> <constructor_path>')
        exit(-1)
    deal(sys.argv[1], sys.argv[2])