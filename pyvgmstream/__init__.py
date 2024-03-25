import pyvgmstream.libpyvgmstream as libpyvgmstream

def convert(source: bytes, extension: str) -> bytes:
    return libpyvgmstream.convert(source, extension)
