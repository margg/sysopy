#ifndef _GENERATOR_H_
#define _GENERATOR_H_

void generateRecordsFile(char *fileName, int recordSize, int numOfRecords);

void copyFile(char *sourceFileName, char *destFileName, int lineLength, int numOfRecords);

void printFileContents(char *fileName, int recordSize, int numOfRecords);

#endif //_GENERATOR_H_
