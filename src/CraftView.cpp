include "CraftView.h"

//Total size of pointer arrays
const int CraftView::size = 20;
//Number of actors populating the level
int CraftView::num_actors = 0;
//Array holding pointers of actors populating the level
StrongActorPtr CraftView::actors[size];
//Holds background texture
sf::Texture CraftView::background_texture;
//Holds background
sf::Sprite CraftView::background;
//Holds level name
std::string CraftView::name;
//Holds level timer text
std::string CraftView::timer_string = "10:00";
//Hold text font
sf::Font CraftView::font;
//Timer position
sf::Vector2f CraftView::timer_position;
//game view
sf::View CraftView::gameView;
//minimap view
sf::View CraftView::minimapView;
//reference to player
StrongActorPtr CraftView::player;

/** Creates and populates a level and all its components based on XML configuration
 ** resource: filename for xml
 ** state: current game state
**/
void CraftView::Create(const char* resource, int* state) {
	//Reference to current location in Actor population array
	//Holds referenced to loaded XML file	
	num_actors = 0;
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
	  	if (!strcmp(attr.name(), "Background")) {
			background_texture.loadFromFile(("./assets/backgrounds/" + (std::string)attr.value()).c_str());
			background = sf::Sprite(background_texture, sf::IntRect(0, 0, 800, 800));
			background.setPosition(sf::Vector2f(0,0));
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

std::string CraftView::getName(void) {
	return name;
}

int CraftView::getNumActors(void) {
	return num_actors;
}

/** Checks to see if level was clicked on and switches to it
 **
**/
void CraftView::update(sf::RenderWindow *window, int* state, float time) {
	float timer_time = 10000 - level_clock.getElapsedTime().asMilliseconds();

	
	
}

/** Checks for events and update accordingly
 **
*
void CraftView::update(EventInterfacePtr e) {

}

/** Renders the map onto the window
 **
**/
void CraftView::render(sf::RenderWindow *window) {
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


/** Clean up level after completion
 **
**/
void CraftView::cleanUp(void) {
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
void CraftView::quit(void) {
	cleanUp();
}