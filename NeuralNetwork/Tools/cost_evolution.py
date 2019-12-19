import matplotlib.pyplot as plt
import sys

if len(sys.argv) < 2:
    raise Exception("You must provide the costs list file path")

for path in sys.argv[1:]:
    file = open(path, "r")
    data = []
    for number in file.read().split('\n'):
        if number != None and number != '':
            data.append(float(number))
    file.close()

    plt.plot(data)
    plt.axis([0, len(data), 0, 0.5])

plt.show()
