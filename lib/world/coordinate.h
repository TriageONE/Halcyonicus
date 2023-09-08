//
// Created by Aron Mantyla on 8/12/23.
//

#ifndef HALCYONICUS_COORDINATE_H
#define HALCYONICUS_COORDINATE_H
class COORDINATE {
public:
    class REGIONCOORD{
    public:
        int x=0, y=0;

        REGIONCOORD(int x, int y){
            this->x = x;
            this->y = y;
        }

        REGIONCOORD()= default;

        friend bool operator!=(const REGIONCOORD& lhs, const REGIONCOORD& rhs) {
            return !(lhs == rhs);
        }

        friend bool operator==(const REGIONCOORD& lhs, const REGIONCOORD& rhs) {
            return (lhs.x == rhs.x) && (lhs.y == rhs.y);
        }

        friend bool operator<(const REGIONCOORD& lhs, const REGIONCOORD& rhs) {
            return (lhs.x < rhs.x) && (lhs.y < rhs.y);
        }

    };

    /**
     * Defines an area in the world as a chunk, a 64x64 area of tiles
     */
    class WORLDCOORD{
    public:
        int x=0, y=0;

        WORLDCOORD(int x, int y) : x{x}, y{y} {}

        WORLDCOORD()= default;

        REGIONCOORD getRegioncoord(){
            int x1 = this->x >> 6;
            int y1 = this->y >> 6;
            return {x1, y1};
        }

        friend bool operator!=(const WORLDCOORD& lhs, const WORLDCOORD& rhs) {
            return !(lhs == rhs);
        }

        friend bool operator==(const WORLDCOORD& lhs, const WORLDCOORD& rhs) {
            return (lhs.x == rhs.x) && (lhs.y == rhs.y);
        }
    };

    /**
     * Defines an exact area in which an entity resides in the world using fixed point location data
     */
    class ENTITYCOORD{
    public:
        long long x=0, y=0;
        int z=0;

        ENTITYCOORD(long long x, long long y, int z) : x{x}, y{y}, z{z} {}

        ENTITYCOORD() = default;

        WORLDCOORD getWorldcoord(){
            int x1 = ( this->x/1000 ) >> 6;
            int y1 = ( this->y/1000 ) >> 6;
            return {x1, y1};
        }

        REGIONCOORD getRegioncoord(){
            return getWorldcoord().getRegioncoord();
        }

        friend bool operator!=(const ENTITYCOORD& lhs, const ENTITYCOORD& rhs) {
            return !(lhs == rhs);
        }

        friend bool operator==(const ENTITYCOORD& lhs, const ENTITYCOORD& rhs) {
            return (lhs.x == rhs.x) && (lhs.y == rhs.y) && (lhs.z == rhs.z);
        }
    };


    /**
     * Defines an area in the world that blocks have as an attribute
     */
    int x=0, y=0;
    short z=0;

    COORDINATE(int x, int y, short z){
        this->x = x;
        this->y = y;
        this->z = z;
    }

    COORDINATE()= default;

    WORLDCOORD getWorldcoord(){
        int x1 = ( (int) this->x ) >> 6;
        int y1 = ( (int) this->y ) >> 6;
        return {x1, y1};
    }

    REGIONCOORD getRegioncoord(){
        return getWorldcoord().getRegioncoord();
    }

    friend bool operator!=(const COORDINATE& lhs, const COORDINATE& rhs) {
        return !(lhs == rhs);
    }

    friend bool operator==(const COORDINATE& lhs, const COORDINATE& rhs) {
        return (lhs.x == rhs.x) && (lhs.y == rhs.y) && (lhs.z == rhs.z);
    }
};





#endif //HALCYONICUS_COORDINATE_H
