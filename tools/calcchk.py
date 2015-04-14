#!/usr/bin/python
from struct import pack, unpack

f = open("firm.hdr.dmp", "rb")
buf = f.read(0x100)
f.close()
chksm = 0
for i in xrange(0x100, 0, -4):
    w = unpack("<I", buf[i-4:i])
    chksm = (chksm + w[0]) & 0xFFFFFFFF
    #print "%08X\n" % w
print "checksum: 0x%08X" % chksm

