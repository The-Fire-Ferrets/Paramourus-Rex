#include "DialogueView.h" 
#include "CraftView.h"
#include "Utilities.h"
#include <vector>
#include <algorithm>
#include <cmath>

// Total size of pointer arrays
const int DialogueView::size = 20;

// Dialogue name
std::string DialogueView::name;
//State of view 0 = intro; 1 = normal
int DialogueView::view_state = 1;
// DIALOGUE VARIABLES
// Total number of dialogue boxes
int DialogueView::numDialogues = 0;
int DialogueView::index = 0;
// Array holding pointers to the dialogue that's to fill the boxes at cutscenes
std::vector<std::string> DialogueView::boxes;
std::vector<std::string> DialogueView::dialogues;
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
sf::RectangleShape DialogueView::backlay;

// Images of Diana and Phil to be rendered
sf::Vector2f DialogueView::dianaPos;
sf::Vector2f DialogueView::philPos;
sf::Vector2f DialogueView::dianaSize;
sf::Vector2f DialogueView::philSize;
sf::Sprite DialogueView::diana_sprite;
sf::Sprite DialogueView::phil_sprite;
sf::Texture DialogueView::phil_texture;
sf::Texture DialogueView::diana_texture;

bool DialogueView::pressed = false;

// dialogue music
sf::SoundBuffer DialogueView::buffer;
sf::Sound DialogueView::sound;

bool DialogueView::solved = false;

/** Searches for the correct dialogue box the player is on and populates the text with what you want Diana to be saying 
 ** resource: filename for XML  file we are getting the dialogue from. Currently just level0, only level we have.
 ** state: current game state (state should be 2 if in dialogue view)
 **/
void DialogueView::Create(const char* resource, int* state){
	dialogues.clear();
	boxes.clear();

	//Reset the index before each iteration
	index = 0;
	// reference to XML file we are getting our info from
	pugi::xml_document doc;

	pugi::xml_parse_result result;
	std::string fileString(resource);
	name = fileString;
	if (name == "Level0") {
		view_state = 2;
	}
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
			background = sf::Sprite(background_texture, sf::IntRect(0, 0, Configuration::getWindowWidth(), Configuration::getWindowHeight()));
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
            phil_texture.loadFromFile(("./assets/sprites/" + (std::string)attr.value()).c_str());
		}
		// loading texture for Diana's image here
		else if (!strcmp(attr.name(), "Diana")){
            diana_texture.loadFromFile(("./assets/sprites/" + (std::string)attr.value()).c_str());
		}
		else if (!strcmp(attr.name(), "PhilSizeX")){
			philSize.x = (std::strtol(attr.value(), &temp, 10));
			if (*temp != '\0') {
				std::cout << "DialogueView::Create: Error reading attribute for " << attr.name() << std::endl;
			}
		}
		else if (!strcmp(attr.name(), "PhilSizeY")){
			philSize.y = (std::strtol(attr.value(), &temp, 10));
			if (*temp != '\0') {
				std::cout << "DialogueView::Create: Error reading attribute for " << attr.name() << std::endl;
			}
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
		else if (!strcmp(attr.name(), "DianaSizeX")){
			dianaSize.x = (std::strtol(attr.value(), &temp, 10));
			if (*temp != '\0') {
				std::cout << "DialogueView::Create: Error reading attribute for " << attr.name() << std::endl;
			}
		}
		else if (!strcmp(attr.name(), "DianaSizeY")){
			dianaSize.y = (std::strtol(attr.value(), &temp, 10));
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
		else if (!strcmp(attr.name(), "Flower")) {
			StrongActorPtr flower = CraftView::getFlower(attr.value());
			if (flower) {
				CraftView::removeFlower(flower);
				solved = true;
			}
		}
	}

	text.setPosition(dialogue_pos);
	text.setColor(sf::Color::Black);

	// Create phil_ and diana_sprite here I think?
	std::cout << "dianaPos.x = " << dianaPos.x << "and y=" << dianaPos.y << std::endl;
	diana_sprite.setPosition(dianaPos);
		diana_sprite = sf::Sprite(diana_texture);
		diana_sprite.setScale(dianaSize.x/(diana_texture.getSize()).x, dianaSize.y/(diana_texture.getSize()).y);
	    // diana_sprite.setPosition(sf::Vector2f(0,Configuration::getWindowHeight() - diana_sprite.getGlobalBounds().height));
	phil_sprite.setPosition(philPos);
	
	unsigned int width = Configuration::getWindowWidth()/1.05;
	unsigned int height = Configuration::getWindowHeight()/4;
	unsigned int posX = Configuration::getWindowWidth()/40;
	unsigned int posY = Configuration::getWindowHeight()/1.4;

	backlay.setPosition(posX, posY);
	backlay.setOutlineColor(sf::Color::Black);
	backlay.setFillColor(sf::Color::White);
	backlay.setSize(sf::Vector2f(width, height));
	backlay.setOutlineThickness(5);
	text.setColor(sf::Color::Black);
	text.setPosition(posX, posY);

	// navigating through xml files and storing the actual dialogue into array
	if (fileString != "Level0") {
		tools = (solved == true) ? (tools.child("Correct")) : (tools.child("Incorrect"));
	}
	for (pugi::xml_node tool = tools.first_child(); tool; tool =tool.next_sibling()){
		std::string str = "";
		for (pugi::xml_attribute attr = tool.first_attribute(); attr; attr = attr.next_attribute()) {
			if (!strcmp(attr.name(), "Speaker") && strcmp(attr.value(), "Narrator")) {
				str += (std::string) attr.value() + ": ";
			}
			if (!strcmp(attr.name(), "Text")){
				str += attr.value();
				dialogues.push_back(str);
			}
			// store values in seperate array, if it's even necessary to change background -- may just take this out
			// if background remains static?
			if (!strcmp(attr.name(), "Background")){

			}
		}
	}

	// generatre dialogue boxes
	for (std::string dialogue : dialogues) {
		std::vector<std::string> fitted = fitStringToDialogueBox(dialogue);
		boxes.insert(boxes.end(), fitted.begin(), fitted.end());
	}

	if (!buffer.loadFromFile("./assets/music/romantic-arpeggio-loop.ogg")) {
		std::cout << "DialogueView::Create: failed to load music" << std::endl;
	}
	sound.setBuffer(buffer);
	sound.setLoop(true);
	sound.play();
}


/** If a key is pressed, updates the index so that 
 ** it switches to the next desired block of text.  Called from main.
 **/
void DialogueView::update(sf::RenderWindow *window, int* state){
	sf::Vector2i mouse_pos = sf::Mouse::getPosition(*window);
	if (mouse_pos.x > 0.f && mouse_pos.x < Configuration::getWindowWidth()
			&& mouse_pos.y > 0.f && mouse_pos.y < Configuration::getWindowHeight())
	{
		if ((sf::Mouse::isButtonPressed(sf::Mouse::Left) && !pressed) || index == 0){
			pressed = true;
			std::cout << index << " " << boxes.size() << std::endl;
			if (index >= boxes.size()){
				if (view_state == 1)
					*state = 0;
				else if (view_state == 2)
					*state = 5;
				cleanUp();
			}
			else{
				text.setString(boxes[index]);
				//text.setString(fitStringToDialogueBox(boxes[index]));
			}
			index++;
		}
		else if (!sf::Mouse::isButtonPressed(sf::Mouse::Left)){
			pressed = false;
		}
	}
}

/** Checks for events and updates accordingly
 * **
 */
void DialogueView::update(EventInterfacePtr e){

}


// Draws the dialogue, Diana and Phil
void DialogueView::render(sf::RenderWindow *window){
	window->draw(background);
	window->draw(backlay);
	window->draw(text);
	// once Diana and Phil sprites are finished, will be rendered here as well
	window->draw(phil_sprite);
	window->draw(diana_sprite);
}

/** Play nice with the resources...
 **
 **/
void DialogueView::cleanUp() {
	sound.stop();
	solved = false;
}

std::vector<std::string> DialogueView::fitStringToDialogueBox(std::string str) {
	// get dialogue box bounds
	int width = Configuration::getWindowWidth()/1.05;
	int beginX = Configuration::getWindowWidth()/40;
	int endX = beginX+width;
	int max_width = endX-beginX;

	int height = Configuration::getWindowHeight()/4;
	int beginY = Configuration::getWindowHeight()/1.4;
	int endY = beginY+height;
	int max_height = (endY-beginY);

	// text object used to see how close each word puts us to the bounds
	sf::Text temp;
	temp.setFont(font);
	temp.setCharacterSize(text.getCharacterSize());

	// current string and width
	std::vector<std::string> boxes;
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

		// general word height (changes, hence the max)
		sf::FloatRect bounds = temp.getGlobalBounds();
		int line_spacing = font.getLineSpacing(temp.getCharacterSize());
		word_height = std::max(bounds.height-bounds.top+line_spacing, word_height);

		// the height of the full string so far
		temp.setString(fitted_string);
		float full_height = temp.getGlobalBounds().height - temp.getGlobalBounds().top;

		// will it go past the horizontal bound?
		if (current_width + word_width > max_width) {
			// will it go past the vertical bound?
			if (max_height - full_height < word_height) {
				boxes.push_back(fitted_string);
				fitted_string = word + " ";
				current_width = word_width;
			}
			else {
				fitted_string += "\n" + word + " ";
				current_width = word_width;
			}
		}
		else {
			// just add to string
			fitted_string += word + " ";
			current_width += word_width;
		}
	}

	// done
	return boxes;
}
