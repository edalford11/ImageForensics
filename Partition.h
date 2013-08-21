//
//  Partition.h
//  ImageForensics
//
//  Created by Eric Alford on 3/20/13.
//  Copyright (c) 2013 Eric Alford. All rights reserved.
//

#ifndef ImageForensics_Partition_h
#define ImageForensics_Partition_h
#include <iostream>

using namespace std;

class Partition
{
public:
    string partitionType;
    long partitionSize, startSectorAddress;
    
    int reservedStart;
    long reservedEnd, reservedSize, sectorsPerCluster, fatStart, fatEnd, fatNum, fatSize, cluster2;
    
    Partition(string partitionType, long partitionSize, long startSectorAddress){
        reservedStart = 0;
        this->partitionType = partitionType;
        this->partitionSize = partitionSize;
        this->startSectorAddress = startSectorAddress;
    }
    ~Partition(void);
};

#endif
