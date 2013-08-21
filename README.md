ImageForensics
==============

This program is written in C++ and will read in a RAW image file that will perform MD5 and SHA1 checksums on the file before analysis. The program will then read the partition tables from the master boot record and extract the partition type, start sector address and the size of each partition in decimal. If the partition is of type FAT 16 or FAT 32, the program will locate the partitions volume boot record and extract the geometric data of the file system. This includes the start sector, end sector and size for the reserved area, sectors per cluster, start sector and ending sector of the FAT area, number of FATs, size of each FAT and the first sector of cluster 2.
