#include "public.h"

void executeDelay(unsigned int uiUs)
{
    unsigned int uiJ = 0;
    unsigned int uiK = 0;
    unsigned int uiCount = 0;

    for(uiCount = 0; uiCount < uiUs; uiCount++)
    {
        for(uiJ = 0; uiJ < 49; uiJ++);
    }
}

