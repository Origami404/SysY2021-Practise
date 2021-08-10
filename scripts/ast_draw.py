import re

def get_direct_info(filename: str, root: str) -> dict[str, str]:
    lines = []
    with open (filename, 'r') as f:
        lines = f.readlines()

    ret: dict[str, str] = {}

    struct_def_pattern = re.compile(r'typedef struct (.*?) {')
    super_pattern = re.compile(r'(.*?) super;')
    for idx, line in enumerate(lines):
        m = struct_def_pattern.match(line.strip())
        if not m: continue

        name = m[1]
        if name == root: continue

        m = super_pattern.match(lines[idx+1].strip())
        if not m: raise RuntimeError('Cannot find super')

        father = m[1]

        ret[name] = father

    return ret


if __name__ == '__main__':
    import sys
    if len(sys.argv) != 3:
        print('Usage: {sys.argv[0]} <ast.h path> <root_ast_node_name>')
        exit(-1)
    root = sys.argv[2]
    fa = get_direct_info(sys.argv[1], root)

    sons: dict[str, list[str]] = {}
    for s, f in fa.items():
        if f not in sons:
            sons[f] = []
        sons[f].append(s)
    
    print(sons)
    
    def print_tree(now: str, tab: int=0):
        print(' '*4*tab + now)
        if now not in sons:
            return
        else:
            for s in sons[now]:
                print_tree(s, tab + 1)
    print_tree(root)
