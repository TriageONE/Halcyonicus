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

    };

    /**
     * Defines an area in the world as a chunk, a 16x16 area of tiles
     */
    class WORLDCOORD{
    public:
        int x=0, y=0;

        WORLDCOORD(int x, int y){
            this->x = x;
            this->y = y;
        }

        WORLDCOORD()= default;

        REGIONCOORD getWorldcoord(){
            int x1 = this->x >> 4;
            int y1 = this->y >> 4;
            return {x1, y1};
        }
    };


    /**
     * Defines an area in the world that entities have as an attribute
     */
    float x=0, y=0;
    char z=0;

    COORDINATE(float x, float y, char z){
        this->x = x;
        this->y = y;
        this->z = z;
    }

    COORDINATE()= default;

    WORLDCOORD getWorldcoord(){
        int x1 = ( (int) this->x ) >> 4;
        int y1 = ( (int) this->y ) >> 4;
        return {x1, y1};
    }
};





#endif //HALCYONICUS_COORDINATE_H
