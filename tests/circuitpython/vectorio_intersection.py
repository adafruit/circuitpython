import vectorio

args = (50, 50, 10, 50, 71, 10)
print(f"circle circle validate False: {args}")
print(vectorio.circle_circle_intersects(*args))

args = (50, 50, 10, 50, 70, 10)
print(f"circle circle validate True: {args}")
print(vectorio.circle_circle_intersects(*args))

args = (50, 50, 10, 50, 29, 10, 10)
print(f"circle rectangle validate False: {args}")
print(vectorio.circle_rectangle_intersects(*args))

args = (50, 50, 10, 50, 30, 10, 10)
print(f"circle rectangle validate True: {args}")
print(vectorio.circle_rectangle_intersects(*args))

args = (0, 0, 50, 50, 50, 60, 10)
padding = 0.1
print(f"line circle validate True: {args}, {padding}")
print(vectorio.line_circle_intersects(*args, padding=padding))

args = (0, 0, 50, 50, 50, 61, 10)
padding = 0.1
print(f"line circle validate False: {args}, {padding}")
print(vectorio.line_circle_intersects(*args, padding=padding))

args = ([(50, 0), (100, 0), (150, 50), (0, 50)], 0, 0, 161, 50, 10)
padding = 0.1
print(f"polygon circle validate False: {args}, {padding}")
print(vectorio.polygon_circle_intersects(*args, padding=padding))

args = ([(50, 0), (100, 0), (150, 50), (0, 50)], 0, 0, 160, 50, 10)
padding = 0.1
print(f"polygon circle validate True: {args}, {padding}")
print(vectorio.polygon_circle_intersects(*args, padding=padding))
