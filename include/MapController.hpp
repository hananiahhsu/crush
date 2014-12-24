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

//controls spawning of solid / block / collectable items in world

#ifndef MAP_CONTROLLER_H_
#define MAP_CONTROLLER_H_

#include <CommandStack.hpp>
#include <Resource.hpp>
#include <ShaderResource.hpp>
#include <Map.hpp>
#include <AnimatedSprite.hpp>
#include <WaterDrawable.hpp>

#include <SFML/System/NonCopyable.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/VertexArray.hpp>

#include <functional>
#include <list>

class Map;
class MapController final : private sf::NonCopyable
{
public:
    enum class MapDrawable
    {
        Item,
        Solid,
        Block,
        Water
    };

    MapController(CommandStack& cs, TextureResource& tr, ShaderResource& sr);
    ~MapController() = default;

    void update(float dt);

    void setSpawnFunction(std::function<void(const Map::Node&)>& func);
    void loadMap(const Map& map);

    sf::Drawable* getDrawable(MapDrawable type);
private:
    struct Item
    {
        Item(const Map::Node& pos, float lt)
            : node(pos), lifeTime(lt){}
        Map::Node node;
        float lifeTime;
    };

    CommandStack& m_commandStack;

    std::vector<Item> m_items;
    float m_itemTime;
    bool m_itemActive;

    TextureResource& m_textureResource;
    ShaderResource& m_shaderResource;
    AnimatedSprite m_itemSprite;
    AnimatedSprite m_blockSprite;

    std::list<WaterDrawable> m_waterDrawables;


    std::function<void(const Map::Node&)> spawn;
    void shuffleItems();


    //TODO rename this as we'll recycle it for other layers of the map
    class SolidDrawable : public sf::Drawable, private sf::NonCopyable
    {
    public:
        explicit SolidDrawable(TextureResource& tr, sf::Shader& shader);
        ~SolidDrawable() = default;

        void addSolid(const sf::Vector2f& position, const sf::Vector2f& size);
    private:
        sf::Texture m_diffuseTexture;
        sf::Texture m_normalTexture;
        sf::Shader& m_shader;

        sf::VertexArray m_vertexArray;

        void draw(sf::RenderTarget& rt, sf::RenderStates states) const override;
    } m_drawable;
};

#endif //MAP_CONTROLLER_H_