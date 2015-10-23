#include "CraftView.h"

//Total size of pointer arrays
const int CraftView::size = 20;

// Total number of flowers
int CraftView::fireFlowers = 0;
int CraftView::waterFlowers = 0;
int CraftView::earthFlowers = 0;
int CraftView::airFlowers = 0;
int CraftView::totalFlowers;

// holds list of flowers actor has collected
std::string CraftView::flower_list[size];

//Holds background texture
sf::Texture CraftView::background_texture;
//Holds background
sf::Sprite CraftView::background;
//Holds level name
std::string CraftView::name;
//Hold text font
sf::Font CraftView::font;
// Holds text "craftable companion" will be speaking
sf::Text CraftView::text;
sf::Vector2f CraftView::text_pos;

/** Creates and populates a level and all its components based on XML configuration
 ** resource: filename for xml
 ** state: current game state
 **/
void CraftView::Create(const char* resource, int* state) {
    //Reference to current location in Actor population array
    //Holds referenced to loaded XML file	
    totalFlowers = 0;
    pugi::xml_document doc;

    //Error check to see if file was loaded correctly
    pugi::xml_parse_result result;
    std::string resource_str(resource);
    if (!(result = doc.load_file(("./assets/" + resource_str + ".xml").c_str()))) {
        std::cout << "CraftView::Create(...): Failed to load" << std::endl;
        std::cout << "Filename: " << resource << " Load result: " << result.description() << std::endl;
    }

    //Used to iterate over XML file to get attributes for this display -- currently none but background
    pugi::xml_node tools = doc.child(resource);
    char* temp;	
    
    for (pugi::xml_attribute attr = tools.first_attribute(); attr; attr = attr.next_attribute()) {
        // Background to hold general backdrop image, + static image of "crafting companion" 
        if (!strcmp(attr.name(), "Background")) {
            background_texture.loadFromFile(("./assets/backgrounds/" + (std::string)attr.value()).c_str());
            background = sf::Sprite(background_texture, sf::IntRect(0, 0, Configuration::getWindowWidth(),  Configuration::getWindowHeight()));
            background.setPosition(sf::Vector2f(0,0));
        }
        else if (!strcmp(attr.name(), "Font")) {
            font.loadFromFile(("./assets/" + (std::string)attr.value()).c_str());
            text.setFont(font);
        }
        else if (!strcmp(attr.name(), "Text_Size")) {
            text.setCharacterSize(std::strtol(attr.value(), &temp, 10));
            if (*temp != '\0') {
                std::cout << "CraftView::Create: Error reading attribute for " << attr.name() << std::endl;
            }
        }
        else if (!strcmp(attr.name(), "Text_X")) {
            text_pos.x = (std::strtol(attr.value(), &temp, 10));
            if (*temp != '\0') {
                std::cout << "CraftView::Create: Error reading attribute for " << attr.name() << std::endl;
            }
        }
        else if (!strcmp(attr.name(), "Text_Y")) {
            text_pos.y = (std::strtol(attr.value(), &temp, 10));
            if (*temp != '\0') {
                std::cout << "CraftView::Create: Error reading attribute for " << attr.name() << std::endl;
            }
        }
    }
    
    //Iterates over XML to get components to add
    for (pugi::xml_node tool = tools.first_child(); tool; tool = tool.next_sibling()) {
      
    }
    
    text.setPosition(text_pos);
    text.setColor(sf::Color::Blue);
    
    // Checks player for current inventory.
    // Accessing this info from LevelView::Player's CollectorComponent.

    if (LevelView::player != NULL){
      StrongActorPtr player = LevelView::player;
      StrongActorComponentPtr ac = player->components[CollectorComponent::id];
      std::shared_ptr<CollectorComponent> cc = std::dynamic_pointer_cast<CollectorComponent>(ac);
      std::vector<StrongActorPtr> flowers = cc->getFlowers();

      
      // we now have the list of flowers; want to iterate through it to determine how many of each flower
      // the player actually has, as well as restore the actor's number of vases to its full count
      for (int i=0; i < flowers.size() ; i++){
	//here, flowerList is a vector full of StrongActorPtrs. we need to determine the id of each strongactorptr 
	//to determine if it is a fire flower, water flower, air flower, or earth flower
	  if (flowers[i]->getId() == "FireFlower"){
	      fireFlowers++;
	  }
	  else if (flowers[i]->getId() == "WaterFlower"){
	      waterFlowers++;
	  }
	  else if (flowers[i]->getId() == "AirFlower"){
	      airFlowers++;
	  }
	  else if (flowers[i]->getId() == "EarthFlower"){
	      earthFlowers++;
	  }
	  
	  //restore player's vases now that it's cleared space in inventory
	  totalFlowers++;
	  cc->setVases(cc->getVases()+1);
      }
    }
    
}

int CraftView::getNumFlowers(void) {
    return totalFlowers;
}

/** Handles events happening in craft view and listens for user input.
 **
 **/
void CraftView::update(sf::RenderWindow *window, int* state) {


}


/** Checks for events and update accordingly
 **
 **/
void CraftView::update(EventInterfacePtr e) {

}

/** Renders the backdrop and menu selects onto the window, as well as option to
 ** head back to map.
 **
 **/
void CraftView::render(sf::RenderWindow *window) {
    // Backlay for crafting companions text
    sf::RectangleShape backlay;
    sf::Vector2u size = window->getSize();
    unsigned int width = size.x/1.3;
    unsigned int height = size.y/4;
    unsigned int posX = size.x/6.6;
    unsigned int posY = size.y/1.4;
    
    // Backlay set off to the side to allow space for item select screens to the left
    backlay.setPosition(posX, posY);
    backlay.setOutlineColor(sf::Color::Black);
    backlay.setFillColor(sf::Color::White);
    backlay.setSize(sf::Vector2f(width,height));
    backlay.setOutlineThickness(5);

    std::string str = "Welcome back, Phil! \n You have " + std::to_string(totalFlowers) + " flowers!"; 
    text.setString(str);

    //Update graphics	
    window->draw(background);
    window->draw(backlay);
    window->draw(text);
}


/** Clean up level after completion
 **
 **/
void CraftView::cleanUp(void) {

            }

/** Quit level
 **
 **/
void CraftView::quit(void) {
    cleanUp();
}
