#include "MapView.h"

//Maxmium number of levels
const int MapView::size = 20;
//Total number of levels
int MapView::num_levels = 0;
//Level names
std::string MapView::levels[size];
//Level sprites
sf::Sprite MapView::sprites[size];
//Level textures
sf::Texture MapView::textures[size];
//Level sprite positions
sf::Vector2f MapView::positions[size];
//Level sprite sizes
sf::Vector2f MapView::sizes[size];
//Map Background sprite
sf::Sprite MapView::background;
//Map Background texture
sf::Texture MapView::background_texture;


/** Creates the map from the give configuration file
 **
**/
void MapView::Create(const char* resource) {
	//Holds referenced to loaded XML file		
	pugi::xml_document doc;
	
	//Error check to see if file was loaded correctly
	pugi::xml_parse_result result;
	std::string resource_str(resource);
    	if (!(result = doc.load_file(("./assets/" + resource_str + ".xml").c_str()))) {
		std::cout << "LevelView::CreateLevel(...): Failed to load" << std::endl;
		std::cout << "Filename: " << resource << " Load result: " << result.description() << std::endl;
	}

	//Used to iterate over XML file to get attributes
	pugi::xml_node tools = doc.child(resource);
	for (pugi::xml_attribute attr = tools.first_attribute(); attr; attr = attr.next_attribute()) {
		if (!strcmp(attr.name(), "Background")) {
			background_texture.loadFromFile(("./assets/backgrounds/" + (std::string)attr.value()).c_str());
			background = sf::Sprite(background_texture, sf::IntRect(0, 0, 800, 800));
			background.setPosition(sf::Vector2f(0,0));
		}
	}

	//Iterates over XML to get components to add
	for (pugi::xml_node tool = tools.first_child(); tool; tool = tool.next_sibling(), num_levels++) {
		levels[num_levels] = tool.name();
		char* temp;	
		for (pugi::xml_attribute attr = tool.first_attribute(); attr; attr = attr.next_attribute()) {
			if (!strcmp(attr.name(), "Sprite")) {
				textures[num_levels].loadFromFile(("./assets/sprites/" + (std::string)attr.value()).c_str());
			}
			else if (!strcmp(attr.name(), "X")) {
				positions[num_levels].x = std::strtol(attr.value(), &temp, 10);
					if (*temp != '\0') {
						std::cout << "MapView::CreateMap: Error reading attribute for " << tool.name() << std::endl;
					}
			}
			else if (!strcmp(attr.name(), "Y")) {
				positions[num_levels].y =std::strtol(attr.value(), &temp, 10);
					if (*temp != '\0') {
						std::cout << "MapView::CreateMap: Error reading attribute for " << tool.name() << std::endl;
					}
			}
			else if (!strcmp(attr.name(),"Width")) {
				sizes[num_levels].x = std::strtol(attr.value(), &temp, 10);
				if (*temp != '\0') {
					std::cout << "Actor::PostInit: Failed to post-initialize: Error reading attribute for " << attr.name() << std::endl;
				}
			}
			else if (!strcmp(attr.name(),"Height")) {
				sizes[num_levels].y = std::strtol(attr.value(), &temp, 10);
				if (*temp != '\0') {
					std::cout << "Actor::PostInit: Failed to post-initialize: Error reading attribute for " << attr.name() << std::endl;
				}
			}
		}
			
	}

	for (int i = 0; i < num_levels; i++) {
		sprites[i] = sf::Sprite(textures[i], sf::IntRect(positions[i].x, positions[i].y, sizes[i].x, sizes[i].y));
		sprites[i].setPosition(positions[i]);
	}
		
}

/** Checks to see if level was clicked on and switches to it
 **
**/
void MapView::update(sf::RenderWindow *window, int* state, float time) {
	if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
		const sf::Vector2i pos = sf::Mouse::getPosition(*window);
		for (int i = 0; i < num_levels; i++) {
			if (sprites[i].getGlobalBounds().contains(pos.x, pos.y)) {
				LevelView::Create(levels[i].c_str(), state);
				DialogueView::Create(levels[i].c_str(), state);
				LevelView::start();
				*state = 1;
			}
		}
	}
}

/** Checks for events and update accordingly
 **
**/
void MapView::update(EventInterfacePtr e) {

}

/** Renders the map onto the window
 **
**/
void MapView::render(sf::RenderWindow *window) {
	window->draw(background);
	for (int i = 0; i < num_levels; i++) {
		window->draw(sprites[i]);
	}
}
