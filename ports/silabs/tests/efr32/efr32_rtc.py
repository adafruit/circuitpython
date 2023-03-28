import rtc
import time

r = rtc.RTC()

# Set date time
r.datetime = time.struct_time((2019, 5, 29, 15, 14, 15, 0, -1, -1))

# Get date time
current_time = r.datetime
print(current_time)
