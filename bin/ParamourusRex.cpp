#include "ParamourusRex.h"

#include "ContactEvent.h"

bool loadConfiguration(void);

int main(int argc, char* argv[])
{
	//Holds the current game state
	// 0 = At the map; 1 = In a level; 2 = Talking to Diana; 3 = At craftable; 4 = quit game; 5 = Title screen
	int state = 5;
	int last_state;

	//Instantiate a global EventManager
	EventManager* eventmanager = new EventManager();
	EventManagerInterfacePtr eventmanagerptr;
	eventmanagerptr.reset(eventmanager);
	EventManagerInterface::set(eventmanagerptr);
	
	//Load Configuration
	if (!loadConfiguration())
		std::cout << "Main: Failed to load configuration" << std::endl;

	//Create window
	sf::RenderWindow App(sf::VideoMode(Configuration::getWindowWidth(), Configuration::getWindowHeight(),64), "Paramourus Rex by Will Hollingsworth, Bina Kakusa, Belol Nessar, and Meghan Tinkler", sf::Style::Titlebar|sf::Style::Close);
	sf::View defaultView = App.getDefaultView();
	App.setView(defaultView);			
	Configuration::setGameViewCenter(sf::Vector2f(Configuration::getGameViewWidth()/2, Configuration::getGameViewHeight()/2));
	//Loads the title
	const char* title = {"Title"};
	TitleView::Create(title);

	//Loads the craft table
	const char* craft = {"Craft"};
	CraftView::Create(craft);

	fr_clock.restart();
	/**
	 ** Main Game Loop
	 ** Uses a set frame rate to control game timing
	 ** Reads input and updates actors and components
	**/
	while(App.isOpen()) {
		last_state = state;
		sf::Event Event;

		//Start structure to control frame rate
		elapsed_ms = ((int) (fr_clock.getElapsedTime().asMilliseconds() / Configuration::getFrameRate()) % (int) (Configuration::getFrameRate())) + 1;
		while(App.pollEvent(Event)) {
			if(Event.type == sf::Event::Closed) {
				state = 4;
			}
		}
		switch(last_state) {
			case 0: 
				App.setView(defaultView);
				Configuration::setGameViewCenter(sf::Vector2f(Configuration::getGameViewWidth()/2, Configuration::getGameViewHeight()/2));
				MapView::update(&App, &state, elapsed_ms);
				break;
			case 1:	
				LevelView::update(&App, &state, elapsed_ms); 
				break;
			case 2: //Display Diana
				App.setView(defaultView);
				Configuration::setGameViewCenter(sf::Vector2f(Configuration::getGameViewWidth()/2, Configuration::getGameViewHeight()/2));
				DialogueView::update(&App, &state);
				break;
			case 3: 
				CraftView::update(&App, &state);
				//Should change to state 0 so user returns to map when done crafting
				break;
			case 4:
				//ExitView::update(&App, &state, elapsed_ms);
				//LevelView::quit();
				//eventmanagerptr->quit();
				App.close();
				break;
			case 5:
				App.setView(defaultView);
				Configuration::setGameViewCenter(sf::Vector2f(Configuration::getGameViewWidth()/2, Configuration::getGameViewHeight()/2));
				TitleView::update(&App, &state, elapsed_ms);
				break;
			default:
				break;
		}
		//End structure to control frame rate

		//Controls rendering actors and components
		App.clear(sf::Color::Black);

		switch(last_state) {
			case 0: 		
				MapView::render(&App);
				break;
			case 1: 
				LevelView::render(&App); 
				break;
			case 2:
				DialogueView::render(&App);
				break;
			case 3:
				CraftView::render(&App);
				break;
			case 4:
				//ExitView::render(&App);
				break;
			case 5:	
				TitleView::render(&App);
				break;
			default: 
				break;
		}
		App.display();

		//Proccess all events
		switch(state) {
			case 1:
				//std::thread(&Pathfinder::generatePath2, LevelView::npc->getStartPos(), LevelView::npc->getPosition(), LevelView::player->getPosition());
				//Pathfinder::generatePath(LevelView::npc->getStartPos(), LevelView::npc->getPosition(), LevelView::player->getPosition());
				break;
			default: 
				break;
		}
		if (!eventmanagerptr->processEvents())
			std::cout << "Main: Failed to process events" << std::endl; 
	}
	
	return 0;
}

/** Loads the configuration file and any configurations
 **
**/
bool loadConfiguration(void) {
	//Configuration Variables
	unsigned int window_height;
	unsigned int window_width;

	float gameview_height;
	float gameview_width;
	unsigned int gameview_center;

	float minimapview_height;
	float minimapview_width;
	sf::Texture minimap_border;
	float frame_rate;
	sf::Texture loading_texture;

	//Load configuration
  	if (!(result = doc.load_file( ("./assets/" + config + ".xml").c_str() ))) {
		std::cout << "Main: Failed to load config" << std::endl;
		std::cout << "Filename: " << config << " Load result: " << result.description() << std::endl;
		return false;
	}

	//Used to iterate over XML file to get attributes
	char * temp;
	pugi::xml_node tools = doc.child(config.c_str());
	for (pugi::xml_node tool = tools.first_child(); tool; tool = tool.next_sibling()) {
		for (pugi::xml_attribute attr = tool.first_attribute(); attr; attr = attr.next_attribute()) {
			if (!strcmp(tool.name(), "Window") && !strcmp(attr.name(), "Height")) {
				window_height = std::strtol(attr.value(), &temp, 10);
				if (*temp != '\0') {
					std::cout << "Main: Failed to load configuration: Error reading attribute for " << attr.name() << std::endl;
					return false;
				}
			}
			else if (!strcmp(tool.name(), "Window") && !strcmp(attr.name(), "Width")) {
				window_width = std::strtol(attr.value(), &temp, 10);
				if (*temp != '\0') {
					std::cout << "Main: Failed to load configuration: Error reading attribute for " << attr.name() << std::endl;
					return false;
				}
			}
			else if (!strcmp(tool.name(), "Time") && !strcmp(attr.name(), "FrameRate")) {
				frame_rate = std::strtol(attr.value(), &temp, 10);
				if (*temp != '\0') {
					std::cout << "Main: Failed to load configuration: Error reading attribute for " << attr.name() << std::endl;
					return false;
				}
			}
			else if (!strcmp(tool.name(), "GameView") && !strcmp(attr.name(), "Height")) {
				gameview_height = std::strtof(attr.value(), &temp);
			}
			else if (!strcmp(tool.name(), "GameView") && !strcmp(attr.name(), "Width")) {
				gameview_width = std::strtof(attr.value(), &temp);
			}
			else if (!strcmp(tool.name(), "GameView") && !strcmp(attr.name(), "Center")) {
				gameview_center = std::strtof(attr.value(), &temp);
			}
			else if (!strcmp(tool.name(), "MiniMapView") && !strcmp(attr.name(), "Height")) {
				minimapview_height = std::strtof(attr.value(), &temp);
			}
			else if (!strcmp(tool.name(), "MiniMapView") && !strcmp(attr.name(), "Width")) {
				minimapview_width = std::strtof(attr.value(), &temp);
			}
			else if (!strcmp(tool.name(), "MiniMapView") && !strcmp(attr.name(), "MinimapBorder")) {
				minimap_border.loadFromFile(("./assets/sprites/" + (std::string)attr.value()).c_str());
			}
			else if (!strcmp(tool.name(), "LoadingView") && !strcmp(attr.name(), "LoadingScreen")) {
				loading_texture.loadFromFile(("./assets/sprites/" + (std::string)attr.value()).c_str());
			}
		}
	}

	//Set up the global configuration
  	Configuration::setWindowDimensions(window_width, window_height);
	Configuration::setGameViewDimensions(gameview_width*window_width, gameview_height*window_height);
	Configuration::setGameViewCenter(sf::Vector2f(gameview_center, gameview_center));
	Configuration::setMiniMapViewDimensions(minimapview_width*window_width, minimapview_height*window_height);
	Configuration::setMinimapBorder(minimap_border);
	Configuration::setLoadingTexture(loading_texture);
	Configuration::setFrameRate(frame_rate);
	return true;
}
