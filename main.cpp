

#include <vector>
#include "lib/entity/entity.h"
#include "lib/hdb/hdb.h"



int main(int argc, char* argv[])
{

    using namespace hlogger;
    ENTITY e2({1626, 10, 91234519}, 100, "basic");
    ENTITY e1({152, 19, 8885584}, 90, "another type");
    ENTITY e3({600, 50, 959}, 80, "third");

    std::vector<ENTITY> ve {e1, e2, e3};

    HDB h;
    h.createNewDatabase({0,0}, FTOOLS::TYPE::ENTITY);

    h.saveEntitiesToDatabase(&ve, {0,0});

    si;
    info << "lole" << nl;
    so;



    return 0;
}
