//
// Created by Triage on 4/1/2023.
//

#ifndef HALCYONICUS_HDB_H
#define HALCYONICUS_HDB_H

#include "../world/regioncoord.h"
#include "../world/region.h"
#include "../sqlite/sqlite3.h"
#include "../entity/entityCluster.h"
#include "iostream"

/**
 * The point of this class is to abstract all of the database functions
 * away from SQLITE and onto an interface that makes sense and removes
 * as much boilerplate as possible
 *
 * When you want to interface with a database, ever, you must use this
 * as this will provide a relatively simple and safe way to work with
 * file based databases
 */
class HDB{
public:

    static int createNewEntityDatabase(REGIONCOORD regioncoord){
        sqlite3 *db;
        std::string sql;
        sqlite3_stmt *stmt;
        int rc;

        std::string path = REGION::prependEntityDir(REGION::parseRegioncoordToEname(regioncoord));
        rc = sqlite3_open(path.c_str(), &db);

        if(rc) {
            fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
            return(rc);
        }

        sql = "CREATE TABLE LAYERS("
              "POSITION SMALLINT NOT NULL, "
              "LEVEL BIT NOT NULL, "
              "ENTITIES BLOB);";

        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr)){
            std::cerr << "Error preparing entityDatabase creation statement for path: "<< path << std::endl;
            sqlite3_close(db);
            return(rc);
        }

        rc = sqlite3_step(stmt);
        if (!rc){
            std::cerr << "Error executing entityDatabase creation sql statement, but passed preparation for path: " << path << std::endl;
            sqlite3_close(db);
            return(rc);
        }

        sqlite3_finalize(stmt);

        return (rc);
    }

    // DATA EXTRACTION //

    /**
     * Reads the entire shard's entity data from layer 12 to zero
     * @param cluster
     */
    static int readEntityData(ENTITYCLUSTER *cluster) {
        sqlite3 *db;
        std::string sql;
        sqlite3_stmt *stmt;
        int rc;

        std::string path = REGION::prependEntityDir(
                REGION::parseRegioncoordToEname(cluster->getWorldcoord().getRegionCoordinates()));
        rc = sqlite3_open(path.c_str(), &db);

        if (rc) {
            fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
            return rc;
        }

        int pos = REGION::findChunkArrayOffset(cluster->getWorldcoord());

        sql = "SELECT ENTITIES, LEVEL FROM LAYERS WHERE POSITION = ?;";
        rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
        if (rc) {
            printf("PrepError executing sql statement\n");
            sqlite3_close(db);
            return rc;
        }

        rc = sqlite3_bind_int(stmt, 1, pos);
        if (rc) {
            printf("PrepError executing sql statement\n");
            sqlite3_close(db);
            return rc;
        }

        while (sqlite3_step(stmt) != SQLITE_DONE) {
            int layer = sqlite3_column_int(stmt, 1);
            string result = (char*) sqlite3_column_blob(stmt, 2);
            cluster->deserializeIntolayer(result, layer);
        }
    }

};
#endif //HALCYONICUS_HDB_H
