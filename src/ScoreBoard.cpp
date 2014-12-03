/*********************************************************************
Matt Marchant 2014
http://trederia.blogspot.com

Crush - Zlib license.

This software is provided 'as-is', without any express or
implied warranty. In no event will the authors be held
liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute
it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented;
you must not claim that you wrote the original software.
If you use this software in a product, an acknowledgment
in the product documentation would be appreciated but
is not required.

2. Altered source versions must be plainly marked as such,
and must not be misrepresented as being the original software.

3. This notice may not be removed or altered from any
source distribution.
*********************************************************************/

#include <ScoreBoard.hpp>
#include <Game.hpp>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>

#include <sstream>

namespace
{
    sf::Text playerOneText;
    sf::Text playerTwoText;

    const sf::Uint16 npcPoints = 200; //points for killing npc
    const sf::Uint16 playerPoints = 300; //points for killing other player
    const sf::Uint16 crushPoints = 500; //points for crushing someone
    const sf::Uint16 suicidePoints = 200; //points deducted for accidentally crushing self
}

ScoreBoard::ScoreBoard(StateStack& stack, State::Context context)
    : m_stack           (stack),
    m_context           (context),
    m_playerOneLives    (5),
    m_playerTwoLives    (-1),
    m_playerOneScore    (0u),
    m_playerTwoScore    (0u)
{
    playerOneText.setFont(context.gameInstance.getFont("default"));
    playerOneText.setPosition({ 60.f, 10.f });
    updateText(Category::PlayerOne);

    playerTwoText.setFont(*playerOneText.getFont());
    playerTwoText.setString("Press Start");
    playerTwoText.setPosition({ 1400.f, 10.f });
}

//public
void ScoreBoard::onNotify(Subject& s, const game::Event& evt)
{
    switch (evt.type)
    {
    case game::Event::Node:
        if (evt.node.action == game::Event::NodeEvent::Despawn)
        {
            switch (evt.node.type)
            {
            case Category::PlayerOne:
                m_playerOneLives--;
                //TODO disable spawning if lives zero  
                updateText(evt.node.type);
                break;
            case Category::PlayerTwo:
                m_playerTwoLives--;
                //TODO disable spawning if lives zero
                updateText(evt.node.type);
                break;
            case Category::Npc:
                //TODO end game if bad guys all dead
                //TODO disable npc spawner
                break;
            default: break;
            }
        }
        else if(evt.node.action == game::Event::NodeEvent::KilledNode)
        {
            auto textUpdateTarget = evt.node.type;
            switch (evt.node.type)
            {
            case Category::PlayerOne:
                if (evt.node.target == Category::Npc)
                {
                    m_playerOneScore += npcPoints;
                }
                else if (evt.node.target == Category::PlayerTwo)
                {
                    m_playerOneScore += playerPoints;
                    m_playerOneLives++;
                }
                break;
            case Category::PlayerTwo:
                if (evt.node.target == Category::Npc)
                {
                    m_playerTwoScore += npcPoints;
                }
                else if (evt.node.target == Category::PlayerOne)
                {
                    m_playerTwoScore += playerPoints;
                    m_playerTwoLives++;
                }
                break;
            case Category::Block:
                if (evt.node.owner == Category::PlayerOne)
                {
                        switch (evt.node.target)
                        {
                        case Category::Npc: //p1 killed bad guy
                        case Category::PlayerTwo: //p1 crushed p2
                            m_playerOneScore += crushPoints;
                            break;
                        case Category::PlayerOne: //p1 killed self, doh
                            (m_playerOneScore > suicidePoints) ?
                                m_playerOneScore -= suicidePoints :
                                m_playerOneScore = 0u;
                            break;
                        default: break;
                        }
                }
                else if (evt.node.owner == Category::PlayerTwo)
                {
                        switch (evt.node.target)
                        {
                        case Category::Npc: //p2 killed bad guy
                        case Category::PlayerOne: //p2 killed p1
                            m_playerTwoScore += crushPoints;
                            break;
                        case Category::PlayerTwo: //p2 crushed self :S
                            (m_playerTwoScore > suicidePoints) ?
                                m_playerTwoScore -= suicidePoints :
                                m_playerTwoScore = 0u;
                            break;
                        default: break;
                        }
                }
                textUpdateTarget = evt.node.owner;
                break;
            default: break;
            }
            if (textUpdateTarget != Category::None)
                updateText(textUpdateTarget);
        }
        break;
    case game::Event::Player:

        break;
    default: break;
    }
}

//private
void ScoreBoard::updateText(Category::Type type)
{
    std::stringstream ss;    
    if (type == Category::PlayerOne)
    {
        if (m_playerOneLives >= 0)
            ss << "Lives: " << m_playerOneLives << "    Score: " << m_playerOneScore;
        else
            ss << "GAME OVER    Score: " << m_playerOneScore;
        playerOneText.setString(ss.str());
    }
    else
    {
        if (m_playerTwoLives >= 0)
            ss << "Lives: " << m_playerTwoLives << "    Score: " << m_playerTwoScore;
        else
            ss << "GAME OVER    Score: " << m_playerTwoScore;
        playerTwoText.setString(ss.str());
    }

    if (m_playerOneLives < 0 && m_playerTwoLives < 0)
    {
        //Gaaaaaaame Oveeeeer!!!
        m_stack.pushState(States::ID::GameOver);
    }
}

void ScoreBoard::draw(sf::RenderTarget& rt, sf::RenderStates states) const
{
    m_context.renderWindow.draw(playerOneText);
    m_context.renderWindow.draw(playerTwoText);
}