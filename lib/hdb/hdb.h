//
// Created by Triage on 4/1/2023.
//

#ifndef HALCYONICUS_HDB_H
#define HALCYONICUS_HDB_H

#include "../sqlite/sqlite3.h"
#include "../world/coordinate.h"
#include "../tools/filetools.h"
#include "../entity/entity.h"
#include "../world/chunk.h"
#include "../tools/compressionTools.h"
#include "../world/block.h"
#include <set>

/**
 * The point of this class is to abstract all of the database functions
 * away from SQLITE and onto an interface that makes sense and removes
 * as much boilerplate as possible
 *
 * When you want to interface with a database, ever, you must use this
 * as this will provide a relatively simple and safe way to work with
 * file based databases
 */
using namespace hlogger;

class HDB{
public:

    static bool handlePrepareFail(int code, COORDINATE::REGIONCOORD regioncoord, FTOOLS::TYPE type) {
        if (code == 101 or code == 1) {
            //THe database tables dont exist
            checkSchema(regioncoord, type);
        } else {
            return false;
        }
    }

    static bool fixDBStructure(COORDINATE::REGIONCOORD regioncoord, FTOOLS::TYPE type) {
        si;
        if (!FTOOLS::checkForDirectoryStructure()){
            //False, the directories do not exist and must be created
            err << "Directories were not found, creating new folders for persistent storage" << nl;
            FTOOLS::createDirectories();
        }
        //The directories are now in place, do we have a database present?
        if (!checkSchema(regioncoord, type)){
            err << "Schema check failed, trying to recreate the database with the proper tables" << nl;
            //The database schema doesnt match, we should create a new database
        }
        so;
        return true;
    }

    //Verifies if the schema of the database matches
    static bool checkSchema(COORDINATE::REGIONCOORD regioncoord, FTOOLS::TYPE type){
        sqlite3 *db;
        std::string sql;
        sqlite3_stmt *stmt;
        int rc;
        si;
        std::string path = FTOOLS::prependDirectory(FTOOLS::parseRegioncoordToFilename(regioncoord, type), type);
        rc = sqlite3_open(path.c_str(), &db);

        if (rc){
            err << "Database schema check fail, ensure that we can access the file: \"" << path << "\" before checking the structure of our database" << nl;
            so;
            return false;
        }

        //Futureproofing for multitable drifting later
       std::set<std::string> tstrings;

        switch (type) {
            case FTOOLS::TYPE::ENTITY:
                tstrings.insert("ENTITIES");
                break;
            case FTOOLS::TYPE::TERRAIN:
                tstrings.insert("TERRAIN");
                break;
            case FTOOLS::TYPE::BLOCK:
                tstrings.insert("BLOCKS");
                break;
            case FTOOLS::TYPE::DATA:
                tstrings.insert("DATA");
                break;
            case FTOOLS::CLIMATE:
                tstrings.insert("CLIMATE");
                break;
        }

        for (const auto& tstring : tstrings){
            sql = "PRAGMA table_info('";
            sql.append(tstring);
            sql.append("');");

            rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
            if (rc){
                err << "Error preparing " << sql << " database persistence section creation statement for path: "<< path << ", error code " << rc << " \"" << sqlite3_errmsg(db) << "\"" << nl;
                sqlite3_finalize(stmt);
                sqlite3_close(db);
                so;
                return false;
            }


            rc = sqlite3_step(stmt);
            if (!rc){
                err << "Error executing " << tstring << " database persistence section creation sql statement, but passed preparation for path: " << path << ", error code " << rc << " \"" << sqlite3_errmsg(db) << "\"" << nl;
                sqlite3_finalize(stmt);
                sqlite3_close(db);
                so;
                return false;
            }

            std::map<std::string, std::string> match;
            int expectation;
            switch (type) {
                case FTOOLS::TYPE::CLIMATE:
                    match = {{"OFFSET", "SMALLINT"},
                             {"DATA", "TINYBLOB"}  };
                    break;
                case FTOOLS::TYPE::ENTITY:
                    match = {{"ID", "BIGINT"},
                             {"TYPE", "TEXT"},
                             {"X", "BIGINT"} ,
                             {"Y", "BIGINT"} ,
                             {"Z", "INT"}    ,
                             {"DATA", "BLOB"}  };
                    break;
                case FTOOLS::TYPE::TERRAIN:
                    match = {{"OFFSET", "SMALLINT"},
                             {"DATA", "TINYBLOB"}};
                    break;
                case FTOOLS::TYPE::BLOCK:
                    match = {{"OFFSET", "SMALLINT"},
                             {"DATA", "BLOB"}};
                    break;
                case FTOOLS::TYPE::DATA:
                    match = {{"KEY", "TEXT"},
                             {"VALUE", "TEXT"}};
                    break;
            }
            expectation = match.size();

            int count = 0;
            while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
                count++;
                int bytes = sqlite3_column_bytes(stmt, 1);
                const auto* tBytes = static_cast<const unsigned char*>(sqlite3_column_blob(stmt, 1));
                std::string key; key.assign(reinterpret_cast<const char*>(tBytes), bytes);

                bytes = sqlite3_column_bytes(stmt, 2);
                tBytes = static_cast<const unsigned char*>(sqlite3_column_blob(stmt, 2));
                std::string value; value.assign(reinterpret_cast<const char*>(tBytes), bytes);
                info << "Schema inspector found NAME: " << key << ", VALUE: " << value << nl;
                if (match.contains(key)){
                    auto pair = match.find(key);
                    if (pair->second != value) {
                        sqlite3_finalize(stmt);
                        warn << "Type mismatch, expected " << pair->second << " rather than " << value << nl;
                        so;
                        return createNewDatabase(regioncoord, type);
                    }
                } else {
                    sqlite3_finalize(stmt);
                    warn << "Key not found, tried for \"" << key << "\"" << nl;
                    so;
                    return createNewDatabase(regioncoord, type);
                }
            }
            sqlite3_finalize(stmt);
            if (count != expectation) {
                err << "Expectation failed for schema check, likely missing a required column" << nl;
                so;
                return createNewDatabase(regioncoord, type);
            }
        }
        so;
        return true;

    }

    /**
     * Creates a new entity database using the region coordinates specified
     * @param regioncoord The region to create the database for
     * @param type the type of database to create
     * @return the SQL integer return code
     */
    static int createNewDatabase(COORDINATE::REGIONCOORD regioncoord, FTOOLS::TYPE type){
        sqlite3 *db;
        std::string sql;
        sqlite3_stmt *stmt;
        int rc;
        si;

        std::string t;

        switch (type) {
            case FTOOLS::TYPE::TERRAIN:
                info << "Preparing TERRAIN DB.." << nl;
                t = "world";
                break;
            case FTOOLS::TYPE::CLIMATE:
                info << "Preparing CLIMATE DB.." << nl;
                t = "world";
                break;
            case FTOOLS::TYPE::ENTITY:
                info << "Preparing ENTITIES DB.." << nl;
                t = "entities";
                break;
            case FTOOLS::TYPE::BLOCK:
                info << "Preparing BLOCK DB.." << nl;
                t = "blocks";
                break;
            case FTOOLS::TYPE::DATA:
                info << "Preparing DATA DB.." << nl;
                t = "data";
                break;
        }
        info << "Creating new database file: " << t << nl;

        std::string path = FTOOLS::prependDirectory(FTOOLS::parseRegioncoordToFilename(regioncoord, type), type);
        rc = sqlite3_open(path.c_str(), &db);

        if(rc) {
            info << "Cannot open database files: " << path << std::endl;
            return rc;
        }

        info << "Database \"" << t << "\" created, trying database construction.." << nl;

        switch (type) {
            case FTOOLS::TYPE::TERRAIN:
                sql = "CREATE TABLE IF NOT EXISTS TERRAIN("
                      "OFFSET INT NOT NULL UNIQUE, "
                      "DATA TINYBLOB NOT NULL);";
                break;
            case FTOOLS::TYPE::CLIMATE:
                sql = "CREATE TABLE IF NOT EXISTS CLIMATE("
                      "OFFSET SMALLINT NOT NULL UNIQUE, "
                      "DATA TINYBLOB NOT NULL);";
                break;
            case FTOOLS::TYPE::ENTITY:
                sql = "CREATE TABLE IF NOT EXISTS ENTITIES("
                      "ID BIGINT NOT NULL PRIMARY KEY, "
                      "TYPE TEXT NOT NULL, "
                      "X BIGINT NOT NULL, "
                      "Y BIGINT NOT NULL, "
                      "Z INT NOT NULL, "
                      "DATA BLOB);";
                break;
            case FTOOLS::TYPE::BLOCK:
                sql = "CREATE TABLE IF NOT EXISTS BLOCKS("
                      "OFFSET INT NOT NULL PRIMARY KEY, "
                      "DATA BLOB);";
                break;
            case FTOOLS::TYPE::DATA:
                goto skip;
        }

        info << "Executing \"" << sql << "\" onto database " << path << nl;
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr)){
            err << "Error preparing " << t << " database persistence section creation statement for path: "<< path << ", error code " << rc << " \"" << sqlite3_errmsg(db) << "\"" << nl;
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            so;
            return(rc);
        }


        rc = sqlite3_step(stmt);
        if (!rc){
            err << "Error executing " << t << " database persistence section creation sql statement, but passed preparation for path: " << path << ", error code " << rc << " \"" << sqlite3_errmsg(db) << "\"" << nl;
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            so;
            return(rc);
        }
        info << "Successful database table creation, creating data table.." << nl;

        skip:
        sql = "CREATE TABLE IF NOT EXISTS HALDATA("
              "KEY STRING NOT NULL, "
              "VALUE TEXT NOT NULL);";

        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr)){
            err << "Error preparing " << t << " database data section creation statement for path: "<< path << ", error code " << rc << "\"" << sqlite3_errmsg(db) << "\"" << nl;
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            so;
            return(rc);
        }

        rc = sqlite3_step(stmt);
        if (!rc){
            err << "Error executing " << t << " database data section creation sql statement, but passed preparation for path: " << path << ", error code " << rc << "\"" << sqlite3_errmsg(db) << "\"" << nl;
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            so;
            return(rc);
        }
        rc = sqlite3_finalize(stmt);

        if (rc){
            err << "Error finalizing " << t << " database data section creation sql statement, but passed stepping for path: " << path << ", error code " << rc << "\"" << sqlite3_errmsg(db) << "\"" << nl;
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            so;
            return(rc);
        }

        info << "Successful database data table creation, database finalized." << nl;

        so;
        return (rc);
    }

    ////////////////
    //ENTITY SECTION
    ////////////////

    /**
     * Saving entities is a process more involved than just saving them, deduplication and cascade deletion is
     * paramount to ensuring that entities are handled correctly. This method takes all the entities in the group
     * and organizes them based on where they are now, and where they were before. If an entity was in a different
     * region before, save that new one to the current database scope now, and delete it from the other one next.
     *
     * we shouldn't open databases for only one transaction and then close it, rather we should prioritize making sure
     * all entities are handled at once from each region. Not everything is stored in one file, so each region is worked
     * with the bulk of its entities to remove inline from the next, ensuring tasks are separated into bulk tasks.
     * @param entities
     * @param regionToSaveTo
     * @return
     */
    static int saveEntitiesToDatabase(std::vector<ENTITY> * entities, COORDINATE::REGIONCOORD regionToSaveTo){
        sqlite3 *db;
        std::string sql;
        sqlite3_stmt *stmt;
        int rc;
        si;
        std::string path = FTOOLS::parseFullPathFromRegionCoord(regionToSaveTo, FTOOLS::TYPE::ENTITY);
        rc = sqlite3_open(path.c_str(), &db);

        if(rc) {
            info << "Cannot open database files: " << path << ", trying to find directory structure.." << std::endl;
            if (!fixDBStructure(regionToSaveTo, FTOOLS::ENTITY)){
                err << "Cannot open entity database, error: " << sqlite3_errmsg(db) << nl;
                so;
                return(rc);
            } else {
                info << "Fixed database structure, continuing.." << nl;
                rc = sqlite3_open(path.c_str(), &db);
                if(rc) {
                    err << "Tried opening database but could not open after fix :'(" << nl;
                    so;
                    return rc;
                }
            }
        }
        /*
         * Scan each entity and compare the regioncoord it was in to the one its currently in now
         * If there is a difference, mark the UUID for special handling
         */

        std::map<long long, COORDINATE::REGIONCOORD> movedUUIDs;
        std::set<long long> toIgnore;
        //Unmoved entities should be implicitly allowable to scope of saving, negating this idea
        //std::map<long long, COORDINATE::REGIONCOORD> unmovedUUIDs;

        for (ENTITY e : *entities){

            //Ensure validity of the regioncoords these entities are in, we dont want to accidentally save something that shouldnt be a part of this database
            if (regionToSaveTo != e.getLocation().getRegioncoord()) toIgnore.insert(e.getUUID());
            //If the regioncoords are not the same, indicating the entity has changed location,
            if (e.getLastSavedLocation().getRegioncoord() != e.getLocation().getRegioncoord()){
                movedUUIDs.insert({e.getUUID(), e.getLastSavedLocation().getRegioncoord()});
            }
        }
        info << "Saving " << entities->size() << " entities to region " << regionToSaveTo.x << ", " << regionToSaveTo.y << nl;
        //Rather have a duplicate than no entity, therefore load the entities as entries into the database using one transaction:

        sql = "BEGIN TRANSACTION;";

        rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
        if (rc){
            err << "Error preparing: "<< sql << "\", code " << rc << "; SQL was not transacted, no changes made; \"" << sqlite3_errmsg(db) << "\"" << nl;
            if (handlePrepareFail(rc, regionToSaveTo, FTOOLS::TYPE::ENTITY)){
                err << "Could not fix database, SQL was not transacted, no changes made;" << nl;
                sqlite3_finalize(stmt);
                sqlite3_close(db);
                so;
                return(rc);
            } else {
                rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
                if (rc){
                    err << "Error preparing: "<< sql << "\", code " << rc << "; Prepare failed twice, with code " << sqlite3_errmsg(db) << "\"" << nl;
                    sqlite3_finalize(stmt);
                    sqlite3_close(db);
                    so;
                    return(rc);
                }
            };

        }

        rc = sqlite3_step(stmt);
        if (!rc){
            err << "Error stepping: \"" << sql << "\", code " << rc << "; SQL was not transacted, no changes made; \"" << sqlite3_errmsg(db) << "\"" << nl;
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            so;
            return(rc);
        }

        long long currentTime = TIMETOOLS::getCurrentEpochTime();
        std::vector<char> data;

        sql = "INSERT INTO ENTITIES(ID, TYPE, X, Y, Z, DATA) "
              //      1 2 3 4 5 6
              "VALUES(?,?,?,?,?,?) "
              "ON CONFLICT(ID) DO "
              "UPDATE SET "
              //    7   8    9    10      11
              "TYPE=? X=?, Y=?, Z=?, DATA=? WHERE ID=?;";
        rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
        if (rc){
            err << "Error preparing: "<< sql << "\", code " << rc << "; SQL was not transacted, no changes made; \"" << sqlite3_errmsg(db) << "\"" << nl;
            if (handlePrepareFail(rc, regionToSaveTo, FTOOLS::TYPE::ENTITY)){
                err << "Could not fix database, SQL was not transacted, no changes made;" << nl;
                sqlite3_finalize(stmt);
                sqlite3_close(db);
                so;
                return(rc);
            } else {
                rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
                if (rc){
                    err << "Error preparing: "<< sql << "\", code " << rc << "; Prepare failed twice, with code " << sqlite3_errmsg(db) << "\"" << nl;
                    sqlite3_finalize(stmt);
                    sqlite3_close(db);
                    so;
                    return(rc);
                }
            };

        }

        sw sw;

        for (ENTITY e : *entities){
            if (toIgnore.contains(e.getUUID())) continue;
            //Perform work on the entitiy so we can have all data handled when we start binding data
            e.updateTimestamp(currentTime);
            e.serializeEntity(&data);

            //Bind all of our data
            sqlite3_bind_int64(stmt, 1, e.getUUID());   //ID
            sqlite3_bind_text(stmt, 2, e.getType().data(), e.getType().length(), SQLITE_STATIC); //TYPE
            sqlite3_bind_int64(stmt, 3, e.getLocation().x); //X
            sqlite3_bind_int64(stmt, 4, e.getLocation().y); //Y
            sqlite3_bind_int(stmt, 5, e.getLocation().z);   //Z
            sqlite3_bind_blob(stmt, 6, data.data(), data.size(), SQLITE_STATIC); //DATA

            //Second half of the statement
            sqlite3_bind_text(stmt, 7, e.getType().data(), e.getType().length(), SQLITE_STATIC); //TYPE
            sqlite3_bind_int64(stmt, 8, e.getLocation().x); //X
            sqlite3_bind_int64(stmt, 9, e.getLocation().y); //Y
            sqlite3_bind_int(stmt, 10, e.getLocation().z);  //Z
            sqlite3_bind_blob(stmt, 11, data.data(), data.size(), SQLITE_STATIC); //DATA
            sqlite3_bind_int64(stmt, 12, e.getUUID());   //ID

            //Step through after data binds are complete to
            sqlite3_step(stmt);
            sqlite3_reset(stmt);
            sw.lap();

        }
        sw.laprs();
        info << "Transacting entities.." << nl;

        sql = "END TRANSACTION;";
        rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
        if (rc){
            err << "Error preparing: "<< sql << "\", code " << rc << "; SQL was not transacted, no changes made; \"" << sqlite3_errmsg(db) << "\"" << nl;
            if (handlePrepareFail(rc, regionToSaveTo, FTOOLS::TYPE::ENTITY)){
                err << "Could not fix database, SQL was not transacted, no changes made;" << nl;
                sqlite3_finalize(stmt);
                sqlite3_close(db);
                so;
                return(rc);
            } else {
                rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
                if (rc){
                    err << "Error preparing: "<< sql << "\", code " << rc << "; Prepare failed twice, with code " << sqlite3_errmsg(db) << "\"" << nl;
                    sqlite3_finalize(stmt);
                    sqlite3_close(db);
                    so;
                    return(rc);
                }
            };

        }

        rc = sqlite3_step(stmt);
        if (!rc){
            err << "Error stepping: \"" << sql << "\", code " << rc << "; SQL was not transacted, no changes made" << std::endl << sqlite3_errmsg(db) << std::endl;
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            so;
            return(rc);
        }

        rc = sqlite3_finalize(stmt);
        if (rc){
            err << "Error finalizing call, code " << rc << "; SQL was not transacted, no changes made" << sqlite3_errmsg(db) << std::endl;
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            so;
            return(rc);
        }
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        sw.lap();
        info << "Finished transaction" << nl;

        //We can assume now that the call to the entity database is finished and we dont have to worry about it anymore.
        // Now we have to handle all the records that we know overlap with another region. All we have to do is organize the areas to delete UUIDS from
        if (movedUUIDs.empty()){
            info << "Entity saving finished" << nl;
            so;
            return 0;
        }

        COORDINATE::REGIONCOORD currentRegionCoord = movedUUIDs[0];
        std::set<COORDINATE::REGIONCOORD> handled;
        std::vector<long long> toDelete;
        unsigned long numDuplicates = movedUUIDs.size();
        long long currentCount = 0;

        while(currentCount < numDuplicates) {
            //Organize the ones we want to work on now into a nice vector
            for (auto e: movedUUIDs) {
                if (e.second == currentRegionCoord) {
                    toDelete.push_back(e.first);
                    currentCount++;
                }
            }

            //instead of reducing the size of the vector, count the amount of elements in the movedUUIDs and whileloop until we hit that number

            //Get the database that we want to manipulate here
            path = FTOOLS::parseFullPathFromRegionCoord(currentRegionCoord, FTOOLS::TYPE::ENTITY);
            rc = sqlite3_open(path.c_str(), &db);

            if(rc) {
                info << "Cannot open database files: " << path << ", trying to find directory structure.." << std::endl;
                if (!fixDBStructure(currentRegionCoord, FTOOLS::ENTITY)){
                    err << "Cannot open entity database, error: " << sqlite3_errmsg(db) << nl;
                    so;
                    return(rc);
                } else {
                    info << "Fixed database structure, continuing.." << nl;
                    rc = sqlite3_open(path.c_str(), &db);
                    if(rc) {
                        err << "Tried opening database but could not open after fix :'(" << nl;
                        so;
                        return rc;
                    }
                }
            }
            sql = "BEGIN TRANSACTION;";

            rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
            if (rc){
                err << "Error preparing: "<< sql << "\", code " << rc << "; SQL was not transacted, no changes made; \"" << sqlite3_errmsg(db) << "\"" << nl;
                if (handlePrepareFail(rc, regionToSaveTo, FTOOLS::TYPE::ENTITY)){
                    err << "Could not fix database, SQL was not transacted, no changes made;" << nl;
                    sqlite3_finalize(stmt);
                    sqlite3_close(db);
                    so;
                    return(rc);
                } else {
                    rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
                    if (rc){
                        err << "Error preparing: "<< sql << "\", code " << rc << "; Prepare failed twice, with code " << sqlite3_errmsg(db) << "\"" << nl;
                        sqlite3_finalize(stmt);
                        sqlite3_close(db);
                        so;
                        return(rc);
                    }
                };

            }

            rc = sqlite3_step(stmt);
            if (!rc) {
                err << "Error stepping: \"" << sql << "\", code " << rc
                          << "; SQL was not transacted, no changes made" << std::endl << sqlite3_errmsg(db)
                          << std::endl;
                sqlite3_finalize(stmt);
                sqlite3_close(db);
                so;
                return (rc);
            }

            sql = "DELETE FROM ENTITIES WHERE UUID=?;";

            rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
            if (rc){
                err << "Error preparing: "<< sql << "\", code " << rc << "; SQL was not transacted, no changes made; \"" << sqlite3_errmsg(db) << "\"" << nl;
                if (handlePrepareFail(rc, regionToSaveTo, FTOOLS::TYPE::ENTITY)){
                    err << "Could not fix database, SQL was not transacted, no changes made;" << nl;
                    sqlite3_finalize(stmt);
                    sqlite3_close(db);
                    so;
                    return(rc);
                } else {
                    rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
                    if (rc){
                        err << "Error preparing: "<< sql << "\", code " << rc << "; Prepare failed twice, with code " << sqlite3_errmsg(db) << "\"" << nl;
                        sqlite3_finalize(stmt);
                        sqlite3_close(db);
                        so;
                        return(rc);
                    }
                };

            }

            for (auto i: toDelete) {
                sqlite3_bind_int64(stmt, 1, i);
                sqlite3_step(stmt);
                sqlite3_reset(stmt);
            }

            rc = sqlite3_finalize(stmt);
            if (!rc) {
                err << "Error finalizing call, code " << rc << "; SQL was not transacted, no changes made"
                          << std::endl << sqlite3_errmsg(db) << std::endl;
                sqlite3_finalize(stmt);
                sqlite3_close(db);
                so;
                return (rc);
            }
            sqlite3_close(db);

            //Cleanup and mark this sector as done
            handled.insert(currentRegionCoord);
            toDelete.clear();

            //Select another coordinate not handled
            for (auto it: movedUUIDs) {
                if (handled.contains(it.second)){
                    continue;
                } else {
                    currentRegionCoord = it.second;
                }

            }
        }
        info << "Entity deduplication finished" << nl;
        so;
        return 0;
    }

    /**
     * The reason deletes should take place with UUIDs rather than location is because it is paramount
     * that minimal interference should take place with entities and their removal. removing something
     * should be explicit and direct, where we should first know what exists where and its properties.
     * basing the deletion off of the unique ID is the most direct and surefire way to do this
     * @param entities
     * @param regionToDeleteFrom
     * @return
     */
    static int deleteEntitiesByUUID(std::vector<ENTITY> * entities, COORDINATE::REGIONCOORD regionToDeleteFrom){
        sqlite3 *db;
        std::string sql;
        sqlite3_stmt *stmt;
        int rc;

        si;

        std::string path = FTOOLS::parseFullPathFromRegionCoord(regionToDeleteFrom, FTOOLS::TYPE::ENTITY);
        rc = sqlite3_open(path.c_str(), &db);

        if(rc) {
            info << "Cannot open database files: " << path << ", trying to find directory structure.." << std::endl;
            if (!fixDBStructure(regionToDeleteFrom, FTOOLS::ENTITY)){
                err << "Cannot open entity database, error: " << sqlite3_errmsg(db) << nl;
                so;
                return(rc);
            } else {
                info << "Fixed database structure, continuing.." << nl;
                rc = sqlite3_open(path.c_str(), &db);
                if(rc) {
                    err << "Tried opening database but could not open after fix :'(" << nl;
                    so;
                    return rc;
                }
            }
        }

        std::vector<unsigned long long> toDelete;

        for (ENTITY e : *entities){

            //Ensure validity of the regioncoords these entities are in, we dont want to accidentally save something that shouldnt be a part of this database
            if (regionToDeleteFrom == e.getLocation().getRegioncoord()) toDelete.push_back(e.getUUID());
        }

        info << "Deleting " << entities->size() << " entities to region " << regionToDeleteFrom.x << ", " << regionToDeleteFrom.y << nl;

        sql = "BEGIN TRANSACTION;";

        rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
        if (rc){
            err << "Error preparing: "<< sql << "\", code " << rc << "; SQL was not transacted, no changes made; \"" << sqlite3_errmsg(db) << "\"" << nl;
            if (handlePrepareFail(rc, regionToDeleteFrom, FTOOLS::TYPE::ENTITY)){
                err << "Could not fix database, SQL was not transacted, no changes made;" << nl;
                sqlite3_finalize(stmt);
                sqlite3_close(db);
                so;
                return(rc);
            } else {
                rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
                if (rc){
                    err << "Error preparing: "<< sql << "\", code " << rc << "; Prepare failed twice, with code " << sqlite3_errmsg(db) << "\"" << nl;
                    sqlite3_finalize(stmt);
                    sqlite3_close(db);
                    so;
                    return(rc);
                }
            };

        }

        rc = sqlite3_step(stmt);
        if (!rc){
            err << "Error stepping: \"" << sql << "\", code " << rc << "; SQL was not transacted, no changes made" << std::endl << sqlite3_errmsg(db) << std::endl;
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            so;
            return(rc);
        }

        sql = "DELETE FROM ENTITIES WHERE UUID=?;";

        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr)){
            err << "Error preparing: "<< sql << "\", code " << rc << "; Skipping this statement. \n" << sqlite3_errmsg(db) << std::endl;
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            so;
            return(rc);
        }

        sw sw;

        for (auto u : toDelete){
            sqlite3_bind_int64(stmt, 1, u);
            sqlite3_step(stmt);
            sqlite3_reset(stmt);
            sw.lap();
        }

        sw.laprs();
        info << "Transacting entities.." << nl;


        sql = "END TRANSACTION;";
        rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
        if (rc){
            err << "Error preparing: "<< sql << "\", code " << rc << "; SQL was not transacted, no changes made; \"" << sqlite3_errmsg(db) << "\"" << nl;
            if (handlePrepareFail(rc, regionToDeleteFrom, FTOOLS::TYPE::ENTITY)){
                err << "Could not fix database, SQL was not transacted, no changes made;" << nl;
                sqlite3_finalize(stmt);
                sqlite3_close(db);
                so;
                return(rc);
            } else {
                rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
                if (rc){
                    err << "Error preparing: "<< sql << "\", code " << rc << "; Prepare failed twice, with code " << sqlite3_errmsg(db) << "\"" << nl;
                    sqlite3_finalize(stmt);
                    sqlite3_close(db);
                    so;
                    return(rc);
                }
            };

        }

        rc = sqlite3_step(stmt);
        if (!rc){
            err << "Error stepping: \"" << sql << "\", code " << rc << "; SQL was not transacted, no changes made" << std::endl << sqlite3_errmsg(db) << std::endl;
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            so;
            return(rc);
        }

        rc = sqlite3_finalize(stmt);
        if (rc){
            err << "Error finalizing call, code " << rc << "; SQL was not transacted, no changes made" << sqlite3_errmsg(db) << std::endl;
            sqlite3_close(db);
            so;
            return(rc);
        }
        sqlite3_close(db);
        sw.lap();
        info << "Finished transaction" << nl;
        so;
        return rc;

    }

    /**
     * The intended behavior of this is to be able to recall entire chunks at once.
     * recalling entities assumes that the current data within the entity vector shall treat
     * all data pulled from the database as older, therefore if there is a conflict and the
     * vector contains an entity by UUID that happens to be currently within the database, the
     * entity will not be overridden and the entity within the vector shall not be touched.
     *
     * The reasoning behind this is that any entity in play is assumed to have more relevant,
     * newer data than that of persistent storage, so subsequent reads to a vector that contains
     * duplicate entires shall not be honored.
     * @param entities
     * @param squareStart
     * @param squareEnd
     * @return
     */
    static int recallEntitiesFromRange(std::vector<ENTITY> * entities, COORDINATE::WORLDCOORD chunk){

        sqlite3 *db;
        std::string sql;
        sqlite3_stmt *stmt;
        int rc;

        si;

        std::string path = FTOOLS::parseFullPathFromRegionCoord(chunk.getRegioncoord(), FTOOLS::TYPE::ENTITY);
        rc = sqlite3_open(path.c_str(), &db);

        if(rc) {
            info << "Cannot open database files: " << path << ", trying to find directory structure.." << std::endl;
            if (!fixDBStructure(chunk.getRegioncoord(), FTOOLS::ENTITY)){
                err << "Cannot open entity database, error: " << sqlite3_errmsg(db) << nl;
                so;
                return(rc);
            } else {
                info << "Fixed database structure, continuing.." << nl;
                rc = sqlite3_open(path.c_str(), &db);
                if(rc) {
                    err << "Tried opening database but could not open after fix :'(" << nl;
                    so;
                    return rc;
                }
            }
        }

        info << "Recalling " << entities->size() << " entities from chunk " << chunk.getRegioncoord().x << ", " << chunk.getRegioncoord().y << nl;

        sql = "BEGIN TRANSACTION;";

        rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
        if (rc){
            err << "Error preparing: "<< sql << "\", code " << rc << "; SQL was not transacted, no changes made; \"" << sqlite3_errmsg(db) << "\"" << nl;
            if (handlePrepareFail(rc, chunk.getRegioncoord(), FTOOLS::TYPE::ENTITY)){
                err << "Could not fix database, SQL was not transacted, no changes made;" << nl;
                sqlite3_finalize(stmt);
                sqlite3_close(db);
                so;
                return(rc);
            } else {
                rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
                if (rc){
                    err << "Error preparing: "<< sql << "\", code " << rc << "; Prepare failed twice, with code " << sqlite3_errmsg(db) << "\"" << nl;
                    sqlite3_finalize(stmt);
                    sqlite3_close(db);
                    so;
                    return(rc);
                }
            };

        }

        rc = sqlite3_step(stmt);
        if (!rc){
            err << "Error stepping: \"" << sql << "\", code " << rc << "; SQL was not transacted, no changes made" << std::endl << sqlite3_errmsg(db) << std::endl;
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            so;
            return(rc);
        }

        //Translate our chunk coordinate to
        COORDINATE upperExtent = chunk.getCoordinate(), lowerExtent = chunk.getCoordinate();
        lowerExtent.x -= 64;
        lowerExtent.y -= 64;

        long long   xl = lowerExtent.x * 1000,
                    yl = lowerExtent.y * 1000,
                    xm = upperExtent.x * 1000,
                    ym = upperExtent.y * 1000;

        sql = "SELECT (ID, TYPE, X, Y, Z, DATA) FROM ENTITIES WHERE X BETWEEN ? AND ? AND WHERE Y BETWEEN ? AND ?";

        rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
        if (rc){
            err << "Error preparing: "<< sql << "\", code " << rc << "; SQL was not transacted, no changes made; \"" << sqlite3_errmsg(db) << "\"" << nl;
            if (handlePrepareFail(rc, chunk.getRegioncoord(), FTOOLS::TYPE::ENTITY)){
                err << "Could not fix database, SQL was not transacted, no changes made;" << nl;
                sqlite3_finalize(stmt);
                sqlite3_close(db);
                so;
                return(rc);
            } else {
                rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
                if (rc){
                    err << "Error preparing: "<< sql << "\", code " << rc << "; Prepare failed twice, with code " << sqlite3_errmsg(db) << "\"" << nl;
                    sqlite3_finalize(stmt);
                    sqlite3_close(db);
                    so;
                    return(rc);
                }
            };

        }

        sqlite3_bind_int64(stmt, 1, xl);
        sqlite3_bind_int64(stmt, 2, xm);
        sqlite3_bind_int64(stmt, 3, yl);
        sqlite3_bind_int64(stmt, 4, ym);

        sw sw;
        unsigned int numEntities = 0;
        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
            unsigned long long id = sqlite3_column_int64(stmt, 0);
            bool skip = false;
            for (auto entity : *entities) {
                if (entity.getUUID() == id) {
                    skip = true;
                    info << "Skipping UUID " << id << ", already exists" << nl;
                    break;
                }
            }
            if (skip) continue;
            //TYPE
            int bytes = sqlite3_column_bytes(stmt, 1);
            const unsigned char* tBytes = static_cast<const unsigned char*>(sqlite3_column_blob(stmt, 1));
            std::string type; type.assign(reinterpret_cast<const char*>(tBytes), bytes);
            //X
            long long x = sqlite3_column_int(stmt, 2);
            //Y
            long long y = sqlite3_column_int(stmt, 3);
            //Z
            int z = sqlite3_column_int(stmt, 4);
            //DATA
            bytes = sqlite3_column_bytes(stmt, 5);
            const unsigned char* idBytes = static_cast<const unsigned char*>(sqlite3_column_blob(stmt, 4));
            std::string data; data.assign(reinterpret_cast<const char*>(idBytes), bytes);

            ENTITY e ({x,y,z}, type, id);
            e.decodeEntityData(data);
            entities->push_back(e);
            info << "Entity \"" << e.getType() << "\" #" << id << " found and added" << nl;
            sw.lap();
            numEntities++;
        }
        sw.laprs();
        info << "Finished adding entities to the vector specified, " << numEntities << " added;" << nl;
        sqlite3_finalize(stmt);
        so;
        return rc;
    }

    /////////////////
    //TERRAIN SECTION
    /////////////////

    /**
     * Used to overwrite new data to the database for chunks, based on location. When providing a set of chunks,
     * a region coordinate must be used when doing this, since only one database file can be opened at once.
     * To allow for cascading database openings would drastically slow the systems at play with one function,
     * so the process should discriminate which chunks are actually in their spot or not, and ignore them if the
     * locational scope doesnt match.
     *
     * @param chunks
     * @return
     */
    static int saveTerrain(std::vector<CHUNK*> * chunks) {
        sqlite3 *db;
        std::string sql;
        sqlite3_stmt *stmt;
        int rc;
        si;

        std::set<COORDINATE::REGIONCOORD> regions;
        for (auto c : * chunks) {
            if (regions.contains(c->location.getRegioncoord())) continue;
            regions.insert(c->location.getRegioncoord());
        }

        for (auto r : regions) {
            std::string path = FTOOLS::parseFullPathFromRegionCoord(r, FTOOLS::TYPE::TERRAIN);
            rc = sqlite3_open(path.c_str(), &db);

            if (rc) {
                info << "Cannot open database files: " << path << ", trying to find directory structure.." << std::endl;
                if (!fixDBStructure(r, FTOOLS::TERRAIN)) {
                    err << "Cannot open entity database, error: " << sqlite3_errmsg(db) << nl;
                    so;
                    return (rc);
                } else {
                    info << "Fixed database structure, continuing.." << nl;
                    rc = sqlite3_open(path.c_str(), &db);
                    if (rc) {
                        err << "Tried opening database but could not open after fix :'(" << nl;
                        so;
                        return rc;
                    }
                }
            }


            // If updating temperature and humidity:
            bool climate = false;
            for (auto c : *chunks){
                if (c->changed){
                    climate = true;
                    break;
                }
            }

            if (climate){
                sql = "INSERT INTO CLIMATE(OFFSET, DATA) "
                      "VALUES(?, ?) "
                      "ON CONFLICT (OFFSET) DO "
                      "UPDATE SET DATA=? WHERE OFFSET=?";
                rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
                if (rc){
                    err << "Error preparing: "<< sql << "\", code " << rc << "; SQL was not transacted, no changes made; \"" << sqlite3_errmsg(db) << "\"" << nl;
                    if (handlePrepareFail(rc, r, FTOOLS::TYPE::CLIMATE)){
                        err << "Could not fix database, SQL was not transacted, no changes made;" << nl;
                        sqlite3_finalize(stmt);
                        sqlite3_close(db);
                        so;
                        return(rc);
                    } else {
                        rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
                        if (rc){
                            err << "Error preparing: "<< sql << "\", code " << rc << "; Prepare failed twice, with code " << sqlite3_errmsg(db) << "\"" << nl;
                            sqlite3_finalize(stmt);
                            sqlite3_close(db);
                            so;
                            return(rc);
                        }
                    };

                }

                for (auto c : *chunks){
                    if (c->location.getRegioncoord() != r) continue;
                    std::vector<unsigned char> data;
                    std::vector<unsigned char> compressedData;
                    if (c->changed) {
                        c->serializeClimate(&data);
                        CTOOLS::zcomp(reinterpret_cast<std::vector<char> *>(&data),
                                      reinterpret_cast<std::vector<char> *>(&compressedData));
                        int offset = c->location.getOffset();

                        sqlite3_bind_int(stmt, 1, offset);
                        sqlite3_bind_blob(stmt, 2, compressedData.data(), (unsigned long) compressedData.size(), SQLITE_STATIC);
                        sqlite3_bind_blob(stmt, 3, compressedData.data(), (unsigned long) compressedData.size(), SQLITE_STATIC);
                        sqlite3_bind_int(stmt, 4, offset);
                        sqlite3_step(stmt);
                        sqlite3_reset(stmt);
                    }
                }
            }

            sqlite3_finalize(stmt);
            sw sw;
            if (!chunks->empty()){
                sql = "INSERT INTO TERRAIN "
                      "VALUES(?, ?) "
                      "ON CONFLICT(OFFSET) "
                      "DO UPDATE SET DATA=? WHERE OFFSET=?;";

                rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
                if (rc){
                    err << "Error preparing: "<< sql << "\", code " << rc << "; SQL was not transacted, no changes made; \"" << sqlite3_errmsg(db) << "\"" << nl;
                    if (handlePrepareFail(rc, r, FTOOLS::TYPE::TERRAIN)){
                        err << "Could not fix database, SQL was not transacted, no changes made;" << nl;
                        sqlite3_finalize(stmt);
                        sqlite3_close(db);
                        so;
                        return(rc);
                    } else {
                        rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
                        if (rc){
                            err << "Error preparing: "<< sql << "\", code " << rc << "; Prepare failed twice, with code " << sqlite3_errmsg(db) << "\"" << nl;
                            sqlite3_finalize(stmt);
                            sqlite3_close(db);
                            so;
                            return(rc);
                        }
                    };

                }
                for (auto c : *chunks) {
                    if (c->location.getRegioncoord() != r) continue;
                    std::vector<unsigned char> data;
                    std::vector<unsigned char> compressedData;
                    int currentLocation = c->location.getOffset();
                    for (auto l : c->layers){
                        if (!l.changed) continue;
                        l.serializeLayer(&data);
                        CTOOLS::zcomp(reinterpret_cast<std::vector<char> *>(&data),
                                      reinterpret_cast<std::vector<char> *>(&compressedData));
                        int offset = l.getOffset(currentLocation);

                        info << "Saving offset " << offset << nl;

                        sqlite3_bind_int(stmt, 1, offset);
                        sqlite3_bind_blob(stmt, 2, compressedData.data(), (unsigned long) compressedData.size(), SQLITE_STATIC);
                        sqlite3_bind_blob(stmt, 3, compressedData.data(), (unsigned long) compressedData.size(), SQLITE_STATIC);
                        sqlite3_bind_int(stmt, 4, offset);
                        sqlite3_step(stmt);
                        sqlite3_reset(stmt);
                        sw.lap();
                    }
                }
                sw.laprs();

            }

            rc = sqlite3_finalize(stmt);
            if (rc){
                err << "Error finalizing call, code " << rc << "; SQL was not transacted, no changes made" << sqlite3_errmsg(db) << std::endl;
                sqlite3_finalize(stmt);
                sqlite3_close(db);
                so;
                return(rc);
            }
            sqlite3_close(db);
        }


        so;
        return rc;
    };

    static int recallTerrain(std::vector<CHUNK*> * chunks, std::vector<COORDINATE::WORLDCOORD> * missingAreas){
        si;

        std::set<COORDINATE::REGIONCOORD> regions;

        info << "Preparing for chunk recall, " << chunks->size() << " to save.." << nl;

        for (auto c : * chunks) {
            if (regions.contains(c->location.getRegioncoord())) continue;
            regions.insert(c->location.getRegioncoord());
        }

        info << "Recalling " << chunks->size() << " chunks from " << regions.size() << " distinct regions" << nl;

        sqlite3 *db;
        std::string sql;
        sqlite3_stmt *stmt;
        int rc;

        for (auto r : regions) {
            std::string path = FTOOLS::parseFullPathFromRegionCoord(r, FTOOLS::TYPE::TERRAIN);
            rc = sqlite3_open(path.c_str(), &db);

            if(rc) {
                info << "Cannot open database files: " << path << ", trying to find directory structure.." << std::endl;
                if (!fixDBStructure(r, FTOOLS::TERRAIN)){
                    err << "Cannot open entity database, error: " << sqlite3_errmsg(db) << nl;
                    so;
                    return(rc);
                } else {
                    info << "Fixed database structure, continuing.." << nl;
                    rc = sqlite3_open(path.c_str(), &db);
                    if(rc) {
                        err << "Tried opening database but could not open after fix :'(" << nl;
                        so;
                        return rc;
                    }
                }
            }

            /*
             * When selecting, there is a chance, high at that, that your database will not contain a chunk, and when
             * this happens, we should mark the occurrence and also skip this operation. We dont need to save exactly when we generate it,
             * only when we ask to save it.
             */
            sql = "SELECT DATA FROM CLIMATE WHERE OFFSET=?;";
            rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
            if (rc){
                err << "Error preparing: "<< sql << "\", code " << rc << "; SQL was not transacted, no changes made; \"" << sqlite3_errmsg(db) << "\"" << nl;
                if (handlePrepareFail(rc, r, FTOOLS::TYPE::CLIMATE)){
                    err << "Could not fix database, SQL was not transacted, no changes made;" << nl;
                    sqlite3_finalize(stmt);
                    sqlite3_close(db);
                    so;
                    return(rc);
                } else {
                    rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
                    if (rc){
                        err << "Error preparing: "<< sql << "\", code " << rc << "; Prepare failed twice, with code " << sqlite3_errmsg(db) << "\"" << nl;
                        sqlite3_finalize(stmt);
                        sqlite3_close(db);
                        so;
                        return(rc);
                    }
                };

            }

            for (auto c : *chunks){
                if (c->location.getRegioncoord() != r) continue;
                sqlite3_bind_int(stmt, 0, c->location.getOffset());
                if ((rc = sqlite3_step(stmt)) == SQLITE_ROW){
                    const void* blob = sqlite3_column_blob(stmt, 0);
                    int blobSize = sqlite3_column_bytes(stmt, 0);
                    std::vector<unsigned char> data;
                    std::vector<unsigned char> uncompressedData;
                    data.assign(static_cast<const char*>(blob), static_cast<const char*>(blob) + blobSize);

                    CTOOLS::zuncomp(reinterpret_cast<std::vector<char> *>(&data),
                                    reinterpret_cast<std::vector<char> *>(&uncompressedData));
                    c->deserializeClimate(&uncompressedData);
                } else {
                    missingAreas->push_back(c->location);
                }
                sqlite3_reset(stmt);
            }

            sql = "SELECT DATA, OFFSET FROM TERRAIN WHERE OFFSET BETWEEN ? AND ?;";
            rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
            if (rc){
                err << "Error preparing: "<< sql << "\", code " << rc << "; SQL was not transacted, no changes made; \"" << sqlite3_errmsg(db) << "\"" << nl;
                if (handlePrepareFail(rc, r, FTOOLS::TYPE::TERRAIN)){
                    err << "Could not fix database, SQL was not transacted, no changes made;" << nl;
                    sqlite3_finalize(stmt);
                    sqlite3_close(db);
                    so;
                    return(rc);
                } else {
                    rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
                    if (rc){
                        err << "Error preparing: "<< sql << "\", code " << rc << "; Prepare failed twice, with code " << sqlite3_errmsg(db) << "\"" << nl;
                        sqlite3_finalize(stmt);
                        sqlite3_close(db);
                        so;
                        return(rc);
                    }
                };

            }



            for (auto c : *chunks){
                if (c->location.getRegioncoord() != r) continue;
                int offset = c->location.getOffset() * 24;
                info << "Pulling from offset " << offset << " to " << offset + 24 << nl;
                sqlite3_bind_int(stmt, 1, offset);
                sqlite3_bind_int(stmt, 2, offset + 24);

                while ((rc = sqlite3_step(stmt)) == SQLITE_ROW){
                    const void* blob = sqlite3_column_blob(stmt, 0);
                    int blobSize = sqlite3_column_bytes(stmt, 0);
                    int layer = sqlite3_column_int(stmt, 1);
                    info << "Handling layer " << offset - layer << nl;
                    std::vector<unsigned char> data;
                    std::vector<unsigned char> uncompressedData;
                    data.assign(static_cast<const char*>(blob), static_cast<const char*>(blob) + blobSize);

                    CTOOLS::zuncomp(reinterpret_cast<std::vector<char> *>(&data),
                                    reinterpret_cast<std::vector<char> *>(&uncompressedData));
                    info << "Data: " << data.size() << ", Uncompressed: " << uncompressedData.size() << nl;

                    c->layers[offset - layer].deserializeLayer(&uncompressedData);
                }
                sqlite3_reset(stmt);
            }
            sqlite3_finalize(stmt);

        }

        info << "Finished recalling " << chunks->size() << " chunks;" << nl;
        so;
        return rc;
    }

    /////////////////
    // Block Section
    /////////////////

    static int saveBlocks(CHUNK * chunk){
        si;
        sqlite3 *db;
        std::string sql;
        sqlite3_stmt *stmt;
        int rc;


        std::string path = FTOOLS::parseFullPathFromRegionCoord(chunk->location.getRegioncoord(), FTOOLS::TYPE::BLOCK);
        rc = sqlite3_open(path.c_str(), &db);

        if(rc) {
            info << "Cannot open database files: " << path << ", trying to find directory structure.." << std::endl;
            if (!fixDBStructure(chunk->location.getRegioncoord(), FTOOLS::BLOCK)){
                err << "Cannot open entity database, error: " << sqlite3_errmsg(db) << nl;
                so;
                return(rc);
            } else {
                info << "Fixed database structure, continuing.." << nl;
                rc = sqlite3_open(path.c_str(), &db);
                if(rc) {
                    err << "Tried opening database but could not open after fix :'(" << nl;
                    so;
                    return rc;
                }
            }
        }

        unsigned char tmp[9];
        std::vector<unsigned char> rawBlocks;
        sw sw;
        for (auto b : chunk->blocks){
            b.serialize(tmp);
            rawBlocks.insert(rawBlocks.end(), tmp, tmp + 9);
        }

        std::vector<unsigned char> compressedBlocks;
        CTOOLS::zcomp(reinterpret_cast<std::vector<char> *>(&rawBlocks),
                      reinterpret_cast<std::vector<char> *>(&compressedBlocks));

        sql = "INSERT INTO BLOCKS(OFFSET,DATA) VALUES(?, ?) "
              "ON CONFLICT(OFFSET) DO "
              "UPDATE SET DATA=? WHERE OFFSET=?;";

        rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
        if (rc){
            err << "Error preparing: "<< sql << "\", code " << rc << "; SQL was not transacted, no changes made; \"" << sqlite3_errmsg(db) << "\"" << nl;
            if (handlePrepareFail(rc, chunk->location.getRegioncoord(), FTOOLS::TYPE::BLOCK)){
                err << "Could not fix database, SQL was not transacted, no changes made;" << nl;
                sqlite3_finalize(stmt);
                sqlite3_close(db);
                so;
                return(rc);
            } else {
                rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
                if (rc){
                    err << "Error preparing: "<< sql << "\", code " << rc << "; Prepare failed twice, with code " << sqlite3_errmsg(db) << "\"" << nl;
                    sqlite3_finalize(stmt);
                    sqlite3_close(db);
                    so;
                    return(rc);
                }
            };

        }
        int offset = chunk->location.getOffset();
        sqlite3_bind_int(stmt, 1, offset);
        sqlite3_bind_blob(stmt, 2, compressedBlocks.data(), compressedBlocks.size(), SQLITE_STATIC);
        sqlite3_bind_blob(stmt, 3, compressedBlocks.data(), compressedBlocks.size(), SQLITE_STATIC);
        sqlite3_bind_int(stmt, 4, offset);

        rc = sqlite3_step(stmt);
        if (!rc){
            err << "Error stepping: \"" << sql << "\", code " << rc << "; SQL was not transacted, no changes made" << std::endl << sqlite3_errmsg(db) << std::endl;
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            so;
            return(rc);
        }
        rc = sqlite3_finalize(stmt);
        if (rc){
            err << "Error finalizing call, code " << rc << "; SQL was not transacted, no changes made" << sqlite3_errmsg(db) << std::endl;
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            so;
            return(rc);
        }
        sw.laprs();
        sqlite3_close(db);
        so;
        return rc;
    }

    static int recallBlocks(CHUNK * chunk){
        si;
        sqlite3 *db;
        std::string sql;
        sqlite3_stmt *stmt;
        int rc;

        std::string path = FTOOLS::parseFullPathFromRegionCoord(chunk->location.getRegioncoord(), FTOOLS::TYPE::BLOCK);
        rc = sqlite3_open(path.c_str(), &db);

        if(rc) {
            info << "Cannot open database files: " << path << ", trying to find directory structure.." << std::endl;
            if (!fixDBStructure(chunk->location.getRegioncoord(), FTOOLS::BLOCK)){
                err << "Cannot open entity database, error: " << sqlite3_errmsg(db) << nl;
                so;
                return(rc);
            } else {
                info << "Fixed database structure, continuing.." << nl;
                rc = sqlite3_open(path.c_str(), &db);
                if(rc) {
                    err << "Tried opening database but could not open after fix :'(" << nl;
                    so;
                    return rc;
                }
            }
        }

        sql = "SELECT DATA FROM BLOCKS WHERE OFFSET = ?;";
        rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
        if (rc){
            err << "Error preparing: "<< sql << "\", code " << rc << "; SQL was not transacted, no changes made; \"" << sqlite3_errmsg(db) << "\"" << nl;
            if (handlePrepareFail(rc, chunk->location.getRegioncoord(), FTOOLS::TYPE::BLOCK)){
                err << "Could not fix database, SQL was not transacted, no changes made;" << nl;
                sqlite3_finalize(stmt);
                sqlite3_close(db);
                so;
                return(rc);
            } else {
                rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
                if (rc){
                    err << "Error preparing: "<< sql << "\", code " << rc << "; Prepare failed twice, with code " << sqlite3_errmsg(db) << "\"" << nl;
                    sqlite3_finalize(stmt);
                    sqlite3_close(db);
                    so;
                    return(rc);
                }
            };

        }

        int offset = chunk->location.getOffset();
        info << "Selecting from offset " << offset << nl;
        sqlite3_bind_int(stmt, 1, offset);

        if ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
            std::vector<unsigned char> compressedBlocks;
            std::vector<unsigned char> rawBlocks;

            const void* blob = sqlite3_column_blob(stmt, 0);
            int blobSize = sqlite3_column_bytes(stmt, 0);

            compressedBlocks.assign(static_cast<const char*>(blob), static_cast<const char*>(blob) + blobSize);

            CTOOLS::zuncomp(reinterpret_cast<std::vector<char> *>(&compressedBlocks),
                            reinterpret_cast<std::vector<char> *>(&rawBlocks));

            if (rawBlocks.size() % 9 != 0){
                warn << "Sizeof rawBlocks was not perfectly divisible by 9, last block datum will be truncated resulting in data loss/corruption!" << nl;
            }
            unsigned char tmp[9];

            sw sw;
            int track = 0;
            chunk->blocks.clear();

            for (auto c : rawBlocks){
                if (track < 8){
                    tmp[track] = c;
                    track++;
                } else {
                    BLOCK b;
                    tmp[track] = c;
                    b.deserialize(tmp);
                    chunk->blocks.push_back(b);
                    track = 0;
                }
            }
            sw.laprs();
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            so;
            return 0;
        } else {
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            info << "No data found at record offset for chunk " << chunk->location.x << "x " << chunk->location.y << "y" << nl;
            so;
            return -1;
        }
    }
};
#endif //HALCYONICUS_HDB_H
