from random import randint

c_file = open('PG_AI/constant.h', 'w+')

constant = []
for i in range(8):
    line = []
    for j in range(8):
        line.append(randint(0, 5))
    constant.append(line)

data = f"const unsigned int priorities[{len(constant)}][{len(constant[0])}] = {{\n"
for i, line in enumerate(constant):
    data += "  {"
    for j, val in enumerate(line):
        data += str(val)
        if j < len(constant[0])-1:
            data += ', '
    data += '}'
    if i < len(constant)-1:
        data += ', '
    data += '\n'
data += '};\n'

c_file.write(data)
