#include "Fuji.h"
#include "MFVector.h"
#include "MFMatrix.h"

const MFVector MFVector::zero = MakeVector(0.0f, 0.0f, 0.0f, 0.0f);
const MFVector MFVector::one = MakeVector(1.0f, 1.0f, 1.0f, 1.0f);
const MFVector MFVector::identity = MakeVector(0.0f, 0.0f, 0.0f, 1.0f);
const MFVector MFVector::up = MakeVector(0.0f, 1.0f, 0.0f, 1.0f);
