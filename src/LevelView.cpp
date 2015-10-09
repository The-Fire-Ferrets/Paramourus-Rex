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
//game view
sf::View LevelView::gameView;
//minimap view
sf::View LevelView::minimapView;
//reference to player
StrongActorPtr LevelView::player;

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
		std::cout << "LevelView::CreateLevel(...): Failed to load" << std::endl;
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
				std::cout << "LevelView::CreateLevel: Error reading attribute for " << attr.name() << std::endl;
			}
		}
		else if (!strcmp(attr.name(), "Text_X")) {
			timer_position.x = (std::strtol(attr.value(), &temp, 10));
			if (*temp != '\0') {
				std::cout << "LevelView::CreateLevel: Error reading attribute for " << attr.name() << std::endl;
			}
		}
		else if (!strcmp(attr.name(), "Text_Y")) {
			timer_position.y = (std::strtol(attr.value(), &temp, 10));
			if (*temp != '\0') {
				std::cout << "LevelView::CreateLevel: Error reading attribute for " << attr.name() << std::endl;
			}
		}
	}
	timer.setPosition(timer_position);
	//Iterates over XML to get components to add
	for (pugi::xml_node tool = tools.first_child(); tool; tool = tool.next_sibling()) {
		actors[num_actors] = ActorFactory::CreateActor(tool.name(), state);
		actors[num_actors]->PostInit(&tool);
		if (num_actors == 0)
			player = actors[num_actors];
		num_actors++;
	}
	//Set views so can only see a quarter of the map at once
	gameView = sf::View(sf::FloatRect(0, 0, Configuration::instance()->getWindowWidth()/4, Configuration::instance()->getWindowHeight()/4));
	//Set minimap to see entire map
	minimapView = sf::View(sf::FloatRect(0, 0, Configuration::instance()->getWindowWidth(), Configuration::instance()->getWindowHeight()));
	*state = 2;
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
	if (timer_time <= 0) {
		*state = 0;
		cleanUp();
	}
	else {
		std::ostringstream out;
		out << std::setprecision(4) << timer_time/1000;
		timer_string = out.str();
		timer.setString(timer_string);
		//Set timer to bottom right corner
		sf::Vector2f gameView_bottom_corner = gameView.getCenter();
		gameView_bottom_corner.x += gameView.getSize().x/2 - timer.getGlobalBounds().width;
		gameView_bottom_corner.y += gameView.getSize().y/2 - timer.getGlobalBounds().height*1.5;
		timer.setPosition(gameView_bottom_corner);

		for (int i = 0; i < num_actors; i ++)
			actors[i]->update(time);
	}
	
}

/** Checks for events and update accordingly
 **
**/
void LevelView::update(EventInterfacePtr e) {

}

/** Renders the map onto the window
 **
**/
void LevelView::render(sf::RenderWindow *window) {
	//Get the player location and center gameView to it
	sf::Vector2f player_pos = player->getPosition();
	sf::Vector2f player_size = player->getSize();
	gameView.setCenter(player_pos.x + player_size.x/2, player_pos.y + player_size.y/2); 
	gameView.setViewport(sf::FloatRect(0, 0, 1, 1));
	window->setView(gameView);
	//Update graphics	
	window->draw(background);
	window->draw(timer);
	for (int i = 0; i < num_actors; i ++)
		actors[i]->render(window);

	//Set minimap view
	minimapView.setViewport(sf::FloatRect(0.90f, 0, 0.10f, 0.10f));
	window->setView(minimapView);
	
	//Update graphics
	window->draw(background);
	window->draw(timer);
	for (int i = 0; i < num_actors; i ++)
		actors[i]->render(window);
}

/** Ready the level for start
 **
**/
void LevelView::start(void) {
	level_clock.restart();
}

/** Clean up level after completion
 **
**/
void LevelView::cleanUp(void) {
	for (int i = 0; i < num_actors; i ++) {
		actors[i]->quit();
	}
	num_actors = 0;
	EventManagerInterface::get()->reset();
	ActorFactory::reset();
}

/** Quit level
 **
**/
void LevelView::quit(void) {
	cleanUp();
}
