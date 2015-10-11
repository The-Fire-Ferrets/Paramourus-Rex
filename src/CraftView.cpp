#include "CraftView.h"

//Total size of pointer arrays
const int CraftView::size = 20;

// Total number of flowers
int CraftView::fireFlowers = 0;
int CraftView::waterFlowers = 0;
int CraftView::earthFlowers = 0;
int CraftView::airFlowers = 0;
int CraftView::totalFlowers;

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
    	if (!(result = doc.load_file(("./assets/levels/" + resource_str + ".xml").c_str()))) {
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
			background = sf::Sprite(background_texture, sf::IntRect(0, 0, 800, 800));
			background.setPosition(sf::Vector2f(0,0));
		}
	}
	//Iterates over XML to get components to add
	for (pugi::xml_node tool = tools.first_child(); tool; tool = tool.next_sibling()) {
	}
}

int CraftView::getNumFlowers(void) {
	return totalFlowers;
}

/** Checks to see if level was clicked on and switches to it
 **
**/
void CraftView::update(sf::RenderWindow *window, int* state) {
	// Add a flower to the inventory if it has been collected
		
	
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
	// Backlay set off to the side to allow space for item select screens to the left
	backlay.setPosition(120, 560);
	backlay.setOutlineColor(sf::Color::Black);
	backlay.setFillColor(sf::Color::White);
	backlay.setSize(sf::Vector2f(660,200));
	backlay.setOutlineThickness(5);
	text.setColor(sf::Color::Black);
	text.setString("Welcome back, Phil!(&#10;) You have ___ flowers!");

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
