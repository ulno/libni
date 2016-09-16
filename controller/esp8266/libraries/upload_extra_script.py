from SCons.Script import DefaultEnvironment
env = DefaultEnvironment()
env.Replace(
    NEWUPLOADER="/home/ulno/bin/esptool-ulno",
#    NEWUPLOADER="/home/ulno/.arduino15/packages/esp8266/tools/esptool/0.4.8/esptool",
    NEWUPLOADERFLAGS=[
        "--port", "$UPLOAD_PORT",
        "--baud", "$UPLOAD_SPEED",
        "write_flash", "0x00000",
    ],
#    NEWUPLOADERFLAGS=[
#        "-cp", "$UPLOAD_PORT",
#        "-cb", "$UPLOAD_SPEED",
#        "-ca", "0x00000",
#    ],
#    UPLOADCMD='$NEWUPLOADER -cd ck $NEWUPLOADERFLAGS -cf $SOURCE'
    UPLOADCMD='$NEWUPLOADER $NEWUPLOADERFLAGS $SOURCE'
)
