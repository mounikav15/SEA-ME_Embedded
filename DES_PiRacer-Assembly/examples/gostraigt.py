# Copyright (C) 2022 twyleg
import time
from piracer.vehicles import PiRacerStandard, PiRacerPro


if __name__ == '__main__':

    # piracer = PiRacerPro()
    piracer = PiRacerStandard()

    while(True):
        piracer.set_throttle_percent(0.5)
        time.sleep(2.0)



