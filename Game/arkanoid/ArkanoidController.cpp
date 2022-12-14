#include "ArkanoidController.hpp"
#include <stdlib.h>

Arkanoid::Arkanoid()
{
	if (!m_wallCollideSound.loadFromFile("resources/ping.wav"))
	{
		stop();
	}

	if (!m_brickCollideSound.loadFromFile("resources/pong.wav"))
	{
		stop();
	}

	for(int y = 0; y < 10; ++y)
	{
		for(int x = 0; x < 9; ++x)
		{
			m_blockStack.emplace_back(BlockInfo());
		}
	}

	m_video->changeTitle("Arkanoid");

	reset();

	m_input->EventKeyDown.connect(Event::CreateCallBack(this, &Arkanoid::onKeyDown));
	m_input->EventQuit.connect(Event::CreateCallBack(this, &Arkanoid::onQuit));
}

void Arkanoid::onTick()
{
	if(m_input->getKeyHeld()->Left.value)
	{
		m_playerPosition.x -= 3.5;

		if(m_playerPosition.x < 0)
		{
			m_playerPosition.x = 0;
		}
	}
	else if(m_input->getKeyHeld()->Right.value)
	{
		m_playerPosition.x += 3.5;

		if(m_playerPosition.x + m_playerSize.x > m_screenSize.x)
		{
			m_playerPosition.x = m_screenSize.x - m_playerSize.x;
		}
	}

	if(collisionAABB(m_ballPosition, m_ballSize, m_playerPosition, m_playerSize))
	{
		m_ballVelocity.y *= -1;

		if(m_ballVelocity.y > -5)
		{
			m_ballVelocity.y -= 0.3;
		}

		float diff = (m_ballPosition.x + (m_ballSize.x / 2)) - (m_playerPosition.x + (m_playerSize.x / 2));

		m_ballVelocity.x = (diff / (m_playerSize.x / 2) * 3);

		m_sound.setBuffer(m_wallCollideSound);
		m_sound.play();
	}

	for(int y = 0; y < 10; ++y)
	{
		for(int x = 0; x < 9; ++x)
		{
			BlockInfo* block = &m_blockStack[x+(y*9)];

			if(block->life == 0)
			{
				continue;
			}

			if(collisionAABB(m_ballPosition, m_ballSize, block->position, block->size))
			{
				sf::Vector2f vector1_diagonal1 = sf::Vector2f((block->position.x + block->size.x) - block->position.x, (block->position.y + block->size.y) - block->position.y);
				sf::Vector2f vector2_diagonal1 = sf::Vector2f((m_ballPosition.x + m_ballSize.x / 2) - block->position.x, (m_ballPosition.y + m_ballSize.y / 2) - block->position.y);

				sf::Vector2f vector1_diagonal2 = sf::Vector2f((block->position.x + block->size.x) - block->position.x, block->position.y - (block->position.y + block->size.y));
				sf::Vector2f vector2_diagonal2 = sf::Vector2f((m_ballPosition.x + m_ballSize.x / 2) - block->position.x, (m_ballPosition.y + m_ballSize.y / 2) - (block->position.y + block->size.y));

				float crossProduct_diagonal1 = (vector1_diagonal1.x * vector2_diagonal1.y) - (vector1_diagonal1.y * vector2_diagonal1.x);
				float crossProduct_diagonal2 = (vector1_diagonal2.x * vector2_diagonal2.y) - (vector1_diagonal2.y * vector2_diagonal2.x);

				if(crossProduct_diagonal1 >= 0 && crossProduct_diagonal2 >= 0)
				{
					m_ballVelocity.y *= -1;
				}
				else if(crossProduct_diagonal1 >= 0 && crossProduct_diagonal2 <= 0)
				{
					m_ballVelocity.x *= -1;
				}
				else if(crossProduct_diagonal1 <= 0 && crossProduct_diagonal2 >= 0)
				{
					m_ballVelocity.x *= -1;
				}
				else if(crossProduct_diagonal1 <= 0 && crossProduct_diagonal2 <= 0)
				{
					m_ballVelocity.y *= -1;
				}

				block->life -= 1;

				m_sound.setBuffer(m_brickCollideSound);
				m_sound.play();
			}
		}
	}

	if(m_ballPosition.x < 0)
	{
		m_ballVelocity.x *= -1;

		m_sound.setBuffer(m_wallCollideSound);
		m_sound.play();
	}
	else if(m_ballPosition.x + m_ballSize.x > m_screenSize.x)
	{
		m_ballVelocity.x *= -1;

		m_sound.setBuffer(m_wallCollideSound);
		m_sound.play();
	}
	else if(m_ballPosition.y < 0)
	{
		m_ballVelocity.y *= -1;

		m_sound.setBuffer(m_wallCollideSound);
		m_sound.play();
	}

	if(m_ballPosition.y + m_ballSize.y > m_screenSize.y - 26)
	{
		reset();
	}

	m_ballPosition += m_ballVelocity;
}

void Arkanoid::onRender()
{
	m_video->clear(sf::Color::Black);

	m_video->drawRectangle(m_playerPosition, m_playerSize, sf::Color::White);
	m_video->drawRectangle(m_ballPosition, m_ballSize, sf::Color::White);

	for(int y = 0; y < 10; ++y)
	{
		for(int x = 0; x < 9; ++x)
		{
			if(m_blockStack[x+(y*9)].life != 0)
			{
				sf::Color color;

				switch(m_blockStack[x+(y*9)].life)
				{
					case 1:
						color = sf::Color::White;
						break;
					case 2:
						color = sf::Color::Red;
						break;
					case 3:
						color = sf::Color::Blue;
						break;
					case 4:
						color = sf::Color::Green;
						break;
					case 5:
						color = sf::Color::Yellow;
						break;
					case 6:
						color = sf::Color::Magenta;
						break;
				}

				m_video->drawRectangle(m_blockStack[x+(y*9)].position, m_blockStack[x+(y*9)].size, color);
			}
		}
	}

	m_video->swapBuffers();
}

void Arkanoid::onKeyDown(int keyCode)
{
	switch (keyCode)
	{
		case KeyId::F2:
			if(m_sound.getVolume() > 0)
			{
				m_sound.setVolume(0);
			}
			else
			{
				m_sound.setVolume(100);
			}
			break;

		case KeyId::Escape:
			stop();
			break;

		case KeyId::F1:
			reset();
			break;
	}
}

void Arkanoid::onQuit()
{
	stop();
}

void Arkanoid::reset()
{
	m_screenSize = sf::Vector2i(640, 480);

	m_ballSize = sf::Vector2f(10, 10);
	m_ballPosition = sf::Vector2f(m_screenSize.x / 2 - m_ballSize.x / 2, m_screenSize.y - m_ballSize.y - 50);
	m_ballVelocity = sf::Vector2f(2, -1.5);

	m_playerSize = sf::Vector2f(64, 16);
	m_playerPosition = sf::Vector2f(m_screenSize.x / 2 - m_playerSize.x / 2, m_screenSize.y - m_playerSize.y - 16);

	for(int y = 0; y < 10; ++y)
	{
		for(int x = 0; x < 9; ++x)
		{
			m_blockStack[x+(y*9)].size = sf::Vector2f(64, 16);
			m_blockStack[x+(y*9)].life = 1 + (y <= 5 ? 5 - y : 0);
			m_blockStack[x+(y*9)].position = sf::Vector2f(6.4+(x*6.4)+(x*64), 8+(y*8)+(y*16));
		}
	}
}
