#ifndef _DATA_COLLECTION_H_
#define _DATA_COLLECTION_H_

#define DATA_COLLECTION_BEGIN    1
#define DATA_COLLECTION_CONTINUE 2
#define DATA_COLLECTION_END      3

#define PROTOCOL_DATA_END     0x16
#define PROTOCOL_DATA_BEGIN   0x68

void dataCollectionTaskNotify(void);
void dataCollectionTaskCreate(void);

#endif

