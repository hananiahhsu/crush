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

#include <MapController.hpp>
#include <Map.hpp>

#include <SFML/Graphics/RectangleShape.hpp>

#include <map>

namespace
{

}

MapController::MapController(CommandStack& cs)
    : m_commandStack(cs)
{

}

//public
void MapController::onNotify(Subject& s, const game::Event& evt)
{
    switch (evt.type)
    {
    case game::Event::Node:
        switch (evt.node.action)
        {
        case game::Event::NodeEvent::Spawn:

            break;
        default: break;
        }
        break;
    default: break;
    }
}

void MapController::setSpawnFunction(std::function<void(Category::Type, const sf::Vector2f&, const sf::Vector2f&)>& func)
{
    spawn = func;
}

void MapController::loadMap(const Map& map)
{
    const auto& nodes = map.getNodes();
    for (const auto& n : nodes) //TODO we want to skip items
        spawn(n.type, n.position, n.size);
}