//
// Created by Aron Mantyla on 3/29/23.
//

#ifndef HALCYONICUS_ENTITYSTORAGE_H
#define HALCYONICUS_ENTITYSTORAGE_H

/**
 * A static class responsible for manipulating entities within files and performing mass serializations of entities in
 * memory
 *
 * This could be a huge class. This class may need to completely rewrite an entire region file. When we want to read from
 * the region file, we should hold the entities in memory and manipulate them in an instance and then write that instance back to
 * storage consistently.
 *
 * The system fills up chunks of memory called an EntityChunk. Entities are stored within EntityChunks, and EntityChunks
 * are part of EntityRegions.
 *
 * EntityRegions may or may not be fully loaded. we can load in entire chunks at a time, but should not load in entire regions at a time.
 * Each chunk contains a vector where we can push and remove entites from. If-
 * Wait! when we have 256 vectors, thats a lot of memory- but is it really? what if we had all the entities in one bucket?
 * performing actions on large vectors is difficult. By splitting vectors into many chunks, we can guarantee not all objects
 * have to be loaded and can operate as required
 *
 * once we are ready to serialize the vector for saving entities, thats a bit difficult, now we should have a way to guarantee
 * that the system has a stable snapshot, so we need to lock the entire region and copy it to a predefined area, however large it may be,
 * then with that static snapshot of the environment
 *
 * The only problem is working with creating a system where we can store every entity in a disk.
 */
class ENTITYSTORAGE{

};
#endif //HALCYONICUS_ENTITYSTORAGE_H
