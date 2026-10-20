count = 0
flag = False

def inc():
    global count
    global flag

    while flag:
        pass

    flag = True
    temp = count
    temp += 1  # temp = temp + 1
    count = temp
    flag = False

inc() // hilo 1
inc() // hilo 2

h1 count = 0 -> h1.temp = 0 -> h1.temp = 1
h2 count = 0 -> h2.temp = 0
h1 h1.temp = count = 1
h2 h2.temp = 1 -> h2.temp = count = 1

count = 1

h1 count = 0 -> h1.temp = 0 -> h1.temp = 1 -> h1.temp = count = 1
h2 count = 1 -> h2.temp = 1 -> h2.temp = 2 -> h2.temp = count = 2

count = 2