#ifndef LEVEL_H
#define LEVEL_H

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics.hpp>

#include <map>
#include <stack>
#include <tuple>
#include <set>
#include <algorithm>
#include <array>
#include "Object.hpp"
#include "O_Living.hpp" // Living muss über Facility und Item, weil die den Player-Typ kennen müssen.
#include "O_Facility.hpp"
#include "O_Item.hpp"

// Stellt ein Level dar und verwaltet die Map etc.

class Level {
public:
    // OLD
    sf::Font font;
    //////////////////////////////////////////////////////////////////////////////////////////////
private:
    Object** map;
    Player* player;
    int mapsizex, mapsizey;

    
public:
    
    Level(uint64_t x, uint64_t y)
        : mapsizex(x), mapsizey(y)
    {

        map = (Object**)(malloc((2 + x * y) * sizeof(Object *)));
        for(int i=0; i<(x*y); i++){
            map[i] = nullptr;
        }
        map[0] = (Object*)x;
        map[1] = (Object*)y;

        printf("Map allocated \n");

        Object::loadTexsheets();

        // OLD:
        // textures = (Tex**)(malloc(5 * sizeof(uint8_t) * sizeof(Tex *)));
        //loadTextures();
        loadFonts();
        ////

        //player = new Player(map, 4, 1);
        /*
        new Item(map, 10, 3);
        new Item(map, 10, 6);
        
        new Destroyer(map, 10, 10);

        new Goal(map, 15, 10);
        
        
        new Barrier(map, 0, 0);
        new Barrier(map, 0, 1);
        new Barrier(map, 0, 2);
        new Barrier(map, 1, 2);
        new Barrier(map, 2, 2);
        new Barrier(map, 3, 3);
        new Barrier(map, 4, 3);
        new Barrier(map, 5, 3);
        new Barrier(map, 6, 3);
        */

        buildOuterBorders();
        dfs(sf::Vector2u(20, 20));
    }

    ~Level(){
        for(int x = 0; x < mapsizex; x++){
            for(int y = 0; y < mapsizey; y++){
                Object* obj = getObject(sf::Vector2u(x, y));
                if(obj != nullptr){
                    delete obj;
                    //std::cout << "deleted object on ("<<x<<" , "<<y<<").\n";
                }
            }
        }
        free(map);
        map = nullptr;
        printf("Map freed \n");
    }
    
    //////////////////////////////////////////////////////////////////////////////////////////////

    Player* getPlayer(){return player;};
    int getMapX(){return mapsizex;};
    int getMapY(){return mapsizey;};



    Object** getNode(sf::Vector2u position){
        if(position.x*position.y >= mapsizex*mapsizey +2
            || position.x*position.y < 0
        ){
            std::cout<< "ERROR: Position (" << position.x <<", "<< position.y <<") out of Array!\n";
            return nullptr;
        }
        return &map[position.x * mapsizey + position.y +2];
    }
    Object* getObject(sf::Vector2u position){
        if(position.x*position.y >= mapsizex*mapsizey +2
            || position.x*position.y < 0
        ){
            std::cout<< "ERROR: Position (" << position.x <<", "<< position.y <<") out of Array!\n";
            return nullptr;
        }
        return *getNode(position);
    }


private:

//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// sf::Text text("Hello SFML", font, 30);
/*/ Load a music to play
        sf::Music music;
        if (!music.openFromFile("nice_music.ogg"))
            return EXIT_FAILURE;
        // Play the music
        music.play();
         */

void loadFonts() {
    if (!font.loadFromFile("../include/KulimPark-Regular.ttf"))
        printf("ERROR: FONT NOT LOADED!");
        //return EXIT_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////

/*
- Würfle Size
- Würfle Startposition
- Mache DFS und merke alle Stellen, an denen eine Barrier entfernt wurde sowie die Endposition (max Depth)
- Bilde Komplement und setze alle Barriers
- Setze Player & Goal an Start und End
*/

// size für dfs ist nicht size der Map! 1 dfs-feld ^= 4 map-feldern plus 1 Feld Zwischenraum für Borders.
void dfs(sf::Vector2u size){
    // Achtung: std::set geht mit sf::Vector2 nicht, weil angeblich nicht vergleichbar. Dummer Compiler. -.-
    // Daher wird hier mit std::arrays statt sf::Vector2 gearbeitet

    uint scalar = 2; // wie viele Mapfields hat ein DFS-Node (zu deutsch: wie breit sind die Gänge)


    std::array<uint, 2> start, end;
    std::set<std::array<uint, 2>> noWall; // durchgang, hier keine wall platzieren
    std::stack<std::array<uint, 2>> maxPath;


    auto getRandomStartposition = [size]() -> std::array<uint, 2>
    {
        // Startposition soll am Rand sein, daher ist das Unterfangen etwas komplexer als ein Einzeiler.
        return {1,1}; // TODO provisorisch
    };


    ////////////////////////////////////////////////////////////////////////////////////////
    /////////// Conversion from DFS-Nodes to Map-Nodes
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    auto getMapFields = [scalar](std::array<uint, 2> node) -> std::set<std::array<uint, 2>>
    {
        std::set<std::array<uint, 2>> mapfields;
        for (uint x=1; x<=scalar; x++){
            for (uint y=1; y<=scalar; y++){
                mapfields.insert({ (node[0] * (scalar+1) + x ) ,  (node[1] * (scalar+1) + y ) });
            }
        }
        return mapfields;
    };
    auto getMapField = [getMapFields](std::array<uint, 2> node) -> std::array<uint, 2>
    {
        std::set<std::array<uint, 2>> mapfields = getMapFields(node);
        uint r = rand() % mapfields.size();
        std::set<std::array<uint, 2>>::iterator it = mapfields.begin();
        std::advance(it, r);
        return *it;
    };
    auto getBorderFields = [scalar](std::array<uint, 2> node1, std::array<uint, 2> node2) -> std::set<std::array<uint, 2>>
    {
        std::cout << "Got Wall-Fields between Positions:  ("<< node1[0] <<", "<< node1[1] <<") - ("<< node2[0] <<", "<< node2[1] <<"):\n";

        std::set<std::array<uint, 2>> mapfields;
        if(node1[0] == node2[0] + 1 && node1[1] == node2[1]){ // node2 ist links von node1
            std::cout << "\tLEFT\n";
            for (uint y=1; y<=scalar; y++){
                mapfields.insert({ (node1[0] * (scalar+1) ) ,  (node1[1] * (scalar+1) + y ) });
            }
        }else if(node1[0] == node2[0] - 1 && node1[1] == node2[1]){ // node2 ist rechts von node1
            std::cout << "\tRIGHT\n";
            for (uint y=1; y<=scalar; y++){
                mapfields.insert({ ((node1[0] * (scalar+1)) + scalar +1 ) ,  (node1[1] * (scalar+1) + y ) });
            }
        }else if(node1[1] == node2[1] + 1 && node1[0] == node2[0]){ // node2 ist unter node1
            std::cout << "\tUNDER\n";
            for (uint x=1; x<=scalar; x++){
                mapfields.insert({ ((node1[0] * (scalar+1)) + x) ,  (node1[1] * (scalar+1)  ) });
            }
        }else if(node1[1] == node2[1] - 1 && node1[0] == node2[0]){ // node2 ist über node1
            std::cout << "\tOVER\n";
            for (uint x=1; x<=scalar; x++){
                mapfields.insert({ ((node1[0] * (scalar+1)) + x) ,  (node1[1] * (scalar+1) + scalar + 1) });
            }
        }
        
        for( auto i : mapfields){
            std::cout << "\t("<< i[0] <<", "<< i[1] <<")\n";
        }
        std::cout << std::endl;
        return mapfields;
    };
    ///////////////////////////////////////////////////////////////////////////////////////////////////


    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////// DFS
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::stack<std::array<uint, 2>> stack;
    std::set<std::array<uint, 2>> visited;

    auto getUnvisitedNeighbors = [size](std::array<uint, 2> node, std::set<std::array<uint, 2>> visited) -> std::set<std::array<uint, 2>>
    {
        std::set<std::array<uint, 2>> neighbors; // possible neighbors

        if(node[0] < size.x -1) neighbors.insert({ node[0] +1, node[1] }); // right
        if(node[0] > 0) neighbors.insert({ node[0] -1, node[1] }); // left
        if(node[1] < size.y -1) neighbors.insert({ node[0], node[1] +1 }); // down
        if(node[1] > 0) neighbors.insert({ node[0], node[1] -1 }); // up

        /*
        // Wirft RAM Exception aus unerfimndlichen gründen ...?
        // Gelernt: Man darf Sets beim iterieren natürlich nicht verändern!
        for(std::array<uint, 2> nb : neighbors) {
            if(visited.contains(nb)){
                neighbors.erase(nb);
            }
        }
        */

        std::set<std::array<uint, 2>> neighborsNotVisited;
        std::set_difference(
            neighbors.begin(),neighbors.end(), 
            visited.begin(), visited.end(), 
            std::inserter(neighborsNotVisited, neighborsNotVisited.end())
        ); // Junge, einfach nein, was ist das für 1 syntax?! -.-
        

        return neighborsNotVisited;
    };

    start = getRandomStartposition();

    stack.push(start);
    visited.insert(start);

    uint maxdepth = 1;
    uint depth = 1;
    while(!stack.empty()){
        std::set<std::array<uint, 2>> neighbors = getUnvisitedNeighbors(stack.top(), visited);
        std::cout << "Stack top: " << stack.top()[0] << ", " << stack.top()[1] << "   visited: " << visited.size() << "\tdepth: " << depth << std::endl;
        if(neighbors.empty()){
            stack.pop();
            depth--;
        }else{
            uint r = rand() % neighbors.size();
            auto it = std::begin(neighbors);
            std::advance(it,r);
            noWall.merge(getBorderFields(stack.top(), *it));
            stack.push(*it); 
            visited.insert(*it);
            depth++;
        }
        if(depth > maxdepth){
            maxdepth = depth;
            maxPath = stack;
        }
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////// Player, Goal
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////

    end = maxPath.top();
    end = getMapField(end);
    new Goal(map, end[0], end[1]);

    start = getMapField(start);
    player = new Player(map, start[0], start[1]);

    /*
    while(maxPath.size() > 1) {
        std::array<uint, 2> cell1 = maxPath.top();
        maxPath.pop();
        std::array<uint, 2> cell2 = maxPath.top();

        std::cout << "(" << cell2[0] << ", " << cell2[1] << ")  ->  (" << cell1[0] << ", " << cell1[1] << ")" << std::endl;
        noWall.merge(getBorderFields(cell1, cell2));
    }
    */



    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////// Walls
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////


    
    std::cout << "noWalls: " << noWall.size() << "\n";

    std::set<std::array<uint, 2>> possibleWalls;
    for (uint x=0; x<size.x; x++){
        for (uint y=0; y<size.y; y++){
            possibleWalls.merge(getBorderFields({x,y}, {x+1,y})); // rechts vom Feld
            possibleWalls.merge(getBorderFields({x,y}, {x,y+1})); // unterm Feld
            possibleWalls.insert({ (x* (scalar+1)) + (scalar+1),  (y* (scalar+1)) + (scalar+1)}); // die Ecke zwischen den Feldern
        }
        std::cout <<  "possible walls: " << possibleWalls.size() << "\n";
    }

    std::set<std::array<uint, 2>> walls;
    std::set_difference(possibleWalls.begin(), possibleWalls.end(), noWall.begin(), noWall.end(), std::inserter(walls, walls.end()));

    /*
    std::set<std::array<uint, 2>> noWallTest;
    noWallTest.merge(getBorderFields({5,5}, {5,6}));
    noWallTest.merge(getBorderFields({5,5}, {5,4}));
    noWallTest.merge(getBorderFields({5,5}, {6,5}));
    noWallTest.merge(getBorderFields({6,5}, {7,5}));
    noWallTest.merge(getBorderFields({7,5}, {7,6}));
    noWallTest.merge(getBorderFields({7,6}, {7,7}));
    noWallTest.merge(getBorderFields({7,7}, {7,8}));
    std::set_difference(possibleWalls.begin(), possibleWalls.end(), noWallTest.begin(), noWallTest.end(), std::inserter(walls, walls.end()));
    */

    

    std::cout <<  "set walls: " << walls.size() <<"\n";

    for(auto wall : walls) {
        new Barrier(map, wall[0], wall[1]);
    }

    

    
};


////////////////////////////////////////////////////////////////////////////////

void buildOuterBorders(){
    for (int x=0; x<mapsizex; x++){
        new Barrier(map, x, 0);
        new Barrier(map, x, mapsizey-1);
    }
    for (int y=1; y<mapsizey-1; y++){
        new Barrier(map, 0, y);
        new Barrier(map, mapsizex-1, y);
    }
}

/*

void Level::drawBackground(int bgRepeteAfter){
    for (int x=0; x<bgRepeteAfter; x++){
        for (int y=0; y<bgRepeteAfter; y++){
            new Object(getTexture('b', 0), ' ', x, y);
        }
    }
}
*/
};




#endif //LEVEL_H