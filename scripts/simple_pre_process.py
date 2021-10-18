
def remove_comment(lines: list[str]) -> list[str]:
    for i, line in enumerate(lines):
        idx = line.find('//')
        if idx > 0:
            lines[i] = line[:idx]
    return lines
    