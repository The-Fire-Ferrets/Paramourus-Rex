#include "ParamourusRex.h"

#include "ContactEvent.h"

bool loadConfiguration(void);

int main(int argc, char* argv[])
{
	//Instantiate a global EventManager
	EventManager* eventmanager = new EventManager();
	EventManagerInterfacePtr eventmanagerptr;
	eventmanagerptr.reset(eventmanager);
	EventManagerInterface::set(eventmanagerptr);
	
	//Load Configuration
	if (!loadConfiguration())
		std::cout << "Main: Failed to load configuration" << std::endl;
	
	//Set up the global configuration
  	Configuration::instance()->setWindowDimensions(window_width, window_height);

	//Create window
	sf::RenderWindow App(sf::VideoMode(window_width, window_height,64), "Paramourus Rex by Will Hollingsworth, Bina Kakusa, Belol Nessar, and Meghan Tinkler", sf::Style::Titlebar|sf::Style::Close);
	sf::View defaultView = App.getDefaultView();
	//Loads the map
	const char* map = {"Map"};	
	MapView::Create(map);


	//Holds the current game state
	// 0 = At the map; 1 = In a level; 2 = Talking to Diana; 3 = At craftable; 4 = quit game
	int state = 0;
	

	/**
	 ** Main Game Loop
	 ** Uses a set frame rate to control game timing
	 ** Reads input and updates actors and components
	**/
	while(App.isOpen()) {
		sf::Event Event;

		//Start structure to control frame rate
		elapsed_ms = fr_clock.restart().asSeconds();
		while(App.pollEvent(Event)) {
			if(Event.type == sf::Event::Closed) {
				state = 4;
			}
		}
		switch(state) {
			case 0: 
				MapView::update(&App, &state, elapsed_ms);
				break;
			case 1:	
				LevelView::update(&App, &state, elapsed_ms); 
				break;
			case 2: //Display Diana
				DialogueView::update(&App, &state);
				break;
			case 3: 
				//CraftView::update(&App, &state, elapsed_ms);
				//Should change to state 0 so user returns to map when done crafting
				break;
			case 4:
				//ExitView::update(&App, &state, elapsed_ms);
				LevelView::quit();
				eventmanagerptr->quit();
				App.close();
				break;
			default:
				break;
		}
		//End structure to control frame rate

		//Controls rendering actors and components
		App.clear(sf::Color::Black);

		switch(state) {
			case 0: 
				App.setView(defaultView);		
				MapView::render(&App);
				break;
			case 1: 
				LevelView::render(&App); 
				break;
			case 2:
				App.setView(defaultView);
				DialogueView::render(&App);
				break;
			case 3:
				App.setView(defaultView);
				//CraftView::render(&App);
				break;
			case 4:
				App.setView(defaultView);
				//ExitView::render(&App);
				break;
			default: 
				break;
		}
		App.display();

		//Proccess all events
		switch(state) {
			default: 
				if (!eventmanagerptr->processEvents())
					std::cout << "Main: Failed to process events" << std::endl; 
				break;
		}
	}
	return 0;
}

/** Loads the configuration file and any configurations
 **
**/
bool loadConfiguration(void) {
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
			if (!strcmp(attr.name(), "Height")) {
				window_height = std::strtol(attr.value(), &temp, 10);
				if (*temp != '\0') {
					std::cout << "Main: Failed to load configuration: Error reading attribute for " << attr.name() << std::endl;
					return false;
				}
			}
			else if (!strcmp(attr.name(), "Width")) {
				window_width = std::strtol(attr.value(), &temp, 10);
				if (*temp != '\0') {
					std::cout << "Main: Failed to load configuration: Error reading attribute for " << attr.name() << std::endl;
					return false;
				}
			}
		}
	}	
	return true;
}
