#pragma once

#include <iostream>
#include <vector>
#include <math.h>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>

/*
*
* Game engine
*
*
*/


class Game {
    private:
        //vars
        sf::RenderWindow* window;
        sf::Event ev;
        sf::VideoMode videoMode;
        sf::Color bg_color;

        //Mouse
        sf::Vector2f mousePosWindow;
        
        
        //sf::VertexArray rays = sf::VertexArray(sf::LineStrip, 2);
        
        //Game objects
        sf::CircleShape source = sf::CircleShape();
        std::vector<sf::VertexArray> rays;
        std::vector<sf::VertexArray> barriers;

        //drawing variables

        bool drawing = false;        
        sf::VertexArray sketch = sf::VertexArray(sf::LineStrip,2);
        
        //constants
        const int step = 90;
        const float accuracy = 10.f;


        //initialise basics

        void initVariables(){
            this->window = nullptr;
            this->bg_color = sf::Color(0,0,0,255);
            this->videoMode.height = 480;
            this->videoMode.width = 640;



        }

        void initWindow(){
            this->window = new sf::RenderWindow(this->videoMode, "My first game", 
	        sf::Style::Titlebar | sf::Style::Close);
            this->window->setFramerateLimit(60);
        }

        void initShapes(){
            this->source.setRadius(10.f);
            this->source.setFillColor(sf::Color::Red);
            

                for(int i = 0; i < this->step; i++){
                    sf::VertexArray ray = sf::VertexArray(sf::LineStrip,2);
                    ray[0].position = sf::Vector2f(0,0);
                    ray[1].position = sf::Vector2f(10,10);
                    rays.push_back(ray);


                }     


            sketch[0].color = sf::Color::Red;
            sketch[1].color = sf::Color::Red;
            sketch[0].position = sf::Vector2f(0,0);
            sketch[1].position = sf::Vector2f(0,0);

            barriers.push_back(sketch);

        }


    
    public:
        Game(){
            this->initVariables();
            this->initWindow();
            this->initShapes();
        }

        virtual ~Game(){
            delete this->window;
        }

        //Accessors

        const bool running() const{
            return this->window->isOpen();
        }

        void updateMousePositions(){
            this->mousePosWindow = sf::Vector2f(sf::Mouse::getPosition(*this->window)); 
        }

        void updateRays(){
            float angle = 0;
            for(auto& ray : rays){
                int radius;
                ray[0].position = this->mousePosWindow;

                float x1, y1;

                std::vector<float> radiuses;

                
                for(auto barrier : barriers){
                    for(radius = 0; radius < 1000; radius++){

                        float b_0_x = barrier[0].position.x;
                        float b_0_y = barrier[0].position.y;
                        float b_1_x = barrier[1].position.x;
                        float b_1_y = barrier[1].position.y;


                        float barrier_a = (b_1_y - b_0_y)/(b_1_x - b_0_x);
                        float barrier_b = b_1_y - b_1_x * barrier_a;

                        x1 = radius * std::sin(angle) + this->mousePosWindow.x;
                        y1 = radius * std::cos(angle) + this->mousePosWindow.y;
                        
                        if(std::abs(y1 - barrier_a * x1 - barrier_b) <= accuracy && 
                        std::max(b_0_x, b_1_x) > x1 && 
                        x1 > std::min(b_0_x,b_1_x) && 
                        std::max(b_0_y,b_1_y)>y1 && 
                        y1 >std::min(b_0_y,b_1_y)){
                            radiuses.push_back(radius); 
                        }



                    }
                   
                }
                
                for(auto r : radiuses){
                    if(r < radius){
                        radius = r;
                    }

                }
                                        
                ray[1].position = sf::Vector2f(radius * std::sin(angle) + this->mousePosWindow.x,radius * std::cos(angle) + this->mousePosWindow.y); 
                angle += (float) 2* 3.14/this->step;
            }





        }

        void updateShapeLocations(){
            this->source.setPosition(sf::Vector2f(this->mousePosWindow.x,this->mousePosWindow.y));
            this->updateRays();
            if(drawing){
                this->sketch[1].position = this->mousePosWindow;
            }
            
        }

        void drawShapes(){
            this->window->draw(this->source); 
            
            for(auto ray : rays){
                this->window->draw(ray);
            }

            for(auto barrier : this->barriers){
                this->window->draw(barrier);            
            }

            if(drawing){
                this->window->draw(sketch);

            }



        }

        void handleDrawing(){
            if(this->drawing){
                drawing = false;
                barriers.push_back(sketch);

            } else {
                drawing = true;
                sketch[0].position = this->mousePosWindow;

            }

        }


        void pollEvents(){
            while(this->window->pollEvent(this->ev)){
                
                switch(this->ev.type){
                    
                    case sf::Event::Closed:
                        this->window->close();
                        break;
                    
                    case sf::Event::KeyPressed:
                        if(this->ev.key.code == sf::Keyboard::Escape){
                            this->window->close();
                        }
                        break;
                    case sf::Event::MouseButtonPressed:
                        handleDrawing();
                        break;
                    
                }
            }
        };


        //update and rendering

        void update(){
            this->pollEvents();
            
            //mouse pos
            this->updateMousePositions();
            //relative to the screen
            this->updateShapeLocations();

        };

        void render(){
            this->window->clear(this->bg_color);
            //draw game
            this->drawShapes();
            //----
            this->window->display();
        };


};