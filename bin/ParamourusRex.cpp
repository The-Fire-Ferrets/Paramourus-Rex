#include "ParamourusRex.h"

int main(int argc, char* argv[])
{
	//Configuration variables
	
	//Load configuration
    if (!(result = doc.load_file(config))) {
		std::cout << "Main: Failed to load" << std::endl;
		std::cout << "Filename: " << resource << " Load result: " << result.description() << std::endl;
	}

	//Used to iterate over XML file to get attributes
	char * temp;
	pugi::xml_node tools = doc.child(((std::string) resource).substr(0, ((std::string) resource).size() - 4).c_str());
	for (pugi::xml_node tool = tools.first_child(); tool; tool = tool.next_sibling()) {
		for (pugi::xml_attribute attr = tool.first_attribute(); attr; attr = attr.next_attribute()) {
			if (!strcmp(attr.name(), "Height")) {
				window_height = std::stoi(attr.value(), temp);				
				if (*temp != '\0') {
					std::cout << "Main: Failed to load configuration: Error reading attribute for " << attr.name() << std::endl;
					return false
				}					
			}
			else if (!strcmp(attr.name(), "Width")) {
				window_width = std::stoi(attr.value(), temp);				
				if (*temp != '\0') {
					std::cout << "Main: Failed to load configuration: Error reading attribute for " << attr.name() << std::endl;
					return false
				}					
			}
				
			if (debug_mode)
				std::cout << attr.value() << std::endl;
		}
	}
	sf::RenderWindow App(sf::VideoMode(window_width, window_height,64), "The Last Dinosaur by Will Hollingsworth, Bina Kakusa, Belol Nessar, and Meghan Tinkler", sf::Style::Titlebar|sf::Style::Close);
	
	//Array holding levels in the order to be played
	std::string levels = ["Level0.xml"];
	
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
		elapsed = fr_clock.getElapsedTime()
		elapsed_ms = elapsed.asSeconds();
		while(App.pollEvent(Event)) {
			if(Event.type == sf::Event::Closed)
				App.close();
		}
		switch(state) {
			case 0: //Create level, for now just load HelloWorldLevel
				LevelFactory::CreateLevel(levels[0], &state);
				state = 1;
				break;
			case 1:
				for (int i = 0; i < LevelFactory::getNumActors(); i ++)
					LevelFactory::actors[i]->update(elapsed_ms);
				break;
			case 2: //Display Diana
				break;
			case 3: //Display craftable
				break;
			case 4:
				for (int i = 0; i < num_actors; i ++)
					actors_list[i]->quit();
				return 0;
			default: 
				break;
		}
		//End structure to control frame rate
		
		//Controls rendering actors and components
		App.clear(sf::Color::Black);

		switch(state) {
			case 0: //Display map
				break;
			case 1: //Display level
				for (int i = 0; i < LevelFactory::getNumActors(); i ++)
					LevelFactory::actors[i]->render(&App);
				break;
			case 2: //Display female
				break;
			case 3: //Display craftable
				break;
			case 4: //Display exit screen
				break;
			default: 
				break;
		}
		App.display();
	}

	return 0;
}

}
