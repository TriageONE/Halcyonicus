//
// Created by Triage on 4/1/2023.
//

#ifndef HALCYONICUS_HDB_H
#define HALCYONICUS_HDB_H


#include <set>
#include "../sqlite/sqlite3.h"
#include "../world/coordinate.h"
#include "../tools/filetools.h"
#include "../entity/entity.h"
#include "../logging/hlogger.h"

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

    static bool fixDBStructure() {
        si;
        if (FTOOLS::checkForDirectoryStructure()){
            info << "Directory structure found as present, perhaps there is a permission node missing?" << std::endl;
            so;
            return false;
        } else {
            info << "Partial/missing directory structure, rebuilding.." << nl;
            if (FTOOLS::createDirectories()){
                info << "Created directory structure" << nl;
                so;
                return true;
            } else {
                err << "Could not create directory structure" << nl;
                so;
                return false;
            }
        }

    }

    /**
     * Creates a new entity database using the region coordinates specified
     * @param regioncoord The region to create the database for
     * @param type the type of database to create
     * @return the SQL integer return code
     */
    int createNewDatabase(COORDINATE::REGIONCOORD regioncoord, FTOOLS::TYPE type){
        sqlite3 *db;
        std::string sql;
        sqlite3_stmt *stmt;
        int rc;
        si;

        std::string t;

        switch (type) {
            case FTOOLS::TYPE::TERRAIN:
                t = "world";
                break;
            case FTOOLS::TYPE::ENTITY:
                t = "entities";
                break;
            case FTOOLS::TYPE::BLOCK:
                t = "blocks";
                break;
            case FTOOLS::TYPE::DATA:
                t = "data";
                break;
        }
        info << "Creating new database: " << t << nl;

        std::string path = FTOOLS::prependDirectory(FTOOLS::parseRegioncoordToFilename(regioncoord, type), type);
        rc = sqlite3_open(path.c_str(), &db);

        if(rc) {
            info << "Cannot open database files: " << path << ", trying to find directory structure.." << std::endl;
            if (!fixDBStructure()){
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

        info << "Database " << t << " created, trying database construction.." << nl;

        switch (type) {
            case FTOOLS::TYPE::TERRAIN:
                sql = "CREATE TABLE IF NOT EXISTS TERRAIN("
                      "OFFSET SMALLINT NOT NULL PRIMARY KEY, "
                      "LAYER CHAR NOT NULL, "
                      "DATA TINYBLOB NOT NULL);";
                break;
            case FTOOLS::TYPE::ENTITY:
                sql = "CREATE TABLE IF NOT EXISTS ENTITIES("
                      "ID BIGINT NOT NULL PRIMARY KEY, "
                      "X BIGINT NOT NULL, "
                      "Y BIGINT NOT NULL, "
                      "Z INT NOT NULL, "
                      "DATA BLOB);";
                break;
            case FTOOLS::TYPE::BLOCK:
                //TODO: Find how the fuck im gonna do this
                break;
            case FTOOLS::TYPE::DATA:
                goto skip;
        }

        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr)){
            err << "Error preparing " << t << " database persistence section creation statement for path: "<< path << ", error code " << rc << " \"" << sqlite3_errmsg(db) << "\"" << nl;
            sqlite3_close(db);
            so;
            return(rc);
        }


        rc = sqlite3_step(stmt);
        if (!rc){
            err << "Error executing " << t << " database persistence section creation sql statement, but passed preparation for path: " << path << ", error code " << rc << " \"" << sqlite3_errmsg(db) << "\"" << nl;
            sqlite3_close(db);
            so;
            return(rc);
        }
        info << "Successful database table creation, creating data table.." << nl;

        skip:
        sql = "CREATE TABLE IF NOT EXISTS HALDATA("
              "KEY STRING NOT NULL, "
              "VALUE STRING NOT NULL);";

        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr)){
            err << "Error preparing " << t << " database data section creation statement for path: "<< path << ", error code " << rc << "\"" << sqlite3_errmsg(db) << "\"" << nl;
            sqlite3_close(db);
            so;
            return(rc);
        }

        rc = sqlite3_step(stmt);
        if (!rc){
            err << "Error executing " << t << " database data section creation sql statement, but passed preparation for path: " << path << ", error code " << rc << "\"" << sqlite3_errmsg(db) << "\"" << nl;
            sqlite3_close(db);
            so;
            return(rc);
        }
        rc = sqlite3_finalize(stmt);

        if (rc){
            err << "Error finalizing " << t << " database data section creation sql statement, but passed stepping for path: " << path << ", error code " << rc << "\"" << sqlite3_errmsg(db) << "\"" << nl;
            sqlite3_close(db);
            so;
            return(rc);
        }

        info << "Successful database data table creation, database finalized." << nl;

        so;
        return (rc);
    }

    //TODO: Try this out, i have no idea if this works properly or not
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
            if (!fixDBStructure()){
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
            } /*else {
                unmovedUUIDs.insert(std::make_pair(e.getUUID(), e.getLastSavedLocation().getRegioncoord()));
            }*/

        }
        info << "Saving " << entities->size() << " entities to region " << regionToSaveTo.x << ", " << regionToSaveTo.y << nl;
        //Rather have a duplicate than no entity, therefore load the entities as entries into the database using one transaction:

        sql = "BEGIN TRANSACTION;";

        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr)){
            err << "Error preparing: "<< sql << "\", code " << rc << "; SQL was not transacted, no changes made" << std::endl << sqlite3_errmsg(db) << std::endl;
            sqlite3_close(db);
            so;
            return(rc);
        }

        rc = sqlite3_step(stmt);
        if (!rc){
            err << "Error stepping: \"" << sql << "\", code " << rc << "; SQL was not transacted, no changes made" << std::endl << sqlite3_errmsg(db) << std::endl;
            sqlite3_close(db);
            so;
            return(rc);
        }

        long long currentTime = TIMETOOLS::getCurrentEpochTime();
        std::vector<char> data;

        sql = "INSERT INTO ENTITIES(ID, X, Y, Z, DATA) VALUES(?,?,?,?,?) "
              "ON CONFLICT(ID) DO "
              "UPDATE SET X=?, Y=?, Z=?, DATA=?;";
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr)){
            err << "Error preparing: "<< sql << "\", code " << rc << "; Skipping this statement. \n" << sqlite3_errmsg(db) << std::endl;
            sqlite3_close(db);
            so;
            return(rc);
        }

        sw sw;

        for (ENTITY e : *entities){
            if (toIgnore.contains(e.getUUID())) continue;
            //Perform work on the entitiy so we can have all data handled when we start binding data
            e.updateTimestamp(currentTime);
            e.serializeEntity(&data);

            //Bind all of our data
            sqlite3_bind_int64(stmt, 1, e.getUUID());
            sqlite3_bind_int64(stmt, 2, e.getLocation().x);
            sqlite3_bind_int64(stmt, 3, e.getLocation().y);
            sqlite3_bind_int(stmt, 4, e.getLocation().z);
            sqlite3_bind_blob(stmt, 5, data.data(), data.size(), SQLITE_STATIC);

            //Second half of the statement
            sqlite3_bind_int64(stmt, 6, e.getLocation().x);
            sqlite3_bind_int64(stmt, 7, e.getLocation().y);
            sqlite3_bind_int(stmt, 8, e.getLocation().z);
            sqlite3_bind_blob(stmt, 9, data.data(), data.size(), SQLITE_STATIC);

            //Step through after data binds are complete to
            sqlite3_step(stmt);
            sqlite3_reset(stmt);
            sw.lap();

        }
        sw.laprs();
        info << "Transacting entities.." << nl;

        sql = "END TRANSACTION;";
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr)){
            err << "Error preparing: "<< sql << "\", code " << rc << "; SQL was not transacted, no changes made.\n" << std::endl << sqlite3_errmsg(db) << std::endl;
            sqlite3_close(db);
            so;
            return(rc);
        }

        rc = sqlite3_step(stmt);
        if (!rc){
            err << "Error stepping: \"" << sql << "\", code " << rc << "; SQL was not transacted, no changes made" << std::endl << sqlite3_errmsg(db) << std::endl;
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
        long long numDuplicates = movedUUIDs.size();
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
                if (!fixDBStructure()){
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

            if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr)) {
                err << "Error preparing: " << sql << "\", code " << rc
                          << "; SQL was not transacted, no changes made" << std::endl << sqlite3_errmsg(db)
                          << std::endl;
                sqlite3_close(db);
                so;
                return (rc);
            }

            rc = sqlite3_step(stmt);
            if (!rc) {
                err << "Error stepping: \"" << sql << "\", code " << rc
                          << "; SQL was not transacted, no changes made" << std::endl << sqlite3_errmsg(db)
                          << std::endl;
                sqlite3_close(db);
                so;
                return (rc);
            }

            sql = "DELETE FROM ENTITIES WHERE UUID=?;";

            if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr)) {
                err << "Error preparing: " << sql << "\", code " << rc << ";\n" << sqlite3_errmsg(db)
                          << std::endl;
                sqlite3_close(db);
                so;
                return (rc);
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
};
#endif //HALCYONICUS_HDB_H
