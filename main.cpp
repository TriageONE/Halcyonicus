#include <iostream>
#include <fstream>
#include "lib/world/world.h"
#include "lib/noise/perlin.h"
#include "lib/world/region.h"

#include "lib/sqlite/sqlite3.h"

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#include <Windows.h>
#include <cstdio>
#endif

using namespace std;
static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
    int i;
    for(i = 0; i<argc; i++) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}
int main() {

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    SetConsoleOutputCP(CP_UTF8);
    // Enable buffering to prevent VS from chopping up UTF-8 byte sequences
    setvbuf(stdout, nullptr, _IOFBF, 2000);

#endif

    /* New test for database functionality */
    sqlite3 *db;
    string sql;
    int rc;

    rc = sqlite3_open("test.db", &db);

    if( rc ) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return(0);
    } else {
        fprintf(stderr, "Opened database successfully\n");
    }

    //Create a new statement
    sqlite3_stmt *stmt;

    /** CREATE A NEW TABLE FOR THE DATABASE **
    sql = "CREATE TABLE LAYERS("
          "POSITION SMALLINT NOT NULL, "
          "LEVEL BIT NOT NULL, "
          "ENTITIES BLOB);";
    rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);

    if( rc != SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        fprintf(stdout, "Table created successfully\n");
    }

    /* *********************** */

    /** INSERT INTO DATABASE A NEW ENTRY PLACE 0 LEVEL 0 **
    if (sqlite3_prepare_v2(db, "INSERT INTO LAYERS VALUES(?, ?, ?)", -1, &stmt, nullptr)) {
        printf("Error executing sql statement\n");
        sqlite3_close(db);
        exit(-1);
    }

    sqlite3_bind_int(stmt, 1, 0);
    sqlite3_bind_int(stmt, 2, 0);
    sqlite3_bind_blob(stmt, 3, (void*) "lole", -1, NULL);

    if (sqlite3_step(stmt)){
        printf("Error executing sql statement\n");
        sqlite3_close(db);
        exit(-1);
    }

    sqlite3_finalize(stmt);

    /* *********************** */

    /** SELECT AND VIEW THE BLOB AT POSITION 0 0 WITHIN TABLE LAYERS **/
    sql = "SELECT ENTITIES FROM LAYERS WHERE POSITION=0 AND LEVEL=0;";
    rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (rc) {
        printf("PrepError executing sql statement\n");
        sqlite3_close(db);
        exit(-1);
    }
    rc = sqlite3_step(stmt);
    if (!rc){
        printf("ExecError executing sql statement\n");
        sqlite3_close(db);
        exit(-1);
    }

    auto* lol = sqlite3_column_blob(stmt, 0);

    string s = string((char*) lol);

    sqlite3_finalize(stmt);

    cout << s << endl;

    /* *********************** */

    sqlite3_close(db);


    /* ARCHIVED TEST FOR WORLD GENERATION
    string seed = "hyperion";

    if(!REGION::checkForDirectoryStructure()){
        cout << "Creating directories..." << endl;
        REGION::createDirectories();
    }

    vector<REGIONCOORD> existenceCache;
    int matches = 0, misses = 0;

    for (int x = 0; x < 16; x++){
        for (int y = 0; y < 16; y++){
            WORLDCOORD wc = WORLDCOORD(x,y,0);
            WORLD w = WORLD(seed, wc);
            w.constrain();
            MAP *h = &w.heightmap;
            //Larger values result in a more zoomed out display
            h->setScalar(0.0075);
            h->setRoughness(0.08);

            w.generate();

            //w.out();

            REGIONCOORD rc = wc.getRegionCoordinates();
            cout << "Region " << rc.getX() << ", " << rc.getY() << endl;
            cout << "WorldShard " << wc.getX() << ", " << wc.getY() << endl;

            if (existenceCache.empty()){
                cout << "Testing to see if the region " << rc.getX() << ", " << rc.getY() << " exists.." << endl;
                bool exists = REGION::regionExists(wc.getRegionCoordinates());
                filesystem::path path = REGION::parseFullPathFromRegionCoord(wc.getRegionCoordinates());
                if (!exists){
                    cout << "Creating empty world with path " << path << endl;
                    REGION::createEmptyWorld(path);
                }
                existenceCache.push_back(rc);
            }
            for (REGIONCOORD r : existenceCache){
                int x1,y1;
                x1=r.getX();
                y1=r.getY();
                if (rc.getY() == y1 && rc.getX() == x1){
                    cout << "Testing to see if the region " << x1 << ", " << y1 << " exists.." << endl;
                    bool exists = REGION::regionExists(wc.getRegionCoordinates());
                    filesystem::path path = REGION::parseFullPathFromRegionCoord(wc.getRegionCoordinates());
                    if (!exists){
                        cout << "Creating empty world with path " << path << endl;
                        REGION::createEmptyWorld(path);
                        existenceCache.push_back(rc);
                    }

                }
            }

            string w1h, w2h;

            w1h = w.getRawHash();
            REGION::writeChunk(&w);

            WORLD w2(seed, wc);
            REGION::readChunk(&w2);
            w2h = w2.getRawHash();

            cout << "##################\nCOMMENCE READ OUTPUT\n##################" << endl;

            //w2.out();

            cout << "WC_" << x << "_" << y << ": \n\t" << w1h << ", \n\t" << w2h << ";\n\t" << ((w1h == w2h) ? "MATCH" : "NO_MATCH") << endl;
            if (w1h == w2h) matches++; else misses++;
        }
    }

    cout << "JOB FINISHED, MATCHES:" << matches << ", MISSES: " << misses << endl;
    return 0;
     */

}
