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
#include <cmath>
#include "Object.hpp"
#include "O_Living.hpp" // Living muss über Facility und Item, weil die den Player-Typ kennen müssen.
#include "O_Facility.hpp"
#include "O_Item.hpp"


extern const uint OBJECTUNIT;
extern const sf::Time UPDATE_TIME;

// Stellt ein Level dar und verwaltet die Map etc.

class Level {
public:
    // OLD
    sf::Font font;
    //////////////////////////////////////////////////////////////////////////////////////////////
private:
    Object** map;
    Player* player;

    /*
    sf::Sprite** bg;
    int bgsize = 1;
    uint bgsizex, bgsizey;
    */
    int bgsize = 10;
    sf::Sprite bg;

    uint64_t mapsizex, mapsizey;


    std::set<std::array<uint, 2>> walls;
    std::set<std::array<uint, 2>> enemies;
    std::set<std::array<uint, 2>> food;
    std::set<std::array<uint, 2>> navis;
    std::set<std::array<uint, 2>> marker;
    std::array<uint, 2> start, end;

    std::map< std::array<uint, 2>, std::array<uint, 2> > navigation;
    // Format: DFS-Pos -> DFS-Pos, näher am Ziel

    uint scalar = 2; // wie viele Mapfields hat ein DFS-Node (zu deutsch: wie breit sind die Gänge)
    
public:
    
    Level(uint64_t x, uint64_t y)
    {
        Object::loadTexsheets();
        loadFonts();
        
        dfs(sf::Vector2u(x, y));
        allocateMap();
        buildMap();
        createBackground();
    }

    ~Level(){
        purgeMap();
    }
    
    //////////////////////////////////////////////////////////////////////////////////////////////

    Player* getPlayer(){return player;};
    int getMapX(){return mapsizex;};
    int getMapY(){return mapsizey;};
    sf::Sprite getBackground(){
        return bg;
    }



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

    void buildMap(){
        buildOuterBorders();
        new Goal(map, start[0], start[1]);
        player = new Player(map, end[0], end[1]);
    
        for(auto e : enemies){
            new Enemy(map, e[0], e[1]);
        }
        for(auto wall : walls) {
            new Barrier(map, wall[0], wall[1]);
        }

        for(auto f : food) {
            new Food(map, f[0], f[1]);
        }
        for(auto n : navis) {
            new Navi(map, n[0], n[1]);
        }
        for(auto m : marker) {
            new Marker(map, m[0], m[1]);
        }

    }

    void allocateMap(){
        map = (Object**)(malloc((2 + mapsizex * mapsizey) * sizeof(Object *)));
        for(int i=0; i<(mapsizex*mapsizey +2); i++){
            map[i] = nullptr;
        }
        map[0] = (Object*)mapsizex;
        map[1] = (Object*)mapsizey;

        printf("Map allocated \n");
    }

    void purgeMap(){
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

    void reset(){
        purgeMap();
        allocateMap();
        buildMap();
    }

//////////////////////////////////////////////////////////////////////////////////////////////

    std::array<uint, 2> mapNodeToDfsNode(uint x, uint y, bool _round=false){
        if(_round) 
            return {(uint) round(((float)x-1.)/((float)scalar+1.)), 
                (uint) round(((float)y-1.)/((float)scalar+1.))};
        else 
            return {(x-1)/(scalar+1), 
            (y-1)/(scalar+1)};
    }

    

    sf::Vector2i getDirection(uint x, uint y){
        std::array<uint, 2> dfsnode = mapNodeToDfsNode(x, y);
        std::array<uint, 2> dfsnode_rounded = mapNodeToDfsNode(x, y, true);

        std::array<uint, 2> newnode = navigation[dfsnode];
        std::array<uint, 2> newnode_rounded = navigation[dfsnode_rounded];

        if(dfsnode[0] < dfsnode_rounded[0] ||dfsnode[1] < dfsnode_rounded[1]){
            // stehe zwischen zwei DFS-Feldern
            if(newnode != dfsnode_rounded){
                dfsnode = dfsnode_rounded;
                newnode = newnode_rounded;
            }
        }
        
        
        return sf::Vector2i(newnode[0]-dfsnode[0], newnode[1]-dfsnode[1]);
    }

    Object getMarkerTemplate(uint x=0, uint y=0){
        return Object(nullptr, x, y, Object::texsheets["breadcrumbs"]);
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

void buildOuterBorders(){
    // seit dfs nur noch obere und linke kante erforderlich.
    for (int x=0; x<mapsizex; x++){
        new Barrier(map, x, 0);
    }
    for (int y=1; y<mapsizey; y++){
        new Barrier(map, 0, y);
    }
}

void createBackground(){
    Texsheet* tex = Object::texsheets["bg0"];
    tex->texture.setRepeated(true);
    bg.setTexture(tex->texture);
    bg.setTextureRect({0, 0, (int)(mapsizex*OBJECTUNIT), (int)(mapsizey*OBJECTUNIT)});
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
    uint scalar = this->scalar;

    mapsizex = ((size.x * (scalar+1)) + scalar + 1)-2; 
    mapsizey = ((size.y * (scalar+1)) + scalar + 1)-2;

    std::set<std::array<uint, 2>> noWall; // durchgang, hier keine wall platzieren
    std::stack<std::array<uint, 2>> maxPath;
    srand (time(NULL));


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
        //std::cout << "Got Wall-Fields between Positions:  ("<< node1[0] <<", "<< node1[1] <<") - ("<< node2[0] <<", "<< node2[1] <<"):\n";

        std::set<std::array<uint, 2>> mapfields;
        if(node1[0] == node2[0] + 1 && node1[1] == node2[1]){ // node2 ist links von node1
            //std::cout << "\tLEFT\n";
            for (uint y=1; y<=scalar; y++){
                mapfields.insert({ (node1[0] * (scalar+1) ) ,  (node1[1] * (scalar+1) + y ) });
            }
        }else if(node1[0] == node2[0] - 1 && node1[1] == node2[1]){ // node2 ist rechts von node1
            //std::cout << "\tRIGHT\n";
            for (uint y=1; y<=scalar; y++){
                mapfields.insert({ ((node1[0] * (scalar+1)) + scalar +1 ) ,  (node1[1] * (scalar+1) + y ) });
            }
        }else if(node1[1] == node2[1] + 1 && node1[0] == node2[0]){ // node2 ist unter node1
            //std::cout << "\tUNDER\n";
            for (uint x=1; x<=scalar; x++){
                mapfields.insert({ ((node1[0] * (scalar+1)) + x) ,  (node1[1] * (scalar+1)  ) });
            }
        }else if(node1[1] == node2[1] - 1 && node1[0] == node2[0]){ // node2 ist über node1
            //std::cout << "\tOVER\n";
            for (uint x=1; x<=scalar; x++){
                mapfields.insert({ ((node1[0] * (scalar+1)) + x) ,  (node1[1] * (scalar+1) + scalar + 1) });
            }
        }
        
        /*
        for( auto i : mapfields){
            std::cout << "\t("<< i[0] <<", "<< i[1] <<")\n";
        }
        std::cout << std::endl;
        */
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
        // Wirft RAM Exception aus unerfindlichen gründen ...?
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

    std::cout << "starting DFS.\n";
    uint maxdepth = 1;
    uint depth = 1;
    uint progress = 0;
    while(!stack.empty()){
        /*
        uint r = rand() % 10;
        if(r == 0){
            // breche aktuellen Pfad ab und gehe n felder zurück
            r = rand() % depth;
            for (uint i = 0; i < r; i++){
                stack.pop();
                depth--;
            }
        }
        */
        std::set<std::array<uint, 2>> neighbors = getUnvisitedNeighbors(stack.top(), visited);

        if(neighbors.empty()){
            std::array<uint, 2> from = stack.top();
            stack.pop();
            if(stack.size() >0){
                navigation[from] = stack.top();
            }
            depth--;
        }else{
            uint r = rand() % neighbors.size();
            auto it = std::begin(neighbors);
            std::advance(it,r);
            noWall.merge(getBorderFields(stack.top(), *it));
            stack.push(*it); 
            visited.insert(*it);

            depth++;
            uint progressRaw = (((float)visited.size() / (float)(size.x * size.y))*100.);
            if(progressRaw > progress){
                progress = progressRaw;
                std::cout << progress << "%\n";
            }
        }
        if(depth > maxdepth){
            maxdepth = depth;
            maxPath = stack;
        }
    }

    std::cout << "DFS done. Starting Map creation.\n";

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////// Player, Goal
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    start = getMapField(start);

    end = maxPath.top();
    end = getMapField(end);

    for(uint i=0; i<3; i++) maxPath.pop();
    food.insert(getMapField(maxPath.top()));
    for(uint i=0; i<5; i++) maxPath.pop();
    enemies.insert(getMapField(maxPath.top()));
    enemies.insert(getMapField(maxPath.top()));
    enemies.insert(getMapField(maxPath.top()));
    enemies.insert(getMapField(maxPath.top()));
    enemies.insert(getMapField(maxPath.top()));
    maxPath.pop();
    enemies.insert(getMapField(maxPath.top()));
    enemies.insert(getMapField(maxPath.top()));
    for(uint i=0; i<3; i++) maxPath.pop();
    enemies.insert(getMapField(maxPath.top()));

    


    ///////////////////////////////////////////////////////////////////////////////////////////
    /////// Walls
    ///////////////////////////////////////////////////////////////////////////////////////////


    std::set<std::array<uint, 2>> possibleWalls;
    for (uint x=0; x<size.x; x++){
        for (uint y=0; y<size.y; y++){
            possibleWalls.merge(getBorderFields({x,y}, {x+1,y})); // rechts vom Feld
            possibleWalls.merge(getBorderFields({x,y}, {x,y+1})); // unterm Feld
            possibleWalls.insert({ (x* (scalar+1)) + (scalar+1),  (y* (scalar+1)) + (scalar+1)}); // die Ecke zwischen den Feldern
        }
        //std::cout <<  "possible walls: " << possibleWalls.size() << "\n";
    }

    
    std::set_difference(possibleWalls.begin(), possibleWalls.end(), noWall.begin(), noWall.end(), std::inserter(walls, walls.end()));



    // TODO Navi-Items, Brot, Bambus, Minotauren zufällig verteilen.
    // TODO Navi-Item braucht DS, die für jedes Feld die Richtung des adjazenten Felds kennt, das zum Ziel führt. -> std::map<std::array<uint, 2>, sf::Vector2i>

    // Wichtig bei Bambus: genug verteilen, um an minotaurus vorbeizukommen
    // Idee: Items schon bei DFS verteilen!!
    //  Das ist effizient und pro 1 Minotaurus können danach (bei umgekehrter DFS, sonst halt andersrum) n Bambusse verteilt werden
    // Navi und Brot: An den Anfang mehr, sonst aber gleichverteilt, an Ende weniger.
}


////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////

};


#endif //LEVEL_H