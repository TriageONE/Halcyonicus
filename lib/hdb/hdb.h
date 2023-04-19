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
              "ENTITIES BLOB);"

              "CREATE UNIQUE INDEX POS_INDEX ON LAYERS (POSITION); ";

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

        sql = "INSERT INTO LAYERS VALUES(?, ?, NULL);";

        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr)){
            std::cerr << "Error preparing entityDatabase creation statement for path: "<< path << std::endl;
            sqlite3_close(db);
            return(rc);
        }

        for (int pos = 0; pos < 256; pos++){

            sqlite3_bind_int(stmt, 1, pos);

            for (int layer = 0; layer < 13; layer++){

                sqlite3_bind_int(stmt, 2, layer);

                sqlite3_step(stmt);

                sqlite3_reset(stmt);

            }
        }

        rc = sqlite3_finalize(stmt);
        cout << "INFO: Database primer finished, created 3328 shell records in region \'" << regioncoord.getX() << ", " << regioncoord.getZ() << "\'" << endl;

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

    /**
     * Will write the entire shard to each layer present within the database and avoid the layers that are empty. Does not
     * discriminate against same layers and will perform the write anyways.
     * @param cluster The entity cluster that holds the data you want to write
     * @return The status code of the SQL ran
     */
    static int writeShardEntityData(ENTITYCLUSTER *cluster){
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

        sql = "UPDATE LAYERS SET DATA = ? WHERE POSITION = ? AND LEVEL = ?;";
        sqlite3_bind_int(stmt, 2, pos);
        for (int i = 0; i < 13; i++){

        }

    }

    /**
     * Will write one level specified by LEVEL to the database from the entityCluster. Does not discriminate against same
     * layers and will perform the write anyways.
     * @param cluster The entity cluster that holds the data you want to write
     * @param level The level within this cluster to write to the database
     * @return The status code of the SQL ran
     */
    static int writeShardEntityData(ENTITYCLUSTER *cluster, int level){
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
        sql = "INSERT INTO LAYERS VALUES(?, ?, ?);";

        rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
        if (rc) {
            printf("PrepError executing sql statement\n");
            sqlite3_close(db);
            return rc;
        }


    }

};
#endif //HALCYONICUS_HDB_H
