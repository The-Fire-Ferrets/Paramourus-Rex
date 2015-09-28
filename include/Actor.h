#ifndef ACTOR_H
#define ACTOR_H

#include "Constants.h"
#include "ActorComponent.h"

class Actor {
	friend class ActorFactory;
	public:
		static int instances;
		int getInstance(void);
		
		ActorComponents components;
		ActorId getId(void);		
		
		explicit Actor(ActorId actorid);
		bool Init(pugi::xml_node* elem);
		void PostInit(void);
		~Actor(void);

		void move(float time);
		
		void update(float time);
		void render(sf::RenderWindow *window);
		void reset(void);
		void restart(void);
		void quit(void);
		
		sf::Vector2f getPosition(void);
		void setPosition(sf::Vector2f pos);
		
		int getDirection(void);
		void setDirection(int dir);	
		
		sf::Vector2f getSize(void);
		void setSize(sf::Vector2f sz);
		
		int getState(void);
		void setState(int actorState);
		int* getGameState(void);
		void setGameState(int state);
		void resetGameState(int* state);
			
		sf::FloatRect* getBoundary(void);		

	private:
		ActorId id;
		
		std::string texture_filename;
		std::string sprite_filename;
		
		sf::Vector2f position;
		sf::Texture texture;
		sf::Vector2f size;
		
		int direction;
		int instance;
		int* game_state;
		int state;

		sf::FloatRect* obstacle;
		sf::FloatRect* boundary;
		
		void AddComponent(StrongActorComponentPtr component);
		void updateBoundary(void);
};

#endif
