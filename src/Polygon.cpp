#include "sfml-util/geometry/Polygon.hpp"

namespace sfu{

    Polygon::Polygon(const sf::Vector2f* points, int n){
        initialize(points, n);
    }
    void Polygon::initialize(const sf::Vector2f* points, int n){;
        lines.clear();
        lines.reserve(n);
        for(int i=1; i <= n; i++){
            lines.emplace_back(points[i - 1], points[i % n]);
        }
    }

}
