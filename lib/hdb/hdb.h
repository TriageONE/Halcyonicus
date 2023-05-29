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
#include "../world/chunk.h"

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

    static long long getCurrentEpochTime() {
        auto currentTime = std::chrono::system_clock::now().time_since_epoch();
        return std::chrono::duration_cast<std::chrono::seconds>(currentTime).count();
    }

    /////////////////
    // ENTITY SECTION
    /////////////////

    /**
     * Creates a new entity database using the region coordinates specified
     * @param regioncoord The region to create the database for
     * @return the SQL integer return code
     */
    int createNewEntityDatabase(REGIONCOORD regioncoord){
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

        sql = "CREATE TABLE CHUNKS("
              "LAST_SAVED BIGINT NOT NULL"
              "RELATIVE_POSITION SMALLINT NOT NULL, "
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

        sql = "INSERT INTO CHUNKS VALUES(?, ?, NULL);";

        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr)){
            std::cerr << "Error preparing entityDatabase creation statement for path: "<< path << std::endl;
            sqlite3_close(db);
            return(rc);
        }

        long long currentTime = getCurrentEpochTime();
        sqlite3_bind_int(stmt, 1, currentTime);

        for (short pos = 0; pos < 16384; pos++){

            sqlite3_bind_int(stmt, 2, pos);

            sqlite3_step(stmt);

            sqlite3_reset(stmt);

        }

        rc = sqlite3_finalize(stmt);
        std::cout << "INFO: Database primer finished, created 16384 shell records in region \'" << regioncoord.getX() << ", " << regioncoord.getZ() << "\'" << std::endl;

        return (rc);
    }

    /**
     * Reads the entire shard's entity data from the chunk specified into the entity cluser
     * @param cluster
     * FIXME: FIXED, UNTESTED
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

        /*
         * "CREATE TABLE CHUNKS("
              "LAST_SAVED BIGINT NOT NULL"
              "RELATIVE_POSITION SMALLINT NOT NULL, "
              "ENTITIES BLOB);"
         */

        sql = "SELECT ENTITIES FROM CHUNKS WHERE RELATIVE_POSITION = ?;";
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
        rc = sqlite3_step(stmt);
        if (rc == SQLITE_ROW){
            const void* data = sqlite3_column_blob(stmt, 0);
            int blobSize = sqlite3_column_bytes(stmt, 0);
            std::vector<char> blobVec(static_cast<const char*>(data), static_cast<const char*>(data) + blobSize);
            cluster->deserializeIntoChunk(blobVec);
        } else {
            std::cout << "ERROR: Nothing was found for the chunk " << pos << " (" << cluster->getWorldcoord().getX() << "x, " << cluster->getWorldcoord().getZ() << "z)" << std::endl;
        }
        return rc;
    }

    /**
     * Will write the entire chunk to the database.
     * @param cluster The entity cluster that holds the data you want to write
     * @return The output status code of the SQL ran
     * FIXME: FINISHED, UNTESTED
     */
    static int writeEntityData(ENTITYCLUSTER *cluster){
        sqlite3 *db;
        std::string sql;
        sqlite3_stmt *stmt = nullptr;
        int rc;

        std::string path = REGION::prependEntityDir(
                REGION::parseRegioncoordToEname(cluster->getWorldcoord().getRegionCoordinates()));
        rc = sqlite3_open(path.c_str(), &db);

        if (rc) {
            fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
            return rc;
        }

        int pos = REGION::findChunkArrayOffset(cluster->getWorldcoord());

        /*"CREATE TABLE CHUNKS("
             "LAST_SAVED BIGINT NOT NULL"
             "RELATIVE_POSITION SMALLINT NOT NULL, "
             "ENTITIES BLOB);"
             */

        long long currentTime = getCurrentEpochTime();

        sql = "UPDATE CHUNKS SET LAST_SAVED = ?, DATA = ? WHERE POSITION = ?;";

        rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
        if (rc) {
            printf("PrepError executing sql statement\n");
            sqlite3_close(db);
            return rc;
        }

        auto data = cluster->serializeCluster();
        sqlite3_bind_int(stmt, 3, pos);
        sqlite3_bind_int(stmt, 1, currentTime);
        sqlite3_bind_blob(stmt, 2, data.data(), data.size(), SQLITE_STATIC);

        rc = sqlite3_step(stmt);
        if (rc) {
            printf("PrepError executing sql statement\n");
            sqlite3_close(db);
            return rc;
        }

        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return rc;
    }

    /**
     * Will write one level specified by LEVEL to the database from the entityCluster. Does not discriminate against same
     * layers and will perform the write anyways.
     * @param cluster The entity cluster that holds the data you want to write
     * @param level The level within this cluster to write to the database
     * @return The status code of the SQL ran
     * FIXME: NOT FINISHED, NO LONGER NEEDED?
     */
    [[deprecated]] static int writeShardEntityData(ENTITYCLUSTER *cluster, int level){
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

    /////////////////
    // CHUNK SECTION
    /////////////////

    int createNewChunkDatabase(REGIONCOORD regioncoord){
        sqlite3 *db;
        std::string sql;
        sqlite3_stmt *stmt;
        int rc;

        std::string path = REGION::prependWorldDir(REGION::parseRegioncoordToFname(regioncoord));
        rc = sqlite3_open(path.c_str(), &db);

        if(rc) {
            fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
            return(rc);
        }

        sql = "CREATE TABLE CHUNKS("
              "LAST_SAVED BIGINT NOT NULL"
              "RELATIVE_POSITION SMALLINT NOT NULL, "
              "BLOCKS BLOB);";

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

        sql = "INSERT INTO CHUNKS VALUES(?, ?, NULL);";

        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr)){
            std::cerr << "Error preparing entityDatabase creation statement for path: "<< path << std::endl;
            sqlite3_close(db);
            return(rc);
        }

        long long currentTime = getCurrentEpochTime();
        sqlite3_bind_int(stmt, 1, currentTime);

        for (short pos = 0; pos < 16384; pos++){

            sqlite3_bind_int(stmt, 2, pos);

            sqlite3_step(stmt);

            sqlite3_reset(stmt);

        }

        rc = sqlite3_finalize(stmt);
        std::cout << "INFO: Database primer finished, created 16384 shell records in region \'" << regioncoord.getX() << ", " << regioncoord.getZ() << "\'" << std::endl;

        return (rc);
    }

/**
     * Reads the chunk data from the database and overwrites the chunk given with the new dataset
     * FIXME: FINISHED, UNTESTED
     */
    static int readChunkData(chunk * c) {
        sqlite3 *db;
        std::string sql;
        sqlite3_stmt *stmt;
        int rc;

        std::string path = REGION::prependWorldDir(
                REGION::parseRegioncoordToFname(c->location.getRegionCoordinates()));
        rc = sqlite3_open(path.c_str(), &db);

        if (rc) {
            fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
            return rc;
        }

        int pos = REGION::findChunkArrayOffset(c->location);

        /*
         * "CREATE TABLE CHUNKS("
              "LAST_SAVED BIGINT NOT NULL"
              "RELATIVE_POSITION SMALLINT NOT NULL, "
              "BLOCKS BLOB);";
         */

        sql = "SELECT BLOCKS FROM CHUNKS WHERE RELATIVE_POSITION = ?;";
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

        rc = sqlite3_step(stmt);
        if (rc == SQLITE_ROW){
            const void* data = sqlite3_column_blob(stmt, 0);
            int blobSize = sqlite3_column_bytes(stmt, 0);
            std::vector<int> blobVec(static_cast<const int*>(data), static_cast<const int*>(data) + blobSize);
            c->deserialize2(blobVec);
        } else {
            std::cout << "ERROR: Nothing was found for the chunk " << pos << " (" << c->location.getX() << "x, " << c->location.getZ() << "z)" << std::endl;
        }
        return rc;
    }

    /**
     * Will write the entire chunk to the database.
     * @param cluster The entity cluster that holds the data you want to write
     * @return The output status code of the SQL ran
     * FIXME: FINISHED, UNTESTED
     */
    static int writeChunkData(chunk * c){
        sqlite3 *db;
        std::string sql;
        sqlite3_stmt *stmt = nullptr;
        int rc;

        std::string path = REGION::prependWorldDir(
                REGION::parseRegioncoordToFname(c->location.getRegionCoordinates()));
        rc = sqlite3_open(path.c_str(), &db);

        if (rc) {
            fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
            return rc;
        }

        int pos = REGION::findChunkArrayOffset(c->location);

        /*"CREATE TABLE CHUNKS("
             "LAST_SAVED BIGINT NOT NULL"
             "RELATIVE_POSITION SMALLINT NOT NULL, "
             "ENTITIES BLOB);"
             */

        long long currentTime = getCurrentEpochTime();

        sql = "UPDATE CHUNKS SET LAST_SAVED = ?, DATA = ? WHERE POSITION = ?;";

        rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
        if (rc) {
            printf("PrepError executing sql statement\n");
            sqlite3_close(db);
            return rc;
        }

        auto data = c->serialize2();
        sqlite3_bind_int(stmt, 3, pos);
        sqlite3_bind_int(stmt, 1, currentTime);
        sqlite3_bind_blob(stmt, 2, data.data(), data.size(), SQLITE_STATIC);

        rc = sqlite3_step(stmt);
        if (rc) {
            printf("PrepError executing sql statement\n");
            sqlite3_close(db);
            return rc;
        }

        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return rc;
    }
};
#endif //HALCYONICUS_HDB_H
