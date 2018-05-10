Import('env')
#env.Replace(FUSESCMD="avrdude $UPLOADERFLAGS -e -U lfuse:w:0xa2:m -U hfuse:w:0xdf:m -U efuse:w:0xff:m")# - 1mhz
env.Replace(FUSESCMD="avrdude $UPLOADERFLAGS -e -U lfuse:w:0xe2:m -U hfuse:w:0xdf:m -U efuse:w:0xff:m") # - 8mhz
# env.Replace(FUSESCMD="avrdude $UPLOADERFLAGS -e -U lock:w:0xFF:m -U lfuse:w:0xa2:m -U hfuse:w:0xdf:m -U efuse:w:0xff:m")