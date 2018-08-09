#pragma once
#include "rwlpcore.h"
#include "rwcore.h"

/* skeleton.h */
typedef struct RsGlobalType RsGlobalType;
struct RsGlobalType
{
    const RwChar *appName;
    RwInt32 maximumWidth;
    RwInt32 maximumHeight;
    // useless stuff
    // ...
};

void InitRenderWareFunctions();

extern RsGlobalType* RsGlobal;