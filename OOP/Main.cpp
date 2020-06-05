#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <time.h>
#include <list>
#pragma warning(disable : 4996)
using namespace sf;
//Screen parameters: W - width, H - height
const int W = 1920;
const int H = 1080;

float DEGTORAD = 0.017453f;
//Animation - Movement animation 
class Animation
{
public:
	float Frame, speed;
	Sprite sprite;
	std::vector<IntRect> frames;

	Animation() {}

	Animation(Texture &t, int x, int y, int w, int h, int count, float Speed)
	{
		Frame = 0;
		speed = Speed;

		for (int i = 0; i<count; i++)
			frames.push_back(IntRect(x + i*w, y, w, h));

		sprite.setTexture(t);
		sprite.setOrigin(w / 2, h / 2);
		sprite.setTextureRect(frames[0]);
	}


	void update()
	{
		Frame += speed;
		int n = frames.size();
		if (Frame >= n) Frame -= n;
		if (n>0) sprite.setTextureRect(frames[int(Frame)]);
	}

	bool isEnd()
	{
		return Frame + speed >= frames.size();
	}

};

//Entity - ojects parameters 
class Entity
{
public:
	float x, y, dx, dy, R, angle;
	bool life;
	std::string name;
	Animation anim;

	Entity()
	{
		life = 1;
	}

	void settings(Animation &a, int X, int Y, float Angle = 0, int radius = 1)
	{
		anim = a;
		x = X; y = Y;
		angle = Angle;
		R = radius;
	}

	virtual void update() {};

	void draw(RenderWindow &app)
	{
		anim.sprite.setPosition(x, y);
		anim.sprite.setRotation(angle + 90);
		app.draw(anim.sprite);

		CircleShape circle(R);
		circle.setFillColor(Color(255, 0, 0, 170));
		circle.setPosition(x, y);
		circle.setOrigin(R, R);
		//app.draw(circle);
	}

	virtual ~Entity() {};
};

//Asteroid - all asteroid info 
class asteroid : public Entity
{
public:
	asteroid()
	{
		dx = rand() % 8 - 4;
		dy = rand() % 8 - 4;
		name = "asteroid";
	}


};



	
int maxSpeed = 2;
//player - all player info
class player : public Entity
{
public:
	bool thrust;
	bool back;
	player()
	{
		name = "player";
	}

	void update()
	{
		if (thrust)
		{
			dx += cos(angle*DEGTORAD)*0.2;
			dy += sin(angle*DEGTORAD)*0.2;
		}
		else
		{
			dx *= 0.59;
			dy *= 0.59;
		}
		if(back)
		{
			dx -= cos(angle*DEGTORAD)*0.2;
			dy -= sin(angle*DEGTORAD)*0.2;
		}
		else
		{
			dx *= 0.99;
			dy *= 0.99;
		}

		float speed = sqrt(dx*dx + dy*dy);
		if (speed>maxSpeed)
		{
			dx *= maxSpeed / speed;
			dy *= maxSpeed / speed;
		}

		x += dx;
		y += dy;

		if (x>W) x = 0; if (x<0) x = W;
		if (y>H) y = 0; if (y<0) y = H;
	}

};

//Describing of interaction between spaceship and asteroid
bool isCollide(Entity *a, Entity *b)
{

	return (b->x - a->x)*(b->x - a->x) +
		(b->y - a->y)*(b->y - a->y)<
		(a->R + b->R)*(a->R + b->R);

}

//Program main menu
void menu(RenderWindow & window) {
	Texture menuTexture1, menuTexture2, menuTexture3,  menuBackground;
	menuTexture1.loadFromFile("images/NewGame.png");
	menuTexture3.loadFromFile("images/Exit.png");
	menuBackground.loadFromFile("images/background2.jpg");

	Sprite menu1(menuTexture1), menu3(menuTexture3), menuBg(menuBackground);//pinned images
	bool isMenu = 1;
	int menuNum = 0;
	menu1.setPosition(100, 30);
	menu3.setPosition(100, 150);
	menuBg.setPosition(345, 0);

	//////////////////////////////Menu choice///////////////////
	while (isMenu)
	{
		menu1.setColor(Color::Yellow);
		menu3.setColor(Color::Yellow);
		menuNum = 0;
		window.clear(Color(129, 181, 221));

		if (IntRect(100, 30, 300, 50).contains(Mouse::getPosition(window))) { menu1.setColor(Color::Blue); menuNum = 1; }
		if (IntRect(100, 150, 300, 50).contains(Mouse::getPosition(window))) { menu3.setColor(Color::Blue); menuNum = 3; }

		if (Mouse::isButtonPressed(Mouse::Left))
		{
			if (menuNum == 1) isMenu = false;
			if (menuNum == 3) { window.close(); isMenu = false; }

		}

		window.draw(menuBg);
		window.draw(menu1);
		window.draw(menu3);

		window.display();
	}
	////////////////////////////////////////////////////
}

//main - attached files and game controls
int main()
{
	srand(time(0));
	RenderWindow app(VideoMode(W, H), "Going to the planet",Style::Fullscreen);
	menu(app);
	app.setFramerateLimit(60);

	SoundBuffer buffer;
	if (!buffer.loadFromFile("music/fireshoot.wav"));
    Sound sound;
	sound.setBuffer(buffer);
	sound.setVolume(100);

	SoundBuffer buffer1;
	if (!buffer1.loadFromFile("music/explosion.wav"));
	Sound sound1;
	sound1.setBuffer(buffer1);
	sound1.setVolume(100);

	Music music;
	if (!music.openFromFile("music/mainmusic.ogg"))
	{
		std::cout << "ERROR" << std::endl;
	}
	music.play();
	music.setVolume(100);

	Texture t1, t2, t4, t6, t7;
	t1.loadFromFile("images/spaceship.png");
	t2.loadFromFile("images/background.jpg");
	t4.loadFromFile("images/rock.png");
	t6.loadFromFile("images/rock_small.png");
	t7.loadFromFile("images/explosions/type_B.png");

	t1.setSmooth(true);
	t2.setSmooth(true);
	Sprite background(t2);
	Animation sRock(t4, 0, 0, 100, 100, 1, 0);
	Animation sRock_small(t6, 0, 0, 64, 64, 1, 0.2);
	Animation sPlayer(t1, 40, 0, 40, 40, 1, 0);
	Animation sPlayer_go(t1, 40, 40, 40, 40, 1, 0);
	std::list<Entity*> entities;

	for (int i = 0; i<15; i++)
	{
		asteroid *a = new asteroid();
		a->settings(sRock, 1000, 560, 0, 30);
		entities.push_back(a);
	}
	
	player *p = new player();
	p->settings(sPlayer, 200, 200, 270, 20);
	entities.push_back(p);
	/////Game controls description/////
	while (app.isOpen())
	{
		Event event;
		if (Keyboard::isKeyPressed(Keyboard::Right)) p->angle += 3;
		if (Keyboard::isKeyPressed(Keyboard::Left))  p->angle -= 3;
		if (Keyboard::isKeyPressed(Keyboard::Up)) p->thrust = true;
		if (Keyboard::isKeyPressed(Keyboard::Down)) p->back = true;
		else p->thrust = false;

		for (auto a : entities)
			for (auto b : entities)
			{
				
				if (a->name == "player" && b->name == "asteroid")
					if (isCollide(a, b))
					{
						t2.loadFromFile("images/suc.jpg");
						t2.setSmooth(true);
						Sprite background(t2);
						sound1.play();
						maxSpeed = 0;
						
					}
				

				


		if (p->thrust)  p->anim = sPlayer_go;
		else   p->anim = sPlayer;
		if (p->back)  p->anim = sPlayer_go;
		else   p->anim = sPlayer;

		for (auto e : entities)
			if (e->name == "explosion")
				if (e->anim.isEnd()) e->life = 0;

		

		for (auto i = entities.begin(); i != entities.end();)
		{
			Entity *e = *i;

			e->update();
			e->anim.update();

			if (e->life == false) { i = entities.erase(i); delete e; }
			else i++;
		}
		//////draw//////
		app.draw(background);

		for (auto i : entities)
			i->draw(app);

		app.display();
	}
	
	return 0;
}
