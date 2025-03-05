with open('model_1.obj') as file:
    v = []
    f = []
    for line in file:
        s = line.split()
        if s[0] == 'v':
            v.append([float(s[1]), float(s[2]), float(s[3])])
        if s[0] == 'f':
            f.append([int(s[1].split('/')[0]), int(s[2].split('/')[0]), int(s[3].split('/')[0])])
