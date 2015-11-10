"""
RfxDevices
==========
@authors: Gabriele Manduchi (IGI Padova)
@copyright: 2008
@license: GNU GPL
=======
"""

from MDSplus import version
if version.ispy3:
    from .ACQIPPSETUP import ACQIPPSETUP
    from .CAENDT5720 import CAENDT5720
    from .CAENV1740 import CAENV1740
    from .CONTIPPSETUP import CONTIPPSETUP
    from .CYGNET4K import CYGNET4K
    # DIO
    from .DIO4 import DIO4
    from .DIO2_ENCDEC import DIO2_ENCDEC

    from .FAKECAMERA import FAKECAMERA
    from .FEMTO import FEMTO
    from .FLIRSC65X import FLIRSC65X
    from .MARTE_CONFIG import MARTE_CONFIG
    # MARTE_COMMON
    from .MARTE_COMMON import MARTE_COMMON
    from .MARTE_DEVICE import MARTE_DEVICE
    from .MARTE_RTSM import MARTE_RTSM
    # MARTE_GENERIC
    from .MARTE_GENERIC import MARTE_GENERIC
    from .MARTE import MARTE
    from .MARTE_BREAKDOWN import MARTE_BREAKDOWN
    from .MARTE_DEQU import MARTE_DEQU
    from .MARTE_EDA1 import MARTE_EDA1
    from .MARTE_EDA1_OUT import MARTE_EDA1_OUT
    from .MARTE_EDA3 import MARTE_EDA3
    from .MARTE_EDA3_OUT import MARTE_EDA3_OUT
    from .MARTE_MHD_AC_BC import MARTE_MHD_AC_BC
    from .MARTE_MHD_BR import MARTE_MHD_BR
    from .MARTE_MHD_BT import MARTE_MHD_BT
    from .MARTE_MHD_CTRL import MARTE_MHD_CTRL
    from .MARTE_MHD_I import MARTE_MHD_I
    from .MARTE_NE import MARTE_NE
    from .MARTE_WAVEGEN import MARTE_WAVEGEN
    from .MARTE_XRAY import MARTE_XRAY

    #from .MILL3 import MILL3
    from .NI6259AI import NI6259AI
    from .NI6368AI import NI6368AI
    from .NI6682 import NI6682
    from .REDPYTADC import REDPYTADC
    from .RFX_PROTECTIONS import RFX_PROTECTIONS
    from .RFXVICONTROL import RFXVICONTROL
    from .RFXWAVESETUP import RFXWAVESETUP
    from .SIS3820 import SIS3820
    from .SPIDER import SPIDER
    from .SPIDER_SM import SPIDER_SM
    from .ZELOS2150GV import ZELOS2150GV
else:
    from ACQIPPSETUP import ACQIPPSETUP
    from CAENDT5720 import CAENDT5720
    from CAENV1740 import CAENV1740
    from CONTIPPSETUP import CONTIPPSETUP
    from CYGNET4K import CYGNET4K
    # DIO
    from DIO4 import DIO4
    from DIO2_ENCDEC import DIO2_ENCDEC

    from FAKECAMERA import FAKECAMERA
    from FEMTO import FEMTO
    from FLIRSC65X import FLIRSC65X
    # MARTE_COMMON
    from MARTE_COMMON import MARTE_COMMON
    from MARTE_DEVICE import MARTE_DEVICE
    from MARTE_RTSM import MARTE_RTSM
    # MARTE_GENERIC
    from MARTE_GENERIC import MARTE_GENERIC
    from MARTE import MARTE
    from MARTE_BREAKDOWN import MARTE_BREAKDOWN
    from MARTE_CONFIG import MARTE_CONFIG
    from MARTE_DEQU import MARTE_DEQU
    from MARTE_EDA1 import MARTE_EDA1
    from MARTE_EDA1_OUT import MARTE_EDA1_OUT
    from MARTE_EDA3 import MARTE_EDA3
    from MARTE_EDA3_OUT import MARTE_EDA3_OUT
    from MARTE_MHD_AC_BC import MARTE_MHD_AC_BC
    from MARTE_MHD_BR import MARTE_MHD_BR
    from MARTE_MHD_BT import MARTE_MHD_BT
    from MARTE_MHD_CTRL import MARTE_MHD_CTRL
    from MARTE_MHD_I import MARTE_MHD_I
    from MARTE_NE import MARTE_NE
    from MARTE_WAVEGEN import MARTE_WAVEGEN
    from MARTE_XRAY import MARTE_XRAY

    #from MILL3 import MILL3
    from NI6259AI import NI6259AI
    from NI6368AI import NI6368AI
    from NI6682 import NI6682
    from REDPYTADC import REDPYTADC
    from RFX_PROTECTIONS import RFX_PROTECTIONS
    from RFXVICONTROL import RFXVICONTROL
    from RFXWAVESETUP import RFXWAVESETUP
    from SIS3820 import SIS3820
    from SPIDER import SPIDER
    from SPIDER_SM import SPIDER_SM
    from ZELOS2150GV import ZELOS2150GV