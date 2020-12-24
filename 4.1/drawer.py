r = open('log.txt', 'r')


proc = ""

for line in r:
    s = line.split('\t')
    if s[0] != proc:
        proc = s[0]
        print("\n\n\033[30m"+proc, end=' ')
    if s[2] == "READ\n":
        k = 0
        while k < int(s[1]):
            print('\033[32m█', end='')
            k += 1
    if s[2] == "WAIT\n":
        k = 0
        while k < int(s[1]):
            print('\033[37m█', end='')
            k += 1
    if s[2] == "WRITE\n":
        k = 0
        while k < int(s[1]):
            print('\033[33m█', end='')
            k += 1

r.close()
print('\n\n\n')
print('\033[32m█████ READ\n'+'\033[37m█████ WAIT\n'+'\033[33m█████ WRITE\n')