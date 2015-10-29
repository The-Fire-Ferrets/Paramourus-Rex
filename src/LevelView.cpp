#include "LevelView.h"

//Total size of pointer arrays
const int LevelView::size = 20;
//Number of actors populating the level
int LevelView::num_actors = 0;
//Array holding pointers of actors populating the level
std::vector<StrongActorPtr> LevelView::actorList;
//Holds background texture
sf::Texture LevelView::background_texture;
//Edge texture
sf::Texture LevelView::edge_texture;
//Holds background
sf::Sprite LevelView::background;
//Holds edge
sf::Sprite LevelView::edge;
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
StrongActorPtr LevelView::player = NULL;
//Level duration in ms
int LevelView::duration;
//Minimap border
sf::Sprite LevelView::minimap_border;

/** Creates and populates a level and all its components based on XML configuration
 ** resource: filename for xml
 ** state: current game state
 **/
void LevelView::Create(const char* resource, int* state, int flowers[]) {
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
			background = sf::Sprite(background_texture, sf::IntRect(0, 0, background_texture.getSize().x, background_texture.getSize().y));
			background.scale((1.0*Configuration::getWindowWidth())/(background_texture.getSize().x), (1.0*Configuration::getWindowHeight())/(background_texture.getSize().y));
			background.setPosition(sf::Vector2f(0,0));
		}
		else if (!strcmp(attr.name(), "Edge")) {
			edge_texture.loadFromFile(("./assets/backgrounds/" + (std::string)attr.value()).c_str());
			edge = sf::Sprite(edge_texture, sf::IntRect(0, 0, edge_texture.getSize().x, edge_texture.getSize().y));
			edge.scale((1.5*Configuration::getWindowWidth())/(edge_texture.getSize().x), (1.5*Configuration::getWindowHeight())/(edge_texture.getSize().y));
			edge.setPosition(sf::Vector2f(-100,-100));
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
		else if (!strcmp(attr.name(), "Duration")) {
			duration = (std::strtol(attr.value(), &temp, 10));
			if (*temp != '\0') {
				std::cout << "LevelView::Create: Error reading attribute for " << attr.name() << std::endl;
			}
		}
	}
	timer.setPosition(timer_position);
	//Iterates over XML to get components to add
	for (pugi::xml_node tool = tools.first_child(); tool; tool = tool.next_sibling()) {
		if (!strcmp(tool.name(), "Player") && player == NULL) {		
			actorList.push_back(ActorFactory::CreateActor(tool.name(), state));
			(actorList.back())->PostInit(&tool);
			player = (actorList.back());
			num_actors++;
		}
		else if (!strcmp(tool.name(), "Player")) {
			actorList.push_back(player);
			(actorList.back())->PostInit(&tool);
			num_actors++;
		}
		else {
			if (!strcmp(tool.name(), "WaterFlower")) {
				int count = flowers[3];
				//int count = 1;
				while(count-- > 0) {
					actorList.push_back(ActorFactory::CreateActor(tool.name(), state));
					(actorList.back())->PostInit(&tool);
					num_actors++;
				}
			}
			else if (!strcmp(tool.name(), "FireFlower")) {
				int count = flowers[0];
				//int count  = 0;
				while(count-- > 0) {
					actorList.push_back(ActorFactory::CreateActor(tool.name(), state));
					(actorList.back())->PostInit(&tool);
					num_actors++;
				}
			}
			else if (!strcmp(tool.name(), "EarthFlower")) {
				int count = flowers[1];
				//int count = 0;
				while(count-- > 0) {
					actorList.push_back(ActorFactory::CreateActor(tool.name(), state));
					(actorList.back())->PostInit(&tool);
					num_actors++;
				}
			}
			else if (!strcmp(tool.name(), "AirFlower")) {
				int count = flowers[2];
				//int count = 0;
				while(count-- > 0) {
					actorList.push_back(ActorFactory::CreateActor(tool.name(), state));
					(actorList.back())->PostInit(&tool);
					num_actors++;
				}
			}
			else {
				int generate = 1;
				for (pugi::xml_attribute attr = tool.first_attribute(); attr; attr = attr.next_attribute()) {
					if (!strcmp(attr.name(), "Generate")) {
						generate = (std::strtol(attr.value(), &temp, 10));
						if (*temp != '\0') {
							std::cout << "LevelView::Create: Error reading attribute for " << attr.name() << std::endl;
						}
					}
				}
				while (generate-- > 0) {
					actorList.push_back(ActorFactory::CreateActor(tool.name(), state));
					(actorList.back())->PostInit(&tool);
					num_actors++;
				}
			}
		}
	}
	
	minimap_border = sf::Sprite(Configuration::getMinimapBorder(), sf::IntRect(0, 0, (Configuration::getMinimapBorder().getSize()).x, (Configuration::getMinimapBorder().getSize()).y));
	minimap_border.setScale(1.0*Configuration::getWindowWidth()/(Configuration::getMinimapBorder().getSize()).x, 1.0*Configuration::getWindowHeight()/(Configuration::getMinimapBorder().getSize()).y);	
	minimap_border.setPosition(sf::Vector2f(0,0));
	//Set views so can only see a quarter of the map at once
	gameView = sf::View(sf::FloatRect(0, 0, Configuration::getGameViewWidth(), Configuration::getGameViewHeight()));
	//Set minimap to see entire map
	minimapView = sf::View(sf::FloatRect(0, 0, Configuration::getMiniMapViewWidth() + 100, Configuration::getMiniMapViewHeight() + 100));


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
	float timer_time = duration - level_clock.getElapsedTime().asMilliseconds();

	if (timer_time <= 0) {
		*state = 2;
		cleanUp();
	}
	else {
		std::ostringstream out;
		out << std::setprecision(4) << timer_time/1000;
		timer_string = out.str();
		timer.setString(timer_string);
		std::vector<StrongActorPtr>::iterator it;
		for (it = actorList.begin(); it != actorList.end(); it++)
			(*it)->update(time);

		//Set timer to bottom right corner
		sf::Vector2f gameView_bottom_corner = Configuration::getGameViewCenter();
		gameView_bottom_corner.x += Configuration::getGameViewWidth()/2 - timer.getGlobalBounds().width;
		gameView_bottom_corner.y += Configuration::getGameViewHeight()/2 - timer.getGlobalBounds().height*1.5;
		timer.setPosition(gameView_bottom_corner);
	}

}

/** Checks for events and update accordingly
 **
 *
 void LevelView::update(EventInterfacePtr e) {

 }
 */

/** Renders the map onto the window
**
**/
void LevelView::render(sf::RenderWindow *window) {
	//Get the player location and center gameView to it
	sf::Vector2f player_pos = player->getPosition();
	sf::Vector2f player_size = player->getSize();
	gameView.setCenter(player_pos.x + player_size.x/2, player_pos.y + player_size.y/2);
	Configuration::setGameViewDimensions(gameView.getSize().x, gameView.getSize().y);
	Configuration::setGameViewCenter(gameView.getCenter());
	gameView.setViewport(sf::FloatRect(0, 0, 1, 1));
	window->setView(gameView);
	//Update graphics	
	window->draw(edge);
	window->draw(background);
	window->draw(minimap_border);
	std::vector<StrongActorPtr>::iterator it;
	for (it = actorList.begin(); it != actorList.end(); it++)
		(*it)->render(window, false);
	player->render(window, false);
	window->draw(timer);

	//Set minimap view
	minimapView.setViewport(sf::FloatRect(.9, 0, .1, .1));
	window->setView(minimapView);

	//Update graphics
	window->draw(background);
	window->draw(timer);
	for (it = actorList.begin(); it != actorList.end(); it++)
		(*it)->render(window, true);
	player->render(window, true);
	//window->draw(minimap_border);
}

/** Ready the level for start
 **
 **/
void LevelView::start(void) {
	level_clock.restart();
}

/** Return the actor with given instance
 **
 **/
StrongActorPtr LevelView::getActor(int instance) {
	std::vector<StrongActorPtr>::iterator it;
	for (it = actorList.begin(); it != actorList.end(); it++) {
		if ((*it)->getInstance() == instance) {
			return *it;
		}
	}
	return NULL;
}

/** Return the actor with given instance
 **
 **/
void LevelView::removeActor(int instance) {
	std::vector<StrongActorPtr>::iterator it;
	for (it = actorList.begin(); it != actorList.end(); ++it) {
		if ((*it)->getInstance() == instance) {
			actorList.erase(it);
			break;
		}
	}
}
/** Clean up level after completion
 **
 **/
void LevelView::cleanUp(void) {
	std::vector<StrongActorPtr>::iterator it;
	for (it = actorList.begin(); it != actorList.end(); it++) {
		if (*it != player) {
			(*it)->quit();
		}
	}
	num_actors = 0;
	EventManagerInterface::get()->reset();
	ActorFactory::reset();
	actorList.clear();
}

/** Quit level
 **
 **/
void LevelView::quit(void) {
	cleanUp();
}
