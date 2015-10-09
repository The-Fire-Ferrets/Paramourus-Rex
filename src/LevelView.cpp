#include "LevelView.h"

//Total size of pointer arrays
const int LevelView::size = 20;
//Number of actors populating the level
int LevelView::num_actors = 0;
//Array holding pointers of actors populating the level
StrongActorPtr LevelView::actors[size];
//Holds background texture
sf::Texture LevelView::background_texture;
//Holds background
sf::Sprite LevelView::background;
//Holds level name
std::string LevelView::name;
//Level clock
sf::Clock LevelView::level_clock;
//Holds Level time
sf::Text LevelView::timer;
//Holds level timer text
std::string LevelView::timer_string = "10:00";
//Hold text font
sf::Font LevelView::font;
//Timer position
sf::Vector2f LevelView::timer_position;

/** Creates and populates a level and all its components based on XML configuration
 ** resource: filename for xml
 ** state: current game state
**/
void LevelView::Create(const char* resource, int* state) {
	//Reference to current location in Actor population array
	//Holds referenced to loaded XML file	
	num_actors = 0;
	pugi::xml_document doc;
	
	//Error check to see if file was loaded correctly
	pugi::xml_parse_result result;
	std::string resource_str(resource);
    	if (!(result = doc.load_file(("./assets/levels/" + resource_str + ".xml").c_str()))) {
		std::cout << "LevelView::Create(...): Failed to load" << std::endl;
		std::cout << "Filename: " << resource << " Load result: " << result.description() << std::endl;
	}

	//Used to iterate over XML file to get attributes
	pugi::xml_node tools = doc.child(resource);
	char* temp;	
	for (pugi::xml_attribute attr = tools.first_attribute(); attr; attr = attr.next_attribute()) {
		if (!strcmp(attr.name(), "Name"))
			name = attr.value();
		else if (!strcmp(attr.name(), "Background")) {
			background_texture.loadFromFile(("./assets/backgrounds/" + (std::string)attr.value()).c_str());
			background = sf::Sprite(background_texture, sf::IntRect(0, 0, 800, 800));
			background.setPosition(sf::Vector2f(0,0));
		}
		else if (!strcmp(attr.name(), "Font")) {
			font.loadFromFile(("./assets/" + (std::string)attr.value()).c_str());
			timer = sf::Text(timer_string, font);
		}
		else if (!strcmp(attr.name(), "Text_Size")) {
			timer.setCharacterSize(std::strtol(attr.value(), &temp, 10));
			if (*temp != '\0') {
				std::cout << "LevelView::Create: Error reading attribute for " << attr.name() << std::endl;
			}
		}
		else if (!strcmp(attr.name(), "Text_X")) {
			timer_position.x = (std::strtol(attr.value(), &temp, 10));
			if (*temp != '\0') {
				std::cout << "LevelView::Create: Error reading attribute for " << attr.name() << std::endl;
			}
		}
		else if (!strcmp(attr.name(), "Text_Y")) {
			timer_position.y = (std::strtol(attr.value(), &temp, 10));
			if (*temp != '\0') {
				std::cout << "LevelView::Create: Error reading attribute for " << attr.name() << std::endl;
			}
		}
	}
	timer.setPosition(timer_position);
	//Iterates over XML to get components to add
	for (pugi::xml_node tool = tools.first_child(); tool; tool = tool.next_sibling()) {
		actors[num_actors] = ActorFactory::CreateActor(tool.name(), state);
		actors[num_actors]->PostInit(&tool);
		num_actors++;
	}
	*state = 1;
}

std::string LevelView::getName(void) {
	return name;
}

int LevelView::getNumActors(void) {
	return num_actors;
}

/** Checks to see if level was clicked on and switches to it
 **
**/
void LevelView::update(sf::RenderWindow *window, int* state, float time) {
	float timer_time = 10000 - level_clock.getElapsedTime().asMilliseconds();
	if (timer_time <= 0)
		*state = 2;
	else {
		timer_string = std::to_string(timer_time / 1000);
		timer.setString(timer_string);
		for (int i = 0; i < num_actors; i ++)
			actors[i]->update(time);
	}
	
}

/** Checks for events and update accordingly
 **
*
void LevelView::update(EventInterfacePtr e) {

}

/** Renders the map onto the window
 **
**/
void LevelView::render(sf::RenderWindow *window) {
	window->draw(background);
	window->draw(timer);
	for (int i = 0; i < num_actors; i ++)
		actors[i]->render(window);
}

void LevelView::start(void) {
	level_clock.restart();
}


