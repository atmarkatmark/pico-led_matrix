import machine
import time
import random

RED = 1 << 2
GREEN = 1 << 1
BLUE = 1
YELLOW = RED | GREEN
MEGENTA = RED | BLUE
CYAN = BLUE | GREEN
WHITE = RED | GREEN | BLUE
BLACK = 0

COLS = 64
ROWS = 32	# 16の倍数

machine.freq(240000000)

PIN_RGB = [machine.Pin(i, machine.Pin.OUT) for i in range(6)]
# PIN_R0 = PIN_RGB[0]
# PIN_G0 = PIN_RGB[1]
# PIN_B0 = PIN_RGB[2]
# PIN_R1 = PIN_RGB[3]
# PIN_G1 = PIN_RGB[4]
# PIN_B1 = PIN_RGB[5]
# PIN_RGB = [machine.Pin(i, machine.Pin.OUT) for i in range(6)]
# PIN_R0 = machine.Pin(0, machine.Pin.OUT)
# PIN_G0 = machine.Pin(1, machine.Pin.OUT)
# PIN_B0 = machine.Pin(2, machine.Pin.OUT)
# PIN_R1 = machine.Pin(3, machine.Pin.OUT)
# PIN_G1 = machine.Pin(4, machine.Pin.OUT)
# PIN_B1 = machine.Pin(5, machine.Pin.OUT)
PIN_E = machine.Pin(10, machine.Pin.OUT)
PIN_A = machine.Pin(6, machine.Pin.OUT)
PIN_B = machine.Pin(7, machine.Pin.OUT)
PIN_C = machine.Pin(8, machine.Pin.OUT)
PIN_D = machine.Pin(9, machine.Pin.OUT)
PIN_CLK = machine.Pin(11, machine.Pin.OUT)
PIN_LAT = machine.Pin(12, machine.Pin.OUT)
PIN_OE = machine.Pin(13, machine.Pin.OUT)

@micropython.native
def random_pix():
    return [[random.randint(BLACK, WHITE) for i in range(COLS)] for j in range(ROWS)]

d = [[1] + [0 for i in range(COLS - 2)] + [1] for i in range(ROWS)]
d[0] = d[ROWS - 1] = [1 for i in range(COLS)]
d[2] = [1 for i in range(COLS)]

def initialize():
    PIN_A.off()
    PIN_B.off()
    PIN_C.off()
    PIN_D.off()
    # PIN_R0.off()
    # PIN_G0.off()
    # PIN_B0.off()
    # PIN_R1.off()
    # PIN_G1.off()
    # PIN_B1.off()
    for i in range(6):
        PIN_RGB[0].off()
    PIN_LAT.off()
    PIN_CLK.off()
    PIN_OE.on()

initialize()

@micropython.native
def show():
    start = time.ticks_us()
    pin_rgb_ref = PIN_RGB
    pin_r0 = PIN_RGB[0]
    pin_g0 = PIN_RGB[1]
    pin_b0 = PIN_RGB[2]
    pin_r1 = PIN_RGB[3]
    pin_g1 = PIN_RGB[4]
    pin_b1 = PIN_RGB[5]
    pin_lat_ref = PIN_LAT
    pin_oe_ref = PIN_OE
    pin_clk_ref = PIN_CLK
    pin_a_ref = PIN_A
    pin_b_ref = PIN_B
    pin_c_ref = PIN_C
    pin_d_ref = PIN_D
    
    for r in range(16):        
        pin_lat_ref.on()
        for c in range(COLS):
            upper = d[r][c]
            lower = d[r + 16][c]
            pin_r0.value(upper & RED)
            pin_g0.value(upper & GREEN)
            pin_b0.value(upper & BLUE)
            pin_r1.value(lower & RED)
            pin_g1.value(lower & GREEN)
            pin_b1.value(lower & BLUE)
            
            pin_clk_ref.on()
            pin_clk_ref.off()
        
        pin_oe_ref.on()
        
        pin_a_ref.value(r & 1)
        pin_b_ref.value((r >> 1) & 1)
        pin_c_ref.value((r >> 2) & 1)
        pin_d_ref.value((r >> 3) & 1)
        # PIN_A.value(r & 1)
        # PIN_B.value((r & 2) >> 1)
        # PIN_C.value((r & 4) >> 2)
        # PIN_D.value((r & 8) >> 3)
        
        pin_lat_ref.off()
        pin_oe_ref.off()
    
    print(time.ticks_us() - start)

d = random_pix()
while True:
    show()
