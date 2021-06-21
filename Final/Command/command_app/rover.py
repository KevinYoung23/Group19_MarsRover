# import sleep

board = [
    ['1-1', '1-2', '1-3', '1-4', '1-5'],
    ['1-1', '1-2', '1-3', '1-4', '1-5'],
    ['1-1', '1-2', '1-3', '1-4', '1-5'],
    ['1-1', '1-2', '1-3', '1-4', '1-5'],
]


def run(n, speed):
    distance = 0

    for i in range(n):
        distance += speed
    return distance

d = run(5, 5)
print(d)
