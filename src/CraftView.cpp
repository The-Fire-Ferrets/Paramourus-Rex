#include "CraftView.h"
#include "CraftEvent.h"
#include "DialogueView.h"

//Create delegate
EventDelegate CraftView::delegate = NULL;
//Total size of pointer arrays
const int CraftView::size = 20;

// Total number of flowers
int CraftView::fireFlowers = 0;
int CraftView::waterFlowers = 0;
int CraftView::earthFlowers = 0;
int CraftView::airFlowers = 0;
int CraftView::sunFlowers = 0;
int CraftView::tulips = 0;
int CraftView::roses = 0;
int CraftView::violets = 0;
int CraftView::orchids = 0;
int CraftView::lilies = 0;
int CraftView::magnolias = 0;
int CraftView::totalFlowers = 0;
std::string CraftView::flower_str;
sf::Text CraftView::flower_text;

// Number of visits to CraftView. Used to determine which dialogue.xml
// file to use after the crafting and level.
int CraftView::total_craft_visits = 1;

// holds list of flowers actor has collected
std::string CraftView::flower_list[size];
std::vector<StrongActorPtr> CraftView::actorList;
StrongActorPtr CraftView::selectedActor1 = nullptr;
StrongActorPtr CraftView::selectedActor2 = nullptr;

// Holds all of sprites to be drawn
sf::Sprite CraftView::sprites[size];
std::vector<std::tuple<sf::Sprite, std::string>> CraftView::testList;

// Holds positions of said sprites
sf::Vector2f CraftView::positions[size];
// Size of sprites
sf::Vector2f CraftView::sizes[size];
// Texture of sprites
sf::Texture CraftView:: textures[size];

//Holds background texture
sf::Texture CraftView::background_texture;
//Holds background
sf::Sprite CraftView::background;
// Holds map sprite
sf::Sprite CraftView::map;
// Hold recipe book sprite
sf::Sprite CraftView::bookSprite;
// Holds sprite for box 1 and 2 of crafting table
sf::Sprite CraftView::box1Sprite;
sf::Sprite CraftView::box2Sprite;
//Hold text font
sf::Font CraftView::font;
// Holds text "craftable companion" will be speaking
sf::Text CraftView::text;
sf::Vector2f CraftView::text_pos;

// Holds actual recipe pages that will be displayed when player clicks on book
sf::Sprite CraftView::recipeBook;

sf::RectangleShape CraftView::backlay;
sf::RectangleShape CraftView::craftButton;

// Ways to exit the crafting table
sf::RectangleShape CraftView::map_button;
sf::RectangleShape CraftView::dialogue_button;
bool CraftView::has_crafted = false;

bool CraftView::pressed;
// Used to determine if there is an item in both boxes of crafting table
bool CraftView::box1;
bool CraftView::box2;
bool CraftView::drawBook;

bool CraftView::has_delegates = false;
std::vector<EventDelegate> CraftView::delegateFuncList;

// craft music
sf::SoundBuffer CraftView::buffer;
sf::Sound CraftView::sound;

// Homer's sprite
sf::Texture CraftView::character_tex;
sf::Sprite  CraftView::character_sprite;

/** Creates and populates a level and all its components based on XML configuration
 ** resource: filename for xml
 ** state: current game state
 **/
void CraftView::Create(const char* resource, int* state) {
    //Reference to current location in Actor population array
    //Holds referenced to loaded XML file
    totalFlowers = 0;
	has_crafted = false;
    pugi::xml_document doc;
	if (delegate == NULL)
		delegate.bind(&CraftView::update);
	if (!has_delegates) {
		CraftView::addDelegate(CraftEvent::event_type);
		has_delegates = true;
	}

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
        // Font for general text on screen
        else if (!strcmp(attr.name(), "Font")) {
            font.loadFromFile(("./assets/" + (std::string)attr.value()).c_str());
            text.setFont(font);
	    flower_text.setFont(font);
        }
        // Size of dialogue text
        else if (!strcmp(attr.name(), "Text_Size")) {
            text.setCharacterSize(std::strtol(attr.value(), &temp, 10));
	    flower_text.setCharacterSize(25);

            if (*temp != '\0') {
                std::cout << "CraftView::Create: Error reading attribute for " << attr.name() << std::endl;
            }
        }
        // Pos of dialogue text
        else if (!strcmp(attr.name(), "Text_X")) {
            text_pos.x = (std::strtol(attr.value(), &temp, 10));
            if (*temp != '\0') {
                std::cout << "CraftView::Create: Error reading attribute for " << attr.name() << std::endl;
            }
        }
        // Pos of dialogue text
        else if (!strcmp(attr.name(), "Text_Y")) {
            text_pos.y = (std::strtol(attr.value(), &temp, 10));
            if (*temp != '\0') {
                std::cout << "CraftView::Create: Error reading attribute for " << attr.name() << std::endl;
            }
        }
        // texture and position of map icon so it can be interacted with
        else if (!strcmp(attr.name(), "Map")) {
            sf::Texture texture;
    	    if (!texture.loadFromFile(("./assets/sprites/" + (std::string)attr.value()).c_str())){
    		  std::cout << "CraftView::Create: Failed to load " << attr.value();
    	    }
    	    map = sf::Sprite(texture, sf::IntRect(0, 0, Configuration::getWindowWidth()/26.6, Configuration::getWindowHeight()/26.6));
    	    map.setPosition(Configuration::getWindowWidth()/1.05,Configuration::getWindowHeight()/40);
        }
        else if(!strcmp(attr.name(), "Book")) {
	    sf::Texture texture;
	    if (!texture.loadFromFile(("./assets/sprites/" + (std::string)attr.value()).c_str())){
    		  std::cout << "CraftView::Create: Failed to load " << attr.value();
    	    }
    	    bookSprite = sf::Sprite(texture, sf::IntRect(0, 0, Configuration::getWindowWidth()/26.6, Configuration::getWindowHeight()/26.6));
	    bookSprite.setPosition(Configuration::getWindowWidth()/40,Configuration::getWindowHeight()/1.06);
	}
		else if(!strcmp(attr.name(), "Recipes")) {
	    sf::Texture texture;
	    if (!texture.loadFromFile(("./assets/sprites/" + (std::string)attr.value()).c_str())){
    		  std::cout << "CraftView::Create: Failed to load " << attr.value();
    	    }
    	    recipeBook= sf::Sprite(texture, sf::IntRect(0, 0, Configuration::getWindowWidth()/1.33, Configuration::getWindowHeight()/1.33));
	    recipeBook.setPosition(Configuration::getWindowWidth()/7,Configuration::getWindowHeight()/15);
	}
    }

    //Iterates over XML to get components to add, primarily sprites to populate the screen with
    int i = 0;
    for (pugi::xml_node tool = tools.first_child(); tool; tool = tool.next_sibling(), i++) {
	  for (pugi::xml_attribute attr = tool.first_attribute(); attr; attr = attr.next_attribute()){
	      if (!strcmp(attr.name(), "Sprite")) {
		  if (!textures[i].loadFromFile(("./assets/sprites/" + (std::string)attr.value()).c_str() + std::string(".png"))){
			std::cout << "CraftView::Create: Failed to load " << attr.value();
		  }
		  // create sprite w/texture, push it to stack w/associated string it was made with
		  testList.push_back(std::make_tuple(sf::Sprite(textures[i], sf::IntRect(0,0, textures[i].getSize().x, textures[i].getSize().y)), (std::string)attr.value()));
	      }
	      // pos of sprite
	      else if (!strcmp(attr.name(), "X")){
		  positions[i].x = std::strtol(attr.value(), &temp, 10);
		  if (*temp != '\0'){
		      std::cout << "CraftView::Create: Error reading attribute for " << tool.name() << std::endl;
		  }
	      }
	      else if (!strcmp(attr.name(), "Y")) {
		  positions[i].y =std::strtol(attr.value(), &temp, 10);
		  if (*temp != '\0') {
		    std::cout << "CraftView::Create: Error reading attribute for " << tool.name() << std::endl;
		  }
	      }
	      else if (!strcmp(attr.name(), "Width")){
		sizes[i].x = std::strtol(attr.value(), &temp, 10);
		if (*temp != '\0') {
		  std::cout << "CraftView::PostInit: Failed to post-initialize: Error reading attribute for " << attr.name() << std::endl;
		}
	      }
	      else if (!strcmp(attr.name(),"Height")) {
		sizes[i].y = std::strtol(attr.value(), &temp, 10);
		if (*temp != '\0') {
		  std::cout << "CraftView::PostInit: Failed to post-initialize: Error reading attribute for " << attr.name() << std::endl;
		}
	      }
	  }
    }

    // Setting dialogue text position
    text.setPosition(text_pos);
    text.setColor(sf::Color::Black);
    flower_text.setColor(sf::Color::Black);

    // Backlay set off to the side to allow space for item select screens to the left
    backlay.setPosition(Configuration::getWindowWidth()/6.6, Configuration::getWindowHeight()/1.4);
    backlay.setOutlineColor(sf::Color::Black);
    backlay.setFillColor(sf::Color::White);
    backlay.setSize(sf::Vector2f(Configuration::getWindowWidth()/1.3,Configuration::getWindowHeight()/4));
    backlay.setOutlineThickness(5);

    
	map_button.setPosition(Configuration::getWindowWidth() - 110, 10);
	map_button.setSize(sf::Vector2f(Configuration::getWindowWidth()/8, Configuration::getWindowHeight()/15));
	map_button.setFillColor(sf::Color::White);
	map_button.setOutlineColor(sf::Color::Black);
	map_button.setOutlineThickness(5.f);

	dialogue_button.setPosition(Configuration::getWindowWidth() - 110, 70);
	dialogue_button.setSize(sf::Vector2f(Configuration::getWindowWidth()/8, Configuration::getWindowHeight()/15));
	dialogue_button.setFillColor(sf::Color::White);
	dialogue_button.setOutlineColor(sf::Color::Black);
	dialogue_button.setOutlineThickness(5.f);

    craftButton.setPosition(Configuration::getWindowWidth()/4, Configuration::getWindowHeight()/3.53);
    craftButton.setFillColor(sf::Color::White);
    craftButton.setOutlineColor(sf::Color::Black);
    craftButton.setOutlineThickness(5);
    craftButton.setSize(sf::Vector2f(Configuration::getWindowWidth()/10, Configuration::getWindowHeight()/15));

    // gather data for sprites to be rendered on screen
    for (int count = 0; count < i; count++){
    	  sprites[count] = sf::Sprite(textures[count], sf::IntRect(0,0, textures[count].getSize().x, textures[count].getSize().y));
	  sprites[count].setScale(sizes[count].x/(textures[count].getSize()).x, sizes[count].y/(textures[count].getSize()).y);
	  sprites[count].setPosition(positions[count]);
	  std::get<0>(testList[count]).setScale(sizes[count].x/(textures[count].getSize()).x, sizes[count].y/(textures[count].getSize()).y);
	  std::get<0>(testList[count]).setPosition(positions[count]);
    }

    // Checks player for current inventory, updates.
    // Accessing this info from LevelView::Player's CollectorComponent.
    if (LevelView::player != NULL){
      StrongActorPtr player = LevelView::player;
      StrongActorComponentPtr ac = player->components[CollectorComponent::id];
      std::shared_ptr<CollectorComponent> cc = std::dynamic_pointer_cast<CollectorComponent>(ac);
      std::vector<StrongActorPtr> flowers = cc->getFlowers();

      // add flowers to persistent list to make available to other classes
      CraftView::actorList.insert(CraftView::actorList.end(), flowers.begin(), flowers.end());

      // iterate through player's inventory to update inventory on screen
      for (int i=0; i < flowers.size() ; i++){
	// here, flowerList is a vector full of StrongActorPtrs. determine the id of each strongactorptr
	// to determine if it is a fire flower, water flower, air flower, or earth flower
	  if (flowers[i]->isOfType("FireFlower")){
	      fireFlowers++;
	  }
	  else if (flowers[i]->isOfType("WaterFlower")){
	      waterFlowers++;
	  }
	  else if (flowers[i]->isOfType("AirFlower")){
	      airFlowers++;
	  }
	  else if (flowers[i]->isOfType("EarthFlower")){
	      earthFlowers++;
	  }

	  //restore player's vases now that it's cleared space in inventory
	  totalFlowers++;
	  cc->setVases(cc->getVases()+1);
      }
    }

	// draw character art
	unsigned int width = Configuration::getWindowWidth()/10;
	unsigned int posX = Configuration::getWindowWidth()/6.6;
	unsigned int posY = Configuration::getWindowHeight()/1.4;

	character_tex.loadFromFile("./assets/sprites/Homer.png");
	character_sprite = sf::Sprite(character_tex);
	character_sprite.setPosition(posX+320, posY-character_tex.getSize().y-4);


	// set text to initial greeting from Homer
    std::string str = "Welcome back, Phil! You have " + std::to_string(totalFlowers) + " flowers! To craft them, click on their icons.";
    text.setString(fitStringToDialogueBox(str));

    if (!buffer.loadFromFile("./assets/music/marina-s-rhythm.ogg")) {
	std::cout << "CraftView::Create: failed to load music" << std::endl;
    }
    sound.setBuffer(buffer);
    sound.setLoop(true);
    sound.play();
}

int CraftView::getNumFlowers(void) {
    return totalFlowers;
}

/** Handles events happening in craft view and listens for user input.
 **
 **/
void CraftView::update(sf::RenderWindow *window, int* state) {
  	EventManagerInterface::setViewDelegate(delegate);
	EventManagerInterface::setCurrentActorList(&actorList);
      // Anticipates clicking in different areas of the screen
      if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && !pressed) {
        pressed = true;
        const sf::Vector2i pos = sf::Mouse::getPosition(*window);
        if (has_crafted && dialogue_button.getGlobalBounds().contains(pos.x, pos.y)) {
		LevelView::player->reset();
		  *state = 2;
		   DialogueView::Create(("Level" + std::to_string(total_craft_visits)).c_str(), state);

		  ++total_craft_visits;
		  cleanUp();
        }
		else if (map_button.getGlobalBounds().contains(pos.x, pos.y)) {
			LevelView::player->reset();
			*state = 0;
			cleanUp();
		}

        bool inList;

        // checking to see if flowers clicked on
        for (int i = 0; i <= 10; i++){
		    // player attempts to use this item for crafting by clicking on them, but only if one of the two boxes are filled
		    if (sprites[i].getGlobalBounds().contains(pos.x,pos.y)){
			      // check if this sprite exists within the flowerlist - if so, sets
			      // selected actor equal to the first flower it finds of that type
			      for (int j = 0; j < actorList.size(); j++){
					    // here, element 1 of testList[i]
					    if (actorList[j]->isOfType(std::get<1>(testList[i]))){
						if (box1 == false){
				    			selectedActor1 = actorList[j];
							break;
				    		}
				    		else if (box2 == false && actorList[j] != selectedActor1){
				    			selectedActor2 = actorList[j];
							break;
				    		}
				    	}
			       }

			      inList = false;

			      if (i == 0 && fireFlowers > 0 && (box1 == false || box2 == false)){
					fireFlowers--;
					inList = true;
			      }
			      else if (i == 1 && waterFlowers > 0 && (box1 == false || box2 == false)){
					waterFlowers--;
					inList = true;
			      }
			      else if (i == 2 && earthFlowers > 0 && (box1 == false || box2 == false)){
					earthFlowers--;
					inList = true;
			      }
			      else if (i == 3 && airFlowers > 0 && (box1 == false || box2 == false)){
					airFlowers--;
					inList = true;
			      }
			      if (i == 4 && sunFlowers > 0 && (box1 == false || box2 == false)){
					sunFlowers--;
					inList = true;
			      }
			      else if (i == 5 && tulips > 0 && (box1 == false || box2 == false)){
					tulips--;
					inList = true;
			      }
			      else if (i == 6 && roses > 0 && (box1 == false || box2 == false)){
					roses--;
					inList = true;
			      }
			      else if (i == 7 && violets > 0 && (box1 == false || box2 == false)){
					violets--;
					inList = true;
			      }
			      else if (i == 8 && lilies > 0 && (box1 == false || box2 == false)){
					lilies--;
					inList = true;
			      }
			      else if (i == 9 && orchids > 0 && (box1 == false || box2 == false)){
					violets--;
					inList = true;
			      }
			      else if (i == 10 && magnolias > 0 && (box1 == false || box2 == false)){
					magnolias--;
					inList = true;
			      }

			      if (box1 == false && inList == true){
					 // draw sprite in box 1
					  box1 = true;
					  box1Sprite = sprites[i];
					  totalFlowers--;
					  // scale the sprite up in size?
					  //box1Sprite.setScale(sprites[i].width*4, sprites[i].height*4);
					  box1Sprite.setPosition(Configuration::getWindowWidth()/5.71,Configuration::getWindowHeight()/12);
					  // do some sort of check here for if it's a base flower -- if it's a tier flower, needs to be scaled differently
					  box1Sprite.setScale(1.0f, 1.0f);
				  }
			      else if (box2 == false && inList == true){
					 // draw sprite in box 2
					box2 = true;
					box2Sprite = sprites[i];
					box2Sprite.setPosition(Configuration::getWindowWidth()/3.2,Configuration::getWindowHeight()/12);
					box2Sprite.setScale(1.0f, 1.0f);
					totalFlowers--;
				  }
		    }
	}

	// Check here for the 'Craft' button to have been clicked and two flowers have been selected to be filled
	if (craftButton.getGlobalBounds().contains(pos.x, pos.y) && box1 == true && box2 == true){


	   StrongActorComponentPtr actor1AC = selectedActor1->components[CraftableComponent::id];
	   std::shared_ptr<CraftableComponent> actor1CC = std::dynamic_pointer_cast<CraftableComponent>(actor1AC);
	   StrongActorComponentPtr actor2AC = selectedActor2->components[CraftableComponent::id];
	   std::shared_ptr<CraftableComponent> actor2CC = std::dynamic_pointer_cast<CraftableComponent>(actor2AC);


	   // Clear sprite image, add newly combined sprite to inventory?
	   if (actor1CC->doesCombineWith(*actor2CC)){
		   std::cout << "combining" << std::endl;
		   box1 = false;
		   box2 = false;
		   //actor1CC->combineWith(*actor2CC);
		   if (!EventManagerInterface::get()->queueEvent(new CraftEvent(0, selectedActor1->getInstance(), selectedActor2->getInstance())))
			   std::cout << "CraftView::update: Unable to queue event" << std::endl;
		   std::cout << "Sent craft event to Flower: " << selectedActor1->getInstance() << " and Flower: " << selectedActor2->getInstance() << std::endl;
		   selectedActor1 = nullptr;
		   selectedActor2 = nullptr;
	   }

	   // update text box to indicate that you cannot combine flowers
	   else {
	     std::cout << "CraftView::Update: Unable to craft " << selectedActor1->getInstance() << " and Flower: " << selectedActor2->getInstance() << std::endl;
	     text.setString(fitStringToDialogueBox("Homer not think Diana-lady like those flowers, Phil. Phil try something else."));
	   }
	}

	// Check to see if flowers within the craft box are clicked to return them to player inventory
	if (box1Sprite.getGlobalBounds().contains(pos.x,pos.y)){
	    std::cout << "CraftView::Update: Returning to inventory flower of type " + selectedActor1->getId().back();
	    box1 = false;
	    returnFlower(selectedActor1);
	}

	// Attempting to remove flower from box2 of craft table and return them to player inventory
	if (box2Sprite.getGlobalBounds().contains(pos.x,pos.y)){
	    std::cout << "CraftView::Update: Returning to inventory flower of type " + selectedActor2->getId().back() << std::endl;
	    box2 = false;
	    returnFlower(selectedActor2);
	}

	// Draw recipe book
	if (bookSprite.getGlobalBounds().contains(pos.x, pos.y)){
	    if (drawBook == true){
	      drawBook = false;
	    }
	    else{
	      drawBook = true;
	    }
	}

     }

      // mouse click released
    else if (!(sf::Mouse::isButtonPressed(sf::Mouse::Left))) {
        pressed = false;
     }

}


/**
 *  Returns the given flower to the player's inventory rather than crafting it
 */
void CraftView::returnFlower(StrongActorPtr flower){

	//std::cout << "are we segfaulting here";
  
      if (flower->isOfType("FireFlower")){
	fireFlowers++;
      }
      else if (flower->isOfType("WaterFlower")){
	waterFlowers++;
      }
      else if (flower->isOfType("EarthFlower")){
	earthFlowers++;
      }
      else if (flower->isOfType("AirFlower")){
	airFlowers++;
      }
      else if (flower->isOfType("SunFlower")){
	sunFlowers++;
      }
      else if (flower->isOfType("Tulip")){
	tulips++;
      }
      else if (flower->isOfType("Rose")){
	roses++;
      }
      else if (flower->isOfType("Violet")){
	violets++;
      }
      else if (flower->isOfType("Lily")){
	lilies++;
      }
      else if (flower->isOfType("Orchid")){
	orchids++;
      }
      else if (flower->isOfType("Magnolia")){
	magnolias++;
      }
      totalFlowers++;
	//std::cout << "end of the returnFlower function";
}

/**
 *  Removes the given flower from the flower count upon visiting Diana
 */
void CraftView::updateFlowerCount(std::string flower){
    
      if (flower == "SunFlower"){
	sunFlowers--;
      }
      else if (flower == "Tulip"){
	tulips--;
      }
      else if (flower == "Rose"){
	roses--;
      }
      else if (flower == "Violet"){
	violets--;
      }
      else if (flower =="Lily"){
	lilies--;
      }
      else if (flower == "Orchid"){
	orchids--;
      }
      else if (flower == "Magnolia"){
	magnolias--;
      }
      totalFlowers--;
}


/** Checks for events and update accordingly
 **
 **/
void CraftView::update(EventInterfacePtr e) {
	// fiugre out who just got crafted so we can display their new state
	EventType event_type = e->getEventType();
	static int visits = 0; // two events are being sent

	if (visits == 0 && e->getReceiver() == -1 && event_type == CraftEvent::event_type) {
		StrongActorPtr sender = CraftView::getFlower(e->getSender());
		if (sender->hasComponent(CraftableComponent::id)) {
			// item crafting completed
			StrongActorComponentPtr ac = sender->components[CraftableComponent::id];
			text.setString(fitStringToDialogueBox("Homer make Phil new " + ac->getType() + "! Phil go talk to nice Diana-lady now?"));
			if (ac->getType() == "SunFlower"){
				sunFlowers++;
			}
			else if (ac->getType() == "Tulip"){
				tulips++;
			}
			else if (ac->getType() == "Rose"){
				roses++;
			}
			else if (ac->getType() == "Violet"){
				violets++;
			}
			else if (ac->getType() == "Lily"){
				lilies++;
			}
			else if (ac->getType() == "Orchid"){
				orchids++;
			}
			else if (ac->getType() == "Magnolia"){
				magnolias++;
			}
			has_crafted = true;
		}
	}
	++visits;
	if (visits == 2) visits = 0;
}

/** Renders the backdrop and menu selects onto the window, as well as option to
 ** head back to map.
 **
 **/
void CraftView::render(sf::RenderWindow *window) {
	float button_width, text_width;
	sf::Vector2f pos;

    //Update graphics
    window->draw(background);
    window->draw(backlay);
    window->draw(text);
    //window->draw(map);
    window->draw(bookSprite);
	window->draw(character_sprite);
	window->draw(map_button);

    // Setting crafting button elements
	sf::Text button_text("Craft", font);
	button_text.setPosition(Configuration::getWindowWidth()/4,Configuration::getWindowHeight()/3.53);
	button_text.setColor(sf::Color::Black);

	sf::Text map_text("Map", font);
	map_text.setColor(sf::Color::Black);
	button_width = map_button.getSize().x;
	text_width   = map_text.getGlobalBounds().width;

	pos = map_button.getPosition();
	pos.x += (button_width-text_width) / 2.f;
	map_text.setPosition(pos);
	window->draw(map_text);

	if (has_crafted) {
		sf::Text dialogue_text("Diana", font);
		dialogue_text.setColor(sf::Color::Black);
		button_width = dialogue_button.getSize().x;
		text_width   = dialogue_text.getGlobalBounds().width;

		pos = dialogue_button.getPosition();
		pos.x += (button_width-text_width) / 2.f;
		dialogue_text.setPosition(pos);

		window->draw(dialogue_button);
		window->draw(dialogue_text);
	}

    // draw sprite in the "crafting table" box if there is one in it
    if (box1 == true)
      window->draw(box1Sprite);
    if (box2 == true)
      window->draw(box2Sprite);

    // draw recipe book if it's open
    if (drawBook == true){
      window->draw(recipeBook);
    }

    // draw craft button and text on it
    window->draw(craftButton);
    window->draw(button_text);

    
    int width = Configuration::getWindowWidth()/13.3;


    // draw flower sprites on left
    for (int i = 0; i < 10; i++){
	window->draw(sprites[i]);
	int height;
	if (i == 0){
	  flower_str = "x " + std::to_string(fireFlowers);
	  height = Configuration::getWindowHeight()/60;
	}
	else if (i ==1){
	  flower_str = "x " + std::to_string(waterFlowers);
	  height = Configuration::getWindowHeight()/12;
	}
	else if (i == 2){
	  flower_str = "x " + std::to_string(earthFlowers);
	  height = Configuration::getWindowHeight()/6.67;
	  
	}
	else if (i == 3){
	  flower_str = "x " + std::to_string(airFlowers);
	  height = Configuration::getWindowHeight()/4.62;
	}
	else if (i == 4){
	  flower_str = "x " + std::to_string(sunFlowers);
	  height = Configuration::getWindowHeight()/3.53;
	  
	}
	else if (i == 5){
	  flower_str = "x " + std::to_string(tulips);
	  height = Configuration::getWindowHeight()/2.86;
	}
	else if (i == 6){
	  flower_str = "x " + std::to_string(roses);
	  height = Configuration::getWindowHeight()/2.4;
	}
	else if (i == 7){
	  flower_str = "x " + std::to_string(violets);
	  height = Configuration::getWindowHeight()/2.06;
	}
	else if (i == 8){
	 flower_str = "x " + std::to_string(lilies);
	  height = Configuration::getWindowHeight()/1.81;
	}
	else if (i == 9){
	 flower_str = "x " + std::to_string(orchids);
	  height = Configuration::getWindowHeight()/1.61;
	}
	else if (i == 10){
	  flower_str = "x " + std::to_string(magnolias);
	  height = Configuration::getWindowHeight()/1.46;
	}
	sf::Text flower_text(flower_str, font);
	flower_text.setPosition(sprites[i].getLocalBounds().left+width, sprites[i].getLocalBounds().top+height);
	flower_text.setColor(sf::Color::Black);
	window->draw(flower_text);
    }

}


/** Clean up level after completion
 **
 **/
void CraftView::cleanUp(void) {
	sound.stop();
	// ++total_craft_visits;
}

/** Quit level
 **
 **/
void CraftView::quit(void) {
    cleanUp();
}

/** Returns a pointer to flower instance, if present
 **
 **/
StrongActorPtr CraftView::getFlower(int instance) {
	for (auto it = CraftView::actorList.begin(); it != CraftView::actorList.end(); it++) {
		if ((*it)->getInstance() == instance) {
			return *it;
		}
	}
	return nullptr;
}

/** Returns a pointer to a flower instance of the specified type, if present
 ** type: the type of flower to look for
 **/
StrongActorPtr CraftView::getFlower(std::string type) {
	for (auto it = CraftView::actorList.begin(); it != CraftView::actorList.end(); it++) {
		std::shared_ptr<ActorComponent> ac = (*it)->components[CraftableComponent::id];
		if (ac->getType() == type) {
			return *it;
		}
	}
	return nullptr;
}

/** Removes the given flower, if present
 ** TODO: does not modify counts of WEFA flowers or the flower_list
 **/
bool CraftView::removeFlower(StrongActorPtr flower) {
	for (auto it = CraftView::actorList.begin(); it != CraftView::actorList.end(); it++) {
		if ( *(*it) == (*flower)) {
			CraftView::actorList.erase(it);
			return true;
		}
	}

	return false;
}

/** Adds an event listener for the given event type
 **
 **/
void CraftView::addDelegate(EventType type) {
	CraftView::delegateFuncList.push_back(delegate);
	if (!EventManagerInterface::get()->addDelegate(delegateFuncList.back(), EventInstance(type, -1))) {
		std::cout << "CraftView::Create: Unable to register delegate function" << std::endl;
	}
}

/** Rewraps the given string such that it can be displayed
 ** appropriately in the dialogue box.  Assumes the string
 ** will fit into a single box.
 ** str: the string to rewrap
 **/
std::string CraftView::fitStringToDialogueBox(std::string str) {
	// get dialogue box bounds
	int box_begin_x = backlay.getPosition().x;
	int box_begin_y = backlay.getPosition().x;
	int box_end_x   = box_begin_x + backlay.getSize().x;
	int box_end_y   = box_begin_y + backlay.getSize().y;

	int text_begin_x = text_pos.x;
	int text_begin_y = text_pos.y;
	int text_end_x   = box_end_x - (text_begin_x - box_begin_x);
	int text_end_y   = box_end_y - (text_begin_y - box_begin_y);

	int max_width = text_end_x - text_begin_x;

	// text object used to see how close each word puts us to the bounds
	sf::Text temp;
	temp.setFont(font);
	temp.setCharacterSize(text.getCharacterSize());

	// current string and width
	std::string fitted_string = "";
	float current_width = 0.f;
	float word_width = 0.f, word_height = 0.f;

	// split the dialogue into words;
	std::vector<std::string> words = split(str, ' ');

	// for each word...
	for (std::string word : words) {
		// get the bounding box
		temp.setString(word + " ");
		word_width = temp.findCharacterPos(temp.getString().getSize()).x;

		// will it go past the horizontal bound?
		if (current_width + word_width > max_width) {
			fitted_string += "\n" + word + " ";
			current_width = word_width;
		}
		else {
			// just add to string
			fitted_string += word + " ";
			current_width += word_width;
		}
	}

	// done
	return fitted_string;
}
