# test MicroPython json behaviour that's not valid in CPy
# CIRCUITPY-CHANGE: This behavior matches CPython
print("SKIP")
raise SystemExit

print(json.dumps(b"1234"))
