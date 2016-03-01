from SCons.Script import DefaultEnvironment
env = DefaultEnvironment()
env.Replace(
    NEWUPLOADER="/home/ulno/bin/esptool-ulno",
    NEWUPLOADERFLAGS=[
        "--port", "$UPLOAD_PORT",
        "--baud", "$UPLOAD_SPEED",
        "write_flash", "0x00000",
    ],
    UPLOADCMD='$NEWUPLOADER $NEWUPLOADERFLAGS $SOURCE'
)
