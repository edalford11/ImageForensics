//
//  ImageForensics.cpp
//  ImageForensics
//
//  Created by Eric Alford on 3/12/13.
//  Copyright (c) 2013 Eric Alford. All rights reserved.
//

#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <hashlibpp.h>
#include "Partition.h"

using namespace std;

char *filePath;
Partition *part1, *part2, *part3, *part4;
unsigned char* buff;

void getMBR(){
    //Partition type ID's
    int sysId[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0E, 0x0F, 0x10,
        0x11, 0x12, 0x14, 0x16, 0x17, 0x18, 0x1B, 0x1C, 0x1E, 0x24, 0x2C, 0x39, 0x3C, 0x40, 0x41, 0x42, 0x4D, 0x4E, 0x4F,
        0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x5C, 0x61, 0x63, 0x64, 0x65, 0x66, 0x70, 0x75, 0x78, 0x80, 0x81, 0x82,
        0x83, 0x84, 0x85, 0x86, 0x87, 0x8E, 0x93, 0x94, 0x9F, 0xA0, 0xA5, 0xA6, 0xA7, 0xA9, 0xB7, 0xB8, 0xBB, 0xC1, 0xC4,
        0xC6, 0xC7, 0xDA, 0xDB, 0xDE, 0xDF, 0xE1, 0xE3, 0xE4, 0xEB, 0xEE, 0xEF, 0xF0, 0xF1, 0xF4, 0xF2, 0xFD, 0xFE, 0xFF};
    string sysNames[] = {"Empty", "FAT12", "XENIX root", "XENIX usr", "FAT16 <32MB", "Extended", "FAT16B (>= 32 MB)",   	 //Partition types names.
        "Installable File System (NTFS HPFS)", "AIX", "AIX bootable", "OS/2 Boot Manager", "Win95 FAT32", "Win95 FAT32 (LBA)",
        "Win95 FAT16 (LBA)", "Win95 Extended (LBA)", "OPUS", "Hidden FAT12", "Compaq diagnostics", "Hidden FAT16 <32MB",
        "Hidden FAT16", "Hidden IFS (HPFS/NTFS)", "AST SmartSleep", "Hidden Win95 FAT32", "Hidden Win95 FAT32 (LBA)",
        "Hidden Win95 FAT16 (LBA)", "NEC DOS", "WildFile/Adaptec GOBack", "Plan 9", "PowerQuest Recoverable Partition",
        "Venix 80286", "PPC PReP Boot", "Veritas Logical Disk Manager", "QNX4.x", "QNX4.x 2nd part", "QNX4.x 3rd part",
        "OnTrack DM", "OnTrack DM6 Aux1", "CP/M", "OnTrack DM6 Aux3", "OnTrackDM6", "EZ-Drive", "Golden Bow", "Priam Edisk",
        "SpeedStor", "GNU HURD or SysV", "Novell Netware 286", "Novell Netware (3.11 and 4.1)", "Novell Netware 386",
        "DiskSecure Multi-Boot", "PC/IX", "XOSL", "Old Minix", "Linux/Minix v1.4b+", "Linux swap / Solaris",
        "Linux native file system (Ext2/3)", "OS/2 hiding type 04h partition", "Linux extended", "NT FAT volume set",
        "NT IFS volume set", "Linux LVM", "Amoeba/Hidden Linux native file system (Ext2/3)", "Amoeba BBT", "BSD/OS",
        "IBM Thinkpad hibernation", "FreeBSD", "OpenBSD", "NeXTSTEP", "NetBSD", "BSDI fs", "BSDI swap", "Boot Wizard hidden",
        "DRDOS / sec (FAT-12)", "DRDOS / sec (FAT-16 < 32M)", "Disabled NT FAT (FAT-16) volume set/DRDOS",
        "Syrinx / Disabled NT IFS volume set", "Non-FS data", "CP/M / CTOS / ...", "Dell Corporation diagnostic partition",
        "BootIt", "DOS access", "DOS R/O", "SpeedStor", "BeOS fs", "EFI GPT", "EFI (FAT-12/16/32)", "Linux/PA-RISC boot",
        "SpeedStor", "SpeedStor", "DOS secondary", "Linux raid autodetect", "LANstep", "Bad Track Table"};
    
    FILE *f;
    f = fopen(filePath, "r");
    if (f==NULL) {fputs ("File error\n",stderr); exit (1);}
    
    fseek (f , 0 , SEEK_END);
    long lSize = ftell(f);
    rewind (f);
    
    buff = (unsigned char*) malloc (sizeof(char)*lSize);
    if (buff == NULL) {fputs ("Memory error\n",stderr); exit (2);}
    
    size_t result = fread (buff,1,lSize,f);
    if (result != lSize) {fputs ("Reading error\n",stderr); exit (3);}
    fclose (f);
    
    
    /*for(int i = 446; i < 512; i++){
        if(i==462 || i==478 || i==494 || i== 510)
            cout << endl;
        cout << hex << (unsigned)buff[i] << " ";
    }
    cout << endl;
    for(int i = 32256; i<32768; i++){
     cout << hex << (unsigned)buff[i] << " ";
     }
     cout << endl;*/
    
    long startSectorAddress = (unsigned int)buff[457] << 24 | (unsigned int)buff[456]<<16 | (unsigned int)buff[455] << 8 | (unsigned int)buff[454];
    int partitionId = buff[450];
    long partitionSize = (unsigned int)buff[461] << 24 | (unsigned int)buff[460]<<16 | (unsigned int)buff[459]<<8 | (unsigned int)buff[458];
    string partitionType = "Not Found";
    for(int i = 0; i<93; i++){
        if(sysId[i] == partitionId){
            partitionType = sysNames[i];
            break;
        }
    }
    part1 = new Partition(partitionType, partitionSize, startSectorAddress);
    
    partitionType = "Not Found";
    startSectorAddress = (unsigned int)buff[473] << 24 | (unsigned int)buff[472]<<16 | (unsigned int)buff[471] << 8 | (unsigned int)buff[470];
    partitionId = buff[466];
    partitionSize = (unsigned int)buff[477] << 24 | (unsigned int)buff[476]<<16 | (unsigned int)buff[475]<<8 | (unsigned int)buff[474];
    for(int i = 0; i<93; i++){
        if(sysId[i] == partitionId){
            partitionType = sysNames[i];
            break;
        }
    }
    part2 = new Partition(partitionType, partitionSize, startSectorAddress);
    
    partitionType = "Not Found";
    startSectorAddress = (unsigned int)buff[489] << 24 | (unsigned int)buff[488]<<16 | (unsigned int)buff[487] << 8 | (unsigned int)buff[486];
    partitionId = buff[482];
    partitionSize = (unsigned int)buff[493] << 24 | (unsigned int)buff[492]<<16 | (unsigned int)buff[491]<<8 | (unsigned int)buff[490];
    for(int i = 0; i<93; i++){
        if(sysId[i] == partitionId){
            partitionType = sysNames[i];
            break;
        }
    }
    part3 = new Partition(partitionType, partitionSize, startSectorAddress);
    
    partitionType = "Not Found";
    startSectorAddress = (unsigned int)buff[505] << 24 | (unsigned int)buff[504]<<16 | (unsigned int)buff[503] << 8 | (unsigned int)buff[502];
    partitionId = buff[498];
    partitionSize = (unsigned int)buff[509] << 24 | (unsigned int)buff[508]<<16 | (unsigned int)buff[507]<<8 | (unsigned int)buff[506];
    for(int i = 0; i<93; i++){
        if(sysId[i] == partitionId){
            partitionType = sysNames[i];
            break;
        }
    }
    part4 = new Partition(partitionType, partitionSize, startSectorAddress);
    
    cout << "(01) " << part1->partitionType << ", " << dec << part1->startSectorAddress << ", " << part1->partitionSize << endl;
    cout << "(02) " << part2->partitionType << ", " << dec << part2->startSectorAddress << ", " << part2->partitionSize << endl;
    cout << "(03) " << part3->partitionType << ", " << dec << part3->startSectorAddress << ", " << part3->partitionSize << endl;
    cout << "(04) " << part4->partitionType << ", " << dec << part4->startSectorAddress << ", " << part4->partitionSize << endl;
    cout << endl;
}

void getVBR(){
    if(part1->partitionType=="FAT16 <32MB" || part1->partitionType=="FAT16B (>= 32 MB)" || part1->partitionType=="Win95 FAT32" || part1->partitionType=="Win95 FAT32 (LBA)" || part1->partitionType=="Win95 FAT16 (LBA)" || part1->partitionType=="Hidden FAT16 <32MB" || part1->partitionType=="Hidden Win95 FAT32" || part1->partitionType=="Hidden Win95 FAT32 (LBA)" || part1->partitionType=="Hidden Win95 FAT16 (LBA)" || part1->partitionType=="DRDOS / sec (FAT-16 < 32M)" || part1->partitionType=="Disabled NT FAT (FAT-16) volume set/DRDOS"){
        
        long vbr = part1->startSectorAddress*512;
        
        part1->reservedSize = (unsigned int)buff[vbr+15] << 8 | (unsigned int)buff[vbr+14];
        part1->reservedEnd = part1->reservedSize - 1;
        part1->sectorsPerCluster = (unsigned int)buff[vbr+13];
        
        part1->fatStart = part1->reservedSize;
        part1->fatNum = (unsigned int)buff[vbr+16];
        //22 and 23
        
        //The fat size location for a FAT16 is different from FAT32
        if(part1->partitionType=="FAT16 <32MB" || part1->partitionType=="FAT16B (>= 32 MB)" || part1->partitionType=="Win95 FAT16 (LBA)" || part1->partitionType=="Hidden FAT16 <32MB" || part1->partitionType=="Hidden Win95 FAT16 (LBA)" || part1->partitionType=="DRDOS / sec (FAT-16 < 32M)" || part1->partitionType=="Disabled NT FAT (FAT-16) volume set/DRDOS"){
            
            part1->fatSize = (unsigned int)buff[vbr+23] << 8 | (unsigned int)buff[vbr+22];
            long rootDirectoryEntries = (unsigned int)buff[vbr+18] << 8 | (unsigned int)buff[vbr+17];
            long rootDirectorySize = (rootDirectoryEntries*32)/512;
            part1->cluster2 = part1->startSectorAddress+(part1->reservedSize+(part1->fatSize*part1->fatNum)+rootDirectorySize);
        }
        else{
            part1->fatSize = (unsigned int)buff[vbr+39] << 24 | (unsigned int)buff[vbr+38]<<16 | (unsigned int)buff[vbr+37]<<8 | (unsigned int)buff[vbr+36];
            part1->cluster2 = part1->startSectorAddress+(part1->reservedSize+(part1->fatSize*part1->fatNum));
        }
        
        part1->fatEnd = (part1->fatSize*part1->fatNum+part1->fatStart)-1;
        
        cout << "Volume Boot Record for Partition 0" << endl;
        cout << "Reserved area: Start sector: " << part1->reservedStart << "  Ending sector: " << part1->reservedEnd  << "  Size: " << part1->reservedSize << " sectors" << endl;
        cout << "Sectors per cluster: " << part1->sectorsPerCluster << " sectors" << endl;
        cout << "FAT area: Start Sector: " << part1->fatStart << "  Ending Sector: " << part1->fatEnd << endl;
        cout << "# of FATs: " << part1->fatNum << endl;
        cout << "The size of each FAT: " << part1->fatSize << " sectors" << endl;
        cout << "The first sector of cluster 2: " << part1->cluster2 << " sectors" << endl;
        cout << endl;
    }
    
    if(part2->partitionType=="FAT16 <32MB" || part2->partitionType=="FAT16B (>= 32 MB)" || part2->partitionType=="Win95 FAT32" || part2->partitionType=="Win95 FAT32 (LBA)" || part2->partitionType=="Win95 FAT16 (LBA)" || part2->partitionType=="Hidden FAT16 <32MB" || part2->partitionType=="Hidden Win95 FAT32" || part2->partitionType=="Hidden Win95 FAT32 (LBA)" || part2->partitionType=="Hidden Win95 FAT16 (LBA)" || part2->partitionType=="DRDOS / sec (FAT-16 < 32M)" || part2->partitionType=="Disabled NT FAT (FAT-16) volume set/DRDOS"){
        
        long vbr = part2->startSectorAddress*512;
        
        part2->reservedSize = (unsigned int)buff[vbr+15] << 8 | (unsigned int)buff[vbr+14];
        part2->reservedEnd = part2->reservedSize - 1;
        part2->sectorsPerCluster = (unsigned int)buff[vbr+13];
        
        part2->fatStart = part2->reservedSize;
        part2->fatNum = (unsigned int)buff[vbr+16];
        
        //The fat size location for a FAT16 is different from FAT32
        if(part2->partitionType=="FAT16 <32MB" || part2->partitionType=="FAT16B (>= 32 MB)" || part2->partitionType=="Win95 FAT16 (LBA)" || part2->partitionType=="Hidden FAT16 <32MB" || part2->partitionType=="Hidden Win95 FAT16 (LBA)" || part2->partitionType=="DRDOS / sec (FAT-16 < 32M)" || part2->partitionType=="Disabled NT FAT (FAT-16) volume set/DRDOS"){
            
            part2->fatSize = (unsigned int)buff[vbr+23] << 8 | (unsigned int)buff[vbr+22];
            long rootDirectoryEntries = (unsigned int)buff[vbr+18] << 8 | (unsigned int)buff[vbr+17];
            long rootDirectorySize = (rootDirectoryEntries*32)/512;
            part2->cluster2 = part2->startSectorAddress+(part2->reservedSize+(part2->fatSize*part2->fatNum)+rootDirectorySize);
        }
        else{
            part2->fatSize = (unsigned int)buff[vbr+39] << 24 | (unsigned int)buff[vbr+38]<<16 | (unsigned int)buff[vbr+37]<<8 | (unsigned int)buff[vbr+36];
            part2->cluster2 = part2->startSectorAddress+(part2->reservedSize+(part2->fatSize*part2->fatNum));
        }
        part2->fatEnd = (part2->fatSize*part2->fatNum+part2->fatStart)-1;
        
        cout << "Volume Boot Record for Partition 1" << endl;
        cout << "Reserved area: Start sector: " << part2->reservedStart << "  Ending sector: " << part2->reservedEnd  << "  Size: " << part2->reservedSize << " sectors" << endl;
        cout << "Sectors per cluster: " << part2->sectorsPerCluster << " sectors" << endl;
        cout << "FAT area: Start sector: " << part2->fatStart << "  Ending Sector: " << part2->fatEnd << endl;
        cout << "# of FATs: " << part2->fatNum << endl;
        cout << "The size of each FAT: " << part2->fatSize << " sectors" << endl;
        cout << "The first sector of cluster 2: " << part2->cluster2 << " sectors" << endl;
        cout << endl;
    }
    
    if(part3->partitionType=="FAT16 <32MB" || part3->partitionType=="FAT16B (>= 32 MB)" || part3->partitionType=="Win95 FAT32" || part3->partitionType=="Win95 FAT32 (LBA)" || part3->partitionType=="Win95 FAT16 (LBA)" || part3->partitionType=="Hidden FAT16 <32MB" || part3->partitionType=="Hidden Win95 FAT32" || part3->partitionType=="Hidden Win95 FAT32 (LBA)" || part3->partitionType=="Hidden Win95 FAT16 (LBA)" || part3->partitionType=="DRDOS / sec (FAT-16 < 32M)" || part3->partitionType=="Disabled NT FAT (FAT-16) volume set/DRDOS"){
        
        long vbr = part3->startSectorAddress*512;
        
        part3->reservedSize = (unsigned int)buff[vbr+15] << 8 | (unsigned int)buff[vbr+14];
        part3->reservedEnd = part3->reservedSize - 1;
        part3->sectorsPerCluster = (unsigned int)buff[vbr+13];
        
        part3->fatStart = part3->reservedSize;
        part3->fatNum = (unsigned int)buff[vbr+16];
        
        //The fat size location for a FAT16 is different from FAT32
        if(part3->partitionType=="FAT16 <32MB" || part3->partitionType=="FAT16B (>= 32 MB)" || part3->partitionType=="Win95 FAT16 (LBA)" || part3->partitionType=="Hidden FAT16 <32MB" || part3->partitionType=="Hidden Win95 FAT16 (LBA)" || part3->partitionType=="DRDOS / sec (FAT-16 < 32M)" || part3->partitionType=="Disabled NT FAT (FAT-16) volume set/DRDOS"){
            
            part3->fatSize = (unsigned int)buff[vbr+23] << 8 | (unsigned int)buff[vbr+22];
            long rootDirectoryEntries = (unsigned int)buff[vbr+18] << 8 | (unsigned int)buff[vbr+17];
            long rootDirectorySize = (rootDirectoryEntries*32)/512;
            part3->cluster2 = part3->startSectorAddress+(part3->reservedSize+(part3->fatSize*part3->fatNum)+rootDirectorySize);
        }
        else{
            part3->fatSize = (unsigned int)buff[vbr+39] << 24 | (unsigned int)buff[vbr+38]<<16 | (unsigned int)buff[vbr+37]<<8 | (unsigned int)buff[vbr+36];
            part3->cluster2 = part3->startSectorAddress+(part3->reservedSize+(part3->fatSize*part3->fatNum));
        }
        part3->fatEnd = (part3->fatSize*part3->fatNum+part3->fatStart)-1;
        
        cout << "Volume Boot Record for Partition 2" << endl;
        cout << "Reserved area: Start sector: " << part3->reservedStart << "  Ending sector: " << part3->reservedEnd  << "  Size: " << part3->reservedSize << " sectors" << endl;
        cout << "Sectors per cluster: " << part3->sectorsPerCluster << " sectors" << endl;
        cout << "FAT area: Start sector: " << part3->fatStart << "  Ending Sector: " << part3->fatEnd << endl;
        cout << "# of FATs: " << part3->fatNum << endl;
        cout << "The size of each FAT: " << part3->fatSize << " sectors" << endl;
        cout << "The first sector of cluster 2: " << part3->cluster2 << " sectors" << endl;
        cout << endl;
    }
    
    if(part4->partitionType=="FAT16 <32MB" || part4->partitionType=="FAT16B (>= 32 MB)" || part4->partitionType=="Win95 FAT32" || part4->partitionType=="Win95 FAT32 (LBA)" || part4->partitionType=="Win95 FAT16 (LBA)" || part4->partitionType=="Hidden FAT16 <32MB" || part4->partitionType=="Hidden Win95 FAT32" || part1->partitionType=="Hidden Win95 FAT32 (LBA)" || part4->partitionType=="Hidden Win95 FAT16 (LBA)" || part4->partitionType=="DRDOS / sec (FAT-16 < 32M)" || part4->partitionType=="Disabled NT FAT (FAT-16) volume set/DRDOS"){
        
        long vbr = part4->startSectorAddress*512;
        
        part4->reservedSize = (unsigned int)buff[vbr+15] << 8 | (unsigned int)buff[vbr+14];
        part4->reservedEnd = part4->reservedSize - 1;
        part4->sectorsPerCluster = (unsigned int)buff[vbr+13];
        
        part4->fatStart = part4->reservedSize;
        part4->fatNum = (unsigned int)buff[vbr+16];
        
        //The fat size location for a FAT16 is different from FAT32
        if(part4->partitionType=="FAT16 <32MB" || part4->partitionType=="FAT16B (>= 32 MB)" || part4->partitionType=="Win95 FAT16 (LBA)" || part4->partitionType=="Hidden FAT16 <32MB" || part4->partitionType=="Hidden Win95 FAT16 (LBA)" || part4->partitionType=="DRDOS / sec (FAT-16 < 32M)" || part4->partitionType=="Disabled NT FAT (FAT-16) volume set/DRDOS"){
            
            part4->fatSize = (unsigned int)buff[vbr+23] << 8 | (unsigned int)buff[vbr+22];
            long rootDirectoryEntries = (unsigned int)buff[vbr+18] << 8 | (unsigned int)buff[vbr+17];
            long rootDirectorySize = (rootDirectoryEntries*32)/512;
            part4->cluster2 = part4->startSectorAddress+(part4->reservedSize+(part4->fatSize*part4->fatNum)+rootDirectorySize);
        }
        else{
            part4->fatSize = (unsigned int)buff[vbr+39] << 24 | (unsigned int)buff[vbr+38]<<16 | (unsigned int)buff[vbr+37]<<8 | (unsigned int)buff[vbr+36];
            part4->cluster2 = part4->startSectorAddress+(part4->reservedSize+(part4->fatSize*part4->fatNum));
        }
        part4->fatEnd = (part4->fatSize*part4->fatNum+part4->fatStart)-1;
        
        cout << "Volume Boot Record for Partition 3" << endl;
        cout << "Reserved area: Start sector: " << part4->reservedStart << "  Ending sector: " << part4->reservedEnd  << "  Size: " << part4->reservedSize << " sectors" << endl;
        cout << "Sectors per cluster: " << part4->sectorsPerCluster << " sectors" << endl;
        cout << "FAT area: Start sector: " << part4->fatStart << "  Ending Sector: " << part4->fatEnd << endl;
        cout << "# of FATs: " << part4->fatNum << endl;
        cout << "The size of each FAT: " << part4->fatSize << " sectors" << endl;
        cout << "The first sector of cluster 2: " << part4->cluster2 << " sectors" << endl;
    }
}

int main(int argc, char* argv[]) {
    filePath = argv[1];
    
    hashwrapper *md5Wrapper = new md5wrapper();
    hashwrapper *shaWrapper = new sha1wrapper();
    string md5Hash, shaHash;
    
    try{
        md5Hash = md5Wrapper->getHashFromFile(argv[1]);
    }
    catch(hlException &e){
        cerr << "ErrorMD5(" << e.error_number() << "):" << e.error_message() << endl;
    }
    delete md5Wrapper;
    try{
        shaHash = shaWrapper->getHashFromFile(argv[1]);
    }
    catch(hlException &e){
        cerr << "ErrorSHA(" << e.error_number() << "):" << e.error_message() << endl;
    }
    delete shaWrapper;
    
    
    string fileName = argv[1];
    string::reverse_iterator i;
    for (i=fileName.rbegin(); i<fileName.rend(); ++i){      //remove the file path and leave only the
        if(*i == '/'){                                      //file name.
            i--;
            break;
        }
    }
    string tempFileName = &*i;
    
    int index = -1;
    for (string::iterator i=tempFileName.begin(); i<tempFileName.end(); ++i){
        index++;                        //remove the file extension
        if(*i == '.'){
            break;
        }
    }
    tempFileName = tempFileName.substr(0, index);
    string fileNameFinal = "MD5-" + tempFileName + ".txt";
    
    ofstream myfile;        //write the MD5 hash to a file with the correct file name.
    myfile.open(fileNameFinal.c_str());
    myfile << md5Hash;
    myfile.close();
    cout << "MD5 Checksum written to file " << fileNameFinal << endl;
    
    fileNameFinal = "SHA1-" + tempFileName + ".txt";
    
    ofstream myfile2;
    myfile2.open(fileNameFinal.c_str());  //write the SHA1 hash to a file with the correct file name.
    myfile2 << shaHash;
    myfile2.close();
    cout << "SHA1 Checksum written to file " << fileNameFinal << endl;
    
    cout << "Proceed with disk image analysis? (y/n)" << endl;
    string input;
    cin >> input;
    
    if(input == "n" || input == "N")
        exit(1);
    
    getMBR();
    getVBR();
}