#include "DialogueView.h"

// Total size of pointer arrays
const int DialogueView::size = 20;

// DIALOGUE VARIABLES
// Total number of dialogue boxes
int DialogueView::numDialogues = 0;
int DialogueView::index = 0;
// Array holding pointers to the dialogue that's to fill the boxes at cutscenes
std::string DialogueView::boxes[size];
// Text in the specific node we are looking at that will show up in Dialogue box
std::string DialogueView::dialogue_str;
//Dialogue setPosition
sf::Vector2f DialogueView::dialogue_pos;

// Actual text to be rendered
sf::Text DialogueView::text;
sf::Font DialogueView::font;

// BACKGROUND VARIABLES
sf::Texture DialogueView::background_texture;
sf::Sprite DialogueView::background;

// Images of Diana and Phil to be rendered
sf::Vector2f DialogueView::dianaPos;
sf::Vector2f DialogueView::philPos;
sf::Sprite DialogueView::Diana;
sf::Sprite DialogueView::Phil;
bool DialogueView::pressed = false;

/** Searches for the correct dialogue box the player is on and populates the text with what you want Diana to be saying 
** resource: filename for XML  file we are getting the dialogue from. Currently just level0, only level we have.
** state: current game state (state should be 2 if in dialogue view)
**/
void DialogueView::Create(const char* resource, int* state){
	//Reset the index before each iteration
	index = 0;
	// reference to XML file we are getting our info from
	pugi::xml_document doc;

	pugi::xml_parse_result result;
	std::string fileString(resource);

	// Checks to make sure XML file exists and was correctly loaded
	if (!(result = doc.load_file(("./assets/dialogue/" + fileString + ".xml").c_str()))){
		std::cout << "DialogueView::Create(...): Failed to load" << std::endl;
		std::cout << "Filename: " << resource << " Load result: " << result.description() << std::endl;
	}
	
	// this is called every time update is called -
	// we iterate through the xml file until we get to the correct index,
	// and then we display the text found in that node.
	//Used to iterate over XML file to get attributes
	pugi::xml_node tools = doc.child(resource);
	char* temp;	
	for (pugi::xml_attribute attr = tools.first_attribute(); attr; attr = attr.next_attribute()) {
		if (!strcmp(attr.name(), "NumDialogues")){
			numDialogues = atoi(attr.value());
		}
		else if (!strcmp(attr.name(), "Background")) {
			background_texture.loadFromFile(("./assets/backgrounds/" + (std::string)attr.value()).c_str());
			background = sf::Sprite(background_texture, sf::IntRect(0, 0, 800, 800));
			background.setPosition(sf::Vector2f(0,0));
		}
		else if (!strcmp(attr.name(), "Font")) {
			font.loadFromFile(("./assets/" + (std::string)attr.value()).c_str());
			text.setFont(font);
		}
		else if (!strcmp(attr.name(), "Text_Size")) {
			text.setCharacterSize(std::strtol(attr.value(), &temp, 10));
			if (*temp != '\0') {
				std::cout << "DialogueView::Create: Error reading attribute for " << attr.name() << std::endl;
			}
		}
		else if (!strcmp(attr.name(), "Text_X")) {
			dialogue_pos.x = (std::strtol(attr.value(), &temp, 10));
			if (*temp != '\0') {
				std::cout << "DialogueView::Create: Error reading attribute for " << attr.name() << std::endl;
			}
		}
		else if (!strcmp(attr.name(), "Text_Y")) {
			dialogue_pos.y = (std::strtol(attr.value(), &temp, 10));
			if (*temp != '\0') {
				std::cout << "DialogueView::Create: Error reading attribute for " << attr.name() << std::endl;
			}
		}
		// loading texture for Phil's image here
		else if (!strcmp(attr.name(), "Phil")){
		  
		}
		// loading texture for Diana's image here
		else if (!strcmp(attr.name(), "Diana")){
		  
		}
		else if (!strcmp(attr.name(), "PhilPosX")){
		    philPos.x = (std::strtol(attr.value(), &temp, 10));
		    if (*temp != '\0') {
			std::cout << "DialogueView::Create: Error reading attribute for " << attr.name() << std::endl;
		    }
		}
		else if (!strcmp(attr.name(), "PhilPosY")){
		    philPos.y = (std::strtol(attr.value(), &temp, 10));
		    if (*temp != '\0') {
			std::cout << "DialogueView::Create: Error reading attribute for " << attr.name() << std::endl;
		    }
		}
		else if (!strcmp(attr.name(), "DianaPosX")){
		    dianaPos.x = (std::strtol(attr.value(), &temp, 10));
		    if (*temp != '\0') {
			std::cout << "DialogueView::Create: Error reading attribute for " << attr.name() << std::endl;
		    }
		}
		else if (!strcmp(attr.name(), "DianaPosY")){
		    dianaPos.y = (std::strtol(attr.value(), &temp, 10));
		    if (*temp != '\0') {
			std::cout << "DialogueView::Create: Error reading attribute for " << attr.name() << std::endl;
		    }
		}
	}
	
	text.setPosition(dialogue_pos);
	Diana.setPosition(dianaPos);
	Phil.setPosition(philPos);
	
	int count = 0;
	
	// navigating through xml files and storing the actual dialogue into array
	for (pugi::xml_node tool = tools.first_child(); tool; tool =tool.next_sibling()){
	    for (pugi::xml_attribute attr = tool.first_attribute(); attr; attr = attr.next_attribute()) {
		if (!strcmp(attr.name(), "Text")){
		    boxes[count] = attr.value();
		    count++;
		}
		// store values in seperate array, if it's even necessary to change background -- may just take this out
		// if background remains static?
		if (!strcmp(attr.name(), "Background")){
		  
		}
	    }
	}

}


/** If a key is pressed, updates the index so that 
** it switches to the next desired block of text.  Called from main.
**/
void DialogueView::update(sf::RenderWindow *window, int* state){
	if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && !pressed){
		pressed = true;
	//sf::Event event;
	//while (window->pollEvent(event)){
	  /*** ISSUE CURRENTLY HERE: Tried doing the MouseButtonPressed event,
	   * but it was rapidly indexing here for the duration of the mouse click --
	   * i.e., no matter how quickly I clicked, it would call this and index about
	   * 50 times, giving no opportunity for the text to display. Working on fixing.
	   ***/
	    //if (event.type == sf::Event::MouseButtonReleased){
		std::cout << index << std::endl;
		if (index > numDialogues){
		    *state = 0;
		}
		else{
		  text.setString(boxes[index]);
		}
		index++;
	    //}
	}
	else if (!sf::Mouse::isButtonPressed(sf::Mouse::Left)){
		pressed = false;
	}
}

/** Checks for events and updates accordingly
 * **
 */
void DialogueView::update(EventInterfacePtr e){

}


// Draws the dialogue, Diana and Phil
void DialogueView::render(sf::RenderWindow *window){
    sf::RectangleShape backlay;
    backlay.setPosition(20, 560);
    backlay.setOutlineColor(sf::Color::Black);
    backlay.setFillColor(sf::Color::White);
    backlay.setSize(sf::Vector2f(760,200));
    backlay.setOutlineThickness(5);
    text.setColor(sf::Color::Black);
    
    window->draw(background);
    window->draw(backlay);
    window->draw(text);
  // once Diana and Phil sprites are finished, will be rendered here as well
}


