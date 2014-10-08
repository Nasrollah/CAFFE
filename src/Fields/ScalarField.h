#ifndef SCALAR_FIELD_H
#define SCALAR_FIELD_H

#include <string>

#include "FieldInterface.h"
#include "SmartPointer3D.h"

class ScalarField : public FieldInterface, public SmartPointer3D<double>
{

private:

public:

    ScalarField(std::string fieldName = "f(x, y, z)",
                int nI = 0,
                int nJ = 0,
                int nK = 0);

};

#endif
