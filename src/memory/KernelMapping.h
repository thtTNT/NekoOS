#pragma once

#include "PageTable.h"

PageTable* initKernelPageTable();

PageTable* createUserPageTable();
